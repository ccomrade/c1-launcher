#pragma once

#include <cstdarg>
#include <cstddef>

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
#endif

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

	__declspec(dllimport) int __stdcall CloseHandle(HANDLE handle);
}

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

	namespace Module
	{
		inline void* Get(const char* name)
		{
			return ::GetModuleHandleA(name);
		}

		inline void* GetEXE()
		{
			return ::GetModuleHandleA(NULL);
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

		inline std::size_t GetEXEPath(char* buffer, std::size_t bufferSize)
		{
			return ::GetModuleFileNameA(NULL, buffer, static_cast<DWORD>(bufferSize));
		}

		namespace Version
		{
			int GetMajor(void* mod);
			int GetMinor(void* mod);
			int GetTweak(void* mod);
			int GetPatch(void* mod);
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

	inline void ReleaseHandle(void* handle)
	{
		::CloseHandle(static_cast<HANDLE>(handle));
	}

	struct File
	{
		enum Access
		{
			READ_ONLY,
			WRITE_ONLY,
			WRITE_ONLY_CREATE,
			READ_WRITE,
			READ_WRITE_CREATE,
		};

		enum SeekBase
		{
			BEGIN,
			CURRENT,
			END,
		};

		void* handle;

	private:
		// no copies
		File(const File&);
		File& operator=(const File&);

	public:
		File() : handle(NULL)
		{
		}

		~File()
		{
			this->Close();
		}

		bool IsOpen() const
		{
			return this->handle != NULL;
		}

		void Swap(File& other)
		{
			void* tmp = this->handle;
			this->handle = other.handle;
			other.handle = tmp;
		}

		bool Open(const char* path, Access access, bool* pCreated = NULL);

		std::size_t Read(void* buffer, std::size_t bufferSize, bool* pError = NULL);
		std::size_t Write(const void* data, std::size_t dataSize, bool* pError = NULL);

		bool Seek(SeekBase base, __int64 offset = 0, unsigned __int64* pNewPos = NULL);
		bool Resize(unsigned __int64 size);

		void Close()
		{
			if (this->handle != NULL)
			{
				ReleaseHandle(this->handle);
				this->handle = NULL;
			}
		}

		static bool Copy(const char* srcPath, const char* dstPath);
	};

	namespace Directory
	{
		bool Create(const char* path, bool* pCreated = NULL);
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

		const char* GetDayName();
		const char* GetMonthName();
	};

	DateTime GetCurrentDateTimeUTC();
	DateTime GetCurrentDateTimeLocal();

	long GetCurrentTimeZoneBias();

	////////////////////////
	// System information //
	////////////////////////

	bool IsVistaOrLater();

	unsigned int GetLogicalProcessorCount();
}
