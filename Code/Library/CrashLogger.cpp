#include <signal.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#include <dbghelp.h>
#include <psapi.h>

#include "CrashLogger.h"

#ifdef BUILD_64BIT
#define ADDR_FMT "%016I64X"
#else
#define ADDR_FMT "%08X"
#endif

#define CRASH_LOGGER_ABORT 0xE0C1C100
#define CRASH_LOGGER_PURE_CALL 0xE0C1C101
#define CRASH_LOGGER_INVALID_PARAM 0xE0C1C102
#define CRASH_LOGGER_ENGINE_ERROR 0xE0C1C103

static CrashLogger::ExtraProvider* g_extraProvider = NULL;
static CrashLogger::LogFileProvider g_logFileProvider;
static const char* g_banner;

static void* ByteOffset(void* base, std::size_t offset)
{
	return static_cast<unsigned char*>(base) + offset;
}

static const char* BaseName(const char* name)
{
	const char* result = name;

	for (; *name; name++)
	{
		if (*name == '/' || *name == '\\')
		{
			result = name + 1;
		}
	}

	return result;
}

static const char* ExceptionCodeToName(unsigned int code)
{
	switch (code)
	{
		case EXCEPTION_ACCESS_VIOLATION:         return "Access violation";
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array index out of bounds";
		case EXCEPTION_BREAKPOINT:               return "Breakpoint";
		case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Unaligned access";
		case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Floating-point denormal operand";
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Floating-point divide by zero";
		case EXCEPTION_FLT_INEXACT_RESULT:       return "Floating-point inexact result";
		case EXCEPTION_FLT_INVALID_OPERATION:    return "Floating-point invalid operation";
		case EXCEPTION_FLT_OVERFLOW:             return "Floating-point overflow";
		case EXCEPTION_FLT_STACK_CHECK:          return "Floating-point stack check";
		case EXCEPTION_FLT_UNDERFLOW:            return "Floating-point underflow";
		case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal instruction";
		case EXCEPTION_IN_PAGE_ERROR:            return "Page error";
		case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer divide by zero";
		case EXCEPTION_INT_OVERFLOW:             return "Integer overflow";
		case EXCEPTION_INVALID_DISPOSITION:      return "Invalid disposition";
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable exception";
		case EXCEPTION_PRIV_INSTRUCTION:         return "Privileged instruction";
		case EXCEPTION_SINGLE_STEP:              return "Single step";
		case EXCEPTION_STACK_OVERFLOW:           return "Stack overflow";
		case 0xE06D7363:                         return "C++ exception";
		case CRASH_LOGGER_ABORT:                 return "Abnormal program termination";
		case CRASH_LOGGER_PURE_CALL:             return "Pure virtual function call";
		case CRASH_LOGGER_INVALID_PARAM:         return "Invalid parameter detected by CRT";
		case CRASH_LOGGER_ENGINE_ERROR:          return "Engine error";
	}

	return "Unknown exception";
}

static void DumpExceptionInfo(std::FILE* file, const EXCEPTION_RECORD* info)
{
	const unsigned int code = info->ExceptionCode;
	const std::size_t address = reinterpret_cast<std::size_t>(info->ExceptionAddress);

	std::fprintf(file, "%s (0x%08X) at 0x" ADDR_FMT, ExceptionCodeToName(code), code, address);

	if (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR)
	{
		const std::size_t dataAddress = info->ExceptionInformation[1];

		switch (info->ExceptionInformation[0])
		{
			case 0: std::fprintf(file, ": Read from 0x"  ADDR_FMT " failed", dataAddress); break;
			case 1: std::fprintf(file, ": Write to 0x"   ADDR_FMT " failed", dataAddress); break;
			case 8: std::fprintf(file, ": Execute at 0x" ADDR_FMT " failed", dataAddress); break;
		}
	}
	else if (code == CRASH_LOGGER_ENGINE_ERROR)
	{
		const char* message = reinterpret_cast<const char*>(info->ExceptionInformation[0]);

		std::fprintf(file, ": %s", message);
	}

	std::fprintf(file, "\n");
	std::fflush(file);
}

static void DumpGlobalMemoryUsage(std::FILE* file)
{
	MEMORYSTATUSEX status = {};
	status.dwLength = sizeof(status);
	if (!GlobalMemoryStatusEx(&status))
	{
		std::fprintf(file, "GlobalMemoryStatusEx failed with error code %u\n", GetLastError());
		std::fflush(file);
		return;
	}

	std::fprintf(file, "Physical memory = %.1f MiB (%.1f MiB available, %.1f%% used)\n",
		static_cast<double>(status.ullTotalPhys) / (1024 * 1024),
		static_cast<double>(status.ullAvailPhys) / (1024 * 1024),
		(100.0 * (status.ullTotalPhys - status.ullAvailPhys)) / status.ullTotalPhys
	);

	std::fprintf(file, "Virtual memory = %.1f MiB (%.1f MiB available, %.1f%% used)\n",
		static_cast<double>(status.ullTotalVirtual) / (1024 * 1024),
		static_cast<double>(status.ullAvailVirtual) / (1024 * 1024),
		(100.0 * (status.ullTotalVirtual - status.ullAvailVirtual)) / status.ullTotalVirtual
	);

	std::fflush(file);
}

