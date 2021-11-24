/**
 * @file
 * @brief Implementation of Crysis launcher.
 */

#include "Library/CPU.h"
#include "Library/CmdLine.h"
#include "Library/Format.h"
#include "Library/WinAPI.h"

#include "ILauncher.h"
#include "Launcher.h"
#include "CrashLogger.h"
#include "Patch.h"

#include "config.h"

#include <intrin.h>

#pragma intrinsic(__rdtsc)

// CryCommon/ISystem.h
SSystemGlobalEnvironment *gEnv;

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
		return CWLAUNCHER_VERSION_MAJOR;
	}

	int GetVersionMinor() override
	{
		return CWLAUNCHER_VERSION_MINOR;
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

void Launcher::Run()
{
	LauncherAPI api;
	SetCmdLine();

	CrashLogger::Init(m_params.logFileName);

	ticksPerNanosecond = WinAPI::GetTSCTicksPerNanosecond();
	LoadEngine();
	PatchEngine();
	StartEngine();

	gEnv = m_params.pSystem->GetGlobalEnvironment();

	CryLogAlways(CWLAUNCHER_VERSION_DESCRIPTION);

	UpdateLoop();

	CryLogAlways("Exit code: %d", m_exitCode);
}

void Launcher::SetCmdLine()
{
	const char *cmdLine = WinAPI::CommandLine();
	const std::size_t length = std::strlen(cmdLine);

	if (length >= sizeof m_params.cmdLine)
	{
		throw std::runtime_error("Command line is too long!");
	}

	std::memcpy(m_params.cmdLine, cmdLine, length + 1);
}

void Launcher::LoadEngine()
{
	m_CrySystem.Load("CrySystem.dll", DLL::NO_UNLOAD);  // unloading Crysis DLLs is not safe

	m_gameBuild = WinAPI::GetCrysisGameBuild(m_CrySystem.GetHandle());
	if (m_gameBuild < 0)
	{
		throw WinAPI::MakeError("Failed to get the game version!");
	}

	switch (m_gameBuild)
	{
		case 5767:
		case 5879:
		case 6115:
		case 6156:
		{
			// Crysis
			break;
		}
		case 6527:
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
			throw std::runtime_error("Crysis Warhead is not supported!");
		}
		default:
		{
			throw std::runtime_error(Format("Unknown game version %d!", m_gameBuild));
		}
	}

	m_CryGame.Load("CryGame.dll", DLL::NO_UNLOAD);
	m_CryNetwork.Load("CryNetwork.dll", DLL::NO_UNLOAD);

	if (!m_params.isDedicatedServer)
	{
		m_CryAction.Load("CryAction.dll", DLL::NO_UNLOAD);
		m_CryRenderD3D10.Load("CryRenderD3D10.dll", DLL::NO_UNLOAD);
	}
}

void Launcher::PatchEngine()
{
	PatchEngine_CryGame();
	PatchEngine_CryAction();
	PatchEngine_CryNetwork();
	PatchEngine_CrySystem();
	PatchEngine_CryRenderD3D10();
}

void Launcher::PatchEngine_CryGame()
{
	void *pCryGame = m_CryGame.GetHandle();
	if (!m_params.isDedicatedServer)
	{
		Patch::CryGame::CanJoinDX10Servers(pCryGame, m_gameBuild);
		Patch::CryGame::EnableDX10Menu(pCryGame, m_gameBuild);

		if (!CmdLine::HasArg("-splash"))
		{
			Patch::CryGame::DisableIntros(pCryGame, m_gameBuild);
		}
	}
	else
	{
		Patch::CryGame::DisableAutoexec(pCryGame, m_gameBuild);
	}
}

void Launcher::PatchEngine_CryAction()
{
	if (!m_params.isDedicatedServer)
	{
		void *pCryAction = m_CryAction.GetHandle();

		Patch::CryAction::AllowDX9ImmersiveMultiplayer(pCryAction, m_gameBuild);
	}
}

void Launcher::PatchEngine_CryNetwork()
{
	void *pCryNetwork = m_CryNetwork.GetHandle();

	Patch::CryNetwork::AllowSameCDKeys(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::EnablePreordered(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::FixInternetConnect(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::PatchGamespy(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::PatchServerProfiler(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::PatchSpamCWaitForEnabled(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::PatchSpamSvRequestStopFire(pCryNetwork, m_gameBuild);
}

void Launcher::PatchEngine_CrySystem()
{
	void *pCrySystem = m_CrySystem.GetHandle();
	void *pWait = &WaitIfNeeded;

	if (!m_params.isDedicatedServer)
	{
		Patch::CrySystem::AllowDX9VeryHighSpec(pCrySystem, m_gameBuild);
		Patch::CrySystem::AllowMultipleInstances(pCrySystem, m_gameBuild);
		Patch::CrySystem::EnableFPSCap(pCrySystem, m_gameBuild, pWait);
		Patch::CrySystem::RemoveSecuROM(pCrySystem, m_gameBuild);
	}

	if (CPU::IsAMD() && !CPU::Has3DNow())
	{
		Patch::CrySystem::Disable3DNow(pCrySystem, m_gameBuild);
	}

	Patch::CrySystem::UnhandledExceptions(pCrySystem, m_gameBuild);
}

void Launcher::PatchEngine_CryRenderD3D10()
{
	if (!m_params.isDedicatedServer)
	{
		void *pCryRenderD3D10 = m_CryRenderD3D10.GetHandle();

		Patch::CryRenderD3D10::FixLowRefreshRateBug(pCryRenderD3D10, m_gameBuild);
	}
}

void Launcher::StartEngine()
{
	IGameStartup::TEntryFunction entry = m_CryGame.GetSymbol<IGameStartup::TEntryFunction>("CreateGameStartup");
	if (!entry)
	{
		throw std::runtime_error("The CryGame DLL is not valid!");
	}

	m_pGameStartup = entry();
	if (!m_pGameStartup)
	{
		throw std::runtime_error("Failed to create the GameStartup Interface!");
	}

	// initialize CryEngine
	if (!m_pGameStartup->Init(m_params))
	{
		throw std::runtime_error("Game initialization failed!");
	}

	//Warping lag fix for Crysis Wars 1.5
	if (m_gameBuild == 6729 && m_params.isDedicatedServer)
	{
#ifdef BUILD_64BIT
		__int64 *pTimer = (__int64 *)(0x380BC0E8); //CryRenderNULL.dll+0xBC0E8
		long long int* pValue = (long long int *)(*pTimer + 0x20);
#else
		__int32 *pTimer = (__int32 *)(0x380C8924); //CryRenderNULL.dll+0xC8924
		long long int* pValue = (long long int *)(*pTimer + 0x18);
#endif
		*pValue = 3000000000000;
	}
}

void Launcher::UpdateLoop()
{
	m_exitCode = m_pGameStartup->Run(NULL);
}

void Launcher::ShutdownEngine()
{
	m_pGameStartup->Shutdown();
}
