#pragma once

#include "../LauncherBase.h"
#include "../CrashLogFileSink.h"

class DedicatedServerLauncher : public LauncherBase
{
	DLL m_CryGame;
	DLL m_CryAction;
	DLL m_CryNetwork;
	DLL m_CrySystem;
	DLL m_CryPhysics;

	CrashLogFileSink m_crashSink;

public:
	DedicatedServerLauncher();
	~DedicatedServerLauncher();

	int Run();

private:
	void LoadEngine();
	void PatchEngine();
};
