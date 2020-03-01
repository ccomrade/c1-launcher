// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

struct ISystem;
struct ISystemUserCallback;
struct ILog;
struct ILogCallback;
struct IValidator;

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
