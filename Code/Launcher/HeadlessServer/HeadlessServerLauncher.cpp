#include <cstdio>
#include <cstdlib>  // std::atoi

#include "Library/CrashLogger.h"
#include "Library/OS.h"
#include "Library/PathTools.h"
#include "Project.h"

#include "../CPUInfo.h"
#include "../LauncherCommon.h"
#include "../MemoryPatch.h"

#include "HeadlessServerLauncher.h"

#define DEFAULT_LOG_FILE_NAME "Server.log"
#define DEFAULT_LOG_VERBOSITY "0"

static void Print(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::vfprintf(stderr, format, args);
	va_end(args);

	std::fputc('\n', stderr);
	std::fflush(stderr);
}

HeadlessServerLauncher* HeadlessServerLauncher::s_self;

HeadlessServerLauncher::HeadlessServerLauncher() : m_pGameStartup(NULL), m_params(), m_dlls()
{
	s_self = this;
}

HeadlessServerLauncher::~HeadlessServerLauncher()
{
	if (m_pGameStartup)
	{
		m_pGameStartup->Shutdown();
	}

	s_self = NULL;
}

int HeadlessServerLauncher::Run()
{
	Print("%s", PROJECT_BANNER);
	Print("Command line: [%s]", OS::CmdLine::GetOnlyArgs());

	m_rootFolder = LauncherCommon::GetRootFolderPath();
	Print("Root folder: \"%s\"", m_rootFolder.c_str());

	const int verbosity = std::atoi(OS::CmdLine::GetArgValue("-verbosity", DEFAULT_LOG_VERBOSITY));
	const char* logFileName = OS::CmdLine::GetArgValue("-logfile", DEFAULT_LOG_FILE_NAME);
	const char* logPrefix = OS::CmdLine::GetArgValue("-logprefix", "");

	m_params.hInstance = OS::EXE::Get();
	m_params.logFileName = DEFAULT_LOG_FILE_NAME;
	m_params.isDedicatedServer = true;
	m_params.pLog = &m_logger;
	m_params.pValidator = &m_validator;
	m_params.pUserCallback = this;

	LauncherCommon::SetParamsCmdLine(m_params, OS::CmdLine::Get());

	CrashLogger::Enable(&HeadlessServerLauncher::OpenLogFile);

	this->LoadEngine();
	this->PatchEngine();

	Print("Log verbosity: %d", verbosity);
	m_logger.SetVerbosity(verbosity);

	Print("Log file: %s", logFileName);
	m_logger.OpenFile(PathTools::Join(m_rootFolder, logFileName).c_str());
	m_logger.SetPrefix(logPrefix);

	Print("Starting CryEngine...");
	m_pGameStartup = LauncherCommon::StartEngine(m_dlls.pCryGame, m_params);

	Print("Ready");

	return m_pGameStartup->Run(NULL);
}

void HeadlessServerLauncher::LoadEngine()
{
	m_dlls.pCrySystem = LauncherCommon::LoadDLL("CrySystem.dll");

	m_dlls.gameBuild = LauncherCommon::GetGameBuild(m_dlls.pCrySystem);
	Print("Game build: %d", m_dlls.gameBuild);

	LauncherCommon::VerifyGameBuild(m_dlls.gameBuild);

	m_dlls.pCryGame = LauncherCommon::LoadDLL("CryGame.dll");
	m_dlls.pCryAction = LauncherCommon::LoadDLL("CryAction.dll");
	m_dlls.pCryNetwork = LauncherCommon::LoadDLL("CryNetwork.dll");
	m_dlls.pCryRenderNULL = LauncherCommon::LoadDLL("CryRenderNULL.dll");
}

void HeadlessServerLauncher::PatchEngine()
{
	if (m_dlls.pCryAction)
	{
		MemoryPatch::CryAction::DisableGameplayStats(m_dlls.pCryAction, m_dlls.gameBuild);
	}

	if (m_dlls.pCryNetwork)
	{
		MemoryPatch::CryNetwork::EnablePreordered(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::AllowSameCDKeys(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixInternetConnect(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::FixFileCheckCrash(m_dlls.pCryNetwork, m_dlls.gameBuild);
		MemoryPatch::CryNetwork::DisableServerProfile(m_dlls.pCryNetwork, m_dlls.gameBuild);
	}

	if (m_dlls.pCrySystem)
	{
		MemoryPatch::CrySystem::DisableCrashHandler(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, m_dlls.gameBuild, &CPUInfo::Detect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, m_dlls.gameBuild, &CrashLogger::OnEngineError);
	}

	if (m_dlls.pCryRenderNULL)
	{
		MemoryPatch::CryRenderNULL::DisableDebugRenderer(m_dlls.pCryRenderNULL, m_dlls.gameBuild);
	}
}

bool HeadlessServerLauncher::OnError(const char* error)
{
	return false;
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
	m_logger.OnUpdate();
}

void HeadlessServerLauncher::GetMemoryUsage(ICrySizer* pSizer)
{
}

std::FILE* HeadlessServerLauncher::OpenLogFile()
{
	return (s_self) ? s_self->m_logger.GetFileHandle() : NULL;
}
