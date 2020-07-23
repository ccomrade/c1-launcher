#define WIN32_LEAN_AND_MEAN
#include <windows.h>  // required by strsafe.h for some reason
#include <strsafe.h>

#include "Format.h"

std::string Format(const char *format, ...)
{
	std::string result;

	if (format)
	{
		va_list args;
		va_start(args, format);
		result = FormatV(format, args);
		va_end(args);
	}

	return result;
}

std::string FormatV(const char *format, va_list args)
{
	char buffer[2048];

	// std::vsnprintf cannot be used because it's C++11
	StringCbVPrintfA(buffer, sizeof buffer, format, args);

	// buffer is always null-terminated
	return std::string(buffer);
}
