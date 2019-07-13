////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IEntitySystem.h
//  Version:     v1.00
//  Created:     17/6/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IEntitySystem_h__
#define __IEntitySystem_h__
#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CRYENTITYDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CRYENTITYDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef CRYENTITYDLL_EXPORTS
	#define CRYENTITYDLL_API DLL_EXPORT
#else
	#define CRYENTITYDLL_API DLL_IMPORT
#endif

#include <IEntity.h>
#include <IEntityClass.h>
#include <smartptr.h>
#include <Cry_Geo.h>

// forward declarations.
struct ISystem;
struct IEntitySystem;
class  ICrySizer;
class  CAreaManager;

/*! Entity iterator interface. This interface is used to traverse trough all the entities in an entity system. In a way, 
	this iterator works a lot like a stl iterator.
*/
struct IEntityIt
{
	virtual void AddRef() = 0;

/*! Deletes this iterator and frees any memory it might have allocated.
*/
	virtual void Release() = 0;

/*! Check whether current iterator position is the end position.
	@return True if iterator at end position.
*/
	virtual bool IsEnd() = 0;

/*! Retrieves next entity
	@return The entity that the iterator points to before it goes to the next
*/
	virtual IEntity * Next() = 0;

	/*! Retrieves current entity
	@return The entity that the iterator points to
	*/
	virtual IEntity * This() = 0;

/*! Positions the iterator at the begining of the entity list
*/
	virtual void MoveFirst() = 0;
};

typedef _smart_ptr<IEntityIt> IEntityItPtr;


