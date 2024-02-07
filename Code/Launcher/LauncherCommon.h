#pragma once

#include <cstdio>
#include <string>

struct CryRender_D3D9_AdapterInfo;
struct CryRender_D3D10_AdapterInfo;
struct CryRender_D3D10_SystemAPI;
struct IGameStartup;
struct ISystem;
struct SSystemInitParams;

namespace LauncherCommon
{
	std::string GetMainFolderPath();
	std::string GetRootFolderPath();

	void* LoadDLL(const char* name);
	void* LoadEXE(const char* name);
	void* LoadCrysisWarheadEXE();

	int GetGameBuild(void* pCrySystem);
	void VerifyGameBuild(int gameBuild);
	bool IsCrysisWarhead(int gameBuild);
	bool IsDX10();

	void SetParamsCmdLine(SSystemInitParams& params, const char* cmdLine);

	IGameStartup* StartEngine(void* pCryGame, SSystemInitParams& params);

	void OnChangeUserPath(ISystem* pSystem, const char* userPath);
	void OnEarlyEngineInit(ISystem* pSystem);
	void OnD3D9Info(CryRender_D3D9_AdapterInfo* info);
	void OnD3D10Info(CryRender_D3D10_AdapterInfo* info);
	bool OnD3D10Init(CryRender_D3D10_SystemAPI* api);

	void LogBytes(const char* message, std::size_t bytes);

	std::FILE* OpenLogFile(const char* defaultFileName);
}
