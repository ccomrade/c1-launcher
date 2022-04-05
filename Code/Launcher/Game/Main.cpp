/**
 * @file
 * @brief Game launcher.
 */

#include "GameLauncher.h"

#include "Project.h"

// declared in CryCommon/CrySystem/ISystem.h
SSystemGlobalEnvironment* gEnv;

// declared in Project.h
const char* const PROJECT_VERSION_DETAILS = PROJECT_MAKE_VERSION_DETAILS("C1-Launcher Game");

// request discrete graphics card
// nVidia
extern "C" __declspec(dllexport) unsigned long NvOptimusEnablement = 1;
// AMD
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

int __stdcall WinMain(void* instance, void* prevInstance, char* cmdLine, int cmdShow)
{
	return GameLauncher().Run();
}
