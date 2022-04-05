#include "Library/CmdLine.h"
#include "Library/CPU.h"
#include "Library/Path.h"
#include "Library/WinAPI.h"

#include "../CrashLogger.h"
#include "../Patch.h"

#include "HeadlessServerLauncher.h"

#include "Project.h"

HeadlessServerLauncher::HeadlessServerLauncher() : m_rootFolder(GetRootFolder()), m_log(m_executor, m_rootFolder)
{
}

HeadlessServerLauncher::~HeadlessServerLauncher()
{
}

int HeadlessServerLauncher::Run()
{
	try
	{
		LogSystem::StdErr("%s", PROJECT_VERSION_DETAILS);
		LogSystem::StdErr("Command line: \"%s\"", WinAPI::GetCmdLineArgsOnly());
		LogSystem::StdErr("Root folder: \"%s\"", m_rootFolder.c_str());

		m_params.hInstance = WinAPI::Module::GetEXE();
		m_params.logFileName = LogSystem::GetDefaultFileName();
		m_params.isDedicatedServer = true;
		m_params.pLog = &m_log;
		m_params.pValidator = &m_validator;
		m_params.pUserCallback = this;

		SetParamsCmdLine(WinAPI::GetCmdLine());

		CrashLogger::Init(m_params.logFileName);

		LoadEngine();
		PatchEngine();

		LogSystem::StdErr("Log file: %s", m_params.logFileName);

		if (!m_log.SetFileName(m_params.logFileName))
		{
			return 1;
		}

		LogSystem::StdErr("Starting CryEngine...");
		StartEngine(m_CryGame);

		LogSystem::StdErr("Ready");

		return UpdateLoop();
	}
	catch (const std::runtime_error& error)
	{
		LogSystem::StdErr("%s", error.what());
		return 1;
	}
}

bool HeadlessServerLauncher::OnError(const char* error)
{
	return true;
}

void HeadlessServerLauncher::OnSaveDocument()
{
}

void HeadlessServerLauncher::OnProcessSwitch()
{
}

void HeadlessServerLauncher::OnInitProgress(const char* message)
{
}

void HeadlessServerLauncher::OnInit(ISystem* pSystem)
{
	gEnv = pSystem->GetGlobalEnvironment();
}

void HeadlessServerLauncher::OnShutdown()
{
}

void HeadlessServerLauncher::OnUpdate()
{
	m_executor.ExecuteMainThreadTasks();
}

void HeadlessServerLauncher::GetMemoryUsage(ICrySizer* pSizer)
{
}

void HeadlessServerLauncher::LoadEngine()
{
	m_CrySystem.Load("CrySystem.dll", DLL::NO_UNLOAD);  // unloading Crysis DLLs is not safe

	m_gameBuild = WinAPI::GetCrysisGameBuild(m_CrySystem.GetHandle());
	if (m_gameBuild < 0)
	{
		throw WinAPI::CurrentError("Failed to get the game version!");
	}

	LogSystem::StdErr("Game build: %d", m_gameBuild);

	VerifyGameBuild();

	m_CryGame.Load("CryGame.dll", DLL::NO_UNLOAD);
	m_CryAction.Load("CryAction.dll", DLL::NO_UNLOAD);
	m_CryNetwork.Load("CryNetwork.dll", DLL::NO_UNLOAD);
	m_CryRenderNULL.Load("CryRenderNULL.dll", DLL::NO_UNLOAD);
}

void HeadlessServerLauncher::PatchEngine()
{
	PatchEngine_CryAction();
	PatchEngine_CryNetwork();
	PatchEngine_CrySystem();
	PatchEngine_CryRenderNULL();
}

void HeadlessServerLauncher::PatchEngine_CryAction()
{
	void* pCryAction = m_CryAction.GetHandle();

	Patch::CryAction::DisableGameplayStats(pCryAction, m_gameBuild);
}

void HeadlessServerLauncher::PatchEngine_CryNetwork()
{
	void* pCryNetwork = m_CryNetwork.GetHandle();

	Patch::CryNetwork::EnablePreordered(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::AllowSameCDKeys(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::FixInternetConnect(pCryNetwork, m_gameBuild);
	Patch::CryNetwork::DisableServerProfile(pCryNetwork, m_gameBuild);
}

void HeadlessServerLauncher::PatchEngine_CrySystem()
{
	void* pCrySystem = m_CrySystem.GetHandle();

	if (CPU::IsAMD() && !CPU::Has3DNow())
	{
		Patch::CrySystem::Disable3DNow(pCrySystem, m_gameBuild);
	}

	Patch::CrySystem::UnhandledExceptions(pCrySystem, m_gameBuild);
}

void HeadlessServerLauncher::PatchEngine_CryRenderNULL()
{
	void* pCryRenderNULL = m_CryRenderNULL.GetHandle();

	Patch::CryRenderNULL::DisableDebugRenderer(pCryRenderNULL, m_gameBuild);
}

std::string HeadlessServerLauncher::GetRootFolder()
{
	std::string rootFolder = CmdLine::GetArgValue("-root");

	if (rootFolder.empty())
	{
		// Bin32 or Bin64 folder
		rootFolder = Path::DirName(WinAPI::Module::GetEXEPath());

		const std::string binFolder = Path::BaseName(rootFolder);

		if (Path::IsNameEqual(binFolder, "Bin32") || Path::IsNameEqual(binFolder, "Bin64"))
		{
			// Crysis main folder
			rootFolder = Path::DirName(rootFolder);
		}
	}

	return rootFolder;
}
