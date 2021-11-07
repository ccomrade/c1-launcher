#include "WinAPI.h"
#include "Format.h"

// this evil header must be isolated as much as possible
#include <windows.h>

//////////////////
// Command line //
//////////////////

const char *WinAPI::CommandLine()
{
	return GetCommandLineA();
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

std::runtime_error WinAPI::MakeError(const char *format, ...)
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

void *WinAPI::DLL_Get(const char *name)
{
	return GetModuleHandleA(name);
}

void *WinAPI::DLL_Load(const char *name)
{
	return LoadLibraryA(name);
}

void *WinAPI::DLL_GetSymbol(void *pDLL, const char *name)
{
	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(pDLL), name));
}

void WinAPI::DLL_Unload(void *pDLL)
{
	FreeLibrary(static_cast<HMODULE>(pDLL));
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
 * @brief Obtains game version from any Crysis DLL.
 * It parses version resource of the specified file.
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
 * @brief Fills read-only memory region with x86 NOP instruction.
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
 * @brief Fills read-only memory region with custom data.
 * The memory region and the data must not overlap.
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
