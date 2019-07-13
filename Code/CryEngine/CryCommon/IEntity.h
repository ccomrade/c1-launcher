////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IEntity.h
//  Version:     v1.00
//  Created:     18/5/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IEntity_h__
#define __IEntity_h__
#pragma once

//////////////////////////////////////////////////////////////////////////
// forward declarations.
//////////////////////////////////////////////////////////////////////////
struct IPhysicalEntity;
struct IEntityClass;
struct IEntityArchetype;
class  CStream;
class  CCamera;
struct EntityCloneState;
struct IEntity;
struct SEntityUpdateContext;
struct IEntitySerializationContext;
struct IScriptTable;
struct AABB;
struct ISound;
class  XmlNodeRef;
class  CDLight;
struct AIObjectParameters;
struct SFogVolumeProperties;
struct IParticleEffect;
struct SpawnParams;

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class CSmartObject;
struct IAIObject;
struct IMaterial;
//////////////////////////////////////////////////////////////////////////

#define FORWARD_DIRECTION Vec3(0,1,0)

// !!! don't change the type !!!
typedef unsigned int EntityId;					//! unique identifier for each entity instance

//////////////////////////////////////////////////////////////////////////
// Unique Entity identifier, only used for saved entities.
//////////////////////////////////////////////////////////////////////////
typedef uint64 EntityGUID;

enum EPartIds 
{
  PARTID_LINKED = 1000,
  PARTID_CGA = 1000000
};

//////////////////////////////////////////////////////////////////////////
#include <IEntityProxy.h>
#include <SerializeFwd.h>
//////////////////////////////////////////////////////////////////////////

struct SEntitySpawnParams
{
	//! The Entity unique identifier (EntityId)
	EntityId      id;

	// Optional entity guid.
	EntityGUID    guid;

	// Class of entity.
	IEntityClass* pClass;
	
	/// Entity archetype.
	IEntityArchetype *pArchetype;

	// The name of the layer the entity resides in, when in the Editor
	const char *sLayerName;

	//////////////////////////////////////////////////////////////////////////
	// Initial Entity parameters.
	//////////////////////////////////////////////////////////////////////////
	//! The name of the entity... does not need to be unique
	const char*   sName;

	// Entity Flags.
	uint32        nFlags;				// e.g. ENTITY_FLAG_CASTSHADOW
	// spawn lock
	bool					bIgnoreLock;
	// to support save games compatible with patched levels (patched levels might use more EntityIDs and save game might conflict with dynamic ones)
	bool					bStaticEntityId;
	// Initial entity position (Local space).
	Vec3          vPosition;
	// Initial entity rotation (Local space).
	Quat          qRotation;
	// Initial entity scale (Local space).
	Vec3          vScale;
	// Any user defined data. It will be available for container when it will be created.
	void*         pUserData;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Optional properties table.
	//////////////////////////////////////////////////////////////////////////
	IScriptTable *pPropertiesTable;
	IScriptTable *pPropertiesInstanceTable;
	//////////////////////////////////////////////////////////////////////////


	SEntitySpawnParams()
	{
		id = 0;
		nFlags = 0;
		bIgnoreLock = false;
		pClass = NULL;
		sName = "";
		sLayerName = "";
		pArchetype = NULL;
		vPosition.Set(0,0,0);
		qRotation.SetIdentity();
		vScale.Set(1,1,1);
		pUserData = 0;
		guid = 0;
		pPropertiesTable = 0;
		pPropertiesInstanceTable = 0;
		bStaticEntityId=false;
	}
};

// Summary
//	Entity Update context structure
struct SEntityUpdateContext
{
	// Current rendering frame id
	int nFrameID;
	// Current camera.
	CCamera *pCamera;
	// Current system time.
	float fCurrTime;
	// Delta frame time (of last frame)
	float fFrameTime;
	// Indicates if a profile entity must update the log
	bool bProfileToLog;
	// Number of updated entities.
	int numUpdatedEntities;
	// Number of visible and updated entities.
	int numVisibleEntities;
	// Maximal view distance
	float fMaxViewDist;
	// Maximal view distance squared
	float fMaxViewDistSquared;
	// Camera source position
	Vec3 vCameraPos;

	// Initialization ctor.
	SEntityUpdateContext() : nFrameID(0),pCamera(0),fCurrTime(0),
		bProfileToLog(false),numVisibleEntities(0),numUpdatedEntities(0),fMaxViewDist(1e+8),fFrameTime(0) {};
};

//////////////////////////////////////////////////////////////////////////
enum EEntityXFormFlags
{
	ENTITY_XFORM_POS          = BIT(1),
	ENTITY_XFORM_ROT          = BIT(2),
	ENTITY_XFORM_SCL          = BIT(3),
	ENTITY_XFORM_FROM_PARENT  = BIT(5),  // When parent changes his transformation.
	ENTITY_XFORM_PHYSICS_STEP = BIT(13),
	ENTITY_XFORM_EDITOR       = BIT(14),
	ENTITY_XFORM_TRACKVIEW    = BIT(15),
	ENTITY_XFORM_TIMEDEMO     = BIT(16),
	ENTITY_XFORM_NOT_REREGISTER= BIT(17), // An optimization flag, when set object will not be re-registered in 3D engine.
	ENTITY_XFORM_USER         = 0x1000000,
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    EEntityEvent defines all events that can be sent to an entity.
//////////////////////////////////////////////////////////////////////////
enum EEntityEvent
{
	// Sent when the entity local or world transformation matrix change (position/rotation/scale).
	// nParam[0] = combination of the EEntityXFormFlags.
	ENTITY_EVENT_XFORM,
	// Sent when the entity timer expire.
	// nParam[0] = TimerId, nParam[1] = milliseconds.
	ENTITY_EVENT_TIMER,
	
