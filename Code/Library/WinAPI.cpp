#include <stdlib.h>
#include <string.h>
#include <intrin.h>
#include <windows.h>

#include "WinAPI.h"
#include "Format.h"

//////////////////
// Command line //
//////////////////

namespace
{
	int FindArgIndex(const char* arg)
	{
		for (int i = 1; i < __argc; i++)
		{
			if (_stricmp(__argv[i], arg) == 0)
			{
				return i;
			}
		}

		return -1;
	}

	const char* GetNextArg(int index)
	{
		return (index >= 0 && (index + 1) < __argc) ? __argv[index + 1] : NULL;
	}
}

const char* WinAPI::CmdLine::Get()
{
	return GetCommandLineA();
}

const char* WinAPI::CmdLine::GetOnlyArgs()
{
	const char* args = Get();

	char separator = ' ';

	if (*args == '"')
	{
		separator = '"';
		args++;
	}
	else if (*args == '\'')
	{
		separator = '\'';
		args++;
	}

	for (; *args; args++)
	{
		if (*args == separator)
		{
			args++;
			break;
		}
	}

	while (*args == ' ')
	{
		args++;
	}

	return args;
}

bool WinAPI::CmdLine::HasArg(const char* arg)
{
	return FindArgIndex(arg) > 0;
}

const char* WinAPI::CmdLine::GetArgValue(const char* arg, const char* defaultValue)
{
	const int index = FindArgIndex(arg);
	const char *value = GetNextArg(index);

	// make sure the value is not another argument
	return (value && value[0] != '-' && value[0] != '+') ? value : defaultValue;
}

////////////
// Errors //
////////////

int WinAPI::CurrentErrorCode()
{
	return GetLastError();
}

std::string WinAPI::GetErrorCodeDescription(int code)
{
	const DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	char buffer[256];
	const DWORD length = FormatMessageA(flags, NULL, code, 0, buffer, sizeof buffer, NULL);

	return std::string(buffer, length);
}

std::runtime_error WinAPI::CurrentError(const char *format, ...)
{
	const int code = CurrentErrorCode();
	const std::string description = GetErrorCodeDescription(code);

	va_list args;
	va_start(args, format);
	const std::string message = FormatV(format, args);
	va_end(args);

	const std::string what = Format("%s\n\nError %d: %s", message.c_str(), code, description.c_str());

	return std::runtime_error(what);
}

/////////////
// Modules //
/////////////

void* WinAPI::DLL::Get(const char* name)
{
	return GetModuleHandleA(name);
}

void* WinAPI::DLL::Load(const char* name)
{
	return LoadLibraryA(name);
}

void WinAPI::DLL::Unload(void* pDLL)
{
	FreeLibrary(static_cast<HMODULE>(pDLL));
}

void* WinAPI::DLL::FindSymbol(void* pDLL, const char* symbolName)
{
	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(pDLL), symbolName));
}

std::string WinAPI::DLL::GetPath(void* pDLL)
{
	char buffer[512];
	const DWORD length = GetModuleFileNameA(static_cast<HMODULE>(pDLL), buffer, sizeof buffer);

	std::string path;

	if (length > 0)
	{
		if (length >= sizeof buffer)
		{
			// the buffer is too small
			// WinXP doesn't set the last error
			SetLastError(ERROR_INSUFFICIENT_BUFFER);
		}
		else
		{
			path.assign(buffer, length);
		}
	}

	return path;
}

/////////////////
// Message box //
/////////////////

void WinAPI::ErrorBox(const char *message)
{
	MessageBoxA(NULL, message, "Error", MB_OK | MB_ICONERROR);
}

///////////////
// Resources //
///////////////

/**
 * Obtains game version from any Crysis DLL.
 *
 * It parses version resource of the specified file.
 *
 * @param pDLL Handle of any Crysis DLL.
 * @return Game build number or -1 if some error occurred.
 */
