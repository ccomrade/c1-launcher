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

static int GetEditorBuild(void* pEditor)
{
	if (!OS::DLL::GetVersion(pEditor, g_version))
	{
		throw StringFormat_SysError("Failed to get the editor version!");
	}

	return g_version.patch;
}

static void VerifyEditorBuild(int editorBuild)
{
	switch (editorBuild)
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
			throw StringFormat_Error("Unknown editor build %d", editorBuild);
		}
	}
}

static int CallAfxWinMain(void* instance, char* cmdLine)
{
	typedef int (__stdcall *TAfxWinMain)(void*, void*, char*, int);

#ifdef BUILD_64BIT
	const std::size_t ordinal = 1225;
#else
	const std::size_t ordinal = 1207;
#endif

	void* mfc80 = OS::DLL::Get("mfc80.dll");
	if (!mfc80)
	{
		throw StringFormat_SysError("Failed to get MFC80 DLL");
	}

	void* pAfxWinMain = OS::DLL::FindSymbol(mfc80, reinterpret_cast<const char*>(ordinal));
	if (!pAfxWinMain)
	{
		throw StringFormat_Error("Failed to get AfxWinMain from MFC80 DLL");
	}

	const int cmdShow = 10;  // SW_SHOWDEFAULT

	return static_cast<TAfxWinMain>(pAfxWinMain)(instance, NULL, cmdLine, cmdShow);
}

EditorLauncher::EditorLauncher() : m_dlls()
{
}

EditorLauncher::~EditorLauncher()
{
}

int EditorLauncher::Run(char* cmdLine)
{
	CrashLogger::Enable(&OpenLogFile);

	this->LoadEngine();
	this->PatchEngine();

	return CallAfxWinMain(m_dlls.pEditor, cmdLine);
}

void EditorLauncher::LoadEngine()
{
	m_dlls.pCrySystem = LauncherCommon::LoadDLL("CrySystem.dll");
	m_dlls.gameBuild = LauncherCommon::GetGameBuild(m_dlls.pCrySystem);
	LauncherCommon::VerifyGameBuild(m_dlls.gameBuild);

	m_dlls.pEditor = LauncherCommon::LoadEXE("Editor.exe");
	m_dlls.editorBuild = GetEditorBuild(m_dlls.pEditor);
	VerifyEditorBuild(m_dlls.editorBuild);

	if (LauncherCommon::IsDX10())
	{
		m_dlls.pCryRenderD3D10 = LauncherCommon::LoadDLL("CryRenderD3D10.dll");
	}
	else
	{
		m_dlls.pCryRenderD3D9 = LauncherCommon::LoadDLL("CryRenderD3D9.dll");
	}
}

void EditorLauncher::PatchEngine()
{
	if (m_dlls.pEditor)
	{
		MemoryPatch::Editor::FixBrokenPanels(m_dlls.pEditor, m_dlls.editorBuild);
		MemoryPatch::Editor::HookVersionInit(m_dlls.pEditor, m_dlls.editorBuild, &OnVersionInit);
	}

	if (m_dlls.pCrySystem)
	{
		MemoryPatch::CrySystem::AllowDX9VeryHighSpec(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::DisableCrashHandler(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::FixCPUInfoOverflow(m_dlls.pCrySystem, m_dlls.gameBuild);
		MemoryPatch::CrySystem::HookCPUDetect(m_dlls.pCrySystem, m_dlls.gameBuild, &CPUInfo::Detect);
		MemoryPatch::CrySystem::HookError(m_dlls.pCrySystem, m_dlls.gameBuild, &CrashLogger::OnEngineError);
		MemoryPatch::CrySystem::HookChangeUserPath(m_dlls.pCrySystem, m_dlls.gameBuild,
			&LauncherCommon::OnChangeUserPath);
	}

	if (m_dlls.pCryRenderD3D9)
	{
		MemoryPatch::CryRenderD3D9::HookAdapterInfo(m_dlls.pCryRenderD3D9, m_dlls.gameBuild,
			&LauncherCommon::OnD3D9Info);
	}

	if (m_dlls.pCryRenderD3D10)
	{
		MemoryPatch::CryRenderD3D10::FixLowRefreshRateBug(m_dlls.pCryRenderD3D10, m_dlls.gameBuild);
		MemoryPatch::CryRenderD3D10::HookAdapterInfo(m_dlls.pCryRenderD3D10, m_dlls.gameBuild,
			&LauncherCommon::OnD3D10Info);
		MemoryPatch::CryRenderD3D10::HookInitAPI(m_dlls.pCryRenderD3D10, m_dlls.gameBuild,
			&LauncherCommon::OnD3D10Init);
	}
}
