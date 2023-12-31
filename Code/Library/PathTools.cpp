#include "OS.h"
#include "PathTools.h"

static bool is_slash(char ch)
{
	return ch == '/' || ch == '\\';
}

bool PathTools::IsAbsolute(StringView path)
{
	if (path.length() >= 1 && is_slash(path[0]))
	{
		// also handles UNC paths
		return true;
	}

	if (path.length() >= 2 && path[1] == ':')
	{
		const char driveLetter = path[0];

		if ((driveLetter >= 'A' && driveLetter <= 'Z')
		 || (driveLetter >= 'a' && driveLetter <= 'z'))
		{
			return true;
		}
	}

	return false;
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

std::string PathTools::GetWorkingDirectory()
{
	char cwd[512];
	return std::string(cwd, OS::GetWorkingDirectory(cwd, sizeof(cwd)));
}

std::string PathTools::GetDocumentsPath()
{
	char path[512];
	return std::string(path, OS::GetDocumentsPath(path, sizeof(path)));
}

std::string PathTools::Prettify(StringView path)
{
	// make sure it's null-terminated
	std::string prettyPath(path.data(), path.length());

	char buffer[512];
	std::size_t length = OS::PretiffyPath(prettyPath.c_str(), buffer, sizeof(buffer));
	if (length > 0)
	{
		prettyPath.assign(buffer, length);
	}

	return prettyPath;
}

std::string PathTools::MakeAbsolute(StringView path)
{
	if (IsAbsolute(path))
	{
		return std::string(path.data(), path.length());
	}

	std::string result = GetWorkingDirectory();
	result += OS_PATH_SLASH;
	result += path;

	return result;
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
