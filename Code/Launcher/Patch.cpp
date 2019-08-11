/**
 * @file
 * @brief Implementation of functions for patching Crysis code.
 */

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// Launcher headers
#include "Patch.h"
#include "Util.h"

/**
 * @brief Allows connecting to DX10 servers with game running in DX9 mode.
 * @param libCryAction CryAction DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchDX9ImmersiveMultiplayer( void *libCryAction, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x2AF92D, 0x1E ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x2B24DD, 0x1A ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x2AF6ED, 0x1E ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x2B239D, 0x1A ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x2B349D, 0x1E ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x2B6361, 0x1A ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x2B394D, 0x1E ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x2B6860, 0x1A ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x1D4ADA, 0x1A ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x1D6B03, 0x15 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x1D4B0A, 0x1A ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x1D6B33, 0x15 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x1D6EDA, 0x1A ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x1D8F32, 0x15 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryAction + 0x1D698A, 0x1A ) < 0
			  || FillNOP( (PBYTE) libCryAction + 0x1D89FC, 0x15 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Disables startup videos.
 * @param libCryGame CryGame DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchSkipIntros( void *libCryGame, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x2EDF9D, 0x10 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x2ED05D, 0x10 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x2F695D, 0x10 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x2F6F4D, 0x10 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x21A91D, 0xD ) < 0
			  || FillNOP( (PBYTE) libCryGame + 0x21A92B, 0x2 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x21ACDD, 0xD ) < 0
			  || FillNOP( (PBYTE) libCryGame + 0x21ACEB, 0x2 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x220CAD, 0xD ) < 0
			  || FillNOP( (PBYTE) libCryGame + 0x220CBB, 0x2 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x220BFD, 0xD ) < 0
			  || FillNOP( (PBYTE) libCryGame + 0x220C0B, 0x2 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Prevents DX10 servers from being grayed-out in server list when game is running in DX9 mode.
 * This patch makes sense only if connecting to DX10 servers with DX9 game is possible (see PatchDX9ImmersiveMultiplayer).
 * @param libCryGame CryGame DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchCanJoinDX10Servers( void *libCryGame, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x327B3C, 0xF ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x32689C, 0xF ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x3343C1, 0x18 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x334791, 0x18 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x23A4BC, 0xA ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x23AB5C, 0xA ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x242CAC, 0xF ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryGame + 0x242F1C, 0xF ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Forces true value for DX10 flag in Flash UI scripts (ActionScript).
 * This patch unlocks DX10 features in "CREATE GAME" menu in DX9 game. It makes sense only if connecting to DX10 servers with
 * DX9 game is possible (see PatchDX9ImmersiveMultiplayer).
 * @param libCryGame CryGame DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchFlashMenuDX10( void *libCryGame, int gameVersion )
{
	// mov al, 0x1
	// nop
	const BYTE code[] = { 0xB0, 0x01, 0x90 };

	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x2ECE24, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x2ED3FE, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x2EBEE4, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x2EC4BE, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x2F5792, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x2F5DBC, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x2F5D7D, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x2F63B7, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x21A00E, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x21A401, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x21A3CE, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x21A7C1, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x22034F, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x220789, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillMem( (PBYTE) libCryGame + 0x22029A, (void*) code, sizeof code ) < 0
			  || FillMem( (PBYTE) libCryGame + 0x2206E2, (void*) code, sizeof code ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Prevents server from kicking players with the same CD key.
 * This is server-side patch.
 * @param libCryNetwork CryNetwork DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchDuplicateCDKey( void *libCryNetwork, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0xE4858, 0x47 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0xE5628, 0x47 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0xE0188, 0x47 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0xE0328, 0x47 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0x608CE, 0x4 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0x5DE79, 0x4 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0x60EF2, 0x4 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCryNetwork + 0x606A5, 0x4 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Disables the SecuROM crap in 64-bit CrySystem.
 * It does nothing in 32-bit build.
 * @param libCrySystem CrySystem DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int Patch64BitSecuROM( void *libCrySystem, int gameVersion )
{
#ifdef BUILD_64BIT
	switch ( gameVersion )
	{
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x4659E, 0x16 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x47B6E, 0x16 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x46FFD, 0x16 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x470B9, 0x16 ) < 0 )
				return -1;
			break;
		}
		default:
		{
			return -1;
		}
	}
#endif

	return 0;
}

/**
 * @brief Allows Very High settings in DX9 mode.
 * @param libCrySystem CrySystem DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchDX9VeryHighSpec( void *libCrySystem, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x45C31, 0x54 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x47201, 0x54 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x46690, 0x54 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x4674C, 0x54 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x59F08, 0x4B ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x5A488, 0x4B ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x5A268, 0x4B ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x59DA8, 0x4B ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Allows running multiple instances of Crysis at once.
 * Note that the first check if any instance is already running is normally done in launcher.
 * @param libCrySystem CrySystem DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchMultipleInstances( void *libCrySystem, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x420DF, 0x68 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x436AF, 0x68 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x42B5F, 0x68 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x42BFF, 0x68 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x57ABF, 0x58 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x5802F, 0x58 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x57E1F, 0x58 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x5794F, 0x58 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Disables custom handling of unhandled exceptions.
 * This patch prevents the engine from enabling its own unhandled exception callback via SetUnhandledExceptionFilter function.
 * The callback is used to do various things after crash, such as calling ReportFault function or creating minidump. It depends
 * on value of sys_WER cvar (0, 1, 2). However, everything done in this callback is completely useless and causes only problems.
 * Even MSDN documentation recommends that applications shouldn't try do such things at their own. Instead, they should let
 * operating system handle fatal errors for them.
 * @param libCrySystem CrySystem DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchUnhandledExceptions( void *libCrySystem, int gameVersion )
{
	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x22986, 0x6  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x22992, 0x7  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x45C8A, 0x16 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x232C6, 0x6  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x232D2, 0x7  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x4725A, 0x16 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x22966, 0x6  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x22972, 0x7  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x466E9, 0x16 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x22946, 0x6  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x22952, 0x7  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x467A5, 0x16 ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x182B7, 0x5  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x182C2, 0xC  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x59F58, 0x13 ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x18437, 0x5  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x18442, 0xC  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x5A4D8, 0x13 ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x18217, 0x5  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x18222, 0xC  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x5A2B8, 0x13 ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillNOP( (PBYTE) libCrySystem + 0x17D67, 0x5  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x17D72, 0xC  ) < 0
			  || FillNOP( (PBYTE) libCrySystem + 0x59DF8, 0x13 ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief Disables use of 3DNow! instructions.
 * This patch fixes the well known crash of 32-bit Crysis on modern AMD processors.
 * @param libCrySystem CrySystem DLL handle.
 * @param gameVersion Game build number.
 * @return 0 if no error occurred, otherwise -1.
 */
int PatchDisable3DNow( void *libCrySystem, int gameVersion )
{
	const BYTE flags = 0x18;  // default feature flags without CPUF_3DNOW flag

	switch ( gameVersion )
	{
	#ifdef BUILD_64BIT
		case 5767:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0xA1AF, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0xA0FF, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0xA0BF, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0xA0FF, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
	#else
		case 5767:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0x9432, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 5879:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0x9472, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6115:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0x9412, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
		case 6156:
		{
			if ( FillMem( (PBYTE) libCrySystem + 0x93E2, (void*) &flags, sizeof flags ) < 0 )
				return -1;
			break;
		}
	#endif
		default:
		{
			return -1;
		}
	}

	return 0;
}

