#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Util.h"

std::wstring Util::ConvertUTF8To16(const char *value, size_t length)
{
	std::wstring result;

	int charCount = MultiByteToWideChar(CP_UTF8, 0, value, length, NULL, 0);
	if (charCount > 0)
	{
		result.resize(charCount);

		MultiByteToWideChar(CP_UTF8, 0, value, length, const_cast<wchar_t*>(result.data()), charCount);
	}

	return result;
}

std::wstring Util::ConvertUTF8To16(const char *value)
{
	std::wstring result;

	int charCount = MultiByteToWideChar(CP_UTF8, 0, value, -1, NULL, 0);
	if (charCount > 0)
	{
		result.resize(charCount - 1);  // the count includes terminating null character in this case

		MultiByteToWideChar(CP_UTF8, 0, value, -1, const_cast<wchar_t*>(result.data()), charCount);
	}

	return result;
}

std::string Util::ConvertUTF16To8(const wchar_t *value, size_t length)
{
	std::string result;

	int charCount = WideCharToMultiByte(CP_UTF8, 0, value, length, NULL, 0, NULL, NULL);
	if (charCount > 0)
	{
		result.resize(charCount);

		WideCharToMultiByte(CP_UTF8, 0, value, length, const_cast<char*>(result.data()), charCount, NULL, NULL);
	}

	return result;
}

std::string Util::ConvertUTF16To8(const wchar_t *value)
{
	std::string result;

	int charCount = WideCharToMultiByte(CP_UTF8, 0, value, -1, NULL, 0, NULL, NULL);
	if (charCount > 0)
	{
		result.resize(charCount - 1);  // the count includes terminating null character in this case

		WideCharToMultiByte(CP_UTF8, 0, value, -1, const_cast<char*>(result.data()), charCount, NULL, NULL);
	}

	return result;
}
