/**
 * @file
 * @brief Crysis launcher.
 */

#pragma once

#include "CryCommon/ISystem.h"
#include "CryCommon/IGameStartup.h"
#include "Library/DLL.h"

struct IGameStartup;

class Launcher
{
	SSystemInitParams m_params;

	int m_exitCode;
	int m_gameBuild;

	DLL m_CryGame;
	DLL m_CryAction;
	DLL m_CryNetwork;
	DLL m_CrySystem;
	DLL m_CryRenderD3D10;

	IGameStartup *m_pGameStartup;

	void SetCmdLine();

	void LoadEngine();
	void PatchEngine();

	void PatchEngine_CryGame();
	void PatchEngine_CryAction();
	void PatchEngine_CryNetwork();
	void PatchEngine_CrySystem();
	void PatchEngine_CryRenderD3D10();

	void StartEngine();
	void UpdateLoop();
	void ShutdownEngine();

public:
	Launcher()
	: m_params(),
	  m_exitCode(0),
	  m_gameBuild(0),
	  m_pGameStartup(NULL)
	{
	}

	~Launcher()
	{
		if (m_pGameStartup)
		{
			ShutdownEngine();
		}
	}

	void SetAppInstance(void *hInstance)
	{
		m_params.hInstance = hInstance;
	}

	void SetLogFileName(const char *logFileName)
	{
		m_params.logFileName = logFileName;
	}

	void SetDedicatedServer(bool isDedicatedServer)
	{
		m_params.isDedicatedServer = isDedicatedServer;
	}

	void Run();

	int GetExitCode() const
	{
		return m_exitCode;
	}
};
