#include <cstdio>

#include "Library/Path.h"
#include "Library/WinAPI.h"

#include "CrashLogFileSink.h"

namespace
{
	std::string GetUserFolderName()
	{
		// TODO: parse Game/Config/Folders.ini
		return "My Games\\Crysis";
	}

	std::string GetPrimaryLogFolder()
	{
		const char* rootArg = WinAPI::CmdLine::GetArgValue("-root", NULL);

		if (rootArg)
		{
			return rootArg;
		}
		else
		{
			const std::string binFolderPath = Path::DirName(WinAPI::EXE::GetPath());

			// the exe is always in Bin32 or Bin64 folder, so go one level up to the main folder
			return Path::Join(binFolderPath, "..");
		}
	}

	std::string GetSecondaryLogFolder()
	{
		return Path::Join(WinAPI::GetDocumentsPath(), GetUserFolderName());
	}

	std::FILE* OpenLogFile(const char* defaultFileName)
	{
		const char* fileName = WinAPI::CmdLine::GetArgValue("-logfile", defaultFileName);

		std::FILE* primaryLogFile = std::fopen(Path::Join(GetPrimaryLogFolder(), fileName).c_str(), "ab");
		if (primaryLogFile)
			return primaryLogFile;

		std::FILE* secondaryLogFile = std::fopen(Path::Join(GetSecondaryLogFolder(), fileName).c_str(), "ab");
		if (secondaryLogFile)
			return secondaryLogFile;

		return NULL;
	}
}

void CrashLogFileSink::OnCrashData(const std::string& data)
{
	if (!m_filename)
		return;

	std::FILE* logFile = OpenLogFile(m_filename);
	if (!logFile)
		return;

	std::fputs(data.c_str(), logFile);

	std::fclose(logFile);
}