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
		DWORD flags,
		const void* source,
		DWORD message,
		DWORD language,
		char* buffer,
		DWORD bufferSize,
		va_list* args
	);

	__declspec(dllimport) HMODULE __stdcall GetModuleHandleA(const char* name);
	__declspec(dllimport) HMODULE __stdcall LoadLibraryA(const char* name);
	__declspec(dllimport) int __stdcall FreeLibrary(HMODULE handle);
	__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE handle, const char* name);
	__declspec(dllimport) DWORD __stdcall GetModuleFileNameA(HMODULE handle, char* buffer, DWORD bufferSize);

	__declspec(dllimport) int __stdcall MessageBoxA(
		HWND parentWindow,
		const char* text,
		const char* title,
		unsigned int type
	);

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
			return ::GetCommandLineA();
		}

		const char* GetOnlyArgs();

		bool HasArg(const char* arg);

		const char* GetArgValue(const char* arg, const char* defaultValue = "");
	}

	////////////
	// Errors //
	////////////

	inline unsigned long GetCurrentErrorCode()
	{
		return ::GetLastError();
	}

	inline std::size_t GetErrorDescription(char* buffer, std::size_t bufferSize, unsigned long code)
	{
		const DWORD flags = 0x200 | 0x1000;  // FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS

		return ::FormatMessageA(flags, NULL, code, 0, buffer, static_cast<DWORD>(bufferSize), NULL);
	}

	/////////////
	// Modules //
	/////////////

	namespace DLL
	{
		inline void* Get(const char* name)
		{
			return ::GetModuleHandleA(name);
		}

		inline void* Load(const char* name)
		{
			return ::LoadLibraryA(name);
		}

		inline void Unload(void* mod)
		{
			::FreeLibrary(static_cast<HMODULE>(mod));
		}

		inline void* FindSymbol(void* mod, const char* symbolName)
		{
			return ::GetProcAddress(static_cast<HMODULE>(mod), symbolName);
		}

		inline std::size_t GetPath(char* buffer, std::size_t bufferSize, void* mod)
		{
			return ::GetModuleFileNameA(static_cast<HMODULE>(mod), buffer, static_cast<DWORD>(bufferSize));
		}

		namespace Version
		{
			int GetMajor(void* mod);
			int GetMinor(void* mod);
			int GetTweak(void* mod);
			int GetPatch(void* mod);
		}
	}

	namespace EXE
	{
		inline void* Get()
		{
			return ::GetModuleHandleA(NULL);
		}

		inline std::size_t GetPath(char* buffer, std::size_t bufferSize)
		{
			return ::GetModuleFileNameA(NULL, buffer, static_cast<DWORD>(bufferSize));
		}

		namespace Version
		{
			using namespace DLL::Version;
		}
	}

	//////////////////
	// Dialog boxes //
	//////////////////

	inline void ErrorBox(const char* message, const char* title = "Error")
	{
		::MessageBoxA(NULL, message, title, 0x0 | 0x10);  // MB_OK | MB_ICONERROR
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
		return ::GetCurrentThreadId();
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
			::InitializeCriticalSection(&m_cs);
		}

		~Mutex()
		{
			::DeleteCriticalSection(&m_cs);
		}

		void Lock()
		{
			::EnterCriticalSection(&m_cs);
		}

		void Unlock()
		{
			::LeaveCriticalSection(&m_cs);
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

			return ::CopyFileA(source, destination, failIfExists) != 0;
		}

		inline bool CreateDirectory(const char* path)
		{
			return ::CreateDirectoryA(path, NULL) != 0 || ::GetLastError() == 183;  // ERROR_ALREADY_EXISTS
		}
	}

	std::size_t GetDocumentsPath(char* buffer, std::size_t bufferSize);

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
		return static_cast<std::size_t>(::GetLocaleInfoA(
			0x800,  // LOCALE_SYSTEM_DEFAULT
			0x59,   // LOCALE_SISO639LANGNAME
			buffer, static_cast<int>(bufferSize)
		));
	}
}
