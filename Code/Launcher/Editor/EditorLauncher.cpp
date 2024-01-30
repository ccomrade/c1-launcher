#include "Library/CrashLogger.h"
#include "Library/OS.h"
#include "Library/StringFormat.h"

#include "../CPUInfo.h"
#include "../LauncherCommon.h"
#include "../MemoryPatch.h"

#include "EditorLauncher.h"

#define DEFAULT_LOG_FILE_NAME "Editor.log"

static OS::DLL::Version g_version;

static std::FILE* OpenLogFile()
{
	return LauncherCommon::OpenLogFile(DEFAULT_LOG_FILE_NAME);
}

static void OnVersionInit(MemoryPatch::Editor::Version* version)
{
	version->file_major = g_version.major;
	version->file_minor = g_version.minor;
	version->file_tweak = g_version.tweak;
	version->file_patch = g_version.patch;

	version->product_major = g_version.major;
	version->product_minor = g_version.minor;
	version->product_tweak = g_version.tweak;
	version->product_patch = g_version.patch;
}

EditorLauncher::EditorLauncher() : m_dlls()
{
}

EditorLauncher::~EditorLauncher()
{
}

int EditorLauncher::Run()
{
	CrashLogger::Enable(&OpenLogFile);

	this->LoadEngine();
	this->PatchEngine();

	void* mfc80 = OS::DLL::Get("mfc80.dll");
	if (!mfc80)
	{
		OS::ErrorBox("Failed to get mfc80.dll");
		return 1;
	}

	typedef int (__stdcall *TAfxWinMain)(void*, void*, char*, int);

#ifdef BUILD_64BIT
	const char* afxWinMain = reinterpret_cast<const char*>(1225);
#else
	const char* afxWinMain = reinterpret_cast<const char*>(1207);
#endif

	TAfxWinMain pAfxWinMain = static_cast<TAfxWinMain>(OS::DLL::FindSymbol(mfc80, afxWinMain));
	if (!pAfxWinMain)
	{
		OS::ErrorBox("Failed to get AfxWinMain");
		return 1;
	}

	char* cmdLine = const_cast<char*>(OS::CmdLine::GetOnlyArgs());
	const int cmdShow = 0xA;  // SW_SHOWDEFAULT

	return pAfxWinMain(m_dlls.pEditor, NULL, cmdLine, cmdShow);
}

void EditorLauncher::LoadEngine()
{
	m_dlls.pCrySystem = LauncherCommon::LoadDLL("CrySystem.dll");

	m_dlls.gameBuild = LauncherCommon::GetGameBuild(m_dlls.pCrySystem);

	LauncherCommon::VerifyGameBuild(m_dlls.gameBuild);

	m_dlls.pEditor = LauncherCommon::LoadEXE("Editor.exe");

	if (!OS::DLL::GetVersion(m_dlls.pEditor, g_version))
	{
		throw StringFormat_SysError("Failed to get the editor version!");
	}

	m_dlls.editorBuild = g_version.patch;

	switch (m_dlls.editorBuild)
	{
		case 5767:
		{
			// Crysis
			break;
		}
		case 6670:
		{
			// Crysis Wars
			break;
		}
		default:
		{
			throw StringFormat_Error("Unknown editor build %d", m_dlls.editorBuild);
		}
	}

	// TODO: renderer
}

void EditorLauncher::PatchEngine()
{
	if (m_dlls.pCrySystem)
	{
		//MemoryPatch::CrySystem::RemoveSecuROM(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::AllowDX9VeryHighSpec(m_dlls.pCrySystem, m_dlls.gameBuild);
		//MemoryPatch::CrySystem::AllowMultipleInstances(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::DisableCrashHandler(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::FixCPUInfoOverflow(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, m_dlls.gameBuild, &CPUInfo::Detect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, m_dlls.gameBuild, &CrashLogger::OnEngineError);
		//MemoryPatch::CrySystem::HookLanguageInit(m_dlls.pCrySystem, m_dlls.gameBuild, &LanguageHook::OnInit);
		MemoryPatch::CrySystem::HookChangeUserPath(m_dlls.pCrySystem, m_dlls.gameBuild,
			&LauncherCommon::OnChangeUserPath);
	}

	if (m_dlls.pEditor)
	{
		MemoryPatch::Editor::HookVersionInit(m_dlls.pEditor, m_dlls.editorBuild, &OnVersionInit);
	}

	// TODO: renderer
}
