#include "Library/CrashLogger.h"
#include "Library/OS.h"

#include "../CPUInfo.h"
#include "../LauncherCommon.h"
#include "../MemoryPatch.h"

#include "DedicatedServerLauncher.h"

#define DEFAULT_LOG_FILE_NAME "Server.log"

static std::FILE* OpenLogFile()
{
	return LauncherCommon::OpenLogFile(DEFAULT_LOG_FILE_NAME);
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

	CrashLogger::Enable(&OpenLogFile);

	this->LoadEngine();
	this->PatchEngine();

	m_pGameStartup = LauncherCommon::StartEngine(m_dlls.isWarhead ? m_dlls.pEXE : m_dlls.pCryGame, m_params);

	return m_pGameStartup->Run(NULL);
}

void DedicatedServerLauncher::LoadEngine()
{
	m_dlls.pCrySystem = LauncherCommon::LoadDLL("CrySystem.dll");

	m_dlls.gameBuild = LauncherCommon::GetGameBuild(m_dlls.pCrySystem);
	m_dlls.isWarhead = LauncherCommon::IsCrysisWarhead(m_dlls.gameBuild);

	LauncherCommon::VerifyGameBuild(m_dlls.gameBuild);

	if (m_dlls.isWarhead)
	{
		m_dlls.pEXE = LauncherCommon::LoadCrysisWarheadEXE();
	}
	else
	{
		m_dlls.pCryGame = LauncherCommon::LoadDLL("CryGame.dll");
	}

	m_dlls.pCryNetwork = LauncherCommon::LoadDLL("CryNetwork.dll");
}

void DedicatedServerLauncher::PatchEngine()
{
	if (m_dlls.pCryNetwork)
	{
		MemoryPatch::CryNetwork::EnablePreordered(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::AllowSameCDKeys(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixInternetConnect(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixFileCheckCrash(m_dlls.pCryNetwork, m_dlls.gameBuild);
	}

	if (m_dlls.pCrySystem)
	{
		MemoryPatch::CrySystem::DisableCrashHandler(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::FixCPUInfoOverflow(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, m_dlls.gameBuild, &CPUInfo::Detect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, m_dlls.gameBuild, &CrashLogger::OnEngineError);
		MemoryPatch::CrySystem::HookChangeUserPath(m_dlls.pCrySystem, m_dlls.gameBuild,
			&LauncherCommon::OnChangeUserPath);
	}
}
