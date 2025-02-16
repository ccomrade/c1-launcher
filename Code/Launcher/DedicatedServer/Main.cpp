/**
 * @file
 * @brief Dedicated server launcher.
 */

#include <stdexcept>

#include "Library/OS.h"

#include "DedicatedServerLauncher.h"

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
