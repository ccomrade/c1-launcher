#pragma once

#include <cstdarg>
#include <cstddef>

////////////////////////////////////////////////////////////////////////////////
// windows.h
////////////////////////////////////////////////////////////////////////////////

#ifndef _INC_WINDOWS
typedef unsigned long DWORD;
typedef void* HANDLE;
typedef void* HMODULE;
typedef void* HWND;

#ifdef _WIN64
typedef __int64 (__stdcall *FARPROC)();
#else
typedef int (__stdcall *FARPROC)();
#endif

struct SECURITY_ATTRIBUTES;

#pragma pack(push, 8)
struct CRITICAL_SECTION
{
	void* reserved1;
	long reserved2;
	long reserved3;
	void* reserved4;
	void* reserved5;
	void* reserved6;
};
#pragma pack(pop)
#endif  // _INC_WINDOWS

extern "C"
{
	__declspec(dllimport) char* __stdcall GetCommandLineA();

	__declspec(dllimport) DWORD __stdcall GetLastError();
	__declspec(dllimport) DWORD __stdcall FormatMessageA(
		DWORD flags, const void* source, DWORD message, DWORD language,
		char* buffer, DWORD bufferSize, va_list* args);

	__declspec(dllimport) HMODULE __stdcall GetModuleHandleA(const char* name);
	__declspec(dllimport) HMODULE __stdcall LoadLibraryA(const char* name);
	__declspec(dllimport) int __stdcall FreeLibrary(HMODULE handle);
	__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE handle, const char* name);

	__declspec(dllimport) int __stdcall MessageBoxA(
		HWND parentWindow, const char* text, const char* title, unsigned int type);

	__declspec(dllimport) DWORD __stdcall GetCurrentThreadId();
	__declspec(dllimport) void __stdcall InitializeCriticalSection(CRITICAL_SECTION* cs);
	__declspec(dllimport) void __stdcall DeleteCriticalSection(CRITICAL_SECTION* cs);
	__declspec(dllimport) void __stdcall EnterCriticalSection(CRITICAL_SECTION* cs);
	__declspec(dllimport) void __stdcall LeaveCriticalSection(CRITICAL_SECTION* cs);

	__declspec(dllimport) int __stdcall CopyFileA(const char* source, const char* destination, int failIfExists);
	__declspec(dllimport) int __stdcall CreateDirectoryA(const char* path, SECURITY_ATTRIBUTES*);

	__declspec(dllimport) int __stdcall GetLocaleInfoA(DWORD locale, DWORD type, char* buffer, int bufferSize);
}

////////////////////////////////////////////////////////////////////////////////

#define OS_NEWLINE "\r\n"
#define OS_NEWLINE_LENGTH 2

#define OS_PATH_SLASH "\\"

namespace OS
{
	//////////////////
	// Command line //
	//////////////////

	namespace CmdLine
	{
		inline const char* Get()
		{
			return GetCommandLineA();
		}

		const char* GetOnlyArgs();

		bool HasArg(const char* arg);

		const char* GetArgValue(const char* arg, const char* defaultValue);
	}

	////////////
	// Errors //
	////////////

	inline unsigned long GetSysError()
	{
		return GetLastError();
	}

	inline std::size_t GetSysErrorDescription(char* buffer, std::size_t bufferSize, unsigned long sysError)
	{
		const DWORD flags = 0x200 | 0x1000;  // FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM

		return FormatMessageA(flags, NULL, sysError, 0, buffer, static_cast<DWORD>(bufferSize), NULL);
	}

	/////////////
	// Modules //
	/////////////

	namespace DLL
	{
		inline void* Get(const char* name)
		{
			return GetModuleHandleA(name);
		}

		inline void* Load(const char* name)
		{
			return LoadLibraryA(name);
		}

		inline void Unload(void* dll)
		{
			FreeLibrary(static_cast<HMODULE>(dll));
		}

		inline void* FindSymbol(void* dll, const char* symbolName)
		{
			return GetProcAddress(static_cast<HMODULE>(dll), symbolName);
		}

		std::size_t GetPath(void* dll, char* buffer, std::size_t bufferSize);

