#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm>

#include <windows.h>
#include <dbghelp.h>

#include "CrashLogger.h"
#include "Format.h"
#include "WinAPI.h"

#include "Project.h"

struct CallStackEntry
{
	size_t address;
	std::string name;
	std::string moduleName;
	std::string sourceFile;
	unsigned int sourceLine;
	unsigned int baseOffset;

	CallStackEntry()
	: address(),
	  name(),
	  moduleName(),
	  sourceFile(),
	  sourceLine(),
	  baseOffset()
	{
	}

	std::string ToString() const
	{
		std::string result;

#ifdef BUILD_64BIT
		result += Format("%016I64X: ", address);
#else
		result += Format("%08X: ", address);
#endif

		if (name.empty())
		{
			result += "??";
		}
		else
		{
			result += name;

			if (baseOffset)
			{
				result += Format(" + 0x%X", baseOffset);
			}
		}

		result += " (";

		if (!sourceFile.empty())
		{
			result += sourceFile;

			if (sourceLine)
			{
				result += Format(":%u", sourceLine);
			}
		}

		result += ") in ";
		result += moduleName.empty() ? "?" : moduleName;

		return result;
	}
};

struct Module
{
	std::string name;
	size_t address;
	size_t size;

	Module()
	: name(),
	  address(),
	  size()
	{
	}

	bool operator<(const Module & other) const
	{
		return address < other.address;
	}

	std::string ToString() const
	{
#ifdef BUILD_64BIT
		return Format("%016I64X - %016I64X %s", address, address + size, name.c_str());
#else
		return Format("%08X - %08X %s", address, address + size, name.c_str());
#endif
	}
};

static const char *BaseName(const char *name)
{
	size_t offset = 0;

	for (size_t i = 0; name[i]; i++)
	{
		if (name[i] == '/' || name[i] == '\\')
		{
			offset = i + 1;
		}
	}

	return name + offset;
}

static bool IsCrysisDLL(const IMAGEHLP_MODULE & moduleInfo)
{
	std::string name = moduleInfo.ModuleName;

	// convert DLL name to lowercase
	std::transform(name.begin(), name.end(), name.begin(), std::tolower);

	// optimization
	if (name.length() < 3 || name[0] != 'c' || name[1] != 'r' || name[2] != 'y')
	{
		return false;
	}

	return name == "cry3dengine"
	    || name == "cryaction"
	    || name == "cryaisystem"
	    || name == "cryanimation"
	    || name == "cryentitysystem"
	    || name == "cryfont"
	    || name == "crygame"
	    || name == "cryinput"
	    || name == "crymovie"
	    || name == "crynetwork"
	    || name == "cryphysics"
	    || name == "cryrenderd3d10"
	    || name == "cryrenderd3d9"
	    || name == "cryrendernull"
	    || name == "cryscriptsystem"
	    || name == "crysoundsystem"
	    || name == "crysystem";
}

#ifdef BUILD_64BIT
static BOOL CALLBACK EnumerateModulesCallback(PCSTR name, DWORD64 address, ULONG size, PVOID context)
#else
static BOOL CALLBACK EnumerateModulesCallback(PCSTR name, ULONG address, ULONG size, PVOID context)
#endif
{
	std::vector<Module> *modules = static_cast<std::vector<Module>*>(context);

	modules->resize(modules->size() + 1);

	Module & result = modules->back();
	result.name = name;
	result.address = address;
	result.size = size;

	return TRUE;
}

class DebugHelper
{
	HANDLE m_process;
	HANDLE m_thread;

	bool m_isInitialized;

public:
	DebugHelper()
	: m_process(),
	  m_thread(),
	  m_isInitialized(false)
	{
	}

	~DebugHelper()
	{
		if (m_isInitialized)
		{
			SymCleanup(m_process);
		}
	}

	bool Init()
	{
		if (m_isInitialized)
		{
			return true;
		}

		m_process = GetCurrentProcess();
		m_thread  = GetCurrentThread();

		if (!SymInitialize(m_process, NULL, TRUE))
		{
			return false;
		}

		SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS);

		m_isInitialized = true;

		return true;
	}

	bool IsInitialized() const
	{
		return m_isInitialized;
	}

	std::vector<CallStackEntry> GetCallStack(const CONTEXT *pExceptionContext) const
	{
		if (!m_isInitialized)
		{
			return std::vector<CallStackEntry>();
		}

		std::vector<CallStackEntry> result;

		CONTEXT context = {};
		std::memcpy(&context, pExceptionContext, sizeof (CONTEXT));

		STACKFRAME frame = {};
		DWORD machine = 0;

#ifdef BUILD_64BIT
		machine = IMAGE_FILE_MACHINE_AMD64;

		frame.AddrPC.Offset = context.Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context.Rbp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context.Rsp;
		frame.AddrStack.Mode = AddrModeFlat;
#else
		machine = IMAGE_FILE_MACHINE_I386;

		frame.AddrPC.Offset = context.Eip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context.Ebp;
		frame.AddrFrame.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context.Esp;
		frame.AddrStack.Mode = AddrModeFlat;
#endif

		while (StackWalk(machine, m_process, m_thread, &frame, &context, NULL,
		                 SymFunctionTableAccess, SymGetModuleBase, NULL))
		{
			const size_t address = frame.AddrPC.Offset;

			result.resize(result.size() + 1);

			CallStackEntry & entry = result.back();
			entry.address = address;

			IMAGEHLP_MODULE moduleInfo = {};
			moduleInfo.SizeOfStruct = sizeof (IMAGEHLP_MODULE);

			if (SymGetModuleInfo(m_process, address, &moduleInfo))
			{
				entry.moduleName = BaseName(moduleInfo.ImageName);
			}

			unsigned char symbolBuffer[sizeof (SYMBOL_INFO) + MAX_SYM_NAME] = {};
			SYMBOL_INFO *pSymbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
			pSymbol->SizeOfStruct = sizeof (SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 symbolOffset = 0;

			if (SymFromAddr(m_process, address, &symbolOffset, pSymbol))
			{
				if (pSymbol->Flags & SYMFLAG_EXPORT && IsCrysisDLL(moduleInfo))
				{
					// drop useless symbols obtained from export tables of Crysis DLLs
				}
				else
				{
					entry.name = pSymbol->Name;
					entry.baseOffset = symbolOffset;
				}
			}

			IMAGEHLP_LINE line = {};
			line.SizeOfStruct = sizeof (IMAGEHLP_LINE);

			DWORD lineOffset = 0;

			if (SymGetLineFromAddr(m_process, address, &lineOffset, &line))
			{
				entry.sourceFile = line.FileName;
				entry.sourceLine = line.LineNumber;
			}
		}

		return result;
	}

	std::vector<Module> GetLoadedModules() const
	{
		std::vector<Module> result;

		if (m_isInitialized)
		{
			EnumerateLoadedModules(m_process, EnumerateModulesCallback, &result);

			std::sort(result.begin(), result.end());
		}

		return result;
	}
};

