#pragma once

#include "StringView.h"

namespace PathTools
{
	StringView BaseName(StringView path);
	StringView DirName(StringView path);

	StringView GetFileExtension(StringView path);
	StringView RemoveFileExtension(StringView path);

	std::string Join(StringView pathA, StringView pathB);
}
