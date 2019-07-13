////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2005.
// -------------------------------------------------------------------------
//  File name:   IGameTokens.h
//  Version:     v1.00
//  Created:     20/10/2005 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef _IGameTokens_h_
#define _IGameTokens_h_
#pragma once

#include "IFlowSystem.h"

enum EGameTokenFlags
{
	EGAME_TOKEN_MODIFIED     = 0x01,    // Token is modified from its default value.
	EGAME_TOKEN_FROMSAVEGAME = 0x02     // This token was read from the Save game.
};

//////////////////////////////////////////////////////////////////////////
// Game Token can be used as any Plot event in the game.
//////////////////////////////////////////////////////////////////////////
struct IGameToken
{
	// Retrieve name of the game token.
	virtual const char* GetName() const = 0;

	// Sets game token flags.
	virtual void SetFlags( uint32 flags ) = 0;
	// Retrieve game token flags.
	virtual uint32 GetFlags() const = 0;

	// Retrieve current data type of the game token.
	virtual EFlowDataTypes GetType() const = 0;
	// Assign data type to the game token.
	virtual void SetType( EFlowDataTypes dataType ) = 0;

	// Assign a new value to the game token.
	virtual void SetValue( const TFlowInputData& val ) = 0;
	// Retrieve a value of the game token.
	// Returns false if a value cannot be retrived.
	virtual bool GetValue( TFlowInputData& val ) const = 0;

	// Set token value from a string.
	virtual void SetValueAsString( const char* sValue,bool bDefault=false ) = 0;
	virtual const char* GetValueAsString() const = 0;

	// a small template helper (yes, in the i/f) to get the value
	// returns true if successful, false otherwise
	template<typename T>
	bool GetValueAs(T &value)
	{
		TFlowInputData data;
		if (GetValue(data)) 
		{
			return data.GetValueWithConversion(value);
		}
		return false;
	}
};

// Events that game token event listener can receive.
enum EGameTokenEvent
{
	EGAMETOKEN_EVENT_CHANGE,
	EGAMETOKEN_EVENT_DELETE
};

//////////////////////////////////////////////////////////////////////////
// Derive your class from this interface to listen for game the token events.
// Listener must also be registered in game token manager.
//////////////////////////////////////////////////////////////////////////
struct IGameTokenEventListener
{
	virtual void OnGameTokenEvent( EGameTokenEvent event,IGameToken *pGameToken ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Manages collection of game tokens.
// Responsible for saving/loading and acessing game tokens.
//////////////////////////////////////////////////////////////////////////
struct IGameTokenSystem
{
	virtual void GetMemoryStatistics( ICrySizer * ) = 0;
	// Create a new token.
	virtual IGameToken* SetOrCreateToken( const char *sTokenName,const TFlowInputData &defaultValue ) = 0;
	// Deletes existing game token.
	virtual void DeleteToken( IGameToken* pToken ) = 0;

	// Find a game token by name.
	virtual IGameToken* FindToken( const char *sTokenName ) = 0;

	// Rename existing game token.
	virtual void RenameToken( IGameToken *pToken,const char *sNewTokenName ) = 0;

	virtual void RegisterListener( const char *sGameToken,IGameTokenEventListener *pListener,bool bForceCreate=false,bool bImmediateCallback=false ) = 0;
	virtual void UnregisterListener( const char *sGameToken,IGameTokenEventListener *pListener ) = 0;

	virtual void RegisterListener( IGameTokenEventListener *pListener ) = 0;
	virtual void UnregisterListener( IGameTokenEventListener *pListener ) = 0;

	// Load all libraries found with given file spec.
	virtual void LoadLibs( const char *sFileSpec ) = 0;

	// Remove all tokens beginning with prefix (e.g. Library)
	virtual void RemoveLibrary( const char *sPrefix) = 0; 

	// Reset system (deletes all libraries)
	virtual void Reset() = 0;

	virtual void Serialize( TSerialize ser ) = 0;

	virtual void DebugDraw() = 0;

	// a small template helper (yes, in the i/f) to get the value of a token
	// returns true if successful (value found and conversion o.k), false otherwise (not found, or conversion failed)
	template<typename T>
	bool GetTokenValueAs(const char* sGameToken, T &value)
	{
		IGameToken *pToken = FindToken(sGameToken);
		if (pToken == 0) return false;
		return pToken->GetValueAs(value);
	}

};

#endif // _IGameTokens_h_
