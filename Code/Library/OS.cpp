#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <shlobj.h>
#include <windows.h>

#include "OS.h"

//////////////////
// Command line //
//////////////////

static int FindArgIndex(const char* arg)
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

static const char* GetNextArg(int index)
{
	if (index < 0 || (index + 1) >= __argc)
	{
		return NULL;
	}
	else
	{
		return __argv[index + 1];
	}
}

static bool IsArgValue(const char* value)
{
	// make sure the value is not another argument
	return value && *value != '-' && *value != '+';
}

const char* OS::CmdLine::Get()
{
	return GetCommandLineA();
}

const char* OS::CmdLine::GetOnlyArgs()
{
	char separator = ' ';
	const char* args = Get();

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

bool OS::CmdLine::HasArg(const char* arg)
{
	return FindArgIndex(arg) > 0;
}

const char* OS::CmdLine::GetArgValue(const char* arg, const char* defaultValue)
{
	const int index = FindArgIndex(arg);
	const char* value = GetNextArg(index);

	return IsArgValue(value) ? value : defaultValue;
}

////////////
// Errors //
////////////

unsigned long OS::GetCurrentErrorCode()
{
	return GetLastError();
}

std::size_t OS::GetErrorDescription(char* buffer, std::size_t bufferSize, unsigned long code)
{
	const DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

	return FormatMessageA(flags, NULL, code, 0, buffer, static_cast<DWORD>(bufferSize), NULL);
}

/////////////
// Modules //
/////////////

void* OS::Module::Get(const char* name)
{
	return GetModuleHandleA(name);
}

void* OS::Module::GetEXE()
{
	return GetModuleHandleA(NULL);
}

void* OS::Module::Load(const char* name)
{
	return LoadLibraryA(name);
}

void OS::Module::Unload(void* mod)
{
	FreeLibrary(static_cast<HMODULE>(mod));
}

void* OS::Module::FindSymbol(void* mod, const char* symbolName)
{
	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(mod), symbolName));
}

std::size_t OS::Module::GetPath(char* buffer, std::size_t bufferSize, void* mod)
{
	return GetModuleFileNameA(static_cast<HMODULE>(mod), buffer, static_cast<DWORD>(bufferSize));
}

std::size_t OS::Module::GetEXEPath(char* buffer, std::size_t bufferSize)
{
	return GetModuleFileNameA(NULL, buffer, static_cast<DWORD>(bufferSize));
}

static const VS_FIXEDFILEINFO* GetFileInfo(void* mod)
{
	HMODULE hMod = static_cast<HMODULE>(mod);

	HRSRC resInfo = FindResourceA(hMod, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (!resInfo)
	{
		return NULL;
	}

	HGLOBAL resData = LoadResource(hMod, resInfo);
	if (!resData)
	{
		return NULL;
	}

	const void* versionRes = LockResource(resData);
	if (!versionRes)
	{
		return NULL;
	}

	const void* versionResKey = static_cast<const unsigned char*>(versionRes) + 0x6;
	if (memcmp(versionResKey, L"VS_VERSION_INFO", 0x20) != 0)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	const void* versionResValue = static_cast<const unsigned char*>(versionResKey) + 0x20 + 0x2;

	const VS_FIXEDFILEINFO* fileInfo = static_cast<const VS_FIXEDFILEINFO*>(versionResValue);
	if (fileInfo->dwSignature != 0xFEEF04BD)
	{
		SetLastError(ERROR_INVALID_DATA);
		return NULL;
	}

	return fileInfo;
}

int OS::Module::Version::GetMajor(void* mod)
{
	const VS_FIXEDFILEINFO* fileInfo = GetFileInfo(mod);

	return (fileInfo) ? HIWORD(fileInfo->dwProductVersionMS) : -1;
}

int OS::Module::Version::GetMinor(void* mod)
{
	const VS_FIXEDFILEINFO* fileInfo = GetFileInfo(mod);

	return (fileInfo) ? LOWORD(fileInfo->dwProductVersionMS) : -1;
}

int OS::Module::Version::GetTweak(void* mod)
{
	const VS_FIXEDFILEINFO* fileInfo = GetFileInfo(mod);

	return (fileInfo) ? HIWORD(fileInfo->dwProductVersionLS) : -1;
}

int OS::Module::Version::GetPatch(void* mod)
{
	const VS_FIXEDFILEINFO* fileInfo = GetFileInfo(mod);

	return (fileInfo) ? LOWORD(fileInfo->dwProductVersionLS) : -1;
}

//////////////////
// Dialog boxes //
//////////////////

void OS::ErrorBox(const char* message, const char* title)
{
	MessageBoxA(NULL, message, title, MB_OK | MB_ICONERROR);
}

///////////
// Hacks //
///////////

bool OS::Hack::FillNop(void* address, std::size_t size)
{
	DWORD oldProtection;
	if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		return false;
	}

	// 0x90 is the opcode of NOP instruction on both x86 and x86-64
	memset(address, '\x90', size);

	if (!VirtualProtect(address, size, oldProtection, &oldProtection))
	{
		return false;
	}

	return true;
}

