#include "OS.h"
#include "PathTools.h"

static bool is_slash(char ch)
{
	return ch == '/' || ch == '\\';
}

StringView PathTools::BaseName(StringView path)
{
	std::size_t length = path.length();

	// remove trailing slashes
	for (; length > 0 && is_slash(path[length - 1]); length--) {}

	// find the beginning of the last path component
	for (; length > 0 && !is_slash(path[length - 1]); length--) {}

	path.remove_prefix(length);

	return path;
}

StringView PathTools::DirName(StringView path)
{
	std::size_t length = path.length();

	// remove trailing slashes
	for (; length > 0 && is_slash(path[length - 1]); length--) {}

	// remove the last path component
	for (; length > 0 && !is_slash(path[length - 1]); length--) {}

	// remove trailing slashes
	for (; length > 0 && is_slash(path[length - 1]); length--) {}

	path.remove_suffix(path.length() - length);

	return path;
}

StringView PathTools::GetFileExtension(StringView path)
{
	path.remove_prefix(RemoveFileExtension(path).length());

	return path;
}

StringView PathTools::RemoveFileExtension(StringView path)
{
	for (std::size_t length = path.length(); length > 0; length--)
	{
		char ch = path[length - 1];

		if (is_slash(ch))
		{
			// no file extension to remove
			break;
		}

		if (ch == '.')
		{
			length--;

			// make sure there is something before the dot
			if (length > 0 && !is_slash(path[length - 1]))
			{
				path.remove_suffix(path.length() - length);
			}

			break;
		}
	}

	return path;
}

std::string PathTools::Join(StringView pathA, StringView pathB)
{
	std::string result;

	if (pathA.empty() || is_slash(pathA.back()) || pathB.empty() || is_slash(pathB.front()))
	{
		result.reserve(pathA.length() + pathB.length());

		result += pathA;
		result += pathB;
	}
	else
	{
		result.reserve(pathA.length() + pathB.length() + 1);

		result += pathA;
		result += OS_PATH_SLASH;
		result += pathB;
	}

	return result;
}