	// Sent for unremovable entities when they are respawn.
	ENTITY_EVENT_INIT,

	// Sent before entity is removed.
	ENTITY_EVENT_DONE,

	// Sent when the entity becomes visible or invisible.
	// nParam[0] is 1 if the entity becomes visible or 0 if the entity becomes invisible.
	ENTITY_EVENT_VISIBLITY,
	// Sent to reset the state of the entity (used from Editor).
  // nParam[0] is 1 if entering gamemode, 0 if exiting
	ENTITY_EVENT_RESET,
	// Sent to parent entity after child entity have been attached.
	// nParam[0] contains ID of child entity.
	ENTITY_EVENT_ATTACH,
	// Sent to parent entity after child entity have been detached.
	// nParam[0] contains ID of child entity.
	ENTITY_EVENT_DETACH,
  // Sent to child entity after it has been detached from the parent.  
  ENTITY_EVENT_DETACH_THIS,
	// Sent when the entity must be hidden.
	ENTITY_EVENT_HIDE,
	// Sent when the entity must become not hidden.
	ENTITY_EVENT_UNHIDE,
	// Sent when a physics processing for the entity must be enabled/disabled.
	// nParam[0] == 1 physics must be enabled if 0 physics must be disabled.
	ENTITY_EVENT_ENABLE_PHYSICS,
	// Sent when a physics in an entity changes state.
	// nParam[0] == 1 physics entity awakes, 0 physics entity get to a sleep state.
	ENTITY_EVENT_PHYSICS_CHANGE_STATE,

	// Sent when script is broadcasting its events.
	// nParam[0] = Pointer to the ASCIIZ string with the name of the script event.
	// nParam[1] = Type of the event value from IEntityClass::EventValueType.
	// nParam[2] = Pointer to the event value depending on the type.
	ENTITY_EVENT_SCRIPT_EVENT,

	// Sent when triggering entity enters to the area proximity, this event sent to all target entities of the area.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_ENTERAREA,
	// Sent when triggering entity leaves the area proximity, this event sent to all target entities of the area.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_LEAVEAREA,
	// Sent when triggering entity is near to the area proximity, this event sent to all target entities of the area.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_ENTERNEARAREA,
	// Sent when triggering entity leaves the near area within proximity region of the outside area border.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_LEAVENEARAREA,
	// Sent when triggering entity moves inside the area within proximity region of the outside area border.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area
	ENTITY_EVENT_MOVEINSIDEAREA,
	// Sent when triggering entity moves inside an area of higher priority then the area this entity is linked to.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area with low prio, nParam[3] = EntityId of Area with high prio
	//ENTITY_EVENT_EXCLUSIVEMOVEINSIDEAREA,
	// Sent when triggering entity moves inside the area within the near region of the outside area border.
	// nParam[0] = TriggerEntityId, nParam[1] = AreaId, nParam[2] = EntityId of Area, fParam[0] = FadeRatio (0-1)
	ENTITY_EVENT_MOVENEARAREA,

	// Sent when an entity with pef_monitor_poststep receives a poststep notification (the hamdler should be thread safe!)
	// fParam[0] = time interval
	ENTITY_EVENT_PHYS_POSTSTEP,
	
	// Sent when Breakable object is broken in physics.
	ENTITY_EVENT_PHYS_BREAK,

	// Sent when AI object of the entity finished executing current order/action.
	ENTITY_EVENT_AI_DONE,

	// Sent when a sound finished or was stopped playing.
	ENTITY_EVENT_SOUND_DONE,

	// Sent when an entity has not been rendered for a while (time specified via cvar "es_not_seen_timeout")
	// - only if ENTITY_FLAG_SEND_NOT_SEEN_TIMEOUT is set.
	ENTITY_EVENT_NOT_SEEN_TIMEOUT,

	// Physical collision
	ENTITY_EVENT_COLLISION,

	// Called when entity is rendered (Only if ENTITY_FLAG_SEND_RENDER_EVENT is set)
	// nParam[0] is a pointer to the current rendering SRenderParams structure.
	ENTITY_EVENT_RENDER,

	// Called when the pre-physics update is done; fParam[0] is the frame time
	ENTITY_EVENT_PREPHYSICSUPDATE,

	// Called when the level is started
	ENTITY_EVENT_START_LEVEL,

	// Called when the game is started (games may start multiple times) 
	ENTITY_EVENT_START_GAME,

	// Called when the entity enters a script state
	ENTITY_EVENT_ENTER_SCRIPT_STATE,

	// Called when the entity leaves a script state
	ENTITY_EVENT_LEAVE_SCRIPT_STATE,

	// Called before we serialized the game from file
	ENTITY_EVENT_PRE_SERIALIZE,

	// Called after we serialized the game from file
	ENTITY_EVENT_POST_SERIALIZE,

