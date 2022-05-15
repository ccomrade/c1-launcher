#pragma once

namespace Patch
{
	namespace CryAction
	{
		void AllowDX9ImmersiveMultiplayer(void *pCryAction, int gameBuild);
		void DisableGameplayStats(void *pCryAction, int gameBuild);
		void PatchSpamTimesOut(void *pCryAction, int gameBuild);
	}

	namespace CryGame
	{
		void CanJoinDX10Servers(void *pCryGame, int gameBuild);
		void DisableAutoexec(void *pCryGame, int gameBuild);
		void DisableIntros(void *pCryGame, int gameBuild);
		void EnableDX10Menu(void *pCryGame, int gameBuild);
	}

	namespace CryNetwork
	{
		void AllowSameCDKeys(void *pCryNetwork, int gameBuild);
		void DisableServerProfile(void *pCryNetwork, int gameBuild);
		void EnablePreordered(void *pCryNetwork, int gameBuild);
		void FixInternetConnect(void *pCryNetwork, int gameBuild);
		void PatchGamespy(void *pCryNetwork, int gameBuild);
		void PatchServerProfiler(void *pCryNetwork, int gameBuild);
		void PatchSpamCWaitForEnabled(void *pCryNetwork, int gameBuild);
		void PatchSpamSvRequestStopFire(void *pCryNetwork, int gameBuild);
	}

	namespace CrySystem
	{
		void AllowDX9VeryHighSpec(void *pCrySystem, int gameBuild);
		void AllowMultipleInstances(void *pCrySystem, int gameBuild);
		void Disable3DNow(void *pCrySystem, int gameBuild);
		void EnableFPSCap(void *pCrySystem, int gameBuild, void *pWait);
		void RemoveSecuROM(void *pCrySystem, int gameBuild);
		void UnhandledExceptions(void *pCrySystem, int gameBuild);
	}

	namespace CryRenderD3D10
	{
		void FixLowRefreshRateBug(void *pCryRenderD3D10, int gameBuild);
	}

	namespace CryRenderNULL
	{
		void DisableDebugRenderer(void *pCryRenderNULL, int gameBuild);
	}

	namespace CryPhysics
	{
		void PatchValidatorLogSpam(void *pCryPhysics, int gameBuild);
	}
}
