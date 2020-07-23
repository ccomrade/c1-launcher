/**
 * @file
 * @brief Dedicated server launcher.
 */

#include "Launcher/Launcher.h"
#include "Launcher/CrysisLibs.h"
#include "Launcher/Patch.h"
#include "Launcher/System.h"

int __stdcall WinMain(void *hInstance, void *hPrevInstance, char *lpCmdLine, int nCmdShow)
{
	Launcher launcher;

	try
	{
		// load the required DLLs
		CrysisLibs libs(CrysisLibs::DEDICATED_SERVER);

		// install memory patches
		Patch(libs);

		launcher.setAppInstance(hInstance);
		launcher.setLogFileName("Server.log");
		launcher.setCmdLine(System::GetCmdLine());  // lpCmdLine doesn't contain program name
		launcher.setDedicatedServer(true);

		// run the server
		launcher.run(libs);
	}
	catch (const Error & error)
	{
		System::ErrorBox(error);
		return 1;
	}

	return launcher.getExitCode();
}