static const char *ExceptionCodeToString(unsigned int code)
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
	}

	return "Unknown";
}

static void AddLine(std::string& data, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	FormatToV(data, format, args);
	va_end(args);

	data += "\r\n";
}

static void DumpExceptionInfo(std::string& data, const EXCEPTION_RECORD *exception)
{
	const unsigned int code = exception->ExceptionCode;

	AddLine(data, "%s exception (0x%08X) at 0x%p", ExceptionCodeToString(code), code, exception->ExceptionAddress);

	if (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR)
	{
		void *address = reinterpret_cast<void*>(exception->ExceptionInformation[1]);

		switch (exception->ExceptionInformation[0])
		{
			case 0:
			{
				AddLine(data, "Read from 0x%p failed", address);
				break;
			}
			case 1:
			{
				AddLine(data, "Write to 0x%p failed", address);
				break;
			}
			case 8:
			{
				AddLine(data, "Execute at 0x%p failed", address);
				break;
			}
		}
	}
}

static void DumpRegisters(std::string& data, const CONTEXT *ctx)
{
	AddLine(data, "Register dump:");

#ifdef BUILD_64BIT
	AddLine(data, "RIP: %016I64X RSP: %016I64X RBP: %016I64X EFLAGS: %08X", ctx->Rip, ctx->Rsp, ctx->Rbp, ctx->EFlags);
	AddLine(data, "RAX: %016I64X RBX: %016I64X RCX: %016I64X RDX: %016I64X", ctx->Rax, ctx->Rbx, ctx->Rcx, ctx->Rdx);
	AddLine(data, "RSI: %016I64X RDI: %016I64X R8:  %016I64X R9:  %016I64X", ctx->Rsi, ctx->Rdi, ctx->R8, ctx->R9);
	AddLine(data, "R10: %016I64X R11: %016I64X R12: %016I64X R13: %016I64X", ctx->R10, ctx->R11, ctx->R12, ctx->R13);
	AddLine(data, "R14: %016I64X R15: %016I64X", ctx->R14, ctx->R15);
#else
	AddLine(data, "EIP: %08X ESP: %08X EBP: %08X EFLAGS: %08X", ctx->Eip, ctx->Esp, ctx->Ebp, ctx->EFlags);
	AddLine(data, "EAX: %08X EBX: %08X ECX: %08X EDX: %08X", ctx->Eax, ctx->Ebx, ctx->Ecx, ctx->Edx);
	AddLine(data, "ESI: %08X EDI: %08X", ctx->Esi, ctx->Edi);
#endif
}

static std::string CreateCrashData(_EXCEPTION_POINTERS *pExceptionInfo)
{
	std::string data;
	data.reserve(8192 - 1);

	AddLine(data, "================================ CRASH DETECTED ================================");
	AddLine(data, "%s", PROJECT_VERSION_DETAILS);

	DumpExceptionInfo(data, pExceptionInfo->ExceptionRecord);
	DumpRegisters(data, pExceptionInfo->ContextRecord);

	DebugHelper dbghelp;
	if (dbghelp.Init())
	{
		std::vector<CallStackEntry> callstack = dbghelp.GetCallStack(pExceptionInfo->ContextRecord);

		AddLine(data, "Callstack:");
		for (size_t i = 0; i < callstack.size(); i++)
		{
			AddLine(data, "%s", callstack[i].ToString().c_str());
		}

		std::vector<Module> modules = dbghelp.GetLoadedModules();

		AddLine(data, "Modules (%u):", modules.size());
		for (size_t i = 0; i < modules.size(); i++)
		{
			AddLine(data, "%s", modules[i].ToString().c_str());
		}
	}
	else
	{
		AddLine(data, "CrashLogger: DebugHelper initialization failed with error code %lu", GetLastError());
	}

	AddLine(data, "Command line:");
	AddLine(data, "%s", GetCommandLineA());

	AddLine(data, "================================================================================");

	return data;
}

static CrashLogger::Sink* g_sink;

static LONG __stdcall CrashHandler(_EXCEPTION_POINTERS *pExceptionInfo)
{
	// disable this crash handler to avoid recursive calls
	SetUnhandledExceptionFilter(NULL);

	if (g_sink)
	{
		const std::string data = CreateCrashData(pExceptionInfo);

		g_sink->OnCrashData(data);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void CrashLogger::SetSink(Sink& sink)
{
	g_sink = &sink;

	SetUnhandledExceptionFilter(CrashHandler);
}
