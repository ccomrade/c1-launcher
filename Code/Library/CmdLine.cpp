#include <stdlib.h>  // __argc, __argv
#include <string.h>  // _stricmp

#include "CmdLine.h"

namespace
{
	int FindArgIndex(const char *arg)
	{
		for (int i = 1; i < __argc; i++)
		{
			if (_stricmp(__argv[i], arg) == 0)
			{
				return i;
			}
		}

		return -1;
	}
}

bool CmdLine::HasArg(const char *arg)
{
	return FindArgIndex(arg) > 0;
}

const char *CmdLine::GetArgValue(const char *arg, const char *defaultValue)
{
	const int index = FindArgIndex(arg);

	if (index > 0 && (index + 1) < __argc)
	{
		const char *value = __argv[index + 1];

		// make sure the value is not another argument
		if (value[0] != '-' && value[0] != '+')
		{
			return value;
		}
	}

	return defaultValue;
}
