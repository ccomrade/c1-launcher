#pragma once

#include "CryCommon/CryGame/IGameStartup.h"
#include "CryCommon/CrySystem/ISystem.h"

class DedicatedServerLauncher
{
	IGameStartup* m_pGameStartup;
	SSystemInitParams m_params;

	struct DLLs
	{
		void* pEXE;
		void* pCryGame;
		void* pCryNetwork;
		void* pCrySystem;

		int gameBuild;
		bool isWarhead;
	};

	DLLs m_dlls;

public:
	DedicatedServerLauncher();
	~DedicatedServerLauncher();

	int Run();

private:
	void LoadEngine();
	void PatchEngine();
};
