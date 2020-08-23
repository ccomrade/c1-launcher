/**
 * @file
 * @brief Implementation of Crysis launcher.
 */

#include <cstring>

#include "CryCommon/IGameStartup.h"
#include "CryCommon/ILog.h"

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

void Launcher::logInfo(const char *format, ...)
{
	// CryEngine must be initialized
	ILog *pLog = m_params.pSystem->GetILog();

	va_list args;
	va_start(args, format);
	pLog->LogV(ILog::eAlways, format, args);
	va_end(args);
}

bool Launcher::run(const DLL & libCryGame)
{
	const int gameVersion = Util::GetCrysisGameBuild(libCryGame);

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

	logInfo(CWLAUNCHER_VERSION_DESCRIPTION);

	//Warping lag fix for Crysis Wars 1.5
	if (gameVersion == 6729 && m_params.isDedicatedServer)
	{
#ifdef BUILD_64BIT
		__int64 *pTimer = (__int64 *)(0x380BC0E8); //CryRenderNULL.dll+0xBC0E8
		long long int* pValue = (long long int *)(*pTimer + 0x20);
#else
		__int32 *pTimer = (__int32 *)(0x380C8924); //CryRenderNULL.dll+0xC8924
		long long int* pValue = (long long int *)(*pTimer + 0x18);
#endif
		*pValue = 3000000000000;
		logInfo("Warping lag fix applied!");
	}

	// enter game update loop
	int status = pGameStartup->Run(NULL);

	pGameStartup->Shutdown();

	return status == 0;
}
