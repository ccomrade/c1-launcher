#include "Library/WinAPI.h"

#include "../CrashLogger.h"
#include "../ILauncher.h"
#include "../Patch.h"

#include "DedicatedServerLauncher.h"
#include "Project.h"

int maxFps = 0;

class LauncherAPI : public ILauncher
{
	static LauncherAPI *s_pInstance;
public:
	LauncherAPI()
	{
		s_pInstance = this;
	}

	~LauncherAPI()
	{
		s_pInstance = NULL;
	}

	static LauncherAPI *Get()
	{
		return s_pInstance;
	}

	const char *GetName() override
	{
		return "CW-Launcher";
	}

	int GetVersionMajor() override
	{
		return PROJECT_VERSION;
	}

	int GetVersionMinor() override
	{
		return 0;
	}
	int GetFPSCap() {
		return maxFps;
	}
	void SetFPSCap(int fps) {
		if (fps < 0)
		{
			maxFps = 0;
		}
		else
		{
			maxFps = fps;
		}
	}
};

LauncherAPI *LauncherAPI::s_pInstance = NULL;

extern "C"
{
	__declspec(dllexport) ILauncher *GetILauncher()
	{
		return LauncherAPI::Get();
	}
}

DedicatedServerLauncher::DedicatedServerLauncher()
{
}

DedicatedServerLauncher::~DedicatedServerLauncher()
{
}

int DedicatedServerLauncher::Run()
{
	LauncherAPI api;
	try
	{
		m_params.hInstance = WinAPI::EXE::Get();
		m_params.logFileName = "Server.log";
		m_params.isDedicatedServer = true;

		SetParamsCmdLine(WinAPI::CmdLine::Get());

		CrashLogger::Init(m_params.logFileName);

		LoadEngine();
		PatchEngine();
		StartEngine(m_CryGame);

		gEnv = m_params.pSystem->GetGlobalEnvironment();

		CryLogAlways("%s", PROJECT_VERSION_DETAILS);

		return UpdateLoop();
	}
	catch (const std::runtime_error& error)
	{
		WinAPI::ErrorBox(error.what());
		return 1;
	}
}

void DedicatedServerLauncher::LoadEngine()
{
	m_CrySystem.Load("CrySystem.dll", DLL::NO_UNLOAD);  // unloading Crysis DLLs is not safe

	m_gameBuild = WinAPI::GetCrysisGameBuild(m_CrySystem.GetHandle());
	if (m_gameBuild < 0)
	{
		throw WinAPI::CurrentError("Failed to get the game version!");
	}

	VerifyGameBuild();

	m_CryGame.Load("CryGame.dll", DLL::NO_UNLOAD);
	m_CryAction.Load("CryAction.dll", DLL::NO_UNLOAD);
	m_CryNetwork.Load("CryNetwork.dll", DLL::NO_UNLOAD);
}

void DedicatedServerLauncher::PatchEngine()
{
	if (m_CryGame.IsLoaded())
	{
		void* pCryGame = m_CryGame.GetHandle();

		Patch::CryGame::DisableAutoexec(pCryGame, m_gameBuild);
	}

	if (m_CryAction.IsLoaded())
	{
		void* pCryAction = m_CryAction.GetHandle();

		Patch::CryAction::DisableGameplayStats(pCryAction, m_gameBuild);
	}

	if (m_CryNetwork.IsLoaded())
	{
		void* pCryNetwork = m_CryNetwork.GetHandle();

		Patch::CryNetwork::AllowSameCDKeys(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::EnablePreordered(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::FixInternetConnect(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::PatchGamespy(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::PatchServerProfiler(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::PatchSpamCWaitForEnabled(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::PatchSpamSvRequestStopFire(pCryNetwork, m_gameBuild);
	}

	if (m_CrySystem.IsLoaded())
	{
		void* pCrySystem = m_CrySystem.GetHandle();

		if (WinAPI::CPU::IsAMD() && !WinAPI::CPU::Has3DNow())
		{
			Patch::CrySystem::Disable3DNow(pCrySystem, m_gameBuild);
		}

		Patch::CrySystem::UnhandledExceptions(pCrySystem, m_gameBuild);
	}
}
