#include <cstdio>

#include "Library/Path.h"
#include "Library/WinAPI.h"

#include "CrashLogFileSink.h"

void CrashLogFileSink::OnCrashData(const std::string& data)
{
	if (!m_filename)
		return;

	const char* rootPath = WinAPI::CmdLine::GetArgValue("-root", "");
	const char* filename = WinAPI::CmdLine::GetArgValue("-logfile", m_filename);

	// TODO: use the user folder instead if this is not writable
	std::FILE* logFile = std::fopen(Path::Join(rootPath, filename).c_str(), "ab");
	if (!logFile)
		return;

	std::fputs(data.c_str(), logFile);

	std::fclose(logFile);
}
