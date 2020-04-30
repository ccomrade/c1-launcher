// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

#include <cstddef>

struct I3DEngine;
struct IAISystem;
struct IAnimationGraphSystem;
struct IBudgetingSystem;
struct ICharacterManager;
struct ICmdLine;
struct IConsole;
struct ICryFont;
struct ICryPak;
struct IDialogSystem;
struct IEntitySystem;
struct IFlowSystem;
struct IFrameProfileSystem;
struct IGame;
struct IGlobalTaskScheduler;
struct IGPUPhysicsManager;
struct IHardwareMouse;
struct IInput;
struct ILog;
struct ILogCallback;
struct IMaterialEffects;
struct IMemoryManager;
struct IMovieSystem;
struct IMusicSystem;
struct INameTable;
struct INetwork;
struct IPhysicalWorld;
struct IRenderer;
struct IScriptSystem;
struct ISoundSystem;
struct IStreamEngine;
struct ISystem;
struct ISystemEventDispatcher;
struct ISystemUserCallback;
struct IThreadTaskManager;
struct ITimer;
struct IValidator;
struct SSystemGlobalEnvironment;

using std::size_t;

/**
 * @brief Reverse engineered CryEngine initialization parameters.
 * Crytek removed content of this structure from the Mod SDK.
 * Total size is 2384 bytes in 32-bit code and 2464 bytes in 64-bit code.
 */
struct SSystemInitParams
{
	void *hInstance;                     // executable handle
	void *hWnd;                          // optional window handle
	ILog *pLog;                          // optional custom log
	ILogCallback *pLogCallback;          // optional log callback
	ISystemUserCallback *pUserCallback;  // optional engine callback
	const char *sLogFileName;            // usually "Game.log" or "Server.log"
	IValidator *pValidator;              // optional custom validator
	char szSystemCmdLine[2048];          // application command line obtained with GetCommandLineA
	char szUserPath[256];                // optional custom user folder in %USERPROFILE%\Documents
	bool bEditor;                        // editor mode
	bool bMinimal;                       // minimal mode - skip initialization of some subsystems
	bool bTesting;                       // test mode
	bool bDedicatedServer;               // launch dedicated server
	ISystem *pSystem;                    // initialized by IGameStartup::Init
	void *pCheckFunc;                    // not used
	void *pProtectedFunctions[10];       // probably not used
};

/**
 * @brief The main engine interface.
 * Initializes and dispatches all engine subsystems.
 */
struct ISystem
{
	struct ILoadingProgressListener
	{
		virtual void OnLoadingProgress(int steps) = 0;
	};

	virtual void Release() = 0;

	virtual SSystemGlobalEnvironment *GetGlobalEnvironment() = 0;
	virtual const char *GetRootFolder() const = 0;

	virtual bool Update(int updateFlags = 0, int pauseMode = 0) = 0;

	virtual void RenderBegin() = 0;
	virtual void Render() = 0;
	virtual void RenderEnd() = 0;
	virtual void RenderStatistics() = 0;

	virtual void *AllocMem(void *oldPtr, size_t newSize) = 0;
	virtual unsigned int GetUsedMemory() = 0;

	virtual const char *GetUserName() = 0;

	virtual int GetCPUFlags() = 0;
	virtual double GetSecondsPerCycle() = 0;

	virtual void DumpMemoryUsageStatistics(bool useKB = false) = 0;

	virtual void Quit() = 0;
	virtual void Relaunch(bool relaunch) = 0;
	virtual bool IsRelaunch() const = 0;
	virtual void SerializingFile(int mode) = 0;
	virtual int IsSerializingFile() const = 0;
	virtual bool IsQuitting() = 0;

	virtual void Error(const char *format, ...) = 0;
	virtual void Warning(int module, int severity, int flags, const char *file, const char *format, ...) = 0;

	virtual bool CheckLogVerbosity(int verbosity) = 0;

	virtual bool IsDedicated() = 0;
	virtual bool IsEditor() = 0;
	virtual bool IsEditorMode() = 0;

	virtual IConsole *GetIConsole() = 0;
	virtual IScriptSystem *GetIScriptSystem() = 0;
	virtual I3DEngine *GetI3DEngine() = 0;
	virtual ISoundSystem *GetISoundSystem() = 0;
	virtual IMusicSystem *GetIMusicSystem() = 0;
	virtual IPhysicalWorld *GetIPhysicalWorld() = 0;
	virtual IMovieSystem *GetIMovieSystem() = 0;
	virtual IAISystem *GetAISystem() = 0;
	virtual IMemoryManager *GetIMemoryManager() = 0;
	virtual IEntitySystem *GetIEntitySystem() = 0;
	virtual ICryFont *GetICryFont()	= 0;
	virtual ICryPak *GetIPak() = 0;
	virtual ILog *GetILog() = 0;
	virtual ICmdLine *GetICmdLine() = 0;
	virtual IStreamEngine *GetStreamEngine() = 0;
	virtual ICharacterManager *GetIAnimationSystem() = 0;
	virtual IValidator *GetIValidator() = 0;
	virtual IFrameProfileSystem *GetIProfileSystem() = 0;
	virtual INameTable *GetINameTable() = 0;
	virtual IBudgetingSystem *GetIBudgetingSystem() = 0;
	virtual IFlowSystem *GetIFlowSystem() = 0;
	virtual IAnimationGraphSystem *GetIAnimationGraphSystem() = 0;
	virtual IDialogSystem *GetIDialogSystem() = 0;
	virtual IHardwareMouse *GetIHardwareMouse() = 0;
	virtual ISystemEventDispatcher *GetISystemEventDispatcher() = 0;
	virtual IGlobalTaskScheduler *GetIGlobalTaskScheduler() = 0;
	virtual IThreadTaskManager *GetIThreadTaskManager() = 0;
	virtual IGame *GetIGame() = 0;
	virtual INetwork *GetINetwork() = 0;
	virtual IRenderer *GetIRenderer() = 0;
	virtual IInput *GetIInput() = 0;
	virtual ITimer *GetITimer() = 0;

	virtual IGPUPhysicsManager *GetIGPUPhysicsManager() = 0;

	virtual void SetLoadingProgressListener(ILoadingProgressListener *pListener) = 0;
	virtual ILoadingProgressListener *GetLoadingProgressListener() const = 0;

	virtual void SetIGame(IGame *pGame) = 0;
	virtual void SetIFlowSystem(IFlowSystem *pFlowSystem) = 0;
	virtual void SetIAnimationGraphSystem(IAnimationGraphSystem *pAnimationGraphSystem) = 0;
	virtual void SetIDialogSystem(IDialogSystem *pDialogSystem) = 0;
	virtual void SetIMaterialEffects(IMaterialEffects *pMaterialEffects) = 0;

	virtual void ChangeUserPath(const char *userPath) = 0;

	virtual void DebugStats(bool checkpoint, bool leaks) = 0;
	virtual void DumpWinHeaps() = 0;
	virtual int DumpMMStats(bool log) = 0;

	virtual void SetForceNonDevMode(bool bValue) = 0;
	virtual bool GetForceNonDevMode() const = 0;
	virtual bool WasInDevMode() const = 0;
	virtual bool IsDevMode() const = 0;
	virtual bool IsMODValid(const char *modName) const = 0;

	// ...
};
