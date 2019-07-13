////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IEntityClass.h
//  Version:     v1.00
//  Created:     18/5/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IEntityClass_h__
#define __IEntityClass_h__
#pragma once

struct IEntity;
struct IEntityProxy;
struct SEntitySpawnParams;
struct IEntityScript;

enum EEntityClassFlags
{
	ECLF_INVISIBLE = 0x0001, // If set this class will not be visible in editor,and entity of this class cannot be placed manually in editor.
	ECLF_DEFAULT   = 0x0002, // If this is default entity class.
};


//////////////////////////////////////////////////////////////////////////
// Description:
//    Entity class defines what is this entity, what script it uses,
//    what user proxy will be spawned with the entity,etc...
//    IEntityClass unique identify type of the entity, Multiple entities
//    share the same entity class.
//    Two entities can be compared if they are of the same type by
//    just comparing thier IEntityClass pointers.
//////////////////////////////////////////////////////////////////////////
struct IEntityClass
{
	// Description:
	//    Destroy IEntityClass object, do not call directly, only EntityRegisty can destroy entity classes.
	virtual void Release() = 0;

	// Description:
	//    Returns the name of the entity class, Class name must be unique among all the entity classes.
	//    If this entity also uses a script, this is the name of the Lua table representing the entity behaviour.
	virtual const char* GetName() const = 0;

	// Description:
	//    Returns entity class flags.
	// See Also:
	//    EEntityClassFlags
	virtual uint32 GetFlags() const = 0;

	// Description:
	//    Set entity class flags.
	// See Also:
	//    EEntityClassFlags
	virtual void SetFlags( uint32 nFlags ) = 0;

	// Description:
	//    Returns the Lua script file name.
	// Returns:
	//    Lua Script filename, return empty string if entity does not use script.
	virtual const char* GetScriptFile() const = 0;

	// Description:
	//    Returns the IEntityScript interface assigned for this entity class.
	// Returns:
	//    IEntityScript interface if this entity have script, or NULL if no script defined for this entity class.
	virtual IEntityScript* GetIEntityScript() const = 0;

