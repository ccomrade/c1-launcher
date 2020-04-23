/**
 * @file
 * @brief Implementation of Crysis launcher.
 */

#include <cstring>

#include "CryCommon/ISystem.h"
#include "CryCommon/IGameStartup.h"

#include "Launcher.h"
#include "CrashLogger.h"
#include "DLL.h"
#include "Util.h"

bool Launcher::InitEngineParams(SSystemInitParams & params, void *hInstance, const char *logFileName)
{
	std::memset(&params, 0, sizeof params);

	params.hInstance = hInstance;
	params.sLogFileName = logFileName;

	const char *cmdLine = Util::GetCmdLine();
	const size_t cmdLineLength = std::strlen(cmdLine);

	if (cmdLineLength >= sizeof params.szSystemCmdLine)
	{
		Util::ErrorBox("Command line is too long!");
		return false;
	}

	std::memcpy(params.szSystemCmdLine, cmdLine, cmdLineLength + 1);

	return true;
}

bool Launcher::Run(const DLL & libCryGame, SSystemInitParams & params)
{
	CrashLogger::Init(params.sLogFileName);

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
	if (!pGameStartup->Init(params))
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
