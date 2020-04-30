/**
 * @file
 * @brief Implementation of Crysis launcher.
 */

#include <cstring>

#include "CryCommon/IGameStartup.h"

#include "Launcher.h"
#include "CrashLogger.h"
#include "DLL.h"
#include "Util.h"

#include "config.h"

bool Launcher::initCmdLine()
{
	const char *cmdLine = Util::GetCmdLine();
	const size_t cmdLineLength = std::strlen(cmdLine);

	if (cmdLineLength >= sizeof m_params.cmdLine)
	{
		Util::ErrorBox("Command line is too long!");
		return false;
	}

	std::memcpy(m_params.cmdLine, cmdLine, cmdLineLength + 1);

	return true;
}

bool Launcher::run(const DLL & libCryGame)
{
	CrashLogger::Init(m_params.logFileName);

	if (!initCmdLine())
	{
		return false;
	}

	IGameStartup::TEntryFunction pEntry = libCryGame.getSymbol<IGameStartup::TEntryFunction>("CreateGameStartup");
	if (!pEntry)
	{
		Util::ErrorBox("The CryGame DLL is not valid!");
		return false;
	}

	IGameStartup *pGameStartup = pEntry();
	if (!pGameStartup)
	{
		Util::ErrorBox("Failed to create the GameStartup Interface!");
		return false;
	}

	// initialize CryEngine
	if (!pGameStartup->Init(m_params))
	{
		Util::ErrorBox("Game initialization failed!");
		pGameStartup->Shutdown();
		return false;
	}

	// enter game update loop
	int status = pGameStartup->Run(NULL);

	pGameStartup->Shutdown();

	return status == 0;
}
