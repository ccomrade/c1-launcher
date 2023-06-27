/**
 * @file
 * @brief Dedicated server launcher.
 */

#include <stdexcept>

#include "Library/OS.h"
#include "Project.h"

#include "DedicatedServerLauncher.h"

////////////////////////////////////////////////////////////////////////////////

const char* const PROJECT_BANNER = "C1-Launcher Dedicated Server " PROJECT_VERSION_STRING;

////////////////////////////////////////////////////////////////////////////////

int __stdcall WinMain(void* instance, void* prevInstance, char* cmdLine, int cmdShow)
{
	try
	{
		return DedicatedServerLauncher().Run();
	}
	catch (const std::runtime_error& ex)
	{
		OS::ErrorBox(ex.what());
		return 1;
	}
}
