#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "System.h"
#include "Format.h"
#include "DLL.h"

const char *System::GetCmdLine()
{
	return GetCommandLineA();
}

void System::ErrorBox(const char *message)
{
	MessageBoxA(NULL, message, "Error", MB_OK | MB_ICONERROR);
}

void System::ErrorBox(const Error & error)
{
	MessageBoxA(NULL, error.what(), "Error", MB_OK | MB_ICONERROR);
}

/**
 * @brief Obtains game version from any Crysis DLL.
 * It parses version resource of the specified file.
 * @param dll Any loaded Crysis DLL.
 * @return Game build number or 0 if some error occurred.
 */
unsigned int System::GetCrysisGameBuild(const DLL & dll)
{
	HMODULE handle = static_cast<HMODULE>(dll.getHandle());
	if (!handle)
		return 0;

	// VERSIONINFO resource always has ID 1
	HRSRC versionResInfo = FindResource(handle, MAKEINTRESOURCE(1), RT_VERSION);
	if (!versionResInfo)
		return 0;

	HGLOBAL versionResData = LoadResource(handle, versionResInfo);
	if (!versionResData)
		return 0;

	void *versionRes = LockResource(versionResData);  // this function does nothing
	if (!versionRes)
		return 0;

	if (std::memcmp(RVA(versionRes, 0x6), L"VS_VERSION_INFO", 0x20) != 0)
		return 0;

	VS_FIXEDFILEINFO *pFileInfo = static_cast<VS_FIXEDFILEINFO*>(RVA(versionRes, 0x6 + 0x20 + 0x2));
	if (pFileInfo->dwSignature != 0xFEEF04BD)
		return 0;

	return LOWORD(pFileInfo->dwFileVersionLS);
}

/**
 * @brief Fills read-only memory region with x86 NOP instruction.
 * @param address Address of the memory region.
 * @param length Size of the memory region in bytes.
 */
void System::FillNOP(void *address, size_t length)
{
	DWORD oldProtection;

	if (!VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		throw SystemError(Format("FillNOP: Failed to change memory protection at %p (+%Iu)!", address, length));
	}

	// 0x90 is opcode of NOP instruction on both x86 and x86-64
	std::memset(address, '\x90', length);

	if (!VirtualProtect(address, length, oldProtection, &oldProtection))
	{
		throw SystemError(Format("FillNOP: Failed to restore memory protection at %p (+%Iu)!", address, length));
	}
}

/**
 * @brief Fills read-only memory region with custom data.
 * The memory region and the data must not overlap.
 * @param address Address of the memory region.
 * @param data Address of the data.
 * @param length Size of the data in bytes.
 */
void System::FillMem(void *address, const void *data, size_t length)
{
	DWORD oldProtection;

	if (!VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection))
	{
		throw SystemError(Format("FillMem: Failed to change memory protection at %p (+%Iu)!", address, length));
	}

	std::memcpy(address, data, length);

	if (!VirtualProtect(address, length, oldProtection, &oldProtection))
	{
		throw SystemError(Format("FillMem: Failed to restore memory protection at %p (+%Iu)!", address, length));
	}
}
