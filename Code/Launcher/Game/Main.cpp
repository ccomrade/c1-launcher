/**
 * @file
 * @brief Game launcher.
 */

#include <cstdlib>

#include "config.h"
#include "Launcher/ILauncher.h"
#include "Launcher/Launcher.h"
#include "Launcher/DLL.h"
#include "Launcher/CPU.h"
#include "Launcher/Patch.h"
#include "Launcher/Util.h"
#include <intrin.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma intrinsic(__rdtsc)

int maxfps = 0;
unsigned long long nextFrameStart = 0;
double ticksPerNanosecond = 0.001f;

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

void WaitIfNeeded() {
	if (maxfps > 0) {
		unsigned long long current;
		do  {
			_mm_lfence();
			current = __rdtsc();
		} while (current < nextFrameStart);
		double nanoseconds = 1000000000.0f / (double) maxfps;
		nextFrameStart = current + (long long)(nanoseconds * ticksPerNanosecond);
	}
}

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
		void *pWait = &WaitIfNeeded;

		if (!Patch::EnableFPSCap(pCrySystem, gameVersion, pWait))
			return false;

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

class LauncherAPI : public ILauncher
{
	static LauncherAPI *s_pInstance;
public:
	LauncherAPI()
	{
		s_pInstance = this;
	}

	~LauncherAPI()
	{
		s_pInstance = NULL;
	}

	static LauncherAPI *Get()
	{
		return s_pInstance;
	}

	const char *GetName() override
	{
		return "CW-Launcher";
	}

	int GetVersionMajor() override
	{
		return CWLAUNCHER_VERSION_MAJOR;
	}

	int GetVersionMinor() override
	{
		return CWLAUNCHER_VERSION_MINOR;
	}
	int GetFPSCap() {
		return maxfps;
	}
	void SetFPSCap(int fps) {
		maxfps = fps;
	}
};

#define DLL_EXPORT __declspec(dllexport)

LauncherAPI *LauncherAPI::s_pInstance = NULL;

// request discrete graphics card
extern "C"
{
	DLL_EXPORT ILauncher *GetILauncher()
	{
		return LauncherAPI::Get();
	}

	// nVidia
	DLL_EXPORT unsigned long NvOptimusEnablement = 1;

	// AMD
	DLL_EXPORT int AmdPowerXpressRequestHighPerformance = 1;
}

void calculateTSCTicksPerNanosecond() {
	//Calculate TSC frequency
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);

	LARGE_INTEGER tStart;
	LARGE_INTEGER tEnd;

	QueryPerformanceCounter(&tStart);
	unsigned long long start = __rdtsc();

	//Sleep for a bit
	Sleep(500);

	QueryPerformanceCounter(&tEnd);
	unsigned long long end = __rdtsc();

	LONGLONG deltaQPC = tEnd.QuadPart - tStart.QuadPart;
	unsigned long long deltaTSC = end - start;

	//Duration in nanoseconds
	double qpcDuration = (double)deltaQPC * 1000000000.0 / (double)Frequency.QuadPart;

	//Calculate TSC ticks per nanosecond
	ticksPerNanosecond = (double)deltaTSC / qpcDuration;
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LauncherAPI api;

	//Get TSC tickrate
	calculateTSCTicksPerNanosecond();

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
