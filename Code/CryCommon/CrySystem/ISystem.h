// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

#include <cstddef>

struct I3DEngine;
struct IAISystem;
struct IAnimationGraphSystem;
struct ICharacterManager;
struct IConsole;
struct ICryFont;
struct ICryPak;
class  ICrySizer;
struct IDialogSystem;
struct IEntitySystem;
struct IFlowSystem;
struct IFrameProfileSystem;
struct IGame;
struct IHardwareMouse;
struct IInput;
struct ILog;
struct ILogCallback;
struct IMovieSystem;
struct IMusicSystem;
struct INameTable;
struct INetwork;
struct IPhysicalWorld;
struct IRenderer;
struct IScriptSystem;
struct ISoundSystem;
struct ISystem;
struct ISystemUserCallback;
struct ITimer;
struct IValidator;

/**
 * Reverse engineered CryEngine initialization parameters.
 *
 * Crytek removed content of this structure from the Mod SDK.
 * Total size is 2384 bytes in 32-bit code and 2464 bytes in 64-bit code.
 */
struct SSystemInitParams
{
	void* hInstance;                     // Executable handle
	void* hWnd;                          // Optional window handle
	ILog* pLog;                          // Optional custom log
	ILogCallback* pLogCallback;          // Optional log callback
	ISystemUserCallback* pUserCallback;  // Optional engine callback
	const char* logFileName;             // Name of the log file
	IValidator* pValidator;              // Optional custom validator

	char cmdLine[2048];                  // Application command line obtained with GetCommandLineA
	char userPath[256];                  // Has no effect

	bool isEditor;                       // Editor mode
	bool isMinimal;                      // Minimal mode
	bool isTesting;                      // Test mode
	bool isDedicatedServer;              // Dedicated server mode

	ISystem* pSystem;                    // Initialized by IGameStartup::Init

	void* reserved[11];                  // Not used
};

/**
 * CryEngine environment.
 */
struct SSystemGlobalEnvironment
{
	ISystem*               pSystem;
	IGame*                 pGame;
	INetwork*              pNetwork;
	IRenderer*             pRenderer;
	IInput*                pInput;
	ITimer*                pTimer;
	IConsole*              pConsole;
	IScriptSystem*         pScriptSystem;
	I3DEngine*             p3DEngine;
	ISoundSystem*          pSoundSystem;
	IMusicSystem*          pMusicSystem;
	IPhysicalWorld*        pPhysicalWorld;
	IMovieSystem*          pMovieSystem;
	IAISystem*             pAISystem;
	IEntitySystem*         pEntitySystem;
	ICryFont*              pCryFont;
	ICryPak*               pCryPak;
	ILog*                  pLog;
	ICharacterManager*     pCharacterManager;
	IFrameProfileSystem*   pFrameProfileSystem;
	INameTable*            pNameTable;
	IFlowSystem*           pFlowSystem;
	IAnimationGraphSystem* pAnimationGraphSystem;
	IDialogSystem*         pDialogSystem;
	IHardwareMouse*        pHardwareMouse;
	// everything is the same in Crysis, Crysis Warhead, and Crysis Wars up to here
	// the following stuff cannot be used because we support all three games
	// ...
};

/**
 * The main engine interface.
 *
 * Initializes and dispatches all engine subsystems.
 */
struct ISystem
{
	virtual void Release() = 0;

	virtual SSystemGlobalEnvironment* GetGlobalEnvironment() = 0;

	// Returns the root folder specified by the command line option "-root <path>"
	virtual const char* GetRootFolder() const = 0;

	// Update all subsystems
	// Arguments:
	//   flags - one or more flags from ESystemUpdateFlags structure
	//   pauseMode - 0 = normal (no pause), 1 = menu/pause, 2 = cutscene
	virtual bool Update(int updateFlags = 0, int pauseMode = 0) = 0;

	// Begin rendering frame.
	virtual void RenderBegin() = 0;
	// Render subsystems.
	virtual void Render() = 0;
	// End rendering frame and swap back buffer.
	virtual void RenderEnd() = 0;

	// Renders the statistics; this is called from RenderEnd, but if the
	// Host application (Editor) doesn't employ the Render cycle in ISystem,
	// it may call this method to render the essencial statistics
	virtual void RenderStatistics () = 0;

	// Common (cross-module) memory allocation function.
	virtual void* AllocMem(void* oldptr, std::size_t newsize) = 0;

	// Returns the current used memory
	virtual unsigned int GetUsedMemory() = 0;

	// Retrieve the name of the user currently logged in to the computer
	virtual const char* GetUserName() = 0;

	// everything is the same in Crysis, Crysis Warhead, and Crysis Wars up to here
	// the following stuff cannot be used because we support all three games
	// ...
};

/**
 * User defined callback, which can be passed to ISystem.
 */
struct ISystemUserCallback
{
	virtual bool OnError(const char* error) = 0;
	virtual void OnSaveDocument() = 0;
	virtual void OnProcessSwitch() = 0;
	virtual void OnInitProgress(const char* message) = 0;
	virtual void OnInit(ISystem* pSystem) = 0;
	virtual void OnShutdown() = 0;
	virtual void OnUpdate() = 0;

	virtual void GetMemoryUsage(ICrySizer* pSizer) = 0;
};

////////////////////////////////////////////////////////////////////////////////

extern SSystemGlobalEnvironment* gEnv;

////////////////////////////////////////////////////////////////////////////////

void CryLog(const char* format, ...);
void CryLogWarning(const char* format, ...);
void CryLogError(const char* format, ...);
void CryLogAlways(const char* format, ...);
void CryLogWarningAlways(const char* format, ...);
void CryLogErrorAlways(const char* format, ...);
void CryLogComment(const char* format, ...);

////////////////////////////////////////////////////////////////////////////////
