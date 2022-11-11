#pragma once

#include <cstddef>
#include <utility>

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
		const char* Get();
		const char* GetOnlyArgs();

		bool HasArg(const char* arg);

		const char* GetArgValue(const char* arg, const char* defaultValue = "");
	}

	////////////
	// Errors //
	////////////

	unsigned long GetCurrentErrorCode();

	std::size_t GetErrorDescription(char* buffer, std::size_t bufferSize, unsigned long code);

	/////////////
	// Modules //
	/////////////

	namespace Module
	{
		void* Get(const char* name);
		void* GetEXE();

		void* Load(const char* name);
		void Unload(void* mod);

		void* FindSymbol(void* mod, const char* symbolName);

		std::size_t GetPath(char* buffer, std::size_t bufferSize, void* mod);
		std::size_t GetEXEPath(char* buffer, std::size_t bufferSize);

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

	void ErrorBox(const char* message, const char* title = "Error");

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

	unsigned long GetCurrentThreadID();

	///////////
	// Files //
	///////////

	void ReleaseHandle(void* handle);

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
			std::swap(this->handle, other.handle);
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