static void DumpProcessMemoryUsage(std::FILE* file)
{
	HMODULE psapi = GetModuleHandleA("psapi.dll");
	if (!psapi)
	{
		psapi = LoadLibraryA("psapi.dll");
	}

	if (!psapi)
	{
		std::fprintf(file, "Loading psapi.dll failed with error code %u\n", GetLastError());
		std::fflush(file);
		return;
	}

	void* getProcessMemoryInfo = GetProcAddress(psapi, "GetProcessMemoryInfo");
	if (!getProcessMemoryInfo)
	{
		std::fprintf(file, "Obtaining GetProcessMemoryInfo failed with error code %u\n", GetLastError());
		std::fflush(file);
		return;
	}

	HANDLE process = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS info = {};
	info.cb = sizeof(info);
	if (!static_cast<BOOL(__stdcall *)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD)>
		(getProcessMemoryInfo)(process, &info, sizeof(info)))
	{
		std::fprintf(file, "GetProcessMemoryInfo failed with error code %u\n", GetLastError());
		std::fflush(file);
		return;
	}

	std::fprintf(file, "Commit charge = %.1f MiB (%.1f MiB peak)\n",
		static_cast<double>(info.PagefileUsage) / (1024 * 1024),
		static_cast<double>(info.PeakPagefileUsage) / (1024 * 1024)
	);

	std::fprintf(file, "Working set = %.1f MiB (%.1f MiB peak)\n",
		static_cast<double>(info.WorkingSetSize) / (1024 * 1024),
		static_cast<double>(info.PeakWorkingSetSize) / (1024 * 1024)
	);

	std::fflush(file);
}

static void DumpRegisters(std::FILE* file, const CONTEXT* context)
{
	std::fprintf(file, "Registers:\n"
#ifdef BUILD_64BIT
	  "RIP: %016I64X RSP: %016I64X RBP: %016I64X EFLAGS: %08X\n"
	  "RAX: %016I64X RBX: %016I64X RCX: %016I64X RDX: %016I64X\n"
	  "RSI: %016I64X RDI: %016I64X R8:  %016I64X R9:  %016I64X\n"
	  "R10: %016I64X R11: %016I64X R12: %016I64X R13: %016I64X\n"
	  "R14: %016I64X R15: %016I64X\n",
	  context->Rip, context->Rsp, context->Rbp, context->EFlags,
	  context->Rax, context->Rbx, context->Rcx, context->Rdx,
	  context->Rsi, context->Rdi, context->R8,  context->R9,
	  context->R10, context->R11, context->R12, context->R13,
	  context->R14, context->R15
#else
	  "EIP: %08X ESP: %08X EBP: %08X EFLAGS: %08X\n"
	  "EAX: %08X EBX: %08X ECX: %08X EDX: %08X\n"
	  "ESI: %08X EDI: %08X\n",
	  context->Eip, context->Esp, context->Ebp, context->EFlags,
	  context->Eax, context->Ebx, context->Ecx, context->Edx,
	  context->Esi, context->Edi
#endif
	);

	std::fflush(file);
}

