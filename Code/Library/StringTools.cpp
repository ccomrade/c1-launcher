#include <stdio.h>  // _vsnprintf

#include "OS.h"
#include "StringTools.h"

// va_copy is not available before VS2013
#if defined(_MSC_VER) && _MSC_VER < 1800
#define va_copy(dest, src) ((dest) = (src))
#endif

std::string StringTools::Format(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string result = FormatV(format, args);
	va_end(args);

	return result;
}

std::string StringTools::FormatV(const char* format, va_list args)
{
	std::string result;
	FormatToV(result, format, args);

	return result;
}

std::size_t StringTools::FormatTo(std::string& result, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::size_t length = FormatToV(result, format, args);
	va_end(args);

	return length;
}

std::size_t StringTools::FormatToV(std::string& result, const char* format, va_list args)
{
	va_list argsCopy;
	va_copy(argsCopy, args);

	char buffer[1024];
	std::size_t length = FormatToV(buffer, sizeof buffer, format, args);

	// make sure the resulting string is not truncated
	if (length < sizeof buffer)
	{
		// do not overwrite the existing content
		result.append(buffer, length);
	}
	else
	{
		const std::size_t existingLength = result.length();

		result.resize(existingLength + length);

		// format string again with proper buffer size
		length = FormatToV(const_cast<char*>(result.data() + existingLength), length + 1, format, argsCopy);

		if (length < result.length())
		{
			result.resize(length);
		}
	}

	va_end(argsCopy);

	return length;
}

std::size_t StringTools::FormatTo(char* buffer, std::size_t bufferSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::size_t length = FormatToV(buffer, bufferSize, format, args);
	va_end(args);

	return length;
}

std::size_t StringTools::FormatToV(char* buffer, std::size_t bufferSize, const char* format, va_list args)
{
	if (!buffer || !bufferSize)
	{
		buffer = NULL;
		bufferSize = 0;
	}
	else
	{
		buffer[0] = '\0';
	}

	std::size_t length = 0;

	if (format)
	{
		va_list argsCopy;
		va_copy(argsCopy, args);

		// snprintf and vsnprintf are not available before C++11, so we have to use Windows-specific stuff
		// beware of non-standard size prefixes in format string
		// %zu and %llu are not supported and %Iu and %I64u must be used instead
		int status = _vsnprintf(buffer, bufferSize, format, args);

		if (status < 0)
		{
			// calculate the required buffer size
			status = _vsnprintf(NULL, 0, format, argsCopy);
		}

		va_end(argsCopy);

		if (status > 0)
		{
			length = static_cast<std::size_t>(status);
		}

		if (length >= bufferSize && bufferSize > 0)
		{
			// make sure the result is always null-terminated
			buffer[bufferSize - 1] = '\0';
		}
	}

	return length;
}

std::runtime_error StringTools::Error(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::runtime_error error = ErrorV(format, args);
	va_end(args);

	return error;
}

std::runtime_error StringTools::ErrorV(const char* format, va_list args)
{
	const std::string message = FormatV(format, args);

	return std::runtime_error(message);
}

std::runtime_error StringTools::OSError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::runtime_error error = OSErrorV(format, args);
	va_end(args);

	return error;
}

std::runtime_error StringTools::OSErrorV(const char* format, va_list args)
{
	const unsigned long code = OS::GetCurrentErrorCode();

	std::string message = FormatV(format, args);

	if (code)
	{
		char description[512];
		if (OS::GetErrorDescription(description, sizeof description, code))
		{
			FormatTo(message, "\n\nError %u: %s", code, description);
		}
		else
		{
			FormatTo(message, "\n\nError %u", code);
		}
	}

	return std::runtime_error(message);
}
