#pragma once

#include <cstdio>
#include <string>

struct IGameStartup;
struct ISystem;
struct SSystemInitParams;

namespace MemoryPatch
{
	namespace CryRenderD3D9
	{
		struct AdapterInfo;
	}

	namespace CryRenderD3D10
	{
		struct AdapterInfo;
		struct SystemAPI;
	}
}

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
	void OnEarlyEngineInit(ISystem* pSystem, const char* banner);
	void OnD3D9Info(MemoryPatch::CryRenderD3D9::AdapterInfo* info);
	void OnD3D10Info(MemoryPatch::CryRenderD3D10::AdapterInfo* info);
	bool OnD3D10Init(MemoryPatch::CryRenderD3D10::SystemAPI* api);
	void OnCryWarning(int, int, const char* format, ...);
	void OnGameWarning(const char* format, ...);

	void LogBytes(const char* message, std::size_t bytes);

	std::FILE* OpenLogFile(const char* defaultFileName);
}
