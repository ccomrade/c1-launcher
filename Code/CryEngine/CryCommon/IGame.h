/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: 
  
 -------------------------------------------------------------------------
  History:
  - 2:8:2004   10:53 : Created by Márcio Martins

*************************************************************************/
#ifndef __IGAME_H__
#define __IGAME_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <ICmdLine.h>
#include <INetwork.h>

struct IPuppetProxy;
struct IGameFramework;

// Summary
//		Main interface used for the game central object
// Description
//		The IGame interface should be implemented in the GameDLL. Game systems 
//		residing in the GameDLL can be initialized and updated inside the Game 
//		object.
// See Also
//		IEditorGame
struct IGame
{
	// Summary
	//   Entry function used to initialize a new IGame of a GameMod
	// Parameters
	//   pGameFramework - a pointer to the game framework, from which a pointer of ISystem can be retrieved
	// Returns
	//   A pointer to the newly created IGame implementation
	typedef IGame *(*TEntryFunction)(IGameFramework* pGameFramework);

	// Description:
	//		Initialize the MOD.
	//		The shutdown method, must be called independent of this method's return value.
	// Arguments:
	//		pCmdLine - Pointer to the command line interface.
	//		pFramework - Pointer to the IGameFramework interface.
	// Return Value:
	//		0 if something went wrong with initialization, non-zero otherwise.
	virtual bool Init(IGameFramework *pFramework) = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) = 0;

	// Description:
	//		Finish initializing the MOD.
	//		Called after the game framework has finished its CompleteInit.
	//		This is the point at which to register game flow nodes etc.
	virtual bool CompleteInit() {return true;};

	// Description:
	//		Shuts down the MOD and delete itself.
	virtual void Shutdown() = 0;

	// Description:
	//		Reloads the currently running map (from level start savegame)
	//		Return the savegame which will be loaded
	virtual string InitMapReloading() = 0;

	// Description:
	//		Updates the MOD.
	// Arguments:
	//		haveFocus - Boolean describing if the game has the input focus or not.
	// Return Value:
	//		0 to terminate the game (i.e. when quitting), non-zero to continue
	virtual int Update(bool haveFocus, unsigned int updateFlags) = 0;

	// Description:
	//		Configures game-side network protocol.
	// Arguments:
	//		isServer - Boolean describing if the channel is the server channel or the client channel.
	virtual void ConfigureGameChannel(bool isServer, IProtocolBuilder *pBuilder) = 0;

  // Description:
	//		Called on the game when entering/exiting game mode in editor
	// Arguments:
	//		bStart - Boolean describing if we enter or exit game mode
	virtual void EditorResetGame(bool bStart) = 0;
	
	// Description:
	//		Called on the game when the local player id is set.
	// Arguments:
	//		playerId - The entity Id of the local player.
	virtual void PlayerIdSet(EntityId playerId) = 0;

	// Description:
	//		Returns a pointer to the game framework beeing used.
	// Return Value:
	//		Pointer to the game framework beeing used.
	virtual IGameFramework *GetIGameFramework() = 0;

	// Description:
	//		Returns the name of the mode. (i.e.: "Capture The Flag")
	// Return Value:
	//		The name of the mode. (i.e.: "Capture The Flag")
	virtual const char *GetLongName() = 0;

	// Description:
	//		Returns a short description of the mode. (i.e.: dc)
	// Return Value:
	//		A short description of the mode. (i.e.: dc)
	virtual const char *GetName() = 0;

	// Description:
	//		Loads a specified action map, used mainly for loading the default action map
	// Return Value:
	//		Void
	virtual void LoadActionMaps(const char* filename) = 0;

	// Description:
	//		Called when playerID is reset in GameChannel
	// Return Value:
	//		Void
	virtual void OnClearPlayerIds() = 0;

	// Description:
	//		The game is reloading the level.
	// Return Value:
	//		Void
	virtual bool IsReloading() = 0;

	// Description:
	//		Auto-Savegame name creation
	// Return Value:
	//		c_str or NULL
	virtual const char* CreateSaveGameName() = 0;

	// Description:
	//		Mapping level filename to "official" name.
	// Return Value:
	//		c_str or NULL
	virtual const char* GetMappedLevelName(const char *levelName) const = 0;
};


#endif //__IGAME_H__
