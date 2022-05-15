#include "Library/CrashLogger.h"
#include "Library/WinAPI.h"

#include "../ILauncher.h"
#include "../Patch.h"

#include "GameLauncher.h"
#include "Project.h"

#include <intrin.h>

#pragma intrinsic(__rdtsc)

int maxFps = 0;
unsigned long long nextFrameStart = 0;
double ticksPerNanosecond = 0.001f;

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

void WaitIfNeeded() {
	if (maxFps > 0) {
		unsigned long long current;
		do {
			current = __rdtsc();
		} while (current < nextFrameStart);
		double nanoseconds = 1000000000.0f / (double) maxFps;
		nextFrameStart = current + (long long)(nanoseconds * ticksPerNanosecond);
	}
}

LauncherAPI *LauncherAPI::s_pInstance = NULL;

extern "C"
{
	__declspec(dllexport) ILauncher *GetILauncher()
	{
		return LauncherAPI::Get();
	}
}

GameLauncher::GameLauncher()
{
}

GameLauncher::~GameLauncher()
{
}

int GameLauncher::Run()
{
	LauncherAPI api;
	try
	{
		m_params.hInstance = WinAPI::EXE::Get();
		m_params.logFileName = "Game.log";

		SetParamsCmdLine(WinAPI::CmdLine::Get());

		m_crashSink.SetFileName(m_params.logFileName);
		CrashLogger::SetSink(m_crashSink);

		ticksPerNanosecond = WinAPI::GetTSCTicksPerNanosecond();

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

void GameLauncher::LoadEngine()
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
	m_CryPhysics.Load("CryPhysics.dll", DLL::NO_UNLOAD);

	if (WinAPI::IsVistaOrLater() && !WinAPI::CmdLine::HasArg("-dx9"))
	{
		m_CryRenderD3D10.Load("CryRenderD3D10.dll", DLL::NO_UNLOAD);
	}
}

void GameLauncher::PatchEngine()
{
	if (m_CryGame.IsLoaded())
	{
		void* pCryGame = m_CryGame.GetHandle();

		Patch::CryGame::CanJoinDX10Servers(pCryGame, m_gameBuild);
		Patch::CryGame::EnableDX10Menu(pCryGame, m_gameBuild);

		if (!WinAPI::CmdLine::HasArg("-splash"))
		{
			Patch::CryGame::DisableIntros(pCryGame, m_gameBuild);
		}
	}

	if (m_CryAction.IsLoaded())
	{
		void* pCryAction = m_CryAction.GetHandle();

		Patch::CryAction::AllowDX9ImmersiveMultiplayer(pCryAction, m_gameBuild);
		Patch::CryAction::DisableGameplayStats(pCryAction, m_gameBuild);
		Patch::CryAction::PatchSpamTimesOut(pCryAction, m_gameBuild);
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
		void *pWait = &WaitIfNeeded;

		Patch::CrySystem::RemoveSecuROM(pCrySystem, m_gameBuild);
		Patch::CrySystem::AllowDX9VeryHighSpec(pCrySystem, m_gameBuild);
		Patch::CrySystem::AllowMultipleInstances(pCrySystem, m_gameBuild);

		if (WinAPI::CPU::IsAMD() && !WinAPI::CPU::Has3DNow())
		{
			Patch::CrySystem::Disable3DNow(pCrySystem, m_gameBuild);
		}

		Patch::CrySystem::EnableFPSCap(pCrySystem, m_gameBuild, pWait);
		Patch::CrySystem::UnhandledExceptions(pCrySystem, m_gameBuild);
	}

	if (m_CryRenderD3D10.IsLoaded())
	{
		void* pCryRenderD3D10 = m_CryRenderD3D10.GetHandle();

		Patch::CryRenderD3D10::FixLowRefreshRateBug(pCryRenderD3D10, m_gameBuild);
	}

	if (m_CryPhysics.IsLoaded())
	{
		void* pCryPhysics = m_CryPhysics.GetHandle();

		Patch::CryPhysics::PatchValidatorLogSpam(pCryPhysics, m_gameBuild);
	}
}
