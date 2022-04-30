/**
 * @file
 * @brief Headless server launcher.
 */

#include "HeadlessServerLauncher.h"

#include "Project.h"

// declared in CryCommon/CrySystem/ISystem.h
SSystemGlobalEnvironment* gEnv;

// declared in Project.h
const char* const PROJECT_VERSION_DETAILS = PROJECT_MAKE_VERSION_DETAILS("CW-Launcher Headless Server");

int main()
{
	return HeadlessServerLauncher().Run();
}
