#include "Library/CrashLogger.h"
#include "Library/OS.h"
#include "Project.h"

#include "../CPUInfo.h"
#include "../CryMallocHook.h"
#include "../LauncherCommon.h"
#include "../MemoryPatch.h"

#include "DedicatedServerLauncher.h"

#define LAUNCHER_BANNER "C1-Launcher Dedicated Server " PROJECT_VERSION_STRING
#define DEFAULT_LOG_FILE_NAME "Server.log"

static std::FILE* OpenLogFile()
{
	return LauncherCommon::OpenLogFile(DEFAULT_LOG_FILE_NAME);
}

static void OnCPUDetect(CPUInfo* info, ISystem* pSystem)
{
	LauncherCommon::OnEarlyEngineInit(pSystem, LAUNCHER_BANNER);

	CPUInfo::Detect(info);
}

DedicatedServerLauncher::DedicatedServerLauncher() : m_pGameStartup(NULL), m_params(), m_dlls()
{
}

DedicatedServerLauncher::~DedicatedServerLauncher()
{
	if (m_pGameStartup)
	{
		m_pGameStartup->Shutdown();
	}
}

int DedicatedServerLauncher::Run()
{
	m_params.hInstance = OS::EXE::Get();
	m_params.logFileName = DEFAULT_LOG_FILE_NAME;
	m_params.isDedicatedServer = true;

	LauncherCommon::SetParamsCmdLine(m_params, OS::CmdLine::Get());

	CrashLogger::Enable(&OpenLogFile, LAUNCHER_BANNER);

	this->LoadEngine();
	this->PatchEngine();

	void* pCryGame = m_dlls.pWarheadExe ? m_dlls.pWarheadExe : m_dlls.pCryGame;
	m_pGameStartup = LauncherCommon::StartEngine(pCryGame, m_params);

	return m_pGameStartup->Run(NULL);
}

void DedicatedServerLauncher::LoadEngine()
{
	m_dlls.pCrySystem = LauncherCommon::LoadDLL("CrySystem.dll");
	m_dlls.gameBuild = LauncherCommon::GetGameBuild(m_dlls.pCrySystem);
	LauncherCommon::VerifyGameBuild(m_dlls.gameBuild);

	CryMallocHook::Init(m_dlls.pCrySystem);

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
}

void DedicatedServerLauncher::PatchEngine()
{
	if (m_dlls.pWarheadExe)
	{
		MemoryPatch::WarheadEXE::HookCryWarning(m_dlls.pWarheadExe, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
		MemoryPatch::WarheadEXE::HookGameWarning(m_dlls.pWarheadExe, m_dlls.gameBuild,
			&LauncherCommon::OnGameWarning);
	}

	if (m_dlls.pCryGame)
	{
		MemoryPatch::CryGame::HookCryWarning(m_dlls.pCryGame, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
		MemoryPatch::CryGame::HookGameWarning(m_dlls.pCryGame, m_dlls.gameBuild,
			&LauncherCommon::OnGameWarning);
	}

	if (m_dlls.pCryAction)
	{
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
		MemoryPatch::CrySystem::DisableCrashHandler(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::FixCPUInfoOverflow(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, m_dlls.gameBuild, &OnCPUDetect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, m_dlls.gameBuild, &CrashLogger::OnEngineError);
		MemoryPatch::CrySystem::HookChangeUserPath(m_dlls.pCrySystem, m_dlls.gameBuild,
			&LauncherCommon::OnChangeUserPath);
		MemoryPatch::CrySystem::HookCryWarning(m_dlls.pCrySystem, m_dlls.gameBuild,
			&LauncherCommon::OnCryWarning);
	}
}
