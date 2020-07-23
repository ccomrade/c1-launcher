// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

#include "ILog.h"

struct I3DEngine;
struct IAISystem;
struct IAnimationGraphSystem;
struct ICharacterManager;
struct IConsole;
struct ICryFont;
struct ICryPak;
struct IDialogSystem;
struct IEntitySystem;
struct IFlowSystem;
struct IFrameProfileSystem;
struct IGame;
struct IHardwareMouse;
struct IInput;
struct IMaterialEffects;
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
 * @brief Reverse engineered CryEngine initialization parameters.
 * Crytek removed content of this structure from the Mod SDK.
 * Total size is 2384 bytes in 32-bit code and 2464 bytes in 64-bit code.
 */
struct SSystemInitParams
{
	void *hInstance;                     //!< Executable handle.
	void *hWnd;                          //!< Optional window handle.
	ILog *pLog;                          //!< Optional custom log.
	ILogCallback *pLogCallback;          //!< Optional log callback.
	ISystemUserCallback *pUserCallback;  //!< Optional engine callback.
	const char *logFileName;             //!< Name of the log file.
	IValidator *pValidator;              //!< Optional custom validator.

	char cmdLine[2048];                  //!< Application command line obtained with GetCommandLineA.
	char userPath[256];                  //!< Optional custom user folder in %USERPROFILE%\Documents.

	bool isEditor;                       //!< Editor mode.
	bool isMinimal;                      //!< Minimal mode.
	bool isTesting;                      //!< Test mode.
	bool isDedicatedServer;              //!< Dedicated server mode.

	ISystem *pSystem;                    //!< Initialized by IGameStartup::Init.

	void *pCheckFunc;                    //!< Not used.
	void *pProtectedFunctions[10];       //!< Not used.
};

/**
 * @brief Global environment.
 */
struct SSystemGlobalEnvironment
{
	ISystem               *pSystem;
	IGame                 *pGame;
	INetwork              *pNetwork;
	IRenderer             *pRenderer;
	IInput                *pInput;
	ITimer                *pTimer;
	IConsole              *pConsole;
	IScriptSystem         *pScriptSystem;
	I3DEngine             *p3DEngine;
	ISoundSystem          *pSoundSystem;
	IMusicSystem          *pMusicSystem;
	IPhysicalWorld        *pPhysicalWorld;
	IMovieSystem          *pMovieSystem;
	IAISystem             *pAISystem;
	IEntitySystem         *pEntitySystem;
	ICryFont              *pCryFont;
	ICryPak               *pCryPak;
	ILog                  *pLog;
	ICharacterManager     *pCharacterManager;
	IFrameProfileSystem   *pFrameProfileSystem;
	INameTable            *pNameTable;
	IFlowSystem           *pFlowSystem;
	IAnimationGraphSystem *pAnimationGraphSystem;
	IDialogSystem         *pDialogSystem;
	IHardwareMouse        *pHardwareMouse;
	IMaterialEffects      *pMaterialEffects;

	bool bClient;
	bool bServer;
	bool bMultiplayer;

	// ...
};

/**
 * @brief The main engine interface.
 * Initializes and dispatches all engine subsystems.
 */
struct ISystem
{
	virtual void Release() = 0;

	virtual SSystemGlobalEnvironment *GetGlobalEnvironment() = 0;

	// ...
};

extern SSystemGlobalEnvironment *gEnv;

inline void CryLog(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eMessage, format, args);
	va_end(args);
}

inline void CryLogWarning(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eWarning, format, args);
	va_end(args);
}

inline void CryLogError(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eError, format, args);
	va_end(args);
}

inline void CryLogAlways(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eAlways, format, args);
	va_end(args);
}

inline void CryLogWarningAlways(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eWarningAlways, format, args);
	va_end(args);
}

inline void CryLogErrorAlways(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eErrorAlways, format, args);
	va_end(args);
}

inline void CryLogComment(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eComment, format, args);
	va_end(args);
}
