#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>

#include "CrashLogger.h"
#include "CmdLine.h"
#include "DLL.h"
#include "Util.h"

#include "config.h"

#ifdef BUILD_64BIT
#define LAUNCHER_VERSION_INFO "C1-Launcher " C1LAUNCHER_VERSION_STRING " 64-bit"
#else
#define LAUNCHER_VERSION_INFO "C1-Launcher " C1LAUNCHER_VERSION_STRING " 32-bit"
#endif

static void AddTrailingSlash(std::string & path)
{
	if (!path.empty() && path[path.length()-1] != '\\' && path[path.length()-1] != '/')
	{
		path += '\\';
	}
}

class Log
{
	std::FILE *m_file;

public:
	Log()
	: m_file(NULL)
	{
	}

	~Log()
	{
		close();
	}

	bool open(const char *defaultFileName)
	{
		std::string name = CmdLine::GetArgValue("-logfile", defaultFileName);
		std::string path = CmdLine::GetArgValue("-root");

		AddTrailingSlash(path);
		path += name;

		if (isOpen())
		{
			close();
		}

		m_file = std::fopen(path.c_str(), "a");

		return isOpen();
	}

	bool isOpen() const
	{
		return m_file != NULL;
	}

	void write(const char *msg)
	{
		if (isOpen())
		{
			std::fputs(msg, m_file);
			std::fputc('\n', m_file);
		}
	}

	void write(const std::string & msg)
	{
		write(msg.c_str());
	}

	void printf(const char *format, ...)
	{
		if (isOpen())
		{
			va_list args;
			va_start(args, format);
			std::vfprintf(m_file, format, args);
			std::fputc('\n', m_file);
			va_end(args);
		}
	}

	void close()
	{
		if (isOpen())
		{
			std::fclose(m_file);
			m_file = NULL;
		}
	}
};

struct CallStackEntry
{
	size_t address;
	std::string name;
	std::string module;
	std::string sourceFile;
	unsigned int sourceLine;
	unsigned int baseOffset;

	CallStackEntry()
	: address(),
	  name(),
	  module(),
	  sourceFile(),
	  sourceLine(),
	  baseOffset()
	{
	}

