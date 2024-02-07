#include <cstring>

#include "CryCommon/CryGame/IGameStartup.h"
#include "CryCommon/CrySystem/ICryPak.h"
#include "CryCommon/CrySystem/ISystem.h"

#include "Library/EXELoader.h"
#include "Library/OS.h"
#include "Library/PathTools.h"
#include "Library/StringFormat.h"
#include "Library/StringView.h"
#include "Project.h"

#include "CryRender.h"
#include "LauncherCommon.h"

std::string LauncherCommon::GetMainFolderPath()
{
	char exePathBuffer[512];
	const StringView exePath(exePathBuffer, OS::EXE::GetPath(exePathBuffer, sizeof(exePathBuffer)));
	if (exePath.empty())
	{
		return std::string();
	}

	StringView mainFolderPath = PathTools::DirName(exePath);

	const StringView exeFolderName = PathTools::BaseName(mainFolderPath);
	const bool insideBin = exeFolderName.compare_no_case("Bin32") == 0
	                    || exeFolderName.compare_no_case("Bin64") == 0;
	if (insideBin)
	{
		// remove Bin32 or Bin64 from the path
		mainFolderPath = PathTools::DirName(mainFolderPath);
	}

	return std::string(mainFolderPath.data(), mainFolderPath.length());
}

std::string LauncherCommon::GetRootFolderPath()
{
	const char* rootArg = OS::CmdLine::GetArgValue("-root", NULL);

	return rootArg ? std::string(rootArg) : GetMainFolderPath();
}

void* LauncherCommon::LoadDLL(const char* name)
{
	void* dll = OS::DLL::Load(name);
	if (!dll)
	{
		const unsigned long sysError = OS::GetSysError();

		if (sysError == 193)  // ERROR_BAD_EXE_FORMAT
		{
			throw StringFormat_SysError("Failed to load %s\n\n"
#ifdef BUILD_64BIT
				"It seems you have 32-bit DLLs in Bin64 directory! Fix it!",
#else
				"It seems you have 64-bit DLLs in Bin32 directory! Fix it!",
#endif
				name);
		}
		else
		{
			throw StringFormat_SysError("Failed to load %s", name);
		}
	}

	return dll;
}

void* LauncherCommon::LoadEXE(const char* name)
{
	EXELoader loader;
	void* exe = loader.Load(name);
	if (!exe)
	{
		const char* error = loader.GetErrorName();
		const char* value = loader.errorValue ? loader.errorValue : "";
		throw StringFormat_SysError(loader.sysError, "Failed to load %s\n\n%s %s", name, error, value);
	}

	return exe;
}

void* LauncherCommon::LoadCrysisWarheadEXE()
{
	// CryGame and CryAction are integrated into the EXE in Crysis Warhead
#ifdef BUILD_64BIT
	return LoadEXE("Crysis64.exe");
#else
	// TODO: SecuROM unpacker
	//return LoadEXE("Crysis.exe");
	throw StringFormat_Error("32-bit Crysis Warhead is not supported!");
#endif
}

int LauncherCommon::GetGameBuild(void* pCrySystem)
{
	OS::DLL::Version version;
	if (!OS::DLL::GetVersion(pCrySystem, version))
	{
		throw StringFormat_SysError("Failed to get the game version!");
	}

	return version.patch;
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
#ifdef BUILD_64BIT
		// 64-bit binaries are missing in the first build of Crysis Warhead
#else
		case 687:
#endif
		case 710:
		case 711:
		{
			// Crysis Warhead
			break;
		}
		default:
		{
			throw StringFormat_Error("Unknown game build %d", gameBuild);
		}
	}
}

bool LauncherCommon::IsCrysisWarhead(int gameBuild)
{
	switch (gameBuild)
	{
		case 687:
		case 710:
		case 711:
		{
			// Crysis Warhead
			return true;
		}
	}

	return false;
}

