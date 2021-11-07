#pragma once

namespace CmdLine
{
	bool HasArg(const char *arg);

	const char *GetArgValue(const char *arg, const char *defaultValue = "");
}
