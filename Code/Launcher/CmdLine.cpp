#include <cstdlib>  // std::atoi
#include <cctype>

#include "CmdLine.h"
#include "System.h"

namespace
{
	const char *GetCmdLineWithoutAppName()
	{
		const char *result = System::GetCmdLine();

		// skip program name
		if (*result == '\"')
		{
			for (result++; *result && *result != '\"'; result++);
		}
		else if (*result == '\'')
		{
			for (result++; *result && *result != '\''; result++);
		}
		else
		{
			for (result++; *result && !std::isspace(*result); result++);
		}

		// skip spaces after program name
		while (std::isspace(*result))
		{
			result++;
		}

		return result;
	}

	const char *GetArgValueBegin(const char *arg)
	{
		const char *result = GetCmdLineWithoutAppName();

		// find the argument
		for (; *result; result++)
		{
			if (std::tolower(*result) == std::tolower(*arg))
			{
				const char *name = arg + 1;  // the first letter already matches

				for (result++; *result && *name; result++, name++)
				{
					if (std::tolower(*result) != std::tolower(*name))
					{
						// skip the rest of argument name
						while (*result && !std::isspace(*result))
						{
							result++;
						}

						break;
					}
				}

				// make sure the argument name really matches
				if (!*name && (!*result || std::isspace(*result)))
				{
					// skip spaces before argument value
					while (std::isspace(*result))
					{
						result++;
					}

					return result;
				}
			}
		}

		return NULL;
	}
}

bool CmdLine::HasArg(const char *arg)
{
	return GetArgValueBegin(arg) != NULL;
}

std::string CmdLine::GetArgValue(const char *arg, const char *defaultValue)
{
	const char *value = GetArgValueBegin(arg);

	if (value && *value)
	{
		const char *end = value;

		if (*value == '\"')
		{
			value++;
			end++;

			while (*end && *end != '\"')
			{
				end++;
			}
		}
		else if (*value == '\'')
		{
			value++;
			end++;

			while (*end && *end != '\'')
			{
				end++;
			}
		}
		else
		{
			while (*end && !std::isspace(*end))
			{
				end++;
			}
		}

		return std::string(value, end-value);
	}

	return (defaultValue) ? std::string(defaultValue) : std::string();
}

int CmdLine::GetArgValueInt(const char *arg, int defaultValue)
{
	const std::string value = GetArgValue(arg);

	return value.empty() ? defaultValue : std::atoi(value.c_str());
}