static void DumpCallStack(std::FILE* file, const CONTEXT* context)
{
	std::fprintf(file, "Callstack:\n");

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

#ifdef BUILD_64BIT
	DWORD machine = IMAGE_FILE_MACHINE_AMD64;

	STACKFRAME frame = {};
	frame.AddrPC.Offset = context->Rip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context->Rbp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context->Rsp;
	frame.AddrStack.Mode = AddrModeFlat;
#else
	DWORD machine = IMAGE_FILE_MACHINE_I386;

	STACKFRAME frame = {};
	frame.AddrPC.Offset = context->Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context->Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context->Esp;
	frame.AddrStack.Mode = AddrModeFlat;
#endif

	CONTEXT localContext = *context;

	SymSetOptions(
		SYMOPT_DEFERRED_LOADS |
		SYMOPT_EXACT_SYMBOLS |
		SYMOPT_FAIL_CRITICAL_ERRORS |
		SYMOPT_LOAD_LINES |
		SYMOPT_NO_PROMPTS |
		SYMOPT_UNDNAME
	);

	if (SymInitialize(process, NULL, TRUE))
	{
		while (StackWalk(machine, process, thread, &frame, &localContext, NULL,
		                 SymFunctionTableAccess, SymGetModuleBase, NULL))
		{
			const std::size_t address = frame.AddrPC.Offset;

			const char* moduleName = "??";
			IMAGEHLP_MODULE moduleInfo = {};
			moduleInfo.SizeOfStruct = sizeof(moduleInfo);
			if (SymGetModuleInfo(process, address, &moduleInfo))
			{
				moduleName = BaseName(moduleInfo.ImageName);
			}

			const char* symbolName = "??";
			unsigned char symbolBuffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {};
			SYMBOL_INFO& symbol = *reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
			symbol.SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol.MaxNameLen = MAX_SYM_NAME;
			DWORD64 symbolOffset = 0;
			if (SymFromAddr(process, address, &symbolOffset, &symbol))
			{
				symbolName = symbol.Name;
			}

			std::fprintf(file, ADDR_FMT " %s: %s", address, moduleName, symbolName);

			IMAGEHLP_LINE line = {};
			line.SizeOfStruct = sizeof(line);
			DWORD lineOffset = 0;
			if (SymGetLineFromAddr(process, address, &lineOffset, &line))
			{
				std::fprintf(file, " (%s:%u)\n", BaseName(line.FileName), line.LineNumber);
			}
			else
			{
				std::fprintf(file, " ()\n");
			}
		}

		SymCleanup(process);
	}
	else
	{
		std::fprintf(file, "SymInitialize failed with error code %u\n", GetLastError());
	}

	std::fflush(file);
}

static void DumpLoadedModules(std::FILE* file)
{
	// old Windows SDKs don't provide complete enough definitions of all required structures
#ifdef BUILD_64BIT
	const std::size_t pebOffset = 0x60;
	const std::size_t ldrOffset = 0x18;
	const std::size_t modListOffset = 0x20;  // InMemoryOrderModuleList
	const std::size_t modBaseOffset = 0x30 - 0x10;
	const std::size_t modSizeOffset = 0x40 - 0x10;
	const std::size_t modNameOffset = 0x48 - 0x10;
#else
	const std::size_t pebOffset = 0x30;
	const std::size_t ldrOffset = 0x0C;
	const std::size_t modListOffset = 0x14;  // InMemoryOrderModuleList
	const std::size_t modBaseOffset = 0x18 - 0x8;
	const std::size_t modSizeOffset = 0x20 - 0x8;
	const std::size_t modNameOffset = 0x24 - 0x8;
#endif

	void* teb = NtCurrentTeb();
	void* peb = *static_cast<void**>(ByteOffset(teb, pebOffset));
	void* ldr = *static_cast<void**>(ByteOffset(peb, ldrOffset));

	LIST_ENTRY* headMod = static_cast<LIST_ENTRY*>(ByteOffset(ldr, modListOffset));

	LIST_ENTRY* firstMod = NULL;
	std::size_t firstModBase = static_cast<std::size_t>(-1);
	unsigned int modCount = 0;

	for (LIST_ENTRY* mod = headMod->Flink; mod != headMod; mod = mod->Flink)
	{
		const std::size_t modBase = *static_cast<std::size_t*>(ByteOffset(mod, modBaseOffset));

		if (modBase < firstModBase)
		{
			firstMod = mod;
			firstModBase = modBase;
		}

		modCount++;
	}

	std::fprintf(file, "Modules (%u):\n", modCount);

	for (LIST_ENTRY* mod = firstMod; mod != NULL;)
	{
		const std::size_t base = *static_cast<std::size_t*>(ByteOffset(mod, modBaseOffset));
		const std::size_t size = *static_cast<unsigned long*>(ByteOffset(mod, modSizeOffset));
		const UNICODE_STRING* wideName = static_cast<UNICODE_STRING*>(ByteOffset(mod, modNameOffset));

		char name[512] = {};
		WideCharToMultiByte(CP_UTF8, 0, wideName->Buffer, wideName->Length, name, sizeof(name), NULL, NULL);

		std::fprintf(file, ADDR_FMT " - " ADDR_FMT " %s\n", base, base + size, name);

		LIST_ENTRY* nextMod = NULL;
		std::size_t nextModBase = static_cast<std::size_t>(-1);

		for (mod = headMod->Flink; mod != headMod; mod = mod->Flink)
		{
			const std::size_t modBase = *static_cast<std::size_t*>(ByteOffset(mod, modBaseOffset));

			if (modBase > base && modBase < nextModBase)
			{
				nextMod = mod;
				nextModBase = modBase;
			}
		}

		mod = nextMod;
	}

	std::fflush(file);
}

static void DumpCommandLine(std::FILE* file)
{
	std::fprintf(file, "Command line:\n");
	std::fprintf(file, "%s\n", GetCommandLineA());
	std::fflush(file);
}

