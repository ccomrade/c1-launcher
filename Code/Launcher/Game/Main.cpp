/**
 * @file
 * @brief Game launcher.
 */

#include <stdexcept>

#include "Library/OS.h"

#include "GameLauncher.h"

extern "C"
{
	// request fast graphics card
	// nVidia
	__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	// AMD
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int __stdcall WinMain(void* instance, void* prevInstance, char* cmdLine, int cmdShow)
{
	try
	{
		return GameLauncher().Run();
	}
	catch (const std::runtime_error& ex)
	{
		OS::ErrorBox(ex.what());
		return 1;
	}
}
