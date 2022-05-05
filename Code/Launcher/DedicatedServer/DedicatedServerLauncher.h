#pragma once

#include "../LauncherBase.h"
#include "../CrashLogFileSink.h"

class DedicatedServerLauncher : public LauncherBase
{
	DLL m_CryGame;
	DLL m_CryNetwork;
	DLL m_CrySystem;

	CrashLogFileSink m_crashSink;

public:
	DedicatedServerLauncher();
	~DedicatedServerLauncher();

	int Run();

private:
	void LoadEngine();
	void PatchEngine();
};
