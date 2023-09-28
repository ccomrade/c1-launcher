/**
 * @file
 * @brief Headless server launcher.
 */

#include <cstdio>
#include <stdexcept>

#include "Project.h"

#include "HeadlessServerLauncher.h"

////////////////////////////////////////////////////////////////////////////////

const char* const PROJECT_BANNER = "C1-Launcher Headless Server " PROJECT_VERSION_STRING;

////////////////////////////////////////////////////////////////////////////////

int main()
{
	try
	{
		return HeadlessServerLauncher().Run();
	}
	catch (const std::runtime_error& ex)
	{
		std::fprintf(stderr, "%s\n", ex.what());
		std::fflush(stderr);
		return 1;
	}
}
