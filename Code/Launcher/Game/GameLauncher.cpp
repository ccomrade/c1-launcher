#include "Library/CrashLogger.h"
#include "Library/OS.h"
#include "Project.h"

#include "../CPUInfo.h"
#include "../LauncherCommon.h"
#include "../MemoryPatch.h"

#include "GameLauncher.h"
#include "LanguageHook.h"

#define LAUNCHER_BANNER "C1-Launcher Game " PROJECT_VERSION_STRING
#define DEFAULT_LOG_FILE_NAME "Game.log"

static std::FILE* OpenLogFile()
{
	return LauncherCommon::OpenLogFile(DEFAULT_LOG_FILE_NAME);
}

static void OnCPUDetect(CPUInfo* info, ISystem* pSystem)
{
	LauncherCommon::OnEarlyEngineInit(pSystem, LAUNCHER_BANNER);

	CPUInfo::Detect(info);
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

	CrashLogger::Enable(&OpenLogFile, LAUNCHER_BANNER);

	this->LoadEngine();
	this->PatchEngine();

	void* pCryGame = m_dlls.pWarheadExe ? m_dlls.pWarheadExe : m_dlls.pCryGame;
	m_pGameStartup = LauncherCommon::StartEngine(pCryGame, m_params);

	return m_pGameStartup->Run(NULL);
}

void GameLauncher::LoadEngine()
{
	m_dlls.pCrySystem = LauncherCommon::LoadDLL("CrySystem.dll");
	m_dlls.gameBuild = LauncherCommon::GetGameBuild(m_dlls.pCrySystem);
	LauncherCommon::VerifyGameBuild(m_dlls.gameBuild);

	if (LauncherCommon::IsCrysisWarhead(m_dlls.gameBuild))
	{
		m_dlls.pWarheadExe = LauncherCommon::LoadCrysisWarheadEXE();
	}
	else
	{
		m_dlls.pCryGame = LauncherCommon::LoadDLL("CryGame.dll");
		m_dlls.pCryAction = LauncherCommon::LoadDLL("CryAction.dll");
	}

	m_dlls.pCryNetwork = LauncherCommon::LoadDLL("CryNetwork.dll");

	if (!m_params.isDedicatedServer && !OS::CmdLine::HasArg("-dedicated"))
	{
		if (LauncherCommon::IsDX10())
		{
			m_dlls.pCryRenderD3D10 = LauncherCommon::LoadDLL("CryRenderD3D10.dll");
		}
		else
		{
			m_dlls.pCryRenderD3D9 = LauncherCommon::LoadDLL("CryRenderD3D9.dll");
		}

#ifdef BUILD_64BIT
		m_dlls.pFMODEx = LauncherCommon::LoadDLL("fmodex64.dll");
#else
		m_dlls.pFMODEx = LauncherCommon::LoadDLL("fmodex.dll");
#endif
	}
}

void GameLauncher::PatchEngine()
{
	const bool patchIntros = !OS::CmdLine::HasArg("-splash");

	if (m_dlls.pWarheadExe)
	{
		MemoryPatch::WarheadEXE::AllowDX9ImmersiveMultiplayer(m_dlls.pWarheadExe, m_dlls.gameBuild);
		MemoryPatch::WarheadEXE::HookCryWarning(m_dlls.pWarheadExe, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
		MemoryPatch::WarheadEXE::HookGameWarning(m_dlls.pWarheadExe, m_dlls.gameBuild,
			&LauncherCommon::OnGameWarning);

		if (patchIntros)
		{
			MemoryPatch::WarheadEXE::DisableIntros(m_dlls.pWarheadExe, m_dlls.gameBuild);
		}

		MemoryPatch::WarheadEXE::FixHInstance(m_dlls.pWarheadExe, m_dlls.gameBuild);
	}

	if (m_dlls.pCryGame)
	{
		MemoryPatch::CryGame::CanJoinDX10Servers(m_dlls.pCryGame, m_dlls.gameBuild);
		MemoryPatch::CryGame::EnableDX10Menu(m_dlls.pCryGame, m_dlls.gameBuild);
		MemoryPatch::CryGame::HookCryWarning(m_dlls.pCryGame, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
		MemoryPatch::CryGame::HookGameWarning(m_dlls.pCryGame, m_dlls.gameBuild,
			&LauncherCommon::OnGameWarning);

		if (patchIntros)
		{
			MemoryPatch::CryGame::DisableIntros(m_dlls.pCryGame, m_dlls.gameBuild);
		}
	}

	if (m_dlls.pCryAction)
	{
		MemoryPatch::CryAction::AllowDX9ImmersiveMultiplayer(m_dlls.pCryAction, m_dlls.gameBuild);
		MemoryPatch::CryAction::HookCryWarning(m_dlls.pCryAction, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
		MemoryPatch::CryAction::HookGameWarning(m_dlls.pCryAction, m_dlls.gameBuild,
			&LauncherCommon::OnGameWarning);
	}

	if (m_dlls.pCryNetwork)
	{
		MemoryPatch::CryNetwork::EnablePreordered(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::AllowSameCDKeys(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixInternetConnect(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixFileCheckCrash(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::HookCryWarning(m_dlls.pCryNetwork, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
	}

	if (m_dlls.pCrySystem)
	{
		MemoryPatch::CrySystem::RemoveSecuROM(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::AllowDX9VeryHighSpec(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::AllowMultipleInstances(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::DisableCrashHandler(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::FixCPUInfoOverflow(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, m_dlls.gameBuild, &OnCPUDetect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, m_dlls.gameBuild, &CrashLogger::OnEngineError);
		MemoryPatch::CrySystem::HookLanguageInit(m_dlls.pCrySystem, m_dlls.gameBuild, &LanguageHook::OnInit);
		MemoryPatch::CrySystem::HookChangeUserPath(m_dlls.pCrySystem, m_dlls.gameBuild,
			&LauncherCommon::OnChangeUserPath);
		MemoryPatch::CrySystem::HookCryWarning(m_dlls.pCrySystem, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
	}

	if (m_dlls.pCryRenderD3D9)
	{
		MemoryPatch::CryRenderD3D9::HookAdapterInfo(m_dlls.pCryRenderD3D9, m_dlls.gameBuild,
			&LauncherCommon::OnD3D9Info);
	}

	if (m_dlls.pCryRenderD3D10)
	{
		MemoryPatch::CryRenderD3D10::FixLowRefreshRateBug(m_dlls.pCryRenderD3D10, m_dlls.gameBuild);
		MemoryPatch::CryRenderD3D10::HookAdapterInfo(m_dlls.pCryRenderD3D10, m_dlls.gameBuild,
			&LauncherCommon::OnD3D10Info);
		MemoryPatch::CryRenderD3D10::HookInitAPI(m_dlls.pCryRenderD3D10, m_dlls.gameBuild,
			&LauncherCommon::OnD3D10Init);
	}

	if (m_dlls.pFMODEx && LauncherCommon::IsFMODExVersionCorrect(m_dlls.pFMODEx, m_dlls.gameBuild))
	{
		MemoryPatch::FMODEx::Fix64BitHeapAddressTruncation(m_dlls.pFMODEx, m_dlls.gameBuild);
	}
}