static void WriteDumpHeader(std::FILE* file)
{
	std::fprintf(file, "================================ CRASH DETECTED ================================\n");
	std::fprintf(file, "%s\n", g_banner);
	std::fflush(file);
}

static void WriteDumpFooter(std::FILE* file)
{
	std::fprintf(file, "================================================================================\n");
	std::fflush(file);
}

static void WriteCrashDump(std::FILE* file, EXCEPTION_POINTERS* exception)
{
	WriteDumpHeader(file);

	DumpExceptionInfo(file, exception->ExceptionRecord);
	DumpGlobalMemoryUsage(file);
	DumpProcessMemoryUsage(file);
	DumpRegisters(file, exception->ContextRecord);
	DumpCallStack(file, exception->ContextRecord);
	DumpLoadedModules(file);
	DumpCommandLine(file);

	CrashLogger::ExtraProvider* extra = g_extraProvider;
	while (extra)
	{
		extra->OnCrash(file);
		extra = extra->next;
	}

	WriteDumpFooter(file);
}

static LONG __stdcall CrashHandler(EXCEPTION_POINTERS* exception)
{
	// avoid recursive calls
	SetUnhandledExceptionFilter(NULL);

	if (g_logFileProvider)
	{
		std::FILE* file = g_logFileProvider();
		if (file)
		{
			WriteCrashDump(file, exception);
			std::fclose(file);
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

static void AbortHandler(int)
{
	RaiseException(CRASH_LOGGER_ABORT, EXCEPTION_NONCONTINUABLE, 0, NULL);
	ExitProcess(1);
}

static void PureCallHandler()
{
	RaiseException(CRASH_LOGGER_PURE_CALL, EXCEPTION_NONCONTINUABLE, 0, NULL);
	ExitProcess(1);
}

static void InvalidParameterHandler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t)
{
	RaiseException(CRASH_LOGGER_INVALID_PARAM, EXCEPTION_NONCONTINUABLE, 0, NULL);
	ExitProcess(1);
}

void CrashLogger::OnEngineError(const char* format, va_list args)
{
	char buffer[256];
	// std::vsnprintf is not supported by VS2005
	_vsnprintf(buffer, sizeof(buffer), format, args);
	buffer[sizeof(buffer) - 1] = '\0';

	const ULONG_PTR params[] = { reinterpret_cast<ULONG_PTR>(buffer) };
	const DWORD paramCount = 1;

	RaiseException(CRASH_LOGGER_ENGINE_ERROR, EXCEPTION_NONCONTINUABLE, paramCount, params);
	ExitProcess(1);
}

void CrashLogger::Enable(LogFileProvider logFileProvider, const char* banner)
{
	g_logFileProvider = logFileProvider;
	g_banner = banner;

	SetUnhandledExceptionFilter(&CrashHandler);

	signal(SIGABRT, &AbortHandler);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);  // suppress abort message (console) and dialog (GUI)

	_set_purecall_handler(&PureCallHandler);
	_set_invalid_parameter_handler(&InvalidParameterHandler);

#if !defined(_MSC_VER) || _MSC_VER != 1400
	// for non-VS2005 compilers
	HMODULE msvcr80 = LoadLibraryA("msvcr80.dll");
	if (msvcr80)
	{
		void* vs2005_signal = GetProcAddress(msvcr80, "signal");
		if (vs2005_signal)
		{
			reinterpret_cast<void(*(*)(int, void(*)(int)))(int)>
				(vs2005_signal)(SIGABRT, &AbortHandler);
		}

		void* vs2005_set_abort_behavior = GetProcAddress(msvcr80, "_set_abort_behavior");
		if (vs2005_set_abort_behavior)
		{
			reinterpret_cast<unsigned int(*)(unsigned int, unsigned int)>
				(vs2005_set_abort_behavior)(0, _WRITE_ABORT_MSG);
		}

		void* vs2005_set_purecall_handler = GetProcAddress(msvcr80, "_set_purecall_handler");
		if (vs2005_set_purecall_handler)
		{
			reinterpret_cast<_purecall_handler(*)(_purecall_handler)>
				(vs2005_set_purecall_handler)(&PureCallHandler);
		}

		void* vs2005_set_invalid_parameter_handler = GetProcAddress(msvcr80, "_set_invalid_parameter_handler");
		if (vs2005_set_invalid_parameter_handler)
		{
			reinterpret_cast<_invalid_parameter_handler(*)(_invalid_parameter_handler)>
				(vs2005_set_invalid_parameter_handler)(&InvalidParameterHandler);
		}
	}
#endif
}

void CrashLogger::AddExtraProvider(ExtraProvider* provider)
{
	if (!g_extraProvider)
	{
		g_extraProvider = provider;
		return;
	}

	ExtraProvider* current = g_extraProvider;
	while (current->next)
	{
		current = current->next;
	}

	current->next = provider;
}
