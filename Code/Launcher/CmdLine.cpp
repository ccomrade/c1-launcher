#include <cstdlib>  // std::atoi
#include <cctype>

#include "CmdLine.h"
#include "Util.h"

static const char *GetCmdLineWithoutAppName()
{
	const char *cmdLine = Util::GetCmdLine();

	// skip program name
	if (*cmdLine == '\"')
	{
		// "C:\test with spaces.exe"
		for (cmdLine++; *cmdLine && *cmdLine != '\"'; cmdLine++);
	}
	else if (*cmdLine == '\'')
	{
		// 'C:\test with spaces.exe'
		for (cmdLine++; *cmdLine && *cmdLine != '\''; cmdLine++);
	}
	else
	{
		// C:\test_no_spaces.exe
		for (cmdLine++; *cmdLine && !std::isspace(*cmdLine); cmdLine++);
	}

	while (std::isspace(*cmdLine))
	{
		cmdLine++;
	}

	return cmdLine;
}

static const char *GetArgValueBegin(const char *arg)
{
	const char *cmdLine = GetCmdLineWithoutAppName();

	// find the argument
	for (; *cmdLine; cmdLine++)
	{
		if (std::tolower(*cmdLine) == std::tolower(*arg))
		{
			const char *tmp = arg + 1;
			for (cmdLine++; *cmdLine && *tmp; cmdLine++, tmp++)
			{
				if (std::tolower(*cmdLine) != std::tolower(*tmp))
				{
					break;
				}
			}

			if (!*tmp)
			{
				// reached end of argument name
				if (!*cmdLine || std::isspace(*cmdLine))
				{
					return cmdLine;
				}
			}
		}
	}

	return NULL;
}

bool CmdLine::HasArg(const char *arg)
{
	return GetArgValueBegin(arg) != NULL;
}

std::string CmdLine::GetArgValue(const char *arg, const char *defaultValue)
{
	const char *valueBegin = GetArgValueBegin(arg);
	if (valueBegin)
	{
		while (std::isspace(*valueBegin))
		{
			valueBegin++;
		}

		if (*valueBegin)
		{
			size_t i = 0;

			if (*valueBegin == '\"')
			{
				valueBegin++;

				while (valueBegin[i] && valueBegin[i] != '\"')
				{
					i++;
				}
			}
			else if (*valueBegin == '\'')
			{
				valueBegin++;

				while (valueBegin[i] && valueBegin[i] != '\'')
				{
					i++;
				}
			}
			else
			{
				while (valueBegin[i] && !std::isspace(valueBegin[i]))
				{
					i++;
				}
			}

			return std::string(valueBegin, i);
		}
	}

	return (defaultValue) ? std::string(defaultValue) : std::string();
}

int CmdLine::GetArgValueInt(const char *arg, int defaultValue)
{
	std::string value = GetArgValue(arg);
	return (value.empty()) ? defaultValue : std::atoi(value.c_str());
}
