#include <cstring>

#include "CryCommon/CryGame/IGameStartup.h"
#include "CryCommon/CrySystem/ISystem.h"

#include "Library/OS.h"
#include "Library/PathTools.h"
#include "Library/StringFormat.h"
#include "Library/StringView.h"
#include "Project.h"

#include "LauncherCommon.h"

std::string LauncherCommon::GetMainFolderPath()
{
	char buffer[512];
	const StringView exePath(buffer, OS::EXE::GetPath(buffer, sizeof(buffer)));

	if (exePath.empty() || exePath.length() >= sizeof(buffer))
	{
		return std::string();
	}

	StringView mainFolderPath = PathTools::DirName(exePath);

	const StringView exeFolderName = PathTools::BaseName(mainFolderPath);
	const bool insideBin = exeFolderName.compare_no_case("Bin32") == 0
	                    || exeFolderName.compare_no_case("Bin64") == 0;

	if (insideBin)
	{
		// remove Bin32 or Bin64
		mainFolderPath = PathTools::DirName(mainFolderPath);
	}

	return std::string(mainFolderPath.data(), mainFolderPath.length());
}

std::string LauncherCommon::GetRootFolderPath()
{
	const char* rootArg = OS::CmdLine::GetArgValue("-root", NULL);

	return rootArg ? std::string(rootArg) : GetMainFolderPath();
}

std::string LauncherCommon::GetUserFolderPath()
{
	char buffer[512];
	const StringView documentsPath(buffer, OS::GetDocumentsPath(buffer, sizeof(buffer)));

	if (documentsPath.empty() || documentsPath.length() >= sizeof(buffer))
	{
		return std::string();
	}

	// TODO: parse Game/Config/Folders.ini
	const StringView userFolder("My Games" OS_PATH_SLASH "Crysis");

	return PathTools::Join(documentsPath, userFolder);
}

void* LauncherCommon::LoadDLL(const char* name)
{
	void* mod = OS::DLL::Load(name);
	if (!mod)
	{
		const unsigned long errorCode = OS::GetCurrentErrorCode();

		if (errorCode == 193)  // ERROR_BAD_EXE_FORMAT
		{
			throw StringFormat_OSError("Failed to load %s\n\n"
#ifdef BUILD_64BIT
				"It seems you have 32-bit DLLs in Bin64 directory! Fix it!",
#else
				"It seems you have 64-bit DLLs in Bin32 directory! Fix it!",
#endif
				name);
		}
		else
		{
			throw StringFormat_OSError("Failed to load %s", name);
		}
	}

	return mod;
}

int LauncherCommon::GetGameBuild(void* pCrySystem)
{
	int gameBuild = OS::DLL::Version::GetPatch(pCrySystem);
	if (gameBuild < 0)
	{
		throw StringFormat_OSError("Failed to get the game version!");
	}

	return gameBuild;
}

void LauncherCommon::VerifyGameBuild(int gameBuild)
{
	switch (gameBuild)
	{
		case 5767:
		case 5879:
		case 6115:
		case 6156:
		{
			// Crysis
			break;
		}
#ifdef BUILD_64BIT
		// 64-bit binaries are missing in the first build of Crysis Wars
#else
		case 6527:
#endif
		case 6566:
		case 6586:
		case 6627:
		case 6670:
		case 6729:
		{
			// Crysis Wars
			break;
		}
		case 687:
		case 710:
		case 711:
		{
			// Crysis Warhead
			throw StringFormat_Error("Crysis Warhead is not supported!");
		}
		default:
		{
			throw StringFormat_Error("Unknown game build %d", gameBuild);
		}
	}
}

void LauncherCommon::SetParamsCmdLine(SSystemInitParams& params, const char* cmdLine)
{
	const std::size_t length = std::strlen(cmdLine);

	if (length >= sizeof(params.cmdLine))
	{
		throw StringFormat_Error("Command line is too long!");
	}

	std::memcpy(params.cmdLine, cmdLine, length + 1);
}

IGameStartup* LauncherCommon::StartEngine(void* pCryGame, SSystemInitParams& params)
{
	void* entry = OS::DLL::FindSymbol(pCryGame, "CreateGameStartup");
	if (!entry)
	{
		throw StringFormat_Error("The CryGame DLL is not valid!");
	}

	IGameStartup* pGameStartup = static_cast<IGameStartup::TEntryFunction>(entry)();
	if (!pGameStartup)
	{
		throw StringFormat_Error("Failed to create the GameStartup Interface!");
	}

	if (!pGameStartup->Init(params))
	{
		throw StringFormat_Error("Game initialization failed!");
	}

	return pGameStartup;
}

void LauncherCommon::OnEarlyEngineInit(ISystem* pSystem)
{
	gEnv = pSystem->GetGlobalEnvironment();

	CryLogAlways("%s", PROJECT_BANNER);
}

std::FILE* LauncherCommon::OpenLogFile(const char* defaultFileName)
{
	const StringView fileName = OS::CmdLine::GetArgValue("-logfile", defaultFileName);

	// try root folder first
	std::FILE* file = std::fopen(PathTools::Join(GetRootFolderPath(), fileName).c_str(), "a");

	if (!file)
	{
		// try user folder instead if root folder is not writable
		file = std::fopen(PathTools::Join(GetUserFolderPath(), fileName).c_str(), "a");
	}

	return file;
}