	// Called when the entity becomes invisible - see the function Invisible()
	ENTITY_EVENT_INVISIBLE,

	// Called when the entity gets out of invisibility - see the function Invisible()
	ENTITY_EVENT_VISIBLE,

	// Called when the entity material change.
	// nParam[0] = pointer to the new IMaterial.
	ENTITY_EVENT_MATERIAL,

  // Called when the entitys material layer mask changes.
  ENTITY_EVENT_MATERIAL_LAYER,

	// Called when the entity gets hits by a weapon.
	ENTITY_EVENT_ONHIT,

	// Called when the entity being picked up by players.
	// nParam[0] : 1 - Pickup.
	//             0 - Throw after Pickup
	// nParam[1] : EntityId of who picked it up.
	// fParam[0] : Force of the throw.
	ENTITY_EVENT_PICKUP,

	// Called when an animation event (placed on animations in editor) is encountered
	// nParam[0]: AnimEventInstance* pEventParameters
	ENTITY_EVENT_ANIM_EVENT,

	// Called from ScriptBind_Entity when script requests to set collidermode.
	// nParam[0]: ColliderMode
	ENTITY_EVENT_SCRIPT_REQUEST_COLLIDERMODE,

	// Last entity event in list.
	ENTITY_EVENT_LAST,
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    SEntityEvent structure describe event id and parameters that can be sent to an entity.
//////////////////////////////////////////////////////////////////////////
struct SEntityEvent
{
	SEntityEvent( const int n0, const int n1, const int n2, const int n3, const float f0, const float f1) { nParam[0]=n0; nParam[1]=n1; nParam[2]=n2; nParam[3]=n3; fParam[0]= f0; fParam[1]=f1; };
	SEntityEvent() { event=ENTITY_EVENT_LAST; nParam[0]=0;nParam[1]=0;nParam[2]=0;nParam[3]=0; fParam[0] = fParam[1] = 0; };
	SEntityEvent( EEntityEvent _event ) : event(_event) { nParam[0]=0;nParam[1]=0;nParam[2]=0;nParam[3]=0; fParam[0] = fParam[1] = 0; };

	// Any event from EEntityEvent enum.
	EEntityEvent event;
	// Event parameters.
	INT_PTR nParam[4];
	float fParam[2];
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    Update policy defines in which cases to call entity update function every frame.
//////////////////////////////////////////////////////////////////////////
enum EEntityUpdatePolicy
{
	ENTITY_UPDATE_NEVER,			//! Never update entity every frame.
	ENTITY_UPDATE_IN_RANGE,			//! Only update entity if it is in specified range from active camera.
	ENTITY_UPDATE_POT_VISIBLE,		//! Only update entity if it is potentially visible.
	ENTITY_UPDATE_VISIBLE,			//! Only update entity if it is visible.
	ENTITY_UPDATE_PHYSICS,			//! Only update entity if it is need to be updated due to physics.
	ENTITY_UPDATE_PHYSICS_VISIBLE,	//! Only update entity if it is need to be updated due to physics or if it is visible.
	ENTITY_UPDATE_ALWAYS,			//! Always update entity every frame.
};


//////////////////////////////////////////////////////////////////////////
// Description:
//    Flags the can be set on entity with SetFlags/GetFlags method.
//////////////////////////////////////////////////////////////////////////
enum EEntityFlags
{
	//////////////////////////////////////////////////////////////////////////
	// Persistent flags (can be set from the editor).
	//////////////////////////////////////////////////////////////////////////
	ENTITY_FLAG_CASTSHADOW               = BIT(1),
	ENTITY_FLAG_UNREMOVABLE              = BIT(2),   // This entity cannot be removed using IEntitySystem::RemoveEntity until this flag is cleared.
	ENTITY_FLAG_GOOD_OCCLUDER            = BIT(3),

