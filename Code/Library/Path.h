#pragma once

#include <string.h>  // _stricmp
#include <string>

namespace Path
{
	inline bool IsSlash(char ch)
	{
		return ch == '/' || ch == '\\';
	}

	inline std::string Join(const char* pathA, const char* pathB)
	{
		std::string result = pathA;

		if (!result.empty() && !IsSlash(result[result.length()-1]) && !IsSlash(*pathB))
		{
			result += '\\';
		}

		result += pathB;

		return result;
	}

	inline std::string Join(const char* pathA, const std::string& pathB)
	{
		return Join(pathA, pathB.c_str());
	}

	inline std::string Join(const std::string& pathA, const char* pathB)
	{
		return Join(pathA.c_str(), pathB);
	}

	inline std::string Join(const std::string& pathA, const std::string& pathB)
	{
		return Join(pathA.c_str(), pathB.c_str());
	}

	inline std::string BaseName(const std::string& path)
	{
		std::size_t endPos = path.length();

		// ignore trailing slashes
		while (endPos > 0 && IsSlash(path[endPos-1]))
		{
			endPos--;
		}

		std::size_t beginPos = endPos;

		// find the beginning of the last path component
		while (beginPos > 0 && !IsSlash(path[beginPos-1]))
		{
			beginPos--;
		}

		return path.substr(beginPos, endPos - beginPos);
	}

	inline std::string DirName(const std::string& path)
	{
		std::size_t endPos = path.length();

		// ignore trailing slashes
		while (endPos > 0 && IsSlash(path[endPos-1]))
		{
			endPos--;
		}

		// ignore the last path component
		while (endPos > 0 && !IsSlash(path[endPos-1]))
		{
			endPos--;
		}

		// ignore trailing slashes
		while (endPos > 0 && IsSlash(path[endPos-1]))
		{
			endPos--;
		}

		return path.substr(0, endPos);
	}

	inline bool IsNameEqual(const char* a, const char* b)
	{
		return _stricmp(a, b) == 0;
	}

	inline bool IsNameEqual(const char* a, const std::string& b)
	{
		return _stricmp(a, b.c_str()) == 0;
	}

	inline bool IsNameEqual(const std::string& a, const char* b)
	{
		return _stricmp(a.c_str(), b) == 0;
	}

	inline bool IsNameEqual(const std::string& a, const std::string& b)
	{
		return a.length() == b.length() && _stricmp(a.c_str(), b.c_str()) == 0;
	}
}
