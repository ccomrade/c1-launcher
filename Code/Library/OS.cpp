#include <cstdlib>
#include <cstring>

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
	const char* args = GetCommandLineA();

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

std::size_t OS::DLL::GetPath(void* dll, char* buffer, std::size_t bufferSize)
{
	std::size_t length = GetModuleFileNameA(static_cast<HMODULE>(dll), buffer, static_cast<DWORD>(bufferSize));

	if (length >= bufferSize)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		length = 0;
	}

	return length;
}

bool OS::DLL::GetVersion(void* dll, Version& result)
{
	HRSRC resInfo = FindResourceA(static_cast<HMODULE>(dll), MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if (!resInfo)
	{
		return false;
	}

	HGLOBAL resData = LoadResource(static_cast<HMODULE>(dll), resInfo);
	if (!resData)
	{
		return false;
	}

	const void* versionRes = LockResource(resData);
	if (!versionRes)
	{
		return false;
	}

	const void* versionResKey = static_cast<const unsigned char*>(versionRes) + 0x6;
	if (std::memcmp(versionResKey, L"VS_VERSION_INFO", 0x20) != 0)
	{
		SetLastError(ERROR_INVALID_DATA);
		return false;
	}

	const void* versionResValue = static_cast<const unsigned char*>(versionResKey) + 0x20 + 0x2;

	const VS_FIXEDFILEINFO* fileInfo = static_cast<const VS_FIXEDFILEINFO*>(versionResValue);
	if (fileInfo->dwSignature != 0xFEEF04BD)
	{
		SetLastError(ERROR_INVALID_DATA);
		return false;
	}

	result.major = HIWORD(fileInfo->dwProductVersionMS);
	result.minor = LOWORD(fileInfo->dwProductVersionMS);
	result.tweak = HIWORD(fileInfo->dwProductVersionLS);
	result.patch = LOWORD(fileInfo->dwProductVersionLS);

	return true;
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
	std::memset(address, '\x90', size);

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

	std::memcpy(address, data, dataSize);

	if (!VirtualProtect(address, dataSize, oldProtection, &oldProtection))
	{
		return false;
	}

	return true;
}

///////////
// Files //
///////////

std::size_t OS::GetWorkingDirectory(char* buffer, std::size_t bufferSize)
{
	std::size_t length = GetCurrentDirectoryA(static_cast<DWORD>(bufferSize), buffer);

	if (length >= bufferSize)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		length = 0;
	}

	return length;
}

std::size_t OS::GetDocumentsPath(char* buffer, std::size_t bufferSize)
{
	char safeBuffer[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, safeBuffer) != S_OK)
	{
		SetLastError(ERROR_PATH_NOT_FOUND);
		return 0;
	}

	const std::size_t length = std::strlen(safeBuffer);

	if (length >= bufferSize)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		return 0;
	}

	std::memcpy(buffer, safeBuffer, length + 1);

	return length;
}

std::size_t OS::PretiffyPath(const char* path, char* buffer, std::size_t bufferSize)
{
	HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
	if (!kernel32)
	{
		return 0;
	}

	typedef DWORD (__stdcall *TGetFinalPathNameByHandleA)(HANDLE, LPSTR, DWORD, DWORD);

	// WinVista+
	TGetFinalPathNameByHandleA pGetFinalPathNameByHandleA =
		reinterpret_cast<TGetFinalPathNameByHandleA>(GetProcAddress(kernel32, "GetFinalPathNameByHandleA"));
	if (!pGetFinalPathNameByHandleA)
	{
		return 0;
	}

	const DWORD fileAccess = FILE_READ_ATTRIBUTES;
	const DWORD fileShare = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	const DWORD fileFlags = FILE_FLAG_BACKUP_SEMANTICS;  // required to open directories

	HANDLE file = CreateFileA(path, fileAccess, fileShare, NULL, OPEN_EXISTING, fileFlags, NULL);
	if (!file)
	{
		return 0;
	}

	std::size_t length = pGetFinalPathNameByHandleA(file, buffer, static_cast<DWORD>(bufferSize), 0);

	CloseHandle(file);

	if (length >= bufferSize)
	{
		SetLastError(ERROR_INSUFFICIENT_BUFFER);
		length = 0;
	}

	if (length >= 4 && buffer[0] == '\\' && buffer[1] == '\\' && buffer[2] == '?' && buffer[3] == '\\')
	{
		if (length >= 6 && buffer[5] == ':')
		{
			// "\\?\C:\..." -> "C:\..."
			std::memmove(buffer, buffer + 4, (length - 4) + 1);
		}
		else if (length >= 8 && buffer[4] == 'U' && buffer[5] == 'N' && buffer[6] == 'C' && buffer[7] == '\\')
		{
			// "\\?\UNC\..." -> "\\..."
			std::memmove(buffer + 1, buffer + 7, (length - 6) + 1);
		}
	}

	return length;
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
	info.dwOSVersionInfoSize = sizeof(info);
	__pragma(warning(suppress:4996))
	GetVersionExW(&info);

	return info.dwMajorVersion >= 6;
}

unsigned int OS::GetLogicalProcessorCount()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);

	return info.dwNumberOfProcessors;
}
