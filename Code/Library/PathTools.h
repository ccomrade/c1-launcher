#pragma once

#include "StringView.h"

namespace PathTools
{
	bool IsAbsolute(StringView path);

	StringView BaseName(StringView path);
	StringView DirName(StringView path);

	StringView GetFileExtension(StringView path);
	StringView RemoveFileExtension(StringView path);

	std::string GetWorkingDirectory();
	std::string GetDocumentsPath();

	std::string Prettify(StringView path);
	std::string MakeAbsolute(StringView path);

	std::string Join(StringView pathA, StringView pathB);
}
