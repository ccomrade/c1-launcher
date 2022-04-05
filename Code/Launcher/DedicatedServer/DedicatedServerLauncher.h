#pragma once

#include "../LauncherBase.h"

class DedicatedServerLauncher : public LauncherBase
{
	DLL m_CryGame;
	DLL m_CryNetwork;
	DLL m_CrySystem;

public:
	DedicatedServerLauncher();
	~DedicatedServerLauncher();

	int Run();

private:
	void LoadEngine();

	void PatchEngine();
	void PatchEngine_CryNetwork();
	void PatchEngine_CrySystem();
};
