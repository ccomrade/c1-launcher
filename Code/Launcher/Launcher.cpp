/**
 * @file
 * @brief Implementation of Crysis launcher.
 */

#include <cstring>

#include "CryCommon/IGameStartup.h"

#include "Launcher.h"
#include "CrysisLibs.h"
#include "CrashLogger.h"
#include "Error.h"

#include "config.h"

// CryCommon/ISystem.h
SSystemGlobalEnvironment *gEnv;

void Launcher::setCmdLine(const char *cmdLine)
{
	const size_t length = std::strlen(cmdLine);

	if (length >= sizeof m_params.cmdLine)
	{
		throw Error("Command line is too long!");
	}

	std::memcpy(m_params.cmdLine, cmdLine, length + 1);
}

void Launcher::run(const CrysisLibs & libs)
{
	CrashLogger::Init(m_params.logFileName);

	IGameStartup::TEntryFunction entry = libs.getCryGame().getSymbol<IGameStartup::TEntryFunction>("CreateGameStartup");
	if (!entry)
	{
		throw Error("The CryGame DLL is not valid!");
	}

	IGameStartup *pGameStartup = entry();
	if (!pGameStartup)
	{
		throw Error("Failed to create the GameStartup Interface!");
	}

	// initialize CryEngine
	if (!pGameStartup->Init(m_params))
	{
		pGameStartup->Shutdown();
		throw Error("Game initialization failed!");
	}

	gEnv = m_params.pSystem->GetGlobalEnvironment();

	CryLogAlways("%s", C1LAUNCHER_VERSION_DESCRIPTION);

	// game update loop
	m_exitCode = pGameStartup->Run(NULL);

	CryLogAlways("Exit code: %d", m_exitCode);

	pGameStartup->Shutdown();
}
