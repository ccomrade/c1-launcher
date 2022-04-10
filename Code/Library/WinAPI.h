#pragma once

#include <cstddef>
#include <cstring>
#include <string>
#include <stdexcept>

#include "FileBase.h"

namespace WinAPI
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

	int CurrentErrorCode();

	std::string GetErrorCodeDescription(int code);

	std::runtime_error CurrentError(const char *format, ...);

	/////////////
	// Modules //
	/////////////

	namespace Module
	{
		void* Get(const char* moduleName);

		void* Load(const char* moduleName);
		void Unload(void* pModule);

		void* FindSymbol(void* pModule, const char* symbolName);

		std::string GetPath(void* pModule);

		inline void* GetEXE()
		{
			return Get(NULL);
		}

		inline std::string GetEXEPath()
		{
			return GetPath(NULL);
		}
	}

	/////////////////
	// Message box //
	/////////////////

	void ErrorBox(const char *message);

	///////////////
	// Resources //
	///////////////

	int GetCrysisGameBuild(void *pDLL);

	bool IsVistaOrLater();

	///////////
	// Hacks //
	///////////

	int FillNOP(void *address, std::size_t length);
	int FillMem(void *address, const void *data, std::size_t length);

	/////////////
	// Threads //
	/////////////

	unsigned long GetCurrentThreadID();

	namespace CriticalSection
	{
		void* Create();

		void Enter(void* pCriticalSection);
		void Leave(void* pCriticalSection);

		void Destroy(void* pCriticalSection);
	}

	///////////
	// Files //
	///////////

	namespace File
	{
		void* Open(const char* path, FileBase::Access access, bool *pCreated = NULL);

		bool Read(void* pFile, std::string& buffer, std::size_t maxLength = 0);
		bool Write(void* pFile, const void* data, std::size_t dataLength);

		bool Seek(void* pFile, FileBase::SeekBase base, __int64 offset = 0, unsigned __int64* pNewPos = NULL);
		bool Resize(void* pFile, unsigned __int64 size);

		void Close(void* pFile);

		bool Copy(const char* sourcePath, const char* destinationPath);
	}

	namespace Directory
	{
		bool Create(const char* path, bool *pCreated = NULL);
	}

	//////////
	// Time //
	//////////

	struct DateTime
	{
		unsigned short year;
		unsigned short month;  // January = 1, February = 2, ..., December = 12
		unsigned short day;
		unsigned short dayOfWeek;  // Monday = 1, Tuesday = 2, ..., Sunday = 7
		unsigned short hour;
		unsigned short minute;
		unsigned short second;
		unsigned short millisecond;

		DateTime() : year(0), month(0), day(0), dayOfWeek(0), hour(0), minute(0), second(0), millisecond(0)
		{
		}

		const char* GetDayName();
		const char* GetMonthName();

		static DateTime GetCurrentUTC();
		static DateTime GetCurrentLocal();

		static long GetTimeZoneBias();
		static void AddTimeZoneOffset(std::string& buffer);
	};

	////////////////////////
	// System information //
	////////////////////////

	namespace CPU
	{
		bool IsAMD();
		bool Has3DNow();
	}
}
