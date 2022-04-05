#include "Library/CmdLine.h"
#include "Library/CPU.h"
#include "Library/WinAPI.h"

#include "../CrashLogger.h"
#include "../Patch.h"

#include "GameLauncher.h"

#include "Project.h"

GameLauncher::GameLauncher()
{
}

GameLauncher::~GameLauncher()
{
}

int GameLauncher::Run()
{
	try
	{
		m_params.hInstance = WinAPI::Module::GetEXE();
		m_params.logFileName = "Game.log";

		SetParamsCmdLine(WinAPI::GetCmdLine());

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

	if (!CmdLine::HasArg("-dx9") && (CmdLine::HasArg("-dx10") || WinAPI::IsVistaOrLater()))
	{
		m_CryRenderD3D10.Load("CryRenderD3D10.dll", DLL::NO_UNLOAD);
	}
}

void GameLauncher::PatchEngine()
{
	PatchEngine_CryGame();
	PatchEngine_CryAction();
	PatchEngine_CryNetwork();
	PatchEngine_CrySystem();
	PatchEngine_CryRenderD3D10();
}

void GameLauncher::PatchEngine_CryGame()
{
	void* pCryGame = m_CryGame.GetHandle();

	Patch::CryGame::CanJoinDX10Servers(pCryGame, m_gameBuild);
	Patch::CryGame::EnableDX10Menu(pCryGame, m_gameBuild);

	if (!CmdLine::HasArg("-splash"))
	{
		Patch::CryGame::DisableIntros(pCryGame, m_gameBuild);
	}
}

void GameLauncher::PatchEngine_CryAction()
{
	void* pCryAction = m_CryAction.GetHandle();

	Patch::CryAction::AllowDX9ImmersiveMultiplayer(pCryAction, m_gameBuild);
}

void GameLauncher::PatchEngine_CryNetwork()
{
	void* pCryNetwork = m_CryNetwork.GetHandle();

	Patch::CryNetwork::EnablePreordered(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::AllowSameCDKeys(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::FixInternetConnect(pCryNetwork, m_gameBuild);
}

void GameLauncher::PatchEngine_CrySystem()
{
	void* pCrySystem = m_CrySystem.GetHandle();

	Patch::CrySystem::RemoveSecuROM(pCrySystem, m_gameBuild);
	Patch::CrySystem::AllowDX9VeryHighSpec(pCrySystem, m_gameBuild);
	Patch::CrySystem::AllowMultipleInstances(pCrySystem, m_gameBuild);

	if (CPU::IsAMD() && !CPU::Has3DNow())
	{
		Patch::CrySystem::Disable3DNow(pCrySystem, m_gameBuild);
	}

	Patch::CrySystem::UnhandledExceptions(pCrySystem, m_gameBuild);
}

void GameLauncher::PatchEngine_CryRenderD3D10()
{
	if (m_CryRenderD3D10.IsLoaded())
	{
		void* pCryRenderD3D10 = m_CryRenderD3D10.GetHandle();

		Patch::CryRenderD3D10::FixLowRefreshRateBug(pCryRenderD3D10, m_gameBuild);
	}
}
