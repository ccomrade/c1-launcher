#pragma once

#include <string>

namespace CmdLine
{
	bool HasArg(const char *arg);

	std::string GetArgValue(const char *arg, const char *defaultValue = NULL);
	int GetArgValueInt(const char *arg, int defaultValue = 0);
}