	std::string toString() const
	{
		std::string result;

		result += "0x";
		result += Util::NumberToString(address, 16);
		result += ": ";

		if (name.empty())
		{
			result += "??";
		}
		else
		{
			result += name;

			if (baseOffset)
			{
				result += " + 0x";
				result += Util::NumberToString(baseOffset, 16);
			}
		}

		result += " (";

		if (!sourceFile.empty())
		{
			result += sourceFile;

			if (sourceLine)
			{
				result += ":";
				result += Util::NumberToString(sourceLine);
			}
		}

		result += ") in ";

		if (module.empty())
		{
			result += "?";
		}
		else
		{
			result += module;
		}

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

static bool IsCrysisDLL(const IMAGEHLP_MODULE & module)
{
	std::string name = module.ModuleName;

	// convert the DLL name to lowercase
	std::transform(name.begin(), name.end(), name.begin(), std::tolower);

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

class DebugHelper
{
	typedef BOOL (__stdcall *TSymInitialize)(HANDLE process, const char *userSearchPath, BOOL invadeProcess);
	typedef BOOL (__stdcall *TSymSetOptions)(DWORD options);
	typedef BOOL (__stdcall *TSymCleanup)(HANDLE process);
	typedef BOOL (__stdcall *TSymFromAddr)(HANDLE process, DWORD64 address, DWORD64 *offset, SYMBOL_INFO *symbol);
	typedef BOOL (__stdcall *TSymGetLineFromAddr)(HANDLE process, size_t address, size_t *offset, IMAGEHLP_LINE *line);
	typedef BOOL (__stdcall *TSymGetModuleInfo)(HANDLE process, size_t address, IMAGEHLP_MODULE *moduleInfo);
	typedef BOOL (__stdcall *TEnumerateLoadedModules)(HANDLE process, PENUMLOADED_MODULES_CALLBACK callback, void *p);
	typedef BOOL (__stdcall *TStackWalk)(DWORD machine, HANDLE process, HANDLE thread, STACKFRAME *frame,
	                                     void *contextRecord, PREAD_PROCESS_MEMORY_ROUTINE pReadMemoryRoutine,
	                                     PFUNCTION_TABLE_ACCESS_ROUTINE pFunctionTableAccessRoutine,
	                                     PGET_MODULE_BASE_ROUTINE pGetModuleBaseRoutine,
	                                     PTRANSLATE_ADDRESS_ROUTINE pTranslateAddressRoutine);

	DLL m_dll;
	HANDLE m_process;
	HANDLE m_thread;
	TSymInitialize m_pSymInitialize;
	TSymSetOptions m_pSymSetOptions;
	TSymCleanup m_pSymCleanup;
	TSymFromAddr m_pSymFromAddr;
	TSymGetLineFromAddr m_pSymGetLineFromAddr;
	TSymGetModuleInfo m_pSymGetModuleInfo;
	TEnumerateLoadedModules m_pEnumerateLoadedModules;
	TStackWalk m_pStackWalk;
	PFUNCTION_TABLE_ACCESS_ROUTINE m_pSymFunctionTableAccess;
	PGET_MODULE_BASE_ROUTINE m_pSymGetModuleBase;
	bool m_isInitialized;

	static BOOL __stdcall EnumerateModulesCallback(const char *name, ULONG_PTR address, unsigned long size, void *param)
	{
		std::vector<Module> *pResult = static_cast<std::vector<Module>*>(param);

		pResult->resize(pResult->size() + 1);

		Module & module = pResult->back();
		module.name = name;
		module.address = address;
		module.size = size;

		return TRUE;
	}

public:
	DebugHelper()
	: m_dll(),
	  m_process(),
	  m_thread(),
	  m_pSymInitialize(),
	  m_pSymSetOptions(),
	  m_pSymCleanup(),
	  m_pSymFromAddr(),
	  m_pSymGetLineFromAddr(),
	  m_pSymGetModuleInfo(),
	  m_pEnumerateLoadedModules(),
	  m_pStackWalk(),
	  m_pSymFunctionTableAccess(),
	  m_pSymGetModuleBase(),
	  m_isInitialized(false)
	{
	}

	~DebugHelper()
	{
		if (m_isInitialized)
		{
			m_pSymCleanup(m_process);
		}
	}

	bool init()
	{
		if (m_isInitialized)
		{
			return true;
		}

		if (!m_dll.load("dbghelp.dll"))
		{
			return false;
		}

		m_process = GetCurrentProcess();
		m_thread  = GetCurrentThread();

		m_pSymInitialize = m_dll.getSymbol<TSymInitialize>("SymInitialize");
		m_pSymSetOptions = m_dll.getSymbol<TSymSetOptions>("SymSetOptions");
		m_pSymCleanup    = m_dll.getSymbol<TSymCleanup>("SymCleanup");
		m_pSymFromAddr   = m_dll.getSymbol<TSymFromAddr>("SymFromAddr");

	#ifdef BUILD_64BIT
		m_pSymGetLineFromAddr     = m_dll.getSymbol<TSymGetLineFromAddr>("SymGetLineFromAddr64");
		m_pSymGetModuleInfo       = m_dll.getSymbol<TSymGetModuleInfo>("SymGetModuleInfo64");
		m_pEnumerateLoadedModules = m_dll.getSymbol<TEnumerateLoadedModules>("EnumerateLoadedModules64");
		m_pStackWalk              = m_dll.getSymbol<TStackWalk>("StackWalk64");
		m_pSymFunctionTableAccess = m_dll.getSymbol<PFUNCTION_TABLE_ACCESS_ROUTINE>("SymFunctionTableAccess64");
		m_pSymGetModuleBase       = m_dll.getSymbol<PGET_MODULE_BASE_ROUTINE>("SymGetModuleBase64");
	#else
		m_pSymGetLineFromAddr     = m_dll.getSymbol<TSymGetLineFromAddr>("SymGetLineFromAddr");
		m_pSymGetModuleInfo       = m_dll.getSymbol<TSymGetModuleInfo>("SymGetModuleInfo");
		m_pEnumerateLoadedModules = m_dll.getSymbol<TEnumerateLoadedModules>("EnumerateLoadedModules");
		m_pStackWalk              = m_dll.getSymbol<TStackWalk>("StackWalk");
		m_pSymFunctionTableAccess = m_dll.getSymbol<PFUNCTION_TABLE_ACCESS_ROUTINE>("SymFunctionTableAccess");
		m_pSymGetModuleBase       = m_dll.getSymbol<PGET_MODULE_BASE_ROUTINE>("SymGetModuleBase");
	#endif

		if (!m_pSymInitialize
		 || !m_pSymSetOptions
		 || !m_pSymCleanup
		 || !m_pSymFromAddr
		 || !m_pSymGetLineFromAddr
		 || !m_pSymGetModuleInfo
		 || !m_pEnumerateLoadedModules
		 || !m_pStackWalk
		 || !m_pSymFunctionTableAccess
		 || !m_pSymGetModuleBase)
		{
			return false;
		}

		if (!m_pSymInitialize(m_process, NULL, TRUE))
		{
			return false;
		}

		m_pSymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES | SYMOPT_FAIL_CRITICAL_ERRORS | SYMOPT_NO_PROMPTS);

		m_isInitialized = true;

		return true;
	}

	bool isInitialized() const
	{
		return m_isInitialized;
	}

	std::vector<CallStackEntry> getCallStack(const CONTEXT *pExceptionContext) const
	{
		std::vector<CallStackEntry> result;

		if (!m_isInitialized)
		{
			return result;  // empty
		}

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

		while (m_pStackWalk(machine, m_process, m_thread, &frame, &context, NULL,
		                    m_pSymFunctionTableAccess, m_pSymGetModuleBase, NULL))
		{
			const size_t address = frame.AddrPC.Offset;

			result.resize(result.size() + 1);

			CallStackEntry & entry = result.back();
			entry.address = address;

			IMAGEHLP_MODULE module = {};
			module.SizeOfStruct = sizeof (IMAGEHLP_MODULE);

			if (m_pSymGetModuleInfo(m_process, address, &module))
			{
				entry.module = BaseName(module.ImageName);
			}

			unsigned char symbolBuffer[sizeof (SYMBOL_INFO) + MAX_SYM_NAME] = {};
			SYMBOL_INFO *pSymbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
			pSymbol->SizeOfStruct = sizeof (SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 symbolOffset = 0;

			if (m_pSymFromAddr(m_process, address, &symbolOffset, pSymbol))
			{
				if (pSymbol->Flags & SYMFLAG_EXPORT && IsCrysisDLL(module))
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

			size_t lineOffset = 0;

			if (m_pSymGetLineFromAddr(m_process, address, &lineOffset, &line))
			{
				entry.sourceFile = line.FileName;
				entry.sourceLine = line.LineNumber;
			}
		}

		return result;
	}

	std::vector<Module> getAllModules() const
	{
		std::vector<Module> result;

		if (!m_isInitialized)
		{
			return result;  // empty
		}

		m_pEnumerateLoadedModules(m_process, DebugHelper::EnumerateModulesCallback, &result);

		std::sort(result.begin(), result.end());

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

static void WriteExceptionInfo(Log & log, const EXCEPTION_RECORD *exception)
{
	const unsigned int code = exception->ExceptionCode;

	log.printf("%s exception (0x%08X) at 0x%p", ExceptionCodeToString(code), code, exception->ExceptionAddress);

	if (code == EXCEPTION_ACCESS_VIOLATION || code == EXCEPTION_IN_PAGE_ERROR)
	{
		void *address = reinterpret_cast<void*>(exception->ExceptionInformation[1]);

		switch (exception->ExceptionInformation[0])
		{
			case 0:
			{
				log.printf("Read from 0x%p failed", address);
				break;
			}
			case 1:
			{
				log.printf("Write to 0x%p failed", address);
				break;
			}
			case 8:
			{
				log.printf("Execute at 0x%p failed", address);
				break;
			}
		}
	}
}

static void DumpRegisters(Log & log, const CONTEXT *ctx)
{
	log.write("Register dump:");

#ifdef BUILD_64BIT
	log.printf("RIP: %016I64X RSP: %016I64X RBP: %016I64X EFLAGS: %08X", ctx->Rip, ctx->Rsp, ctx->Rbp, ctx->EFlags);
	log.printf("RAX: %016I64X RBX: %016I64X RCX: %016I64X RDX: %016I64X", ctx->Rax, ctx->Rbx, ctx->Rcx, ctx->Rdx);
	log.printf("RSI: %016I64X RDI: %016I64X R8:  %016I64X R9:  %016I64X ", ctx->Rsi, ctx->Rdi, ctx->R8, ctx->R9);
	log.printf("R10: %016I64X R11: %016I64X R12: %016I64X R13: %016I64X", ctx->R10, ctx->R11, ctx->R12, ctx->R13);
	log.printf("R14: %016I64X R15: %016I64X", ctx->R14, ctx->R15);
#else
	log.printf("EIP: %08X ESP: %08X EBP: %08X EFLAGS: %08X", ctx->Eip, ctx->Esp, ctx->Ebp, ctx->EFlags);
	log.printf("EAX: %08X EBX: %08X ECX: %08X EDX: %08X", ctx->Eax, ctx->Ebx, ctx->Ecx, ctx->Edx);
	log.printf("ESI: %08X EDI: %08X", ctx->Esi, ctx->Edi);
#endif
}

static void LogCrash(Log & log, _EXCEPTION_POINTERS *pExceptionInfo)
{
	log.write("================================ CRASH DETECTED ================================");

	log.write(LAUNCHER_VERSION_INFO);

	WriteExceptionInfo(log, pExceptionInfo->ExceptionRecord);
	DumpRegisters(log, pExceptionInfo->ContextRecord);

	DebugHelper dbghelp;
	if (dbghelp.init())
	{
		std::vector<CallStackEntry> callstack = dbghelp.getCallStack(pExceptionInfo->ContextRecord);

		log.write("Callstack:");
		for (size_t i = 0; i < callstack.size(); i++)
		{
			log.write(callstack[i].toString());
		}

		std::vector<Module> modules = dbghelp.getAllModules();

		log.printf("Modules (%u):", modules.size());
		for (size_t i = 0; i < modules.size(); i++)
		{
			const Module & dll = modules[i];

		#ifdef BUILD_64BIT
			log.printf("%016I64X - %016I64X %s", dll.address, dll.address + dll.size, dll.name.c_str());
		#else
			log.printf("%08X - %08X %s", dll.address, dll.address + dll.size, dll.name.c_str());
		#endif
		}
	}
	else
	{
		log.printf("CrashLogger: DebugHelper initialization failed with error code %lu", GetLastError());
	}

	log.write("================================================================================");
}

static const char *g_logFileName;

static LONG __stdcall CrashHandler(_EXCEPTION_POINTERS *pExceptionInfo)
{
	Log log;

	if (log.open(g_logFileName))
	{
		LogCrash(log, pExceptionInfo);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void CrashLogger::Init(const char *logFileName)
{
	g_logFileName = logFileName;

	SetUnhandledExceptionFilter(CrashHandler);
}
