#include <stdio.h>  // _vsnprintf

#include "OS.h"
#include "StringFormat.h"

// va_copy is not available before VS2013
#if defined(_MSC_VER) && _MSC_VER < 1800
#define va_copy(dest, src) ((dest) = (src))
#endif

static bool FastFormatToV(std::string& result, const char* format, va_list args)
{
	char buffer[512];
	// snprintf and vsnprintf are not available before C++11, so we have to use Windows-specific stuff
	// beware of non-standard size prefixes in format string
	// %zu and %llu are not supported and %Iu and %I64u must be used instead
	int status = _vsnprintf(buffer, sizeof(buffer), format, args);
	if (status < 0)
	{
		return false;
	}

	result.append(buffer, static_cast<std::size_t>(status));

	return true;
}

static void SlowFormatToV(std::string& result, const char* format, va_list args)
{
	va_list argsCopy;
	va_copy(argsCopy, args);

	int status = _vsnprintf(NULL, 0, format, args);
	if (status >= 0)
	{
		const std::size_t length = static_cast<std::size_t>(status);
		const std::size_t offset = result.length();

		result.resize(offset + length);

		// no need for length + 1 because _vsnprintf is happy even without writing null byte at the end
		status = _vsnprintf(const_cast<char*>(result.data() + offset), length, format, argsCopy);
		if (status >= 0)
		{
			result.resize(offset + static_cast<std::size_t>(status));
		}
		else
		{
			result.resize(offset);
		}
	}

	va_end(argsCopy);
}

std::string StringFormat(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string result = StringFormatV(format, args);
	va_end(args);

	return result;
}

std::string StringFormatV(const char* format, va_list args)
{
	std::string result;
	StringFormatToV(result, format, args);

	return result;
}

void StringFormatTo(std::string& result, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	StringFormatToV(result, format, args);
	va_end(args);
}

void StringFormatToV(std::string& result, const char* format, va_list args)
{
	va_list argsCopy;
	va_copy(argsCopy, args);

	if (!FastFormatToV(result, format, args))
	{
		SlowFormatToV(result, format, argsCopy);
	}

	va_end(argsCopy);
}

void StringFormatToBuffer(char* buffer, std::size_t bufferSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	StringFormatToBufferV(buffer, bufferSize, format, args);
	va_end(args);
}

void StringFormatToBufferV(char* buffer, std::size_t bufferSize, const char* format, va_list args)
{
	int status = _vsnprintf(buffer, bufferSize, format, args);

	// make sure the buffer is always null-terminated
	if (bufferSize > 0)
	{
		if (status < 0)
		{
			buffer[0] = '\0';
		}
		else if (static_cast<std::size_t>(status) >= bufferSize)
		{
			buffer[bufferSize - 1] = '\0';
		}
	}
}

std::runtime_error StringFormat_Error(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	const std::string message = StringFormatV(format, args);
	va_end(args);

	return std::runtime_error(message);
}

std::runtime_error StringFormat_SysError(const char* format, ...)
{
	const unsigned long sysError = OS::GetSysError();

	va_list args;
	va_start(args, format);
	std::runtime_error error = StringFormatV_SysError(sysError, format, args);
	va_end(args);

	return error;
}

std::runtime_error StringFormat_SysError(unsigned long sysError, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::runtime_error error = StringFormatV_SysError(sysError, format, args);
	va_end(args);

	return error;
}

std::runtime_error StringFormatV_SysError(unsigned long sysError, const char* format, va_list args)
{
	std::string message = StringFormatV(format, args);

	if (sysError != 0)
	{
		char description[512];
		if (OS::GetSysErrorDescription(description, sizeof(description), sysError))
		{
			StringFormatTo(message, "\n\nError %lu: %s", sysError, description);
		}
		else
		{
			StringFormatTo(message, "\n\nError %lu", sysError);
		}
	}

	return std::runtime_error(message);
}
