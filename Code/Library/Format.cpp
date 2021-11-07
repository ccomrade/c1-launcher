#include <stdio.h>  // _vsnprintf

#include "Format.h"

std::string Format(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	std::string result = FormatV(format, args);
	va_end(args);

	return result;
}

std::string FormatV(const char *format, va_list args)
{
	std::string result;

	if (format)
	{
		char buffer[2048];
		// snprintf and vsnprintf are not available before C++11, so we have to use Windows-specific stuff
		// beware of non-standard size prefixes in format string
		// for example, %zu and %llu are not supported and %Iu and %I64u must be used instead
		_vsnprintf(buffer, sizeof buffer, format, args);

		result = buffer;
	}

	return result;
}
