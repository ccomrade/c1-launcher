#pragma once

#include "../LauncherBase.h"

class GameLauncher : public LauncherBase
{
	DLL m_CryGame;
	DLL m_CryAction;
	DLL m_CryNetwork;
	DLL m_CrySystem;
	DLL m_CryRenderD3D10;

public:
	GameLauncher();
	~GameLauncher();

	int Run();

private:
	void LoadEngine();
	void PatchEngine();
};
