#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Error.h"
#include "Format.h"

std::string Error::CreateWhat(const std::string & message, int code, const std::string & description)
{
	return Format("%s\nError %d: %s", message.c_str(), code, description.c_str());
}

int SystemError::GetCurrentCode()
{
	return GetLastError();
}

std::string SystemError::GetCodeDescription(int code)
{
	char buffer[512];

	size_t length = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code,
	                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, sizeof buffer, NULL);

	return std::string(buffer, length);
}