int WinAPI::GetCrysisGameBuild(void *pDLL)
{
	HRSRC resourceInfo = FindResourceA(static_cast<HMODULE>(pDLL), MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (!resourceInfo)
		return -1;

	HGLOBAL resourceData = LoadResource(static_cast<HMODULE>(pDLL), resourceInfo);
	if (!resourceData)
		return -1;

	const void *versionRes = LockResource(resourceData);
	if (!versionRes)
		return -1;

	// https://docs.microsoft.com/en-us/windows/win32/menurc/vs-versioninfo
	const void *versionResKey = static_cast<const unsigned char*>(versionRes) + 0x6;
	if (std::memcmp(versionResKey, L"VS_VERSION_INFO", 0x20) != 0)
		return -1;

	const void *versionResValue = static_cast<const unsigned char*>(versionResKey) + 0x20 + 0x2;

	// https://docs.microsoft.com/en-us/windows/win32/api/verrsrc/ns-verrsrc-vs_fixedfileinfo
	const VS_FIXEDFILEINFO *pFileInfo = static_cast<const VS_FIXEDFILEINFO*>(versionResValue);
	if (pFileInfo->dwSignature != 0xFEEF04BD)
		return -1;

	return LOWORD(pFileInfo->dwFileVersionLS);
}

///////////
// Hacks //
///////////

/**
 * Fills read-only memory region with x86 NOP instruction.
 *
 * @param address Address of the memory region.
 * @param length Size of the memory region in bytes.
 * @return 0 on success or -1 if some error occurred.
 */
int WinAPI::FillNOP(void *address, std::size_t length)
{
	DWORD oldProtection = 0;

	if (!VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection))
		return -1;

	// 0x90 is opcode of NOP instruction on both x86 and x86-64
	std::memset(address, '\x90', length);

	if (!VirtualProtect(address, length, oldProtection, &oldProtection))
		return -1;

	return 0;
}

/**
 * Fills read-only memory region with custom data.
 *
 * The memory region and the data must not overlap.
 *
 * @param address Address of the memory region.
 * @param data The data.
 * @param length Size of the data in bytes.
 * @return 0 on success or -1 if some error occurred.
 */
int WinAPI::FillMem(void *address, const void *data, std::size_t length)
{
	DWORD oldProtection = 0;

	if (!VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection))
		return -1;

	std::memcpy(address, data, length);

	if (!VirtualProtect(address, length, oldProtection, &oldProtection))
		return -1;

	return 0;
}

/////////////
// Threads //
/////////////

unsigned long WinAPI::GetCurrentThreadID()
{
	return GetCurrentThreadId();
}

void* WinAPI::CriticalSection::Create()
{
	CRITICAL_SECTION* pCriticalSection = new CRITICAL_SECTION;
	InitializeCriticalSection(pCriticalSection);
	return pCriticalSection;
}

void WinAPI::CriticalSection::Enter(void* pCriticalSection)
{
	EnterCriticalSection(static_cast<CRITICAL_SECTION*>(pCriticalSection));
}

void WinAPI::CriticalSection::Leave(void* pCriticalSection)
{
	LeaveCriticalSection(static_cast<CRITICAL_SECTION*>(pCriticalSection));
}

void WinAPI::CriticalSection::Destroy(void* pCriticalSection)
{
	DeleteCriticalSection(static_cast<CRITICAL_SECTION*>(pCriticalSection));
	delete static_cast<CRITICAL_SECTION*>(pCriticalSection);
}

///////////
// Files //
///////////

namespace
{
	DWORD ToNativeFileAccessMode(FileBase::Access access)
	{
		switch (access)
		{
			case FileBase::READ_ONLY:
			{
				return GENERIC_READ;
			}
			case FileBase::WRITE_ONLY:
			case FileBase::WRITE_ONLY_CREATE:
			{
				return GENERIC_WRITE;
			}
			case FileBase::READ_WRITE:
			case FileBase::READ_WRITE_CREATE:
			{
				return GENERIC_READ | GENERIC_WRITE;
			}
		}

		return 0;
	}

	DWORD ToNativeFileCreationDisposition(FileBase::Access access)
	{
		switch (access)
		{
			case FileBase::READ_ONLY:
			case FileBase::WRITE_ONLY:
			case FileBase::READ_WRITE:
			{
				return OPEN_EXISTING;
			}
			case FileBase::WRITE_ONLY_CREATE:
			case FileBase::READ_WRITE_CREATE:
			{
				return OPEN_ALWAYS;
			}
		}

		return 0;
	}

