#include "Library/WinAPI.h"

#include "../CrashLogger.h"
#include "../Patch.h"

#include "DedicatedServerLauncher.h"

#include "Project.h"

DedicatedServerLauncher::DedicatedServerLauncher()
{
}

DedicatedServerLauncher::~DedicatedServerLauncher()
{
}

int DedicatedServerLauncher::Run()
{
	try
	{
		m_params.hInstance = WinAPI::Module::GetEXE();
		m_params.logFileName = "Server.log";
		m_params.isDedicatedServer = true;

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
	m_CryNetwork.Load("CryNetwork.dll", DLL::NO_UNLOAD);
}

void DedicatedServerLauncher::PatchEngine()
{
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

		if (WinAPI::CPU::IsAMD() && !WinAPI::CPU::Has3DNow())
		{
			Patch::CrySystem::Disable3DNow(pCrySystem, m_gameBuild);
		}

		Patch::CrySystem::UnhandledExceptions(pCrySystem, m_gameBuild);
	}
}
