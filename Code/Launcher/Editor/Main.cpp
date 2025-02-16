/**
 * @file
 * @brief Editor launcher.
 */

#include <stdexcept>

#include "Library/OS.h"

#include "EditorLauncher.h"

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
		return EditorLauncher().Run(cmdLine);
	}
	catch (const std::runtime_error& ex)
	{
		OS::ErrorBox(ex.what());
		return 1;
	}
}
