#include "Library/CrashLogger.h"
#include "Library/OS.h"

#include "../CPUInfo.h"
#include "../LauncherCommon.h"
#include "../MemoryPatch.h"

#include "GameLauncher.h"
#include "LanguageHook.h"

#define DEFAULT_LOG_FILE_NAME "Game.log"

static std::FILE* OpenLogFile()
{
	return LauncherCommon::OpenLogFile(DEFAULT_LOG_FILE_NAME);
}

static void LogBytes(const char* message, std::size_t bytes)
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

static void OnD3D9Info(MemoryPatch::CryRenderD3D9::AdapterInfo* info)
{
	CryLogAlways("D3D9 Adapter: %s", info->description);
	CryLogAlways("D3D9 Adapter: PCI %04x:%04x (rev %02x)", info->vendor_id, info->device_id, info->revision);

	// no memory info available
}

static void OnD3D10Info(MemoryPatch::CryRenderD3D10::AdapterInfo* info)
{
	CryLogAlways("D3D10 Adapter: %ls", info->description);
	CryLogAlways("D3D10 Adapter: PCI %04x:%04x (rev %02x)", info->vendor_id, info->device_id, info->revision);

	LogBytes("D3D10 Adapter: Dedicated video memory = ", info->dedicated_video_memory);
	LogBytes("D3D10 Adapter: Dedicated system memory = ", info->dedicated_system_memory);
	LogBytes("D3D10 Adapter: Shared system memory = ", info->shared_system_memory);
}

GameLauncher::GameLauncher() : m_pGameStartup(NULL), m_params(), m_dlls()
{
}

GameLauncher::~GameLauncher()
{
	if (m_pGameStartup)
	{
		m_pGameStartup->Shutdown();
	}
}

int GameLauncher::Run()
{
	m_params.hInstance = OS::EXE::Get();
	m_params.logFileName = DEFAULT_LOG_FILE_NAME;

	LauncherCommon::SetParamsCmdLine(m_params, OS::CmdLine::Get());

	CrashLogger::Enable(&OpenLogFile);

	this->LoadEngine();
	this->PatchEngine();

	m_pGameStartup = LauncherCommon::StartEngine(m_dlls.pCryGame, m_params);

	return m_pGameStartup->Run(NULL);
}

void GameLauncher::LoadEngine()
{
	m_dlls.pCrySystem = LauncherCommon::LoadDLL("CrySystem.dll");

	m_dlls.gameBuild = LauncherCommon::GetGameBuild(m_dlls.pCrySystem);

	LauncherCommon::VerifyGameBuild(m_dlls.gameBuild);

	m_dlls.pCryGame = LauncherCommon::LoadDLL("CryGame.dll");
	m_dlls.pCryAction = LauncherCommon::LoadDLL("CryAction.dll");
	m_dlls.pCryNetwork = LauncherCommon::LoadDLL("CryNetwork.dll");

	if (!m_params.isDedicatedServer && !OS::CmdLine::HasArg("-dedicated"))
	{
		if (!OS::CmdLine::HasArg("-dx9") && (OS::CmdLine::HasArg("-dx10") || OS::IsVistaOrLater()))
		{
			m_dlls.pCryRenderD3D10 = LauncherCommon::LoadDLL("CryRenderD3D10.dll");
		}
		else
		{
			m_dlls.pCryRenderD3D9 = LauncherCommon::LoadDLL("CryRenderD3D9.dll");
		}
	}
}

void GameLauncher::PatchEngine()
{
	if (m_dlls.pCryGame)
	{
		MemoryPatch::CryGame::CanJoinDX10Servers(m_dlls.pCryGame, m_dlls.gameBuild);
		MemoryPatch::CryGame::EnableDX10Menu(m_dlls.pCryGame, m_dlls.gameBuild);

		if (!OS::CmdLine::HasArg("-splash"))
		{
			MemoryPatch::CryGame::DisableIntros(m_dlls.pCryGame, m_dlls.gameBuild);
		}
	}

	if (m_dlls.pCryAction)
	{
		MemoryPatch::CryAction::AllowDX9ImmersiveMultiplayer(m_dlls.pCryAction, m_dlls.gameBuild);
	}

	if (m_dlls.pCryNetwork)
	{
		MemoryPatch::CryNetwork::EnablePreordered(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::AllowSameCDKeys(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixInternetConnect(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixFileCheckCrash(m_dlls.pCryNetwork, m_dlls.gameBuild);
	}

	if (m_dlls.pCrySystem)
	{
		MemoryPatch::CrySystem::RemoveSecuROM(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::AllowDX9VeryHighSpec(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::AllowMultipleInstances(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::DisableCrashHandler(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, m_dlls.gameBuild, &CPUInfo::Detect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, m_dlls.gameBuild, &CrashLogger::OnEngineError);
		MemoryPatch::CrySystem::HookLanguageInit(m_dlls.pCrySystem, m_dlls.gameBuild, &LanguageHook::OnInit);
	}

	if (m_dlls.pCryRenderD3D9)
	{
		MemoryPatch::CryRenderD3D9::HookAdapterInfo(m_dlls.pCryRenderD3D9, m_dlls.gameBuild, &OnD3D9Info);
	}

	if (m_dlls.pCryRenderD3D10)
	{
		MemoryPatch::CryRenderD3D10::FixLowRefreshRateBug(m_dlls.pCryRenderD3D10, m_dlls.gameBuild);
		MemoryPatch::CryRenderD3D10::HookAdapterInfo(m_dlls.pCryRenderD3D10, m_dlls.gameBuild, &OnD3D10Info);
	}
}
