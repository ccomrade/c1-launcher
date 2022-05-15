#pragma once

#include "../LauncherBase.h"
#include "../CrashLogFileSink.h"

class GameLauncher : public LauncherBase
{
	DLL m_CryGame;
	DLL m_CryAction;
	DLL m_CryNetwork;
	DLL m_CrySystem;
	DLL m_CryRenderD3D10;
	DLL m_CryPhysics;

	CrashLogFileSink m_crashSink;

public:
	GameLauncher();
	~GameLauncher();

	int Run();

private:
	void LoadEngine();
	void PatchEngine();
};