	DWORD ToNativeFileSeek(FileBase::SeekBase base)
	{
		switch (base)
		{
			case FileBase::BEGIN:   return FILE_BEGIN;
			case FileBase::CURRENT: return FILE_CURRENT;
			case FileBase::END:     return FILE_END;
		}

		return 0;
	}
}

void* WinAPI::File::Open(const char* path, FileBase::Access access, bool *pCreated)
{
	const DWORD mode = ToNativeFileAccessMode(access);
	const DWORD share = FILE_SHARE_READ;
	const DWORD creation = ToNativeFileCreationDisposition(access);
	const DWORD attributes = FILE_ATTRIBUTE_NORMAL;

	HANDLE handle = CreateFileA(path, mode, share, NULL, creation, attributes, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	if (pCreated)
	{
		*pCreated = (GetLastError() != ERROR_ALREADY_EXISTS);
	}

	return handle;
}

bool WinAPI::File::Read(void* pFile, std::string& buffer, std::size_t maxLength)
{
	HANDLE handle = static_cast<HANDLE>(pFile);

	if (maxLength == 0)
	{
		// read everything from the current position to the end of the file
		unsigned __int64 currentPos = 0;
		unsigned __int64 endPos = 0;

		if (!Seek(handle, FileBase::CURRENT, 0, &currentPos)
		 || !Seek(handle, FileBase::END, 0, &endPos)
		 || !Seek(handle, FileBase::BEGIN, currentPos))  // restore position
		{
			return false;
		}

		if (currentPos < endPos)
		{
			const unsigned __int64 distance = endPos - currentPos;

			if (distance >= 0x80000000)  // 2 GiB
			{
				SetLastError(ERROR_FILE_TOO_LARGE);
				return false;
			}

			maxLength = distance;
		}
	}

	buffer.resize(maxLength);

	void* targetBuffer = const_cast<char*>(buffer.data());
	DWORD targetBufferSize = static_cast<DWORD>(buffer.length());

	DWORD bytesRead = 0;

	if (!ReadFile(handle, targetBuffer, targetBufferSize, &bytesRead, NULL))
	{
		return false;
	}

	if (bytesRead != targetBufferSize)
	{
		buffer.resize(bytesRead);
	}

	return true;
}

bool WinAPI::File::Write(void* pFile, const void* data, std::size_t dataLength)
{
#ifdef BUILD_64BIT
	if (dataLength >= 0xFFFFFFFF)
	{
		SetLastError(ERROR_BUFFER_OVERFLOW);
		return false;
	}
#endif

	std::size_t totalBytesWritten = 0;

	do
	{
		const void *buffer = static_cast<const unsigned char*>(data) + totalBytesWritten;
		const DWORD bufferSize = static_cast<DWORD>(dataLength - totalBytesWritten);

		DWORD bytesWritten = 0;

		if (!WriteFile(static_cast<HANDLE>(pFile), buffer, bufferSize, &bytesWritten, NULL))
		{
			return false;
		}

		totalBytesWritten += bytesWritten;
	}
	while (totalBytesWritten < dataLength);

	return true;
}

bool WinAPI::File::Seek(void* pFile, FileBase::SeekBase base, __int64 offset, unsigned __int64* pNewPos)
{
	LARGE_INTEGER offsetValue;
	offsetValue.QuadPart = offset;

	if (!SetFilePointerEx(static_cast<HANDLE>(pFile), offsetValue, &offsetValue, ToNativeFileSeek(base)))
	{
		return false;
	}

	if (pNewPos)
	{
		*pNewPos = static_cast<unsigned __int64>(offsetValue.QuadPart);
	}

	return true;
}

bool WinAPI::File::Resize(void* pFile, unsigned __int64 size)
{
	// the offset is interpreted as an unsigned value
	const __int64 offset = static_cast<__int64>(size);

	return Seek(pFile, FileBase::BEGIN, offset) && SetEndOfFile(static_cast<HANDLE>(pFile));
}

void WinAPI::File::Close(void* pFile)
{
	CloseHandle(static_cast<HANDLE>(pFile));
}

bool WinAPI::File::Copy(const char* sourcePath, const char* destinationPath)
{
	return CopyFile(sourcePath, destinationPath, FALSE);
}

bool WinAPI::Directory::Create(const char* path, bool *pCreated)
{
	bool created = true;

	if (!CreateDirectoryA(path, NULL))
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			created = false;
		}
		else
		{
			return false;
		}
	}

	if (pCreated)
	{
		*pCreated = created;
	}

	return true;
}

