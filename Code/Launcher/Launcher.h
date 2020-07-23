/**
 * @file
 * @brief Crysis launcher.
 */

#pragma once

#include "CryCommon/ISystem.h"

struct CrysisLibs;

class Launcher
{
	SSystemInitParams m_params;
	int m_exitCode;

public:
	Launcher()
	: m_params(),
	  m_exitCode(0)
	{
	}

	int getExitCode() const
	{
		return m_exitCode;
	}

	void setAppInstance(void *hInstance)
	{
		m_params.hInstance = hInstance;
	}

	void setLogFileName(const char *logFileName)
	{
		m_params.logFileName = logFileName;
	}

	void setDedicatedServer(bool isDedicatedServer)
	{
		m_params.isDedicatedServer = isDedicatedServer;
	}

	void setCmdLine(const char *cmdLine);

	void run(const CrysisLibs & libs);
};
