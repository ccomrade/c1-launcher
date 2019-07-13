/**
 * @file
 * @brief Implementation of utilities.
 */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <intrin.h>  // __cpuid

// Launcher headers
#include "Util.h"

/**
 * @brief Fills read-only memory region with x86 NOP instruction.
 * @param address Memory region address.
 * @param length Memory region size in bytes.
 * @return 0 if no error occurred, otherwise -1.
 */
int FillNOP( void *address, size_t length )
{
	DWORD oldProtection;

	if ( VirtualProtect( address, length, PAGE_EXECUTE_READWRITE, &oldProtection ) == 0 )
		return -1;

	// 0x90 is opcode of NOP instruction for both x86 and x86_64
	memset( address, '\x90', length );

	if ( VirtualProtect( address, length, oldProtection, &oldProtection ) == 0 )
		return -1;

	return 0;
}

/**
 * @brief Fills read-only memory region with custom data.
 * @param address Memory region address.
 * @param data The data copied to the memory region.
 * @param length Size of the data in bytes.
 * @return 0 if no error occurred, otherwise -1.
 */
int FillMem( void *address, void *data, size_t length )
{
	DWORD oldProtection;

	if ( VirtualProtect( address, length, PAGE_EXECUTE_READWRITE, &oldProtection ) == 0 )
		return -1;

	memcpy( address, data, length );

	if ( VirtualProtect( address, length, oldProtection, &oldProtection ) == 0 )
		return -1;

	return 0;
}

/**
 * @brief Obtains game version from any Crysis DLL.
 * It parses version resource of the specified file.
 * @param lib Handle to any loaded Crysis DLL.
 * @return Game build number or -1 if some error occurred.
 */
int GetCrysisGameVersion( void *lib )
{
	// VERSIONINFO resource always has ID 1
	HRSRC versionResInfo = FindResource( (HMODULE) lib, MAKEINTRESOURCE( 1 ), RT_VERSION );
	if ( versionResInfo == NULL )
		return -1;

	HGLOBAL versionResData = LoadResource( (HMODULE) lib, versionResInfo );
	if ( versionResData == NULL )
		return -1;

	void *versionRes = LockResource( versionResData );  // this function does nothing
	if ( versionRes == NULL )
		return -1;

	if ( memcmp( (PBYTE) versionRes + 0x6, L"VS_VERSION_INFO", 0x20 ) != 0 )
		return -1;

	VS_FIXEDFILEINFO *pFileInfo = (VS_FIXEDFILEINFO*) ((PBYTE) versionRes + 0x6 + 0x20 + 0x2);
	if ( pFileInfo->dwSignature != 0xFEEF04BD )
		return -1;

	return LOWORD( pFileInfo->dwFileVersionLS );
}

/**
 * @brief Checks if AMD processor is being used.
 * @return True if we are running on AMD processor, otherwise false.
 */
bool HasAMDProcessor()
{
	const char *vendorID = "AuthenticAMD";

	int cpuInfo[4];
	__cpuid( cpuInfo, 0x0 );

	const int *id = reinterpret_cast<const int*>( vendorID );

	return cpuInfo[1] == id[0]   // first part is in EBX register
	    && cpuInfo[3] == id[1]   // second part is in EDX register
	    && cpuInfo[2] == id[2];  // third part is in ECX register
}

/**
 * @brief Checks if processor supports 3DNow! instructions.
 * @return True if 3DNow! instruction set is available, otherwise false.
 */
bool Is3DNowSupported()
{
	int cpuInfo[4];
	__cpuid( cpuInfo, 0x80000001 );

	return (cpuInfo[3] & (1 << 31)) != 0;  // check bit 31 in EDX register
}

