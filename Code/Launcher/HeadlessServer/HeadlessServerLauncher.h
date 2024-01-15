#pragma once

#include <string>

#include "CryCommon/CryGame/IGameStartup.h"
#include "CryCommon/CrySystem/ISystem.h"

#include "Logger.h"
#include "NullValidator.h"

class HeadlessServerLauncher : private ISystemUserCallback
{
	IGameStartup* m_pGameStartup;
	SSystemInitParams m_params;

	struct DLLs
	{
		void* pEXE;
		void* pCryGame;
		void* pCryAction;
		void* pCryNetwork;
		void* pCrySystem;
		void* pCryRenderNULL;

		int gameBuild;
		bool isWarhead;
	};

	DLLs m_dlls;

	Logger m_logger;
	NullValidator m_validator;

	std::string m_rootFolder;

public:
	HeadlessServerLauncher();
	~HeadlessServerLauncher();

	int Run();

private:
	void LoadEngine();
	void PatchEngine();

	// ISystemUserCallback
	bool OnError(const char* error) override;
	void OnSaveDocument() override;
	void OnProcessSwitch() override;
	void OnInitProgress(const char* message) override;
	void OnInit(ISystem* pSystem) override;
	void OnShutdown() override;
	void OnUpdate() override;
	void GetMemoryUsage(ICrySizer* pSizer) override;

	static HeadlessServerLauncher* s_self;
	static std::FILE* OpenLogFile();
};