//////////
// Time //
//////////

namespace
{
	WinAPI::DateTime FromNativeDateTime(const SYSTEMTIME& dateTime)
	{
		WinAPI::DateTime result;
		result.year        = dateTime.wYear;
		result.month       = dateTime.wMonth;
		result.day         = dateTime.wDay;
		result.dayOfWeek   = (dateTime.wDayOfWeek == 0) ? 7 : dateTime.wDayOfWeek;
		result.hour        = dateTime.wHour;
		result.minute      = dateTime.wMinute;
		result.second      = dateTime.wSecond;
		result.millisecond = dateTime.wMilliseconds;

		return result;
	}
}

const char* WinAPI::DateTime::GetDayName()
{
	switch (dayOfWeek)
	{
		case 1: return "Monday";
		case 2: return "Tuesday";
		case 3: return "Wednesday";
		case 4: return "Thursday";
		case 5: return "Friday";
		case 6: return "Saturday";
		case 7: return "Sunday";
	}

	return "";
}

const char* WinAPI::DateTime::GetMonthName()
{
	switch (month)
	{
		case 1:  return "January";
		case 2:  return "February";
		case 3:  return "March";
		case 4:  return "April";
		case 5:  return "May";
		case 6:  return "June";
		case 7:  return "July";
		case 8:  return "August";
		case 9:  return "September";
		case 10: return "October";
		case 11: return "November";
		case 12: return "December";
	}

	return "";
}

WinAPI::DateTime WinAPI::DateTime::GetCurrentUTC()
{
	SYSTEMTIME dateTime;
	GetSystemTime(&dateTime);

	return FromNativeDateTime(dateTime);
}

WinAPI::DateTime WinAPI::DateTime::GetCurrentLocal()
{
	SYSTEMTIME dateTime;
	GetLocalTime(&dateTime);

	return FromNativeDateTime(dateTime);
}

long WinAPI::DateTime::GetTimeZoneBias()
{
	TIME_ZONE_INFORMATION tz;
	switch (GetTimeZoneInformation(&tz))
	{
		case TIME_ZONE_ID_UNKNOWN:
		{
			return tz.Bias;
		}
		case TIME_ZONE_ID_STANDARD:
		{
			return tz.Bias + tz.StandardBias;
		}
		case TIME_ZONE_ID_DAYLIGHT:
		{
			return tz.Bias + tz.DaylightBias;
		}
		case TIME_ZONE_ID_INVALID:
		{
			return 0;
		}
	}

	return 0;
}

void WinAPI::DateTime::AddTimeZoneOffset(std::string& buffer)
{
	long bias = GetTimeZoneBias();

	if (bias == 0)
	{
		buffer += 'Z';  // UTC
	}
	else
	{
		char sign = '-';

		if (bias < 0)
		{
			bias = -bias;
			sign = '+';
		}

		FormatTo(buffer, "%c%02lu%02lu", sign, bias / 60, bias % 60);
	}
}

////////////////////////
// System information //
////////////////////////

bool WinAPI::CPU::IsAMD()
{
	const char *vendorID = "AuthenticAMD";  // must be 12 characters long

	int registers[4];
	__cpuid(registers, 0x0);

	const int *id = reinterpret_cast<const int*>(vendorID);

	return registers[1] == id[0]   // 1st part is in EBX register
	    && registers[3] == id[1]   // 2nd part is in EDX register
	    && registers[2] == id[2];  // 3rd part is in ECX register
}

bool WinAPI::CPU::Has3DNow()
{
	int registers[4];
	__cpuid(registers, 0x80000001);

	int bit3DNow = registers[3] & (1 << 31);  // bit 31 in EDX register

	return bit3DNow != 0;
}

bool WinAPI::IsVistaOrLater()
{
	OSVERSIONINFOW info = {};
	info.dwOSVersionInfoSize = sizeof info;
	GetVersionExW(&info);

	return info.dwMajorVersion >= 6;
}
