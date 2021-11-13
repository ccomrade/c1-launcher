/**
 * @file
 * @brief Dedicated server launcher.
 */

#include "Launcher/Launcher.h"
#include "Library/WinAPI.h"

int __stdcall WinMain(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
	Launcher launcher;

	try
	{
		launcher.SetAppInstance(hInstance);
		launcher.SetLogFileName("Server.log");
		launcher.SetDedicatedServer(true);

		launcher.Run();
	}
	catch (const std::runtime_error & error)
	{
		WinAPI::ErrorBox(error.what());
		return 1;
	}

	return launcher.GetExitCode();
}