bool OS::Hack::FillMem(void* address, const void* data, std::size_t dataSize)
{
	DWORD oldProtection;
	if (!VirtualProtect(address, dataSize, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		return false;
	}

	memcpy(address, data, dataSize);

	if (!VirtualProtect(address, dataSize, oldProtection, &oldProtection))
	{
		return false;
	}

	return true;
}

/////////////
// Threads //
/////////////

unsigned long OS::GetCurrentThreadID()
{
	return GetCurrentThreadId();
}

///////////
// Files //
///////////

void OS::ReleaseHandle(void* handle)
{
	CloseHandle(static_cast<HANDLE>(handle));
}

static DWORD ToNativeFileAccessMode(OS::File::Access access)
{
	switch (access)
	{
		case OS::File::READ_ONLY:
		{
			return GENERIC_READ;
		}
		case OS::File::WRITE_ONLY:
		case OS::File::WRITE_ONLY_CREATE:
		{
			return GENERIC_WRITE;
		}
		case OS::File::READ_WRITE:
		case OS::File::READ_WRITE_CREATE:
		{
			return GENERIC_READ | GENERIC_WRITE;
		}
	}

	return 0;
}

static DWORD ToNativeFileCreationDisposition(OS::File::Access access)
{
	switch (access)
	{
		case OS::File::READ_ONLY:
		case OS::File::WRITE_ONLY:
		case OS::File::READ_WRITE:
		{
			return OPEN_EXISTING;
		}
		case OS::File::WRITE_ONLY_CREATE:
		case OS::File::READ_WRITE_CREATE:
		{
			return OPEN_ALWAYS;
		}
	}

	return 0;
}

static DWORD ToNativeFileSeek(OS::File::SeekBase base)
{
	switch (base)
	{
		case OS::File::BEGIN:   return FILE_BEGIN;
		case OS::File::CURRENT: return FILE_CURRENT;
		case OS::File::END:     return FILE_END;
	}

	return 0;
}

bool OS::File::Open(const char* path, OS::File::Access access, bool* pCreated)
{
	this->Close();

	const DWORD mode = ToNativeFileAccessMode(access);
	const DWORD share = FILE_SHARE_READ;
	const DWORD creation = ToNativeFileCreationDisposition(access);
	const DWORD attributes = FILE_ATTRIBUTE_NORMAL;

	HANDLE file = CreateFileA(path, mode, share, NULL, creation, attributes, NULL);
	if (file == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	if (pCreated)
	{
		*pCreated = (GetLastError() != ERROR_ALREADY_EXISTS);
	}

	this->handle = file;

	return true;
}

std::size_t OS::File::Read(void* buffer, std::size_t bufferSize, bool* pError)
{
	bool isError = false;

	DWORD bytesRead;
	if (!ReadFile(static_cast<HANDLE>(this->handle), buffer, static_cast<DWORD>(bufferSize), &bytesRead, NULL))
	{
		isError = true;
		bytesRead = 0;
	}

	if (pError)
	{
		*pError = isError;
	}

	return bytesRead;
}

std::size_t OS::File::Write(const void* data, std::size_t dataSize, bool* pError)
{
#ifdef BUILD_64BIT
	// WriteFile cannot write more than 4 GiB - 1 at once because DWORD is 32-bit even in 64-bit code
	if (dataSize >= 0xFFFFFFFF)
	{
		SetLastError(ERROR_BUFFER_OVERFLOW);

		if (pError)
		{
			*pError = true;
		}

		return 0;
	}
#endif

	bool isError = false;

	DWORD bytesWritten;
	if (!WriteFile(static_cast<HANDLE>(this->handle), data, static_cast<DWORD>(dataSize), &bytesWritten, NULL))
	{
		isError = true;
		bytesWritten = 0;
	}

	if (pError)
	{
		*pError = isError;
	}

	return bytesWritten;
}

bool OS::File::Seek(OS::File::SeekBase base, __int64 offset, unsigned __int64* pNewPos)
{
	const DWORD seek = ToNativeFileSeek(base);

	LARGE_INTEGER offsetValue;
	offsetValue.QuadPart = offset;

	if (!SetFilePointerEx(static_cast<HANDLE>(this->handle), offsetValue, &offsetValue, seek))
	{
		return false;
	}

	if (pNewPos)
	{
		*pNewPos = static_cast<unsigned __int64>(offsetValue.QuadPart);
	}

	return true;
}

bool OS::File::Resize(unsigned __int64 size)
{
	// the offset is interpreted as an unsigned value
	const __int64 offset = static_cast<__int64>(size);

	return this->Seek(OS::File::BEGIN, offset) && SetEndOfFile(static_cast<HANDLE>(this->handle));
}

bool OS::File::Copy(const char* srcPath, const char* dstPath)
{
	const BOOL failIfExists = FALSE;

	return CopyFile(srcPath, dstPath, failIfExists) == TRUE;
}

bool OS::Directory::Create(const char* path, bool* pCreated)
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

std::size_t OS::GetDocumentsPath(char* buffer, std::size_t bufferSize)
{
	const int id = CSIDL_PERSONAL | CSIDL_FLAG_CREATE;
	const DWORD flags = SHGFP_TYPE_CURRENT;

	char* targetBuffer = buffer;

	char safeBuffer[MAX_PATH + 1];
	if (bufferSize <= MAX_PATH)
	{
		targetBuffer = safeBuffer;
	}

	if (SHGetFolderPathA(NULL, id, NULL, flags, targetBuffer) != S_OK)
	{
		SetLastError(ERROR_PATH_NOT_FOUND);
		return 0;
	}

	const std::size_t resultLength = strlen(targetBuffer);

	if (targetBuffer != buffer)
	{
		if (resultLength >= bufferSize)
		{
			SetLastError(ERROR_BUFFER_OVERFLOW);
			return 0;
		}

		memcpy(buffer, targetBuffer, resultLength + 1);
	}

	return resultLength;
}

//////////
// Time //
//////////

static OS::DateTime FromNativeDateTime(const SYSTEMTIME& dateTime)
{
	OS::DateTime result;
	result.year        = dateTime.wYear;
	result.month       = dateTime.wMonth;
	result.dayOfWeek   = dateTime.wDayOfWeek;
	result.day         = dateTime.wDay;
	result.hour        = dateTime.wHour;
	result.minute      = dateTime.wMinute;
	result.second      = dateTime.wSecond;
	result.millisecond = dateTime.wMilliseconds;

	if (result.dayOfWeek == 0)
	{
		result.dayOfWeek = 7;
	}

	return result;
}

const char* OS::DateTime::GetDayName()
{
	switch (this->dayOfWeek)
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

const char* OS::DateTime::GetMonthName()
{
	switch (this->month)
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

OS::DateTime OS::GetCurrentDateTimeUTC()
{
	SYSTEMTIME dateTime;
	GetSystemTime(&dateTime);

	return FromNativeDateTime(dateTime);
}

OS::DateTime OS::GetCurrentDateTimeLocal()
{
	SYSTEMTIME dateTime;
	GetLocalTime(&dateTime);

	return FromNativeDateTime(dateTime);
}

long OS::GetCurrentTimeZoneBias()
{
	TIME_ZONE_INFORMATION tz;
	switch (GetTimeZoneInformation(&tz))
	{
		case TIME_ZONE_ID_UNKNOWN:  return tz.Bias;
		case TIME_ZONE_ID_STANDARD: return tz.Bias + tz.StandardBias;
		case TIME_ZONE_ID_DAYLIGHT: return tz.Bias + tz.DaylightBias;
		case TIME_ZONE_ID_INVALID:  return 0;
	}

	return 0;
}

////////////////////////
// System information //
////////////////////////

bool OS::IsVistaOrLater()
{
	OSVERSIONINFOW info = {};
	info.dwOSVersionInfoSize = sizeof info;
	GetVersionExW(&info);

	return info.dwMajorVersion >= 6;
}

unsigned int OS::GetLogicalProcessorCount()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);

	return info.dwNumberOfProcessors;
}