	//////////////////////////////////////////////////////////////////////////
	ENTITY_FLAG_WRITE_ONLY               = BIT(5),
	ENTITY_FLAG_NOT_REGISTER_IN_SECTORS  = BIT(6),
	ENTITY_FLAG_CALC_PHYSICS             = BIT(7),
	ENTITY_FLAG_CLIENT_ONLY              = BIT(8),
	ENTITY_FLAG_SERVER_ONLY              = BIT(9),
	ENTITY_FLAG_CUSTOM_VIEWDIST_RATIO    = BIT(10),   // This entity have special custom view distance ratio (AI/Vehicles must have it).
	ENTITY_FLAG_CALCBBOX_USEALL          = BIT(11),		// use character and objects in BBOx calculations
	ENTITY_FLAG_VOLUME_SOUND					   = BIT(12),   // Entity is a volume sound (will get moved around by the sound proxy)
	ENTITY_FLAG_HAS_AI         					 = BIT(13),		// Entity has an AI object
	ENTITY_FLAG_TRIGGER_AREAS            = BIT(14),   // This entity will trigger areas when it enters them.
	ENTITY_FLAG_NO_SAVE                  = BIT(15),   // This entity will not be saved.
	ENTITY_FLAG_NET_PRESENT              = BIT(16),   // This entity Must be present on network.
	ENTITY_FLAG_CLIENTSIDE_STATE         = BIT(17),   // Prevents error when state changes on the client and does not sync state changes to the client.
	ENTITY_FLAG_SEND_RENDER_EVENT        = BIT(18),   // When set entity will send ENTITY_EVENT_RENDER every time its rendered.
	ENTITY_FLAG_NO_PROXIMITY             = BIT(19),   // Entity will not be registered in the partition grid and can not be found by proximity queries.
	ENTITY_FLAG_ON_RADAR                 = BIT(20),   // Entity will be relevant for radar.
	ENTITY_FLAG_UPDATE_HIDDEN					   = BIT(21),   // Entity will be update even when hidden.  
	ENTITY_FLAG_NEVER_NETWORK_STATIC	   = BIT(22),   // Entity should never be considered a static entity by the network system
	ENTITY_FLAG_IGNORE_PHYSICS_UPDATE    = BIT(23),	  // Used by Editor only, (dont set)
	ENTITY_FLAG_SPAWNED								   = BIT(24),	 // Entity was spawned dynamically without a class
	ENTITY_FLAG_SLOTS_CHANGED					   = BIT(25),	 // Entity's slots were changed dynamically
	ENTITY_FLAG_MODIFIED_BY_PHYSICS		   = BIT(26),  // Entity was procedurally modified by physics
	ENTITY_FLAG_OUTDOORONLY						   = BIT(27),  // same as Brush->Outdoor only
	ENTITY_FLAG_SEND_NOT_SEEN_TIMEOUT    = BIT(28),  // Entity will be sent ENTITY_EVENT_NOT_SEEN_TIMEOUT if it is not rendered for 30 seconds
  ENTITY_FLAG_RECVWIND	    				   = BIT(29),  // Receives wind
	ENTITY_FLAG_LOCAL_PLAYER             = BIT(30),
	ENTITY_FLAG_AI_HIDEABLE              = BIT(31),  // AI can use the object to calculate automatic hide points.

};

//////////////////////////////////////////////////////////////////////////
// Description:
//    Flags the can be passed to IEntity::Serialize()
//////////////////////////////////////////////////////////////////////////
enum EEntitySerializeFlags
{
	// serialize proxies
	ENTITY_SERIALIZE_PROXIES    = BIT(1),
	// serialize properties common to all entities (position, rotation, scale)
	ENTITY_SERIALIZE_POSITION   = BIT(2),
	ENTITY_SERIALIZE_ROTATION   = BIT(3),
	ENTITY_SERIALIZE_SCALE      = BIT(4),
	ENTITY_SERIALIZE_GEOMETRIES = BIT(5),
	ENTITY_SERIALIZE_PROPERTIES = BIT(6)
};

enum EEntityGetSetSlotFlags
{
	ENTITY_SLOT_ACTUAL = 0x40000000
};

struct IEntityLink
{
	char         name[16]; // Name of the link.
	EntityId     entityId; // Entity targeted by the link.
	IEntityLink* next;     // Pointer to the next link, or NULL if last link.
};

// Description:
//    Interface to entity object.
struct IEntity
{
	enum EEntityLoadFlags
	{
		EF_AUTO_PHYSICALIZE = 0x0001,
	};
	enum EAttachmentFlags
	{
		// Keep world transformation of entity when attaching or detaching it
		ATTACHMENT_KEEP_TRANSFORMATION = 0x01,
	};

	// Description:
	//     Retrieves the runtime unique identifier of this entity assigned to it by the Entity System.
	//     EntityId may not be the same when saving/loading entity.
	//     EntityId is mostly used in runtime for fast and unique identificatoin of entities..
	// Return:
	//     The entity ID.
	virtual EntityId GetId() const = 0;

	// Description:
	//     Retrieves the globally unique identifier of this entity assigned to it by the Entity System.
	//     EntityGuid is garanted to be the same  when saving/loading entity, it is also same in the editor and in the game.
	// Return:
	//     The entity globally unique identifier.
	virtual EntityGUID GetGuid() const = 0;

	// Description:
	//     Retrieves the entity class pointer.
	//     Entity class defines entity type, what script it will use, etc...
	// Return:
	//     Pointer to the entity class interface.
	virtual IEntityClass* GetClass() const = 0;

	// Description:
	//     Retrieves the entity archetype.
	//     Entity archetype contain definition for entity script properties.
	// Return:
	//     Pointer to the entity archetype interface.
	virtual IEntityArchetype* GetArchetype() = 0;

	// Description:
	//     Set entity flags, completly replaces all flags which are already set in the entity.
	// Arguments:
	//     flags - flag values which are defined in EEntityFlags
	virtual void SetFlags( uint32 flags ) = 0;
	// Description:
	//     Get current entity flags.
	virtual uint32 GetFlags() const = 0;
	// Description:
	//     Adds flag/s to the current set of entity flags (logical OR).
	// Arguments:
	//     flagsToAdd - Combination of bit flags to add.
	virtual void AddFlags( uint32 flagsToAdd ) = 0;
	// Description:
	//     Removes flag/s from the current set of entity flags (logical AND NOT).
	// Arguments:
	//     flagsToClear - Combination of bit flags to remove.
	virtual void ClearFlags( uint32 flagsToClear ) = 0;
	// Description:
	//     Check if the specified entity flag is set.
	// Arguments:
	//     flagsToCheck - Combination of bit flags to check.
	virtual bool CheckFlags( uint32 flagsToCheck ) const = 0;

