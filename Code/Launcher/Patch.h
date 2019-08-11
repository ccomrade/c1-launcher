/**
 * @file
 * @brief Functions for patching Crysis code.
 */

#pragma once

// CryAction
int PatchDX9ImmersiveMultiplayer( void *libCryAction, int gameVersion );

// CryGame
int PatchSkipIntros( void *libCryGame, int gameVersion );
int PatchCanJoinDX10Servers( void *libCryGame, int gameVersion );
int PatchFlashMenuDX10( void *libCryGame, int gameVersion );

// CryNetwork
int PatchDuplicateCDKey( void *libCryNetwork, int gameVersion );

// CrySystem
int Patch64BitSecuROM( void *libCrySystem, int gameVersion );
int PatchDX9VeryHighSpec( void *libCrySystem, int gameVersion );
int PatchMultipleInstances( void *libCrySystem, int gameVersion );
int PatchUnhandledExceptions( void *libCrySystem, int gameVersion );
int PatchDisable3DNow( void *libCrySystem, int gameVersion );

