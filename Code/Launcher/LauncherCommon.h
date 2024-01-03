#pragma once

#include <cstdio>
#include <string>

struct IGameStartup;
struct ISystem;
struct SSystemInitParams;

namespace LauncherCommon
{
	std::string GetMainFolderPath();
	std::string GetRootFolderPath();

	void* LoadDLL(const char* name);

	int GetGameBuild(void* pCrySystem);
	void VerifyGameBuild(int gameBuild);

	void SetParamsCmdLine(SSystemInitParams& params, const char* cmdLine);

	IGameStartup* StartEngine(void* pCryGame, SSystemInitParams& params);

	void OnChangeUserPath(ISystem* pSystem, const char* userPath);
	void OnEarlyEngineInit(ISystem* pSystem);

	std::FILE* OpenLogFile(const char* defaultFileName);
}