	// Description:
	//    Loads the script.
	//    It is safe to call LoadScript multiple times, only first time the script will be loaded, if bForceReload is not specified.
	virtual bool LoadScript( bool bForceReload ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// UserProxyCreateFunc is a function pointer type, 
	// by calling this function EntitySystem can create user defined UserProxy class for an entity in SpawnEntity.
	// Ex:
	// IEntityProxy* CreateUserProxy( IEntity *pEntity,SEntitySpawnParams &params )
	// { 
	//   return new CUserProxy( pEntity,params );
	// }
	typedef IEntityProxy* (*UserProxyCreateFunc)( IEntity *pEntity,SEntitySpawnParams &params,void *pUserData );

	// Description:
	//    Returns pointer to the user defined function to create UserProxy.
	// Returns:
	//    Return ContainerCreateFunc function pointer.
	virtual UserProxyCreateFunc GetUserProxyCreateFunc() const = 0;
	
	// Description:
	//    Returns pointer to the user defined function to create UserProxy.
	// Returns:
	//    Return ContainerCreateFunc function pointer.
	virtual void* GetUserProxyData() const = 0;

	// Description:
	//    Assign pointer to the user defined function to create UserProxy.
	//    If assigned it will be called on each SpawnEntity call to create corresponding user proxy.
	// Arguments:
	//    pFunc - User defined callback function that will be called by entity system with entity of this class is created.
	//    pUserData - Any user data pointer that is passed as a last parameter to the user defined function callback.
	virtual void SetUserProxyCreateFunc( UserProxyCreateFunc pFunc,void *pUserData=NULL ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Events info for this entity class.
	//////////////////////////////////////////////////////////////////////////
	enum EventValueType
	{
		EVT_INT,
		EVT_FLOAT,
		EVT_BOOL,
		EVT_VECTOR,
		EVT_ENTITY,
		EVT_STRING
	};
	struct SEventInfo
	{
		const char *name;			// Name of event.
		EventValueType type;  // Type of event value.
		bool bOutput;					// Input or Output event.
	};
	
	// Description:
	//    Returns number of input and output events defiend in the entity script.
	// See Also:
	//    GetEventInfo
	// Returns:
	//    Return Number of events.
	virtual int GetEventCount() = 0;

	// Description:
	//    Retrieve information about input/output event of the entity.
	// See Also:
	//    SEventInfo, GetEventCount
	// Arguments:
	//    nIndex - Index of the event to retrieve, must be in 0 to GetEventCount()-1 range.
	// Returns:
	//    Specified event description in SEventInfo structure.
	virtual SEventInfo GetEventInfo( int nIndex ) = 0;

	// Description:
	//    Find event by name.
	// See Also:
	//    SEventInfo, GetEventInfo
	// Arguments:
	//    sEvent - Name of the event.
	//    event - Output parameter for event.
	// Returns:
	//    True if event found and event parameter is initialized.
	virtual bool FindEventInfo( const char *sEvent,SEventInfo &event ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    This interface is the repository of the the various entity classes, it allows
//    creation and modification of entities types.
//    There`s only one IEntityClassRegistry interface can exist per EntitySystem.
//    Every entity class that can be spawned must be registered in this interface.
// See Also:
//    IEntitySystem::GetClassRegistry
//////////////////////////////////////////////////////////////////////////
struct IEntityClassRegistry
{
	// Description:
	//    Register a new entity class.
	// Returns:
	//    true if successfully registered.
	virtual bool RegisterClass( IEntityClass *pClass ) = 0;
	
	// Description:
	//    Unregister an entity class.
	// Returns:
	//    true if successfully unregistered.
	virtual bool UnregisterClass( IEntityClass *pClass ) = 0;

	// Description:
	//    Retrieves pointer to the IEntityClass interface by entity class name.
	// Returns:
	//    Pointer to the IEntityClass interface, or NULL if class not found.
	virtual IEntityClass* FindClass( const char *sClassName ) const = 0;

	// Description:
	//    Retrieves pointer to the IEntityClass interface for a default entity class.
	// Returns:
	//    Pointer to the IEntityClass interface, It can never return NULL.
	virtual IEntityClass* GetDefaultClass() const = 0;

	// Description:
	//    Load all entity class description xml files with extension ".ent" from specified directory.
	// Arguments:
	//    sPath - Path where to search for .ent files.
	virtual void LoadClasses( const char *sPath,bool bOnlyNewClasses=false ) = 0;

	struct SEntityClassDesc
	{
		SEntityClassDesc() : flags(0),sName(""),sScriptFile(""),pUserProxyCreateFunc(NULL),pUserProxyData(NULL) {};

		int           flags;
		const char*   sName;
		const char*   sScriptFile;
		IEntityClass::UserProxyCreateFunc pUserProxyCreateFunc;
		void*         pUserProxyData;
	};

	// Description:
	//    Register standard entity class, if class id not specified (is zero), generate a new class id.
	// Returns:
	//    Pointer to the new created and registered IEntityClass interface, or NULL if failed.
	virtual IEntityClass* RegisterStdClass( const SEntityClassDesc &entityClassDesc ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Registry iterator.
	//////////////////////////////////////////////////////////////////////////

	// Description:
	//    Move the entity class iterator to the begin of the registry.
	//    To iterate over all entity classes, ex:
	//    ...
	//    IEntityClass *pClass = NULL;
	//    for (pEntityRegistry->IteratorMoveFirst(); pClass = pEntityRegistry->IteratorNext();;)
	//    {
	//       pClass
	//    ...
	//    }
	virtual void IteratorMoveFirst() = 0;
	
	// Description:
	//    Get the next entity class in the registry.
	// Returns:
	//    Return a pointer to the next IEntityClass interface, or NULL if is the end
	virtual IEntityClass* IteratorNext() = 0;
	
	// Description:
	//    Return the number of entity classes in the registry.
	// Returns:
	//    Return a pointer to the next IEntityClass interface, or NULL if is the end
	virtual int GetClassCount() const = 0;
};


#endif // __IEntityClass_h__