// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

#include <cstddef>

struct I3DEngine;
struct IAISystem;
struct IAnimationGraphSystem;
struct IBudgetingSystem;
struct ICharacterManager;
class  ICmdLine;
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
	// everything is the same in Crysis and Crysis Wars up to here
	// the following stuff cannot be used because we support both Crysis and Crysis Wars
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

	// Gets current supported CPU features flags (CPUF_SSE, CPUF_SSE2, CPUF_3DNOW, CPUF_MMX)
	virtual int GetCPUFlags() = 0;

	// Get seconds per processor tick
	virtual double GetSecondsPerCycle() = 0;

	// dumps the memory usage statistics to the log, default MB (can be KB)
	virtual void DumpMemoryUsageStatistics(bool useKB = false) = 0;

	// Quit the application
	virtual void Quit() = 0;
	// Tells the system if it is relaunching or not
	virtual void Relaunch(bool relaunch) = 0;
	virtual bool IsRelaunch() const = 0;
	// Tells the system in which way we are using the serialization system.
	virtual void SerializingFile(int mode) = 0;
	virtual int IsSerializingFile() const = 0;
	// return true if the application is in the shutdown phase
	virtual bool IsQuitting() = 0;

	// Display error message.
	// Logs it to console and file and error message box.
	// Then terminates execution.
	virtual void Error(const char* format, ...) = 0;

	// Report warning to current Validator object.
	// Not terminates execution.
	virtual void Warning(int subsystem, int severity, int flags, const char* file, const char* format, ...) = 0;

	// Compare specified verbosity level to the one currently set.
	virtual bool CheckLogVerbosity(int verbosity) = 0;

	// returns true if this is dedicated server application
	virtual bool IsDedicated() = 0;

	// returns true if this is editor application
	virtual bool IsEditor() = 0;

	// returns true if this is editor application and not in game mode
	virtual bool IsEditorMode() = 0;

	// return the related subsystem interface
	virtual IConsole* GetIConsole() = 0;
	virtual IScriptSystem* GetIScriptSystem() = 0;
	virtual I3DEngine* GetI3DEngine() = 0;
	virtual ISoundSystem* GetISoundSystem() = 0;
	virtual IMusicSystem* GetIMusicSystem() = 0;
	virtual IPhysicalWorld* GetIPhysicalWorld() = 0;
	virtual IMovieSystem* GetIMovieSystem() = 0;
	virtual IAISystem* GetAISystem() = 0;
	virtual IMemoryManager* GetIMemoryManager() = 0;
	virtual IEntitySystem* GetIEntitySystem() = 0;
	virtual ICryFont* GetICryFont() = 0;
	virtual ICryPak* GetIPak() = 0;
	virtual ILog* GetILog() = 0;
	virtual ICmdLine* GetICmdLine() = 0;
	virtual IStreamEngine* GetStreamEngine() = 0;
	virtual ICharacterManager* GetIAnimationSystem() = 0;
	virtual IValidator* GetIValidator() = 0;
	virtual IFrameProfileSystem* GetIProfileSystem() = 0;
	virtual INameTable* GetINameTable() = 0;
	virtual IBudgetingSystem* GetIBudgetingSystem() = 0;
	virtual IFlowSystem* GetIFlowSystem() = 0;
	virtual IAnimationGraphSystem* GetIAnimationGraphSystem() = 0;
	virtual IDialogSystem* GetIDialogSystem() = 0;
	virtual IHardwareMouse* GetIHardwareMouse() = 0;

	// everything is the same in Crysis and Crysis Wars up to here
	// the following stuff cannot be used because we support both Crysis and Crysis Wars
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
