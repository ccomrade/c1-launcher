#include "OS.h"
#include "PathTools.h"

static bool IsSlash(char ch)
{
	return ch == '/' || ch == '\\';
}

StringView PathTools::BaseName(StringView path)
{
	// remove trailing slashes
	while (path.IsNotEmpty() && IsSlash(path.Back()))
	{
		path.PopBack();
	}

	std::size_t length = path.length;

	// find the beginning of the last path component
	while (length > 0 && !IsSlash(path[length - 1]))
	{
		length--;
	}

	// keep only the last path component
	path.RemovePrefix(length);

	return path;
}

StringView PathTools::DirName(StringView path)
{
	// remove trailing slashes
	while (path.IsNotEmpty() && IsSlash(path.Back()))
	{
		path.PopBack();
	}

	// remove the last path component
	while (path.IsNotEmpty() && !IsSlash(path.Back()))
	{
		path.PopBack();
	}

	// remove trailing slashes
	while (path.IsNotEmpty() && IsSlash(path.Back()))
	{
		path.PopBack();
	}

	return path;
}

StringView PathTools::GetFileExtension(StringView path)
{
	path.RemovePrefix(RemoveFileExtension(path).length);

	return path;
}

StringView PathTools::RemoveFileExtension(StringView path)
{
	StringView originalPath = path;

	while (path.IsNotEmpty() && !IsSlash(path.Back()))
	{
		if (path.Back() == '.')
		{
			path.PopBack();

			if (path.IsEmpty() || IsSlash(path.Back()))
			{
				// ignore leading dot
				break;
			}

			return path;
		}
		else
		{
			path.PopBack();
		}
	}

	// no file extension to remove
	return originalPath;
}

std::string PathTools::Join(StringView pathA, StringView pathB)
{
	const std::size_t totalLength = pathA.length + pathB.length;

	std::string result;

	if (pathA.IsNotEmpty() && !IsSlash(pathA.Back())
	 && pathB.IsNotEmpty() && !IsSlash(pathB.Front()))
	{
		result.reserve(totalLength + 1);

		result += pathA;
		result += OS_PATH_SLASH;
		result += pathB;
	}
	else
	{
		result.reserve(totalLength);

		result += pathA;
		result += pathB;
	}

	return result;
}
