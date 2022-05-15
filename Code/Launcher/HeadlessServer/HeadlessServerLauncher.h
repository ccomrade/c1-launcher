#pragma once

#include "../LauncherBase.h"

#include "Executor.h"
#include "LogSystem.h"
#include "NullValidator.h"

class HeadlessServerLauncher : public LauncherBase, public ISystemUserCallback
{
	std::string m_rootFolder;

	Executor m_executor;
	LogSystem m_log;
	NullValidator m_validator;

	DLL m_CryGame;
	DLL m_CryAction;
	DLL m_CryNetwork;
	DLL m_CrySystem;
	DLL m_CryRenderNULL;
	DLL m_CryPhysics;

public:
	HeadlessServerLauncher();
	~HeadlessServerLauncher();

	int Run();

	bool OnError(const char* error) override;
	void OnSaveDocument() override;
	void OnProcessSwitch() override;
	void OnInitProgress(const char* message) override;
	void OnInit(ISystem* pSystem) override;
	void OnShutdown() override;
	void OnUpdate() override;

	void GetMemoryUsage(ICrySizer* pSizer) override;

private:

	void LoadEngine();
	void PatchEngine();

	static std::string GetRootFolder();
};
