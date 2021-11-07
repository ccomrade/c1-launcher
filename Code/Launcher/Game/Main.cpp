/**
 * @file
 * @brief Game launcher.
 */

#include "Launcher/Launcher.h"
#include "Library/WinAPI.h"

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
		launcher.SetAppInstance(hInstance);
		launcher.SetLogFileName("Game.log");

		launcher.Run();
	}
	catch (const std::runtime_error & error)
	{
		WinAPI::ErrorBox(error.what());
		return 1;
	}

	return launcher.GetExitCode();
}
