/**
 * @file
 * @brief Crysis launcher.
 */

#pragma once

#include "CryCommon/ISystem.h"

struct DLL;

class Launcher
{
	SSystemInitParams m_params;

	bool initCmdLine();
	void logInfo(const char *format, ...);

public:
	Launcher()
	: m_params()
	{
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

	bool run(const DLL & libCryGame);
};
