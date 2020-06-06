/**
 * @file
 * @brief Functions for patching Crysis code.
 */

#pragma once

namespace Patch
{
	// CryAction
	bool AllowDX9ImmersiveMultiplayer(void *pCryAction, int gameVersion);

	// CryGame
	bool DisableIntros(void *pCryGame, int gameVersion);
	bool CanJoinDX10Servers(void *pCryGame, int gameVersion);
	bool EnableDX10Menu(void *pCryGame, int gameVersion);

	// CryNetwork
	bool EnablePreordered(void *pCryNetwork, int gameVersion);
	bool AllowSameCDKeys(void *pCryNetwork, int gameVersion);
	bool PatchGamespy(void* pCryNetwork, int gameVersion);

	// CrySystem
	bool EnableFPSCap(void *pCrySystem, int gameVersion);
	bool RemoveSecuROM(void *pCrySystem, int gameVersion);
	bool AllowDX9VeryHighSpec(void *pCrySystem, int gameVersion);
	bool AllowMultipleInstances(void *pCrySystem, int gameVersion);
	bool UnhandledExceptions(void *pCrySystem, int gameVersion);
	bool Disable3DNow(void *pCrySystem, int gameVersion);
}
