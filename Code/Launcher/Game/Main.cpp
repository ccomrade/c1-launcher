/**
 * @file
 * @brief Game launcher.
 */

#include <cstdlib>

#include "Launcher/Launcher.h"
#include "Launcher/DLL.h"
#include "Launcher/CPU.h"
#include "Launcher/Patch.h"
#include "Launcher/Util.h"

struct CrysisLibs
{
	DLL CryAction;
	DLL CryGame;
	DLL CryNetwork;
	DLL CrySystem;

	bool load()
	{
		// some Crysis DLLs can cause crash when being unloaded
		// so NO_RELEASE flag is used here to disable explicit unloading of the DLLs

		if (!CryGame.load("CryGame.dll", DLL::NO_RELEASE))
		{
			Util::ErrorBox("Failed to load the CryGame DLL!");
			return false;
		}

		if (!CrySystem.load("CrySystem.dll", DLL::NO_RELEASE))
		{
			Util::ErrorBox("Failed to load the CrySystem DLL!");
			return false;
		}

		if (!CryAction.load("CryAction.dll", DLL::NO_RELEASE))
		{
			Util::ErrorBox("Failed to load the CryAction DLL!");
			return false;
		}

		if (!CryNetwork.load("CryNetwork.dll", DLL::NO_RELEASE))
		{
			Util::ErrorBox("Failed to load the CryNetwork DLL!");
			return false;
		}

		return true;
	}
};

static bool InstallMemoryPatches(const CrysisLibs & libs, int gameVersion)
{
	// CryAction
	{
		void *pCryAction = libs.CryAction.getHandle();

		if (!Patch::AllowDX9ImmersiveMultiplayer(pCryAction, gameVersion))
			return false;
	}

	// CryGame
	{
		void *pCryGame = libs.CryGame.getHandle();

		if (!Patch::DisableIntros(pCryGame, gameVersion))
			return false;

		if (!Patch::CanJoinDX10Servers(pCryGame, gameVersion))
			return false;

		if (!Patch::EnableDX10Menu(pCryGame, gameVersion))
			return false;
	}

	// CryNetwork
	{
		void *pCryNetwork = libs.CryNetwork.getHandle();

		if (!Patch::EnablePreordered(pCryNetwork, gameVersion))
			return false;

		if (!Patch::AllowSameCDKeys(pCryNetwork, gameVersion))  // useful for non-dedicated servers
			return false;

		if (!Patch::PatchGamespy(pCryNetwork, gameVersion))  // g.jedi95.us gamespy
			return false;
	}

	// CrySystem
	{
		void *pCrySystem = libs.CrySystem.getHandle();

		if (!Patch::RemoveSecuROM(pCrySystem, gameVersion))
			return false;

		if (!Patch::AllowDX9VeryHighSpec(pCrySystem, gameVersion))
			return false;

		if (!Patch::AllowMultipleInstances(pCrySystem, gameVersion))
			return false;

		if (!Patch::UnhandledExceptions(pCrySystem, gameVersion))
			return false;

		if (CPU::IsAMD() && !CPU::Has3DNow())
		{
			if (!Patch::Disable3DNow(pCrySystem, gameVersion))
				return false;
		}
	}

	return true;
}

int __stdcall WinMain(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
	CrysisLibs libs;

	if (!libs.load())
	{
		return EXIT_FAILURE;
	}

	const int gameVersion = Util::GetCrysisGameBuild(libs.CrySystem);
	if (!gameVersion)
	{
		Util::ErrorBox("Failed to obtain game version!");
		return EXIT_FAILURE;
	}

	// make sure all versions listed here are also supported in Patch.cpp
	switch (gameVersion)
	{
		// Crysis
		case 5767:
		case 5879:
		case 6115:
		case 6156:
		// Crysis Wars
		case 6527:
		case 6566:
		case 6586:
		case 6627:
		case 6670:
		case 6729:
		{
			if (!InstallMemoryPatches(libs, gameVersion))
			{
				Util::ErrorBox("Failed to apply memory patch!");
				return EXIT_FAILURE;
			}

			break;
		}
		default:
		{
			Util::ErrorBox("Unsupported version of the game!");
			return EXIT_FAILURE;
		}
	}

	Launcher launcher;

	launcher.setAppInstance(hInstance);
	launcher.setLogFileName("Game.log");

	return launcher.run(libs.CryGame) ? EXIT_SUCCESS : EXIT_FAILURE;
}

#define DLL_EXPORT __declspec(dllexport)

// request discrete graphics card
extern "C"
{
	// nVidia
	DLL_EXPORT unsigned long NvOptimusEnablement = 1;

	// AMD
	DLL_EXPORT int AmdPowerXpressRequestHighPerformance = 1;
}