	// Description:
	//     Check if this entity was marked for deletion.
	//     If this function returns true, it will be deleted on next frame, and it is pointless to perform any operations on such entity.
	// Returns:
	//     True if entity marked for deletion, false otherwise.
	virtual bool IsGarbage() const = 0;

	// Description:
	//     Change the entity name.
	//     Entity name does not have to be unique, but for the sake of easier finding entities by name it is better to not 
	//     assign same name to different entities.
	// See Also:
	//     GetName
	// Arguments:
	//     sName - New name for the entity.
	virtual void SetName( const char *sName ) = 0;
	
	// Description:
	//     Get entity name.
	// See Also:
	//     SetName
	// Returns:
	//     Name of the entity.
	virtual const char* GetName() const = 0;

	// Return textual description of entity for logging.
	virtual const char* GetEntityTextDescription() const = 0;

	// Description:
	//    Serialize entity parameters to/from XML.
	virtual void SerializeXML( XmlNodeRef &entityNode,bool bLoading ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Entity Hierarchy.
	//////////////////////////////////////////////////////////////////////////
	
	// Description:
	//     Attach the child entity to this entity.
	//     The child entity will inherit all the transformation of the parent entity.
	// See Also: DetachThis,DetachAll
	// Arguments:
	//     pChildEntity - Child entity to attach.
	//     nAttachFlags - Combination of the EAttachmentFlags flags.
	virtual void AttachChild( IEntity *pChildEntity,int nAttachFlags=0 ) = 0;
	// Description:
	//     Detach all the child entities attached to this entity.
	// See Also: DetachThis,AttachChild
	// Arguments:
	//     nDetachFlags - Combination of the EAttachmentFlags flags.
	virtual void DetachAll( int nDetachFlags=0  ) = 0;
	// Description:
	//     Detach this entity from the parent entity (assumes that this entity is the child entity).
	// See Also: AttachChild,DetachAll,GetParent
	// Arguments:
	//     nDetachFlags - Combination of the EAttachmentFlags flags.
	virtual void DetachThis( int nDetachFlags=0,int nWhyFlags=0  ) = 0;
	
	// Description:
	//     Retrieves the number of attached child entities.
	// See Also: GetChild,GetParent
	virtual int  GetChildCount() const = 0;
	// Description:
	//     Retrieves the attached child entity by index.
	// See Also: GetChildCount,GetParent
	// Arguments:
	//     nIndex - Index of the child entity, must be 0 <= nIndex < GetChildCount().
	// Return:
	//     Pointer to the child entity interface.
	virtual IEntity* GetChild( int nIndex ) const = 0;
	
	// Description:
	//     Retrieves the parent of this entity.
	// See Also: GetChild,DetachThis
	// Return:
	//     Pointer to the parent entity interface, or NULL if this entity does not have a parent.
	virtual IEntity* GetParent() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Entity transformation.
	//////////////////////////////////////////////////////////////////////////
	
	// Description:
	//     Set entity transformation materix in the world space.
	// See Also: SetWorldTM,GetWorldTM,SetLocalTM,GetLocalTM
	// Arguments:
	//     tm - World space transformation matrix (Non unifrorm scale is not supported).
	virtual void SetWorldTM( const Matrix34 &tm,int nWhyFlags=0 ) = 0;
	// Description:
	//     Set entity transformation matrix in the local entity space.
	// See Also: SetWorldTM,GetWorldTM,SetLocalTM,GetLocalTM
	// Arguments:
	//     tm - Local space transformation matrix (Non unifrorm scale is not supported).
	virtual void SetLocalTM( const Matrix34 &tm,int nWhyFlags=0 ) = 0;
	// Description:
	//     Retrieves the entity transformation matrix in the world space.
	// See Also: SetWorldTM,GetWorldTM,SetLocalTM,GetLocalTM
	// Return:
	//     World space transformation matrix (Include transformations of all parent entities).
	virtual const Matrix34& GetWorldTM() const = 0;
	// Description:
	//     Retrieves the entity transformation matrix in the local entity space.
	// See Also: SetWorldTM,GetWorldTM,SetLocalTM,GetLocalTM
	// Return:
	//     Local space transformation matrix.
	virtual Matrix34 GetLocalTM() const = 0;

	// Description:
	//     Retrieves the entity axis aligned bounding box in the world space.
	// See Also: GetLocalBounds
	// Arguments:
	//     bbox - Output parameter for the bounding box.
	virtual void GetWorldBounds( AABB &bbox ) = 0;
	// Description:
	//     Retrieves the entity axis aligned bounding box in the local entity space.
	// See Also: GetLocalBounds
	// Arguments:
	//     bbox - Output parameter for the bounding box.
	virtual void GetLocalBounds( AABB &bbox ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Set the entity local space position.
	// See Also: SetPos,GetPos,SetRotation,GetRotation,SetScale,GetScale
	virtual void SetPos( const Vec3 &vPos,int nWhyFlags=0 ) = 0;
	// Description:
	//     Retrieves the entity local space position.
	// See Also: SetPos,GetPos,SetRotation,GetRotation,SetScale,GetScale
	virtual const Vec3& GetPos() const = 0;

	// Description:
	//     Set the entity local space rotation quaternion.
	// See Also: SetPos,GetPos,SetRotation,GetRotation,SetScale,GetScale
	virtual void SetRotation( const Quat &qRotation,int nWhyFlags=0 ) = 0;
	// Description:
	//     Retrieves the entity local space rotation quaternion.
	// See Also: SetPos,GetPos,SetRotation,GetRotation,SetScale,GetScale
	virtual const Quat& GetRotation() const = 0;

	// Description:
	//     Set the entity local space scale.
	// See Also: SetPos,GetPos,SetRotation,GetRotation,SetScale,GetScale
	virtual void SetScale( const Vec3 &vScale,int nWhyFlags=0 ) = 0;
	// Description:
	//     Retrieves the entity local space scale.
	// See Also: SetPos,GetPos,SetRotation,GetRotation,SetScale,GetScale
	virtual const Vec3& GetScale() const = 0;

	// Description:
	//     Set the entity position,rotation and scale at once.
	// See Also: SetPos,GetPos,SetRotation,GetRotation,SetScale,GetScale
	virtual void SetPosRotScale( const Vec3 &vPos,const Quat &qRotation,const Vec3 &vScale,int nWhyFlags=0 ) = 0;

	// Description:
	//     Helper function to retrieve world space entity position.
	// See Also: GetPos,GetWorldTM
	virtual Vec3 GetWorldPos() const = 0;
	// Description:
	//     Helper function to retrieve world space entity orientation angles.
	// See Also: GetRotation,GetWorldTM
	virtual Ang3 GetWorldAngles() const = 0;
	// Description:
	//     Helper function to retrieve world space entity orientation quaternion
	// See Also: GetRotation,GetWorldTM
	virtual Quat GetWorldRotation() const = 0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Activates entity, if entity is active it will be updated every frame.
	// See Also: IsActive
	virtual void Activate( bool bActive ) = 0;
	// Description:
	//     Check if the entity is active now.
	// See Also: Activate
	virtual bool IsActive() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Activates entity, if entity is active it will be updated every frame.
	// See Also: IsActive
	virtual void PrePhysicsActivate( bool bActive ) = 0;
	// Description:
	//     Check if the entity is active now.
	// See Also: Activate
	virtual bool IsPrePhysicsActive() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Save or Load entity parameters to/from stream using serialization context class.
	// Arguments:
	//     serializer - Serialization context class, provides all the information needed to properly
	//                   serialize entity contents.
	//     nFlags - Additional custom serialization flags.
	virtual void Serialize( TSerialize serializer, int nFlags=0 ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Send event to the entity.
	// Arguments:
	//     event - Event description (event id, parameters).
	virtual bool SendEvent( SEntityEvent &event ) = 0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Starts the entity timer, entity timers are owned by entity, every entity can have it`s own 
	//     independet timers, so TimerId must not be unique across multiple entities.
	//     When timer finishes entity system will signal to the entity *once* with an event ENTITY_EVENT_TIME.
	//     Multiple timers can be started simultaniously with different timer ids.
	//     If some timer is not yet finished and another timer with the same timerID is set, the new one
	//     will override old timer, and old timer will not send finish event.
	// See Also: KillTimer
	// Arguments:
	//     nTimerId - Timer ID, multiple timers with different IDs are possible.
	//     nMilliSeconds - Timer timeout time in milliseconds.
	virtual void SetTimer( int nTimerId,int nMilliSeconds ) = 0;
	// Description:
	//     Stop already started entity timer with this id.
	// See Also: SetTimer
	// Arguments:
	//     nTimerId - Timer ID of the timer started for this entity.
	virtual void KillTimer( int nTimerId ) = 0;

	// Description:
	//     Hides this entity, makes it invisible and disable its physics.
	// See Also: IsHidden
	// Arguments:
	//     bHide - if true hide the entity, is false unhides it.
	virtual void Hide( bool bHide ) = 0;
	// Description:
	//     Checks if the entity is hidden.
	// See Also: Hide
	virtual bool IsHidden() const = 0;

	// Description:
	//    Makes the entity invisible and disable its physics.	
	//		Different from hide in that the entity is still updated.
	virtual void Invisible( bool bInvisible ) = 0;
	// Description:
	//     Checks if the entity is invisible.
	// See Also: Hide
	virtual bool IsInvisible() const = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual CSmartObject* GetSmartObject() const = 0;
	virtual void SetSmartObject( CSmartObject* pSmartObject ) = 0;
	//////////////////////////////////////////////////////////////////////////
	virtual IAIObject* GetAI() = 0;
	//////////////////////////////////////////////////////////////////////////
	virtual bool RegisterInAISystem(unsigned short type, const AIObjectParameters &params) = 0;
	//////////////////////////////////////////////////////////////////////////

	// Description:
	//     Changes the entity update policy.
	//     Update policy of entity defines the automatic activation rules for the entity.
	//     Ex. When physical object becomes awaken it will activate the entity, and when will go to sleep
	//     again will deactivate it.
	//     Or entity can become active every time that it becomes visible, and deactivated when goes out of view.
	//     There are multiple such predefined update policies exist, consider EEntityUpdatePolicy enum.
	// See Also: GetUpdatePolicy
	// Arguments:
	//     eUpdatePolicy - Update policy, one of EEntityUpdatePolicy enums.
	virtual void SetUpdatePolicy( EEntityUpdatePolicy eUpdatePolicy ) = 0;
	// Description:
	//     Retrieves the entity update policy.
	// See Also: SetUpdatePolicy
	virtual EEntityUpdatePolicy GetUpdatePolicy() const = 0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Entity Proxies Interfaces access functions.
	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Retrieves a pointer to the specified proxy interface in the entity.
	// Arguments:
	//     proxy - Proxy interface identifier.
	// Return:
	//     Valid pointer to the requested entity proxy object, or NULL if such proxy not exist.
	virtual IEntityProxy* GetProxy( EEntityProxy proxy ) const = 0;
	// Description:
	//     Replaces thethe specified proxy interface in the entity.
	// Arguments:
	//     proxy - Proxy interface identifier.
	//     pProxy - Pointer to the Proxy interface.
	virtual void SetProxy(EEntityProxy proxy, IEntityProxy *pProxy) = 0;
	// Description:
	//     Creates a specified proxy in the entity.
	// Arguments:
	//     proxy - Proxy interface identifier.
	// Return:
	//     Valid pointer to the created entity proxy object, or NULL if creation failed.
	virtual IEntityProxy* CreateProxy( EEntityProxy proxy ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Physics.
	//////////////////////////////////////////////////////////////////////////
	virtual void Physicalize( SEntityPhysicalizeParams &params ) = 0;
	// Description:
	//    Returns a physical entity assigned to an entity.
	virtual IPhysicalEntity* GetPhysics() const = 0;

	virtual int PhysicalizeSlot(int slot, SEntityPhysicalizeParams &params) = 0;
	virtual void UnphysicalizeSlot(int slot) = 0;
	virtual void UpdateSlotPhysics(int slot) = 0;

	virtual void SetPhysicsState( XmlNodeRef & physicsState ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Custom entity material.
	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Assign a custom material to the entity.
	// Arguments:
	//     pMaterial - Pointer to custom material interface.
	virtual void SetMaterial( IMaterial *pMaterial ) = 0;

	// Description:
	//     Retrieve a custom material set to the entity.
	// Return:
	//     Pointer to custom material interface or NULL if custom material not set.
	virtual IMaterial* GetMaterial() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Working with Slots.
	//////////////////////////////////////////////////////////////////////////
	// Description:
	//     Check if the slot with specified index exist.
	// Arguments:
	//     nIndex - Index of required slot.
	virtual bool IsSlotValid( int nIndex ) const = 0;

	// Description:
	//    Free slot at specified index.
	virtual void FreeSlot( int nIndex ) = 0;

	// Description:
	//     Get number of allocated object slots in the entity.
	virtual int GetSlotCount() const = 0;

	// Description:
	//     Get object slot information.
	// Arguments:
	//     nIndex - Index of required slot.
	//     slotInfo - Output variable where description of the slot will be written.
	virtual bool GetSlotInfo( int nIndex,SEntitySlotInfo &slotInfo ) const = 0;

	// Returns world transformation matrix of the object slot.
	virtual const Matrix34& GetSlotWorldTM( int nSlot ) const = 0;
	
	// Description:
	//		Returns local transformation matrix relative to host entity transformation matrix of the object slot.
	// Arguments:
	//     nSlot - Index of required slot.
	//     bRelativeToParent - flag specifying whether the local transformation matrix is relative to the parent slot or the entity
	virtual const Matrix34& GetSlotLocalTM( int nSlot, bool bRelativeToParent) const = 0;
	// Set local transformation matrix of the object slot.
	virtual void SetSlotLocalTM( int nSlot,const Matrix34 &localTM,int nWhyFlags=0 ) = 0;

	// Description:
	//     Attach child slot to the parent slot.
	//     This will form hierarchical transformation relationship between object slots.
	// Arguments:
	//     nParentIndex - Index of the parent slot (Child slot will be attached to this one).
	//     nChildIndex  - Index of the child slot.
	virtual bool SetParentSlot( int nParentIndex,int nChildIndex ) = 0;

	// Description:
	//     Assign a custom material to the specified object slot.
	// Arguments:
	//     nSlot - Index of the slot, if -1 assign this material to all existing slots.
	//     pMaterial - Pointer to custom material interface.
	virtual void SetSlotMaterial( int nSlot,IMaterial *pMaterial ) = 0;

	// Description:
	//     Set the flags of the specified slot.
	// Arguments:
	//     nSlot - Index of the slot, if -1 apply to all existing slots.
	//     nFlags - Flags to set.
	virtual void SetSlotFlags( int nSlot,uint32 nFlags ) = 0;

	// Description:
	//     Retrieves the flags of the specified slot.
	// Arguments:
	//     nSlot - Index of the slot.
	// Return:
	//     The slot flags, or 0 if specified slot is not valid.
	virtual uint32 GetSlotFlags( int nSlot ) const = 0;


	// Description:
	//     Returns true if character is to be updated.
	// Arguments:
	//     nSlot - Index of the slot.
	// Return:
	//     Returns true if character is to be updated.
	virtual bool ShouldUpdateCharacter( int nSlot ) const = 0;

	// Description:
	//     Fast method to get the character at the specified slot.
	// Arguments:
	//     nSlot - Index of the slot.
	// Return:
	//     Character pointer or NULL if character with this slot does not exist.
	virtual ICharacterInstance* GetCharacter( int nSlot ) = 0;

	// Description:
	//     Sets character instance of a slot, and creates slot if necessary
	// Arguments:
	//     nSlot - Index of a slot, or -1 if a new slot need to be allocated
	//		 pCharacter - pointer to character instance
	// Return:
	//    An integer which refer to the slot index which used
	virtual int SetCharacter( ICharacterInstance *pCharacter, int nSlot ) = 0;

	// Description:
	//     Fast method to get the static object at the specified slot.
	// Arguments:
	//     nSlot - Index of the slot; | with ENTITY_SLOT_ACTUAL to disable compound statobj handling
	// Return:
	//     StatObj pointer or NULL if stat object with this slot does not exist.
	virtual IStatObj* GetStatObj( int nSlot ) = 0;

	// Description:
	//     Fast method to get the particle em at the specified slot.
	// Arguments:
	//     nSlot - Index of the slot.
	// Return:
	//     IParticleEmitter pointer or NULL if stat object with this slot does not exist.
	virtual IParticleEmitter* GetParticleEmitter( int nSlot ) = 0;

	// Description:
	//     Sets static object of a slot, creates slot if necessary
	// Arguments:
	//     nSlot - Index of a slot, or -1 if a new slot need to be allocated; 
	//						 | with ENTITY_SLOT_ACTUAL to disable compound statobj handling
	//		 pStatObj - pointer to the new static object
	//     mass - new mass of the slot, negative value to keep the current
	// Return:
	//    An integer which refer to the slot index which used
	virtual int SetStatObj( IStatObj *pStatObj, int nSlot,bool bUpdatePhysics, float mass=-1.0f) = 0;

	// Description:
	//     Load static geometry to the specified slot, or to next available slot.
	//     If same object is already loaded in this slot, operation is ignored.
	//     If this slot number is occupied by different kind of object it is overwritten with static object.
	//     nLoadFlags - @see ELoadFlags
	// Return:
	//     Slot id where the object was loaded, or -1 if loading failed.
	virtual int LoadGeometry( int nSlot,const char *sFilename,const char *sGeomName=NULL,int nLoadFlags=0 ) = 0;

	// Description:
	//     Loads character to the specified slot, or to next available slot.
	//     If same character is already loaded in this slot, operation is ignored.
	//     If this slot number is occupied by different kind of object it is overwritten.
	// Return:
	//     Slot id where the object was loaded, or -1 if loading failed.
	virtual int LoadCharacter( int nSlot,const char *sFilename,int nLoadFlags=0 ) = 0;

	// Description:
	//     Loads a new particle emitter to the specified slot, or to next available slot.
	//     If same character is already loaded in this slot, operation is ignored.
	//     If this slot number is occupied by different kind of object it is overwritten.
	// Return:
	//     Slot id where the particle emitter was loaded, or -1 if loading failed.
	virtual int	LoadParticleEmitter( int nSlot, IParticleEffect* pEffect, SpawnParams const* params = NULL, bool bPrime = false, bool bSerialize = false ) = 0;
	virtual int	SetParticleEmitter( int nSlot, IParticleEmitter* pEmitter, bool bSerialize = false ) = 0;

	// Description:
	//     Loads a light source to the specified slot, or to next available slot.
	// Return:
	//     Slot id where the light source was loaded, or -1 if loading failed.
	virtual int LoadLight( int nSlot,CDLight *pLight ) = 0;

	// Description:
	//     Loads a cloud XML file to the specified slot, or to next available slot.
	// Return:
	//     Slot id where the cloud was loaded, or -1 if loading failed.
	virtual int LoadCloud( int nSlot,const char *sCloudFilename ) = 0;

	// Description:
	//     Loads a fog volume XML file to the specified slot, or to next available slot.
	// Return:
	//     Slot id where the fog volume was loaded, or -1 if loading failed.
	virtual int LoadFogVolume( int nSlot, const SFogVolumeProperties& properties ) = 0;

	// Description:
	//     Invalidates the entity's and all its children's tranformation matrices!
	virtual void InvalidateTM( int nWhyFlags=0 ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Easy Script table access.
	//////////////////////////////////////////////////////////////////////////
	IScriptTable* GetScriptTable() const;

	// Description:
	//		Enable/Disable physics by flag
	virtual void EnablePhysics(bool enable) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Entity links.
	//////////////////////////////////////////////////////////////////////////
	// Get pointer to the first entity link.
	virtual IEntityLink* GetEntityLinks() = 0;
	virtual IEntityLink* AddEntityLink( const char *sLinkName,EntityId entityId ) = 0;
	virtual void RemoveEntityLink( IEntityLink* pLink ) = 0;
	virtual void RemoveAllEntityLinks() = 0;
	//////////////////////////////////////////////////////////////////////////

	// Description:
	//    Retuns an attached child entity that corresponds to the physical part partId
	//    set partId to the child's partId
	virtual IEntity *UnmapAttachedChild(int &partId) = 0;

	// Description:
	//    Returns true if entity is completely initialized
	virtual bool IsInitialized() const = 0;
};

//////////////////////////////////////////////////////////////////////////
// Inline implementation.
//////////////////////////////////////////////////////////////////////////
inline IScriptTable* IEntity::GetScriptTable() const
{
	IEntityScriptProxy* pScriptProxy = (IEntityScriptProxy*)GetProxy(ENTITY_PROXY_SCRIPT);
	if (pScriptProxy)
		return pScriptProxy->GetScriptTable();
	return NULL;
}

#endif // __IEntity_h__
