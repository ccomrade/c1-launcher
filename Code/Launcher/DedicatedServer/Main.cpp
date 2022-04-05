/**
 * @file
 * @brief Dedicated server launcher.
 */

#include "DedicatedServerLauncher.h"

#include "Project.h"

// declared in CryCommon/CrySystem/ISystem.h
SSystemGlobalEnvironment* gEnv;

// declared in Project.h
const char* const PROJECT_VERSION_DETAILS = PROJECT_MAKE_VERSION_DETAILS("C1-Launcher Dedicated Server");

int __stdcall WinMain(void* instance, void* prevInstance, char* cmdLine, int cmdShow)
{
	return DedicatedServerLauncher().Run();
}
