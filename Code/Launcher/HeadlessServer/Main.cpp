/**
 * @file
 * @brief Headless server launcher.
 */

#include <cstdio>
#include <stdexcept>

#include "HeadlessServerLauncher.h"

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