bool LauncherCommon::IsDX10()
{
	return !OS::CmdLine::HasArg("-dx9") && (OS::CmdLine::HasArg("-dx10") || OS::IsVistaOrLater());
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

static void SetUserDir(const char* path)
{
	ICryPak* pCryPak = gEnv->pCryPak;

	pCryPak->MakeDir(path);
	pCryPak->SetAlias("%USER%", path, true);
}

static void SetUserDirInMyGames(const char* name)
{
	std::string path = PathTools::GetDocumentsPath();
	path += OS_PATH_SLASH;
	path += "My Games";
	path += OS_PATH_SLASH;
	path += name;

	SetUserDir(path.c_str());
}

static bool HandleUserPathArg()
{
	const char* userPath = OS::CmdLine::GetArgValue("-userpath", NULL);
	if (!userPath)
	{
		return false;
	}

	SetUserDir(PathTools::MakeAbsolute(userPath).c_str());

	return true;
}

static bool HandleUserDirNameArg()
{
	const char* userDirName = OS::CmdLine::GetArgValue("-userdirname", NULL);
	if (!userDirName)
	{
		return false;
	}

	SetUserDirInMyGames(userDirName);

	return true;
}

static bool HandleModUserDirName()
{
	const char* mod = OS::CmdLine::GetArgValue("-mod", NULL);
	if (!mod)
	{
		return false;
	}

	// absolute path is not needed because main directory is set as working directory at this point
	std::string modDLLPath = "Mods";
	modDLLPath += OS_PATH_SLASH;
	modDLLPath += mod;
	modDLLPath += OS_PATH_SLASH;
#ifdef BUILD_64BIT
	modDLLPath += "Bin64";
#else
	modDLLPath += "Bin32";
#endif
	modDLLPath += OS_PATH_SLASH;
	modDLLPath += mod;
	modDLLPath += ".dll";

	void* modDLL = OS::DLL::Load(modDLLPath.c_str());
	if (!modDLL)
	{
		// the mod has no DLL or it failed to load
		// continue engine startup and let CryGame's mod loader to deal with it
		return false;
	}

	typedef const char* (*TGetUserDirName)();

	TGetUserDirName pGetUserDirName = static_cast<TGetUserDirName>(OS::DLL::FindSymbol(modDLL, "GetUserDirName"));
	if (!pGetUserDirName)
	{
		// the mod is not requesting a custom user directory
		return false;
	}

	const char* modUserDirName = pGetUserDirName();
	if (!modUserDirName || !*modUserDirName)
	{
		// do nothing with empty string
		return false;
	}

	SetUserDirInMyGames(modUserDirName);

	// keep the mod DLL loaded to avoid wasting time by loading it again in CryGame's mod loader
	return true;
}

void LauncherCommon::OnChangeUserPath(ISystem* pSystem, const char* userPath)
{
	gEnv = pSystem->GetGlobalEnvironment();

	if (HandleUserPathArg() || HandleUserDirNameArg() || HandleModUserDirName())
	{
		return;
	}

	SetUserDir(PathTools::Join(PathTools::GetDocumentsPath(), userPath).c_str());
}

void LauncherCommon::OnEarlyEngineInit(ISystem* pSystem)
{
	gEnv = pSystem->GetGlobalEnvironment();

	CryLogAlways("%s", PROJECT_BANNER);

	const std::string mainDir = PathTools::GetWorkingDirectory();
	const std::string rootDir = PathTools::Prettify(pSystem->GetRootFolder());
	const std::string userDir = PathTools::Prettify(gEnv->pCryPak->GetAlias("%USER%"));

	CryLogAlways("Main directory: %s", mainDir.c_str());
	CryLogAlways("Root directory: %s", rootDir.empty() ? mainDir.c_str() : rootDir.c_str());
	CryLogAlways("User directory: %s", userDir.c_str());
}

void LauncherCommon::OnD3D9Info(CryRender_D3D9_AdapterInfo* info)
{
	CryLogAlways("D3D9 Adapter: %s", info->description);
	CryLogAlways("D3D9 Adapter: PCI %04x:%04x (rev %02x)", info->vendor_id, info->device_id, info->revision);
}

void LauncherCommon::OnD3D10Info(CryRender_D3D10_AdapterInfo* info)
{
	CryLogAlways("D3D10 Adapter: %ls", info->description);
	CryLogAlways("D3D10 Adapter: PCI %04x:%04x (rev %02x)", info->vendor_id, info->device_id, info->revision);

	LogBytes("D3D10 Adapter: Dedicated video memory = ", info->dedicated_video_memory);
	LogBytes("D3D10 Adapter: Dedicated system memory = ", info->dedicated_system_memory);
	LogBytes("D3D10 Adapter: Shared system memory = ", info->shared_system_memory);
}

bool LauncherCommon::OnD3D10Init(CryRender_D3D10_SystemAPI* api)
{
	void* d3d10 = OS::DLL::Load("d3d10.dll");
	if (!d3d10)
	{
		return false;
	}

	api->pD3D10 = d3d10;
	api->pD3D10CreateDevice = OS::DLL::FindSymbol(d3d10, "D3D10CreateDevice");

	void* dxgi = OS::DLL::Load("dxgi.dll");
	if (!dxgi)
	{
		return false;
	}

	api->pDXGI = dxgi;
	api->pCreateDXGIFactory = OS::DLL::FindSymbol(dxgi, "CreateDXGIFactory");

	return true;
}

void LauncherCommon::LogBytes(const char* message, std::size_t bytes)
{
	const char* unit = "";
	char units[6][2] = { "K", "M", "G", "T", "P", "E" };

	for (int i = 0; i < 6 && bytes >= 1024; i++)
	{
		unit = units[i];
		bytes /= 1024;
	}

	CryLogAlways("%s%u%s", message, static_cast<unsigned int>(bytes), unit);
}

std::FILE* LauncherCommon::OpenLogFile(const char* defaultFileName)
{
	const StringView fileName = OS::CmdLine::GetArgValue("-logfile", defaultFileName);

	// try root folder first
	std::FILE* file = std::fopen(PathTools::Join(GetRootFolderPath(), fileName).c_str(), "a");

	if (!file && gEnv)
	{
		// try user folder instead if root folder is not writable
		file = std::fopen(PathTools::Join(gEnv->pCryPak->GetAlias("%USER%"), fileName).c_str(), "a");
	}

	return file;
}
