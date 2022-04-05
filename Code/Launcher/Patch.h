#pragma once

namespace Patch
{
	namespace CryAction
	{
		void AllowDX9ImmersiveMultiplayer(void *pCryAction, int gameBuild);
		void DisableGameplayStats(void *pCryAction, int gameBuild);
	}

	namespace CryGame
	{
		void DisableIntros(void *pCryGame, int gameBuild);
		void CanJoinDX10Servers(void *pCryGame, int gameBuild);
		void EnableDX10Menu(void *pCryGame, int gameBuild);
	}

	namespace CryNetwork
	{
		void EnablePreordered(void *pCryNetwork, int gameBuild);
		void AllowSameCDKeys(void *pCryNetwork, int gameBuild);
		void FixInternetConnect(void *pCryNetwork, int gameBuild);
		void DisableServerProfile(void *pCryNetwork, int gameBuild);
	}

	namespace CrySystem
	{
		void RemoveSecuROM(void *pCrySystem, int gameBuild);
		void AllowDX9VeryHighSpec(void *pCrySystem, int gameBuild);
		void AllowMultipleInstances(void *pCrySystem, int gameBuild);
		void UnhandledExceptions(void *pCrySystem, int gameBuild);
		void Disable3DNow(void *pCrySystem, int gameBuild);
	}

	namespace CryRenderD3D10
	{
		void FixLowRefreshRateBug(void *pCryRenderD3D10, int gameBuild);
	}

	namespace CryRenderNULL
	{
		void DisableDebugRenderer(void *pCryRenderNULL, int gameBuild);
	}
}