		namespace Version
		{
			int GetMajor(void* dll);
			int GetMinor(void* dll);
			int GetTweak(void* dll);
			int GetPatch(void* dll);
		}
	}

	namespace EXE
	{
		inline void* Get()
		{
			return DLL::Get(NULL);
		}

		inline std::size_t GetPath(char* buffer, std::size_t bufferSize)
		{
			return DLL::GetPath(NULL, buffer, bufferSize);
		}

		namespace Version
		{
			inline int GetMajor() { return DLL::Version::GetMajor(NULL); }
			inline int GetMinor() { return DLL::Version::GetMinor(NULL); }
			inline int GetTweak() { return DLL::Version::GetTweak(NULL); }
			inline int GetPatch() { return DLL::Version::GetPatch(NULL); }
		}
	}

	//////////////////
	// Dialog boxes //
	//////////////////

	inline void ErrorBox(const char* message, const char* title = "Error")
	{
		MessageBoxA(NULL, message, title, 0x0 | 0x10);  // MB_OK | MB_ICONERROR
	}

	///////////
	// Hacks //
	///////////

	namespace Hack
	{
		bool FillNop(void* address, std::size_t size);
		bool FillMem(void* address, const void* data, std::size_t dataSize);
	}

	/////////////
	// Threads //
	/////////////

	inline unsigned long GetCurrentThreadID()
	{
		return GetCurrentThreadId();
	}

	class Mutex
	{
		CRITICAL_SECTION m_cs;

		// no copies
		Mutex(const Mutex&);
		Mutex& operator=(const Mutex&);

	public:
		Mutex()
		{
			InitializeCriticalSection(&m_cs);
		}

		~Mutex()
		{
			DeleteCriticalSection(&m_cs);
		}

		void Lock()
		{
			EnterCriticalSection(&m_cs);
		}

		void Unlock()
		{
			LeaveCriticalSection(&m_cs);
		}
	};

	template<class T>
	class LockGuard
	{
		T& m_lock;

	public:
		explicit LockGuard(T& lock) : m_lock(lock)
		{
			m_lock.Lock();
		}

		~LockGuard()
		{
			m_lock.Unlock();
		}
	};

	///////////
	// Files //
	///////////

	namespace FileSystem
	{
		inline bool CopyFile(const char* source, const char* destination)
		{
			const int failIfExists = 0;

			return ::CopyFileA(source, destination, failIfExists);
		}

		inline bool CreateDirectory(const char* path)
		{
			return ::CreateDirectoryA(path, NULL) || GetLastError() == 183;  // ERROR_ALREADY_EXISTS
		}
	}

	std::size_t GetWorkingDirectory(char* buffer, std::size_t bufferSize);
	std::size_t GetDocumentsPath(char* buffer, std::size_t bufferSize);

	std::size_t PretiffyPath(const char* path, char* buffer, std::size_t bufferSize);

	//////////
	// Time //
	//////////

	struct DateTime
	{
		unsigned short year;
		unsigned short month;  // January = 1, February = 2, ..., December = 12
		unsigned short dayOfWeek;  // Monday = 1, Tuesday = 2, ..., Sunday = 7
		unsigned short day;
		unsigned short hour;
		unsigned short minute;
		unsigned short second;
		unsigned short millisecond;

		DateTime() : year(0), month(0), dayOfWeek(0), day(0), hour(0), minute(0), second(0), millisecond(0)
		{
		}
	};

	DateTime GetCurrentDateTimeUTC();
	DateTime GetCurrentDateTimeLocal();

	long GetCurrentTimeZoneBias();

	////////////////////////
	// System information //
	////////////////////////

	bool IsVistaOrLater();

	unsigned int GetLogicalProcessorCount();

	// https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
	inline std::size_t GetSystemLanguageCode(char* buffer, std::size_t bufferSize)
	{
		const DWORD locale = 0x800;  // LOCALE_SYSTEM_DEFAULT
		const DWORD type = 0x59;     // LOCALE_SISO639LANGNAME

		return static_cast<std::size_t>(GetLocaleInfoA(locale, type, buffer, static_cast<int>(bufferSize)));
	}
}