/*! A callback interface for a class that wants to be aware when new entities are being spawned or removed. A class that implements
	this interface will be called every time a new entity is spawned, removed, or when an entity container is to be spawned.
*/
struct IEntitySystemSink
{
	/*! This callback is called before this entity is spawned. The entity will be created and added to the list of entities,
	if this function returns true. Returning false will abort the spawning process.
	@param params The parameters that will be used to spawn the entity.
	*/
	virtual bool OnBeforeSpawn( SEntitySpawnParams &params ) = 0;

/*! This callback is called when this entity has finished spawning. The entity has been created and added to the list of entities, 
 but has not been initialized yet.
	@param e The entity that was just spawned
*/
	virtual void OnSpawn( IEntity *pEntity,SEntitySpawnParams &params ) = 0;

/*! Called when an entity is being removed.
	@param e The entity that is being removed. This entity is still fully valid.
	@return true to allow removal, false to deny
*/
	virtual bool OnRemove( IEntity *pEntity ) = 0;

/*! Called in response to an entity event.
	@param e The entity that is being removed. This entity is still fully valid.
*/
	virtual void OnEvent( IEntity *pEntity, SEntityEvent &event ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Interface to the entity archetype.
//////////////////////////////////////////////////////////////////////////
struct IEntityArchetype
{
	// Retrieve entity class of the archetype.
	virtual IEntityClass* GetClass() const = 0;
	virtual const char* GetName() const = 0;
	virtual IScriptTable* GetProperties() = 0;
	virtual void LoadFromXML( XmlNodeRef &propertiesNode ) = 0;
};

//////////////////////////////////////////////////////////////////////////
struct IEntityEventListener
{
	virtual void OnEntityEvent( IEntity *pEntity,SEntityEvent &event ) = 0;
};

// forward decl from physics interface
struct EventPhys;

//////////////////////////////////////////////////////////////////////////
struct IBreakableManager
{
	enum EBReakageType
	{
		BREAKAGE_TYPE_DESTROY = 0,
		BREAKAGE_TYPE_FREEZE_SHATTER,
	};
	struct BreakageParams
	{
		EBReakageType type;         // Type of the breakage.
		float	fParticleLifeTime;		// Average lifetime of particle pieces.
		int		nGenericCount;				// If not 0, force particle pieces to spawn generically, this many times.
		bool	bForceEntity;					// Force pieces to spawn as entities.
		bool	bMaterialEffects;			// Automatically create "destroy" and "breakage" material effects on pieces.
		bool	bOnlyHelperPieces;	  // Only spawn helper pieces.

		// Impulse params.
		float	fExplodeImpulse;			// Outward impulse to apply.
		Vec3	vHitImpulse;					// Hit impulse and center to apply.
		Vec3	vHitPoint;

		BreakageParams()
		{
			memset(this, 0, sizeof(*this));
		}
	};
	virtual void BreakIntoPieces( struct IEntity *pEntity, int nSlot, int nPiecesSlot, BreakageParams const& Breakage ) = 0;

	// Attach the effect & params specified by material of object in slot.
	virtual void AttachSurfaceEffect( IEntity* pEntity, int nSlot, const char* sType, struct SpawnParams const& paramsIn ) = 0;

	// Check if static object can be shattered, by checking it`s surface types.
	virtual bool CanShatter( IStatObj *pStatObj ) = 0;

	// Check if entity can be shattered, by checking surface types of geometry or character.
	virtual bool CanShatterEntity( IEntity *pEntity,int nSlot=-1 ) = 0;

	virtual void FakePhysicsEvent( EventPhys * pEvent ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Structure used by proximity query in entity system.
//////////////////////////////////////////////////////////////////////////
struct SEntityProximityQuery
{
	AABB box; // Report entities within this bounding box.
	IEntityClass *pEntityClass;
	uint32 nEntityFlags;

	// Output.
	IEntity** pEntities;
	int nCount;

	SEntityProximityQuery()
	{
		nCount = 0;
		pEntities = 0;
		pEntityClass = 0;
		nEntityFlags = ~(0);
	}
};

/*! Interface to the system that manages the entities in the game, their creation, deletion and upkeep. The entities are kept in a map
 indexed by their uniqie entity ID. The entity system updates only unbound entities every frame (bound entities are updated by their
 parent entities), and deletes the entities marked as garbage every frame before the update. The entity system also keeps track of entities
 that have to be drawn last and with more zbuffer resolution.
*/
#ifndef _NO_IENTITY
struct IEntitySystem
#else
struct IEntitySystemBase
#endif
{
	// Release entity system.
	virtual void Release() = 0;

	// Update entity system and all entities before physics is called. (or as early as possible after input in the multithreaded physics case)
	// This function executes once per frame.
	virtual void PrePhysicsUpdate() = 0;

/*! Update entity system and all entities. This function executes once a frame.
*/
	virtual	void	Update() = 0;

/*! Reset whole entity system, and destroy all entities.
*/
	virtual void	Reset() = 0;

	// Description:
	//     Delete any pending entities (which got marked for deletion)
	virtual void  DeletePendingEntities() = 0;


	// Description:
	//     Retrieve the entity class registry interface.
	// Return:
	//     Pointer to the valid entity class registry interface.
	virtual IEntityClassRegistry* GetClassRegistry() = 0;

/*! Spawns a new entity according to the data in the Entity Descriptor
	@param ed	Entity descriptor structure that describes what kind of entity needs to be spawned
	@param bAutoInit If true automatically initialize entity.
	@return The spawned entity if successfull, NULL if not.
	@see CEntityDesc
*/
	virtual IEntity* SpawnEntity( SEntitySpawnParams &params,bool bAutoInit=true ) = 0;

/*! Initialize entity if entity was spawned not initialized (with bAutoInit false in SpawnEntity)
		Used only by Editor, to setup properties & other things before initializing entity,
		do not use this directly.
		@param pEntity Pointer to just spawned entity object.
		@param ed	Entity descriptor structure that describes what kind of entity needs to be spawned.
		@return true if succesfully initialized entity.
*/
	virtual bool InitEntity( IEntity* pEntity,SEntitySpawnParams &params ) = 0;
	
/*! Retrieves entity from its unique id.
	@param id The unique ID of the entity required
	@return The entity if one with such an ID exists, and NULL if no entity could be matched with the id
*/
  virtual IEntity* GetEntity( EntityId id ) const = 0;

/*! Find first entity with given name.
	@param name The name to look for
	@return The entity if found, 0 if failed
*/
	virtual IEntity* FindEntityByName( const char *sEntityName ) const = 0;

	// might be needed to call before loading of entities to be sure we get the requested ids
	// Arguments:
	//   id - must not be 0
	virtual void ReserveEntityId( const EntityId id ) = 0;
	
/*! Remove an entity by ID
	@param entity	The id of the entity to be removed
	@param bForceRemoveNow If true forces immediately delete of entity, overwise will delete entity on next update.
*/
	virtual void	RemoveEntity( EntityId entity,bool bForceRemoveNow=false ) = 0;

/*! Get number of entities stored in entity system.
	@return The number of entities
*/
	virtual uint32 GetNumEntities() const = 0;

/*! Get entity iterator. This iterator interface can be used to traverse all the entities in this entity system.
	@return An entityIterator
	@see IEntityIt
*/
	virtual IEntityIt * GetEntityIterator() = 0;

	// Description:
	//    Sends the same event to all entities in Entity System.
	// Arguments:
	//    event - Event to send.
	virtual void SendEventToAll( SEntityEvent &event ) = 0;

	// Get all entities within proximity of the specified bounding box.
	// Query is not exact, entities reported can be a few meters away from the bounding box.
	virtual int QueryProximity( SEntityProximityQuery &query ) = 0;

	// Call this when you know dimensions of the level, before entities are created.
	virtual void ResizeProximityGrid( int nWidth,int nHeight ) = 0;
	//////////////////////////////////////////////////////////////////////////
	

/*! Get all entities in specified radius.
		 physFlags is one or more of PhysicalEntityFlag.
	 @see PhysicalEntityFlag
*/
	virtual int	GetPhysicalEntitiesInBox( const Vec3 &origin, float radius, IPhysicalEntity **&pList, int physFlags = (1<<1)|(1<<2)|(1<<3)|(1<<4) ) const = 0;

	// Description.
	//   Retrieve host entity from the physical entity.
	virtual IEntity* GetEntityFromPhysics( struct IPhysicalEntity *pPhysEntity ) const = 0;

/*! Add the sink of the entity system. The sink is a class which implements IEntitySystemSink.
	@param sink	Pointer to the sink, must not be 0
	@see IEntitySystemSink
*/
	virtual void	AddSink( IEntitySystemSink *sink ) = 0;

/*! Remove listening sink from the entity system. The sink is a class which implements IEntitySystemSink.
	@param sink	Pointer to the sink, must not be 0
	@see IEntitySystemSink
*/
	virtual void RemoveSink( IEntitySystemSink *sink ) = 0;

	// Description:
	//    Pause all entity timers.
	// Arguments:
	//    bPause - true to pause timer, false to resume.
	virtual void	PauseTimers( bool bPause,bool bResume=false ) = 0;

/*! Checks whether a given entity ID is already used
*/
	virtual bool IsIDUsed( EntityId nID ) const = 0;

	//! Puts the memory statistics of the entities into the given sizer object
	//! According to the specifications in interface ICrySizer
	virtual void GetMemoryStatistics(ICrySizer *pSizer) = 0;

	// Pointer to original ISystem.
	virtual ISystem* GetSystem() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//    Load entities exported from Editor.
	virtual void LoadEntities( XmlNodeRef &objectsNode ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Register Entity Event`s listeners.
	//////////////////////////////////////////////////////////////////////////
	virtual void AddEntityEventListener( EntityId nEntity,EEntityEvent event,IEntityEventListener *pListener ) = 0;
	virtual void RemoveEntityEventListener( EntityId nEntity,EEntityEvent event,IEntityEventListener *pListener ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Entity GUIDs
	//////////////////////////////////////////////////////////////////////////
	// Find entity by Entity GUID.
	virtual EntityId FindEntityByGuid( const EntityGUID &guid ) const = 0;

	// Access to area manager.
	virtual CAreaManager* GetAreaManager() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//    Return the breakable manager interface 
	virtual IBreakableManager* GetBreakableManager() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Entity archetypes.
	//////////////////////////////////////////////////////////////////////////
	virtual IEntityArchetype* LoadEntityArchetype( const char *sArchetype ) = 0;
	virtual IEntityArchetype* CreateEntityArchetype( IEntityClass *pClass,const char *sArchetype ) = 0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Serializes basic entity system members (timers etc. ) to/from a savegame;
	virtual void Serialize(TSerialize ser) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Makes sure the next SpawnEntity will use the id provided (if it's in use, the current entity is deleted)
	virtual void SetNextSpawnId(EntityId id) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Resets any area state for the specified entity.
	virtual void ResetAreas() = 0;

  //////////////////////////////////////////////////////////////////////////
  // Description:
  //     Dumps entities in system
  virtual void DumpEntities() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Do not spawn any entities unless forced to.
	virtual void LockSpawning(bool lock) = 0;
};

extern "C"
{
	CRYENTITYDLL_API struct IEntitySystem* CreateEntitySystem( ISystem *pISystem );
}

typedef struct IEntitySystem * (* PFNCREATEENTITYSYSTEM)( ISystem *pISystem );

#endif // __IEntitySystem_h__
