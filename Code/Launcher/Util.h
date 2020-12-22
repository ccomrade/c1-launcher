#pragma once

#include <string>

namespace Util
{
	inline char ToLower(char ch)
	{
		return (ch >= 'A' && ch <= 'Z') ? (ch - 'A') + 'a' : ch;
	}

	inline wchar_t ToLower(wchar_t ch)
	{
		return (ch >= L'A' && ch <= L'Z') ? (ch - L'A') + L'a' : ch;
	}

	inline const char *BaseName(const char *name)
	{
		const char *result = name;

		for (; *name; name++)
		{
			if (*name == '/' || *name == '\\')
			{
				result = name + 1;
			}
		}

		return result;
	}

	inline const wchar_t *BaseName(const wchar_t *name)
	{
		const wchar_t *result = name;

		for (; *name; name++)
		{
			if (*name == L'/' || *name == L'\\')
			{
				result = name + 1;
			}
		}

		return result;
	}

	std::wstring ConvertUTF8To16(const char *value, size_t length);
	std::wstring ConvertUTF8To16(const char *value);

	inline std::wstring ConvertUTF8To16(const std::string & value)
	{
		return ConvertUTF8To16(value.c_str(), value.length());
	}

	std::string ConvertUTF16To8(const wchar_t *value, size_t length);
	std::string ConvertUTF16To8(const wchar_t *value);

	inline std::string ConvertUTF16To8(const std::wstring & value)
	{
		return ConvertUTF16To8(value.c_str(), value.length());
	}
}
