#include "Library/CrashLogger.h"
#include "Library/WinAPI.h"

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
		m_params.hInstance = WinAPI::EXE::Get();
		m_params.logFileName = "Game.log";

		SetParamsCmdLine(WinAPI::CmdLine::Get());

		m_crashSink.SetFileName(m_params.logFileName);
		CrashLogger::SetSink(m_crashSink);

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

	if (!WinAPI::CmdLine::HasArg("-dx9") && (WinAPI::CmdLine::HasArg("-dx10") || WinAPI::IsVistaOrLater()))
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
	}

	if (m_CryNetwork.IsLoaded())
	{
		void* pCryNetwork = m_CryNetwork.GetHandle();

		Patch::CryNetwork::EnablePreordered(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::AllowSameCDKeys(pCryNetwork, m_gameBuild);
		Patch::CryNetwork::FixInternetConnect(pCryNetwork, m_gameBuild);
	}

	if (m_CrySystem.IsLoaded())
	{
		void* pCrySystem = m_CrySystem.GetHandle();

		Patch::CrySystem::RemoveSecuROM(pCrySystem, m_gameBuild);
		Patch::CrySystem::AllowDX9VeryHighSpec(pCrySystem, m_gameBuild);
		Patch::CrySystem::AllowMultipleInstances(pCrySystem, m_gameBuild);

		if (WinAPI::CPU::IsAMD() && !WinAPI::CPU::Has3DNow())
		{
			Patch::CrySystem::Disable3DNow(pCrySystem, m_gameBuild);
		}

		Patch::CrySystem::UnhandledExceptions(pCrySystem, m_gameBuild);
	}

	if (m_CryRenderD3D10.IsLoaded())
	{
		void* pCryRenderD3D10 = m_CryRenderD3D10.GetHandle();

		Patch::CryRenderD3D10::FixLowRefreshRateBug(pCryRenderD3D10, m_gameBuild);
	}
}
