// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IGAMEOBJECTSYSTEM_H__
#define __IGAMEOBJECTSYSTEM_H__

#pragma once

#include "IEntityClass.h"
#include "IGameFramework.h"
#include "INetwork.h"

enum EGameObjectEventFlags
{
	eGOEF_ToScriptSystem = 0x0001,
	eGOEF_ToGameObject   = 0x0002,
	eGOEF_ToExtensions   = 0x0004,

	eGOEF_ToAll          = eGOEF_ToScriptSystem | eGOEF_ToGameObject | eGOEF_ToExtensions
};

struct IGameObject;
struct SGameObjectEvent;

struct IGameObjectBoxListener
{
	virtual void OnEnter( int id, EntityId entity ) = 0;
	virtual void OnLeave( int id, EntityId entity ) = 0;
	virtual void OnRemoveParent() = 0;
};

struct IGameObjectSystem
{
	// if this is set as the user data for a GameObject with Preactivated Extension
	// spawn, then it will be called back to provide additional initialization
	struct SEntitySpawnParamsForGameObjectWithPreactivatedExtension
	{
		bool (*hookFunction)( IEntity * pEntity, IGameObject*, void * pUserData );
		void * pUserData;
	};

	typedef uint16 ExtensionID;
	static const ExtensionID InvalidExtensionID = ~ExtensionID(0);
	typedef IGameObjectExtension *(*GameObjectExtensionFactory)();

	virtual ExtensionID GetID( const char * name ) = 0;
	virtual const char * GetName( ExtensionID id ) = 0;
	virtual IGameObjectExtension * Instantiate( ExtensionID id, IGameObject * pObject ) = 0;
	virtual void BroadcastEvent( const SGameObjectEvent& evt ) = 0;

	static const uint32 InvalidEventID = ~uint32(0);
	virtual void RegisterEvent( uint32 id, const char* name ) = 0;
	virtual uint32 GetEventID( const char* name ) = 0;
	virtual const char* GetEventName( uint32 id ) = 0;

	virtual IGameObject *CreateGameObjectForEntity(EntityId entityId) = 0;

	virtual void RegisterExtension( const char * name, IGameObjectExtensionCreatorBase * pCreator, IEntityClassRegistry::SEntityClassDesc * pEntityCls ) = 0;
	virtual void DefineProtocol( bool server, IProtocolBuilder * pBuilder ) = 0;

	virtual void PostUpdate( float frameTime ) = 0;
	virtual void SetPostUpdate( IGameObject * pGameObject, bool enable ) = 0;

	virtual void SetSpawnSerializer( TSerialize* ) = 0;
};

// Summary
//   Structure used to define a game object event
struct SGameObjectEvent
{
	SGameObjectEvent( uint32 event, uint16 flags, IGameObjectSystem::ExtensionID target = IGameObjectSystem::InvalidExtensionID, void * param = 0)
	{
		this->event = event;
		this->target = target;
		this->flags = flags;
		this->ptr = 0;
		this->param = param;
	}
	uint32 event;
	IGameObjectSystem::ExtensionID target;
	uint16 flags;
	void * ptr;
	void * param; // optional parameter of event (ugly)
};

#endif
