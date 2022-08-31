// Copyright (C) 2001-2004 Crytek GmbH

#pragma once

#include "IGameRef.h"

struct SSystemInitParams;

struct IGameStartup
{
	/**
	 * Entry function used to create a new instance of the game.
	 */
	typedef IGameStartup* (*TEntryFunction)();

	/**
	 * Initializes the game and/or any mod and gets the IGame interface.
	 *
	 * The shutdown function must be called independently of the return value of this function.
	 *
	 * @param startupParams System initialization parameters.
	 * @return Pointer to a IGame interface or NULL if something went wrong.
	 */
	virtual IGameRef Init(SSystemInitParams& startupParams) = 0;

	/**
	 * Shutdowns the game and any loaded mod and deletes itself.
	 */
	virtual void Shutdown() = 0;

	/**
	 * Updates the game.
	 *
	 * @param haveFocus Boolean describing if the game has the input focus or not.
	 * @param updateFlags Usually zero.
	 * @return Zero to terminate the game, non-zero to continue.
	 */
	virtual int Update(bool haveFocus, unsigned int updateFlags) = 0;

	/**
	 * Returns a restart level and thus triggers a restart.
	 *
	 * @param levelName Pointer to name of the level to load after restart.
	 * @return True if restart is requested, otherwise false.
	 */
	virtual bool GetRestartLevel(char** levelName) = 0;

	/**
	 * Returns whether a patch needs to be installed.
	 *
	 * @return Path to downloaded patch executable or NULL.
	 */
	virtual const char* GetPatch() const = 0;

	/**
	 * Retrieves the next mod to use in case the engine got a restart request.
	 *
	 * @param modName Buffer to store mod name.
	 * @param modNameMaxLength Size of the mod name buffer.
	 * @return True if mod is requested, otherwise false.
	 */
	virtual bool GetRestartMod(char* modName, int modNameMaxLength) = 0;

	/**
	 * Initializes and controls the game loop.
	 *
	 * @param autoStartLevelName Level to jump into or NULL for normal behaviour.
	 * @return Zero when the game terminated normally, otherwise non-zero.
	 */
	virtual int Run(const char* autoStartLevelName) = 0;
};
