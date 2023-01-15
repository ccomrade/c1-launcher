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

	if (index < 0 || (index + 1) >= __argc)
	{
		return defaultValue;
	}

	const char* value = __argv[index + 1];

	// make sure the value is not another argument
	if (*value == '-' || *value == '+')
	{
		return defaultValue;
	}

	return value;
}

/////////////
// Modules //
/////////////

static __declspec(noinline) const VS_FIXEDFILEINFO* GetFileInfo(void* mod)
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

///////////
// Files //
///////////

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
