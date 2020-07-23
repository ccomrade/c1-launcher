/**
 * @file
 * @brief Game launcher.
 */

#include "Launcher/Launcher.h"
#include "Launcher/CrysisLibs.h"
#include "Launcher/Patch.h"
#include "Launcher/System.h"

// request discrete graphics card
extern "C"
{
	// nVidia
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;

	// AMD
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int __stdcall WinMain(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
	Launcher launcher;

	try
	{
		// load the required DLLs
		CrysisLibs libs(CrysisLibs::GAME);

		// install memory patches
		Patch(libs);

		launcher.setAppInstance(hInstance);
		launcher.setLogFileName("Game.log");
		launcher.setCmdLine(System::GetCmdLine());  // lpCmdLine doesn't contain program name

		// run the game
		launcher.run(libs);
	}
	catch (const Error & error)
	{
		System::ErrorBox(error);
		return 1;
	}

	return launcher.getExitCode();
}
