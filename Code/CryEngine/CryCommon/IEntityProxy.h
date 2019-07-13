////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IEntityProxy.h
//  Version:     v1.00
//  Created:     28/9/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: Definition of all proxy interfaces used by an Entity.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IEntityProxy_h__
#define __IEntityProxy_h__
#pragma once

#include <SerializeFwd.h>
#include "ISound.h" // needed for enum ESoundSemantic

struct SEntitySpawnParams;
struct SEntityEvent;
struct IShaderPublicParams;
struct IFlowGraph;
struct IEntityEventListener;
struct SSGHandle;

typedef uint32 tSoundID;


//////////////////////////////////////////////////////////////////////////
// Entity proxies that can be hosted by the entity.
//////////////////////////////////////////////////////////////////////////
enum EEntityProxy
{
	ENTITY_PROXY_RENDER,
	ENTITY_PROXY_PHYSICS,
	ENTITY_PROXY_SCRIPT,
	ENTITY_PROXY_SOUND,
	ENTITY_PROXY_AI,
	ENTITY_PROXY_AREA,
	ENTITY_PROXY_BOIDS,
	ENTITY_PROXY_BOID_OBJECT,
	ENTITY_PROXY_CAMERA,
	ENTITY_PROXY_FLOWGRAPH,
	ENTITY_PROXY_SUBSTITUTION,
	ENTITY_PROXY_TRIGGER,
	ENTITY_PROXY_ROPE,

	ENTITY_PROXY_USER,

	// Always the last entry of the enum.
	ENTITY_PROXY_LAST
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    Base interface to access to various entity proxy objects.
//////////////////////////////////////////////////////////////////////////
struct IEntityProxy
{
	virtual EEntityProxy GetType() = 0;

	// Called when the subsystem initialize.
	virtual void Init( IEntity *pEntity,SEntitySpawnParams &params ) = 0;

	// Called when the entity is destroyed! At this point, all proxies are valid! No memory should be deleted here!
	virtual void Done() = 0;

	// Description:
	//    When host entity is destroyed every proxy will be called with the Release method to delete itself.
	virtual void Release() = 0;

	// Description:
	//    Update will be called every time the host Entity is updated.
	// Arguments:
	//    ctx - Update context of the host entity, provides all the information needed to update the entity.
	virtual	void Update( SEntityUpdateContext &ctx ) = 0;

	// Description:
	//    By overriding this function proxy will be able to handle events sent from the host Entity.
	// Arguments:
	//    event - Event structure, contains event id and parameters.
	virtual	void ProcessEvent( SEntityEvent &event ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Serialize proxy to/from XML.
	virtual void SerializeXML( XmlNodeRef &entityNode,bool bLoading ) = 0;

	// Description:
	//    Serialize proxy with a TSerialize
	// Arguments:
	//    ser - the object to serialize with
	virtual void Serialize( TSerialize ser ) = 0;

	// Description:
	//    Returns true if this proxy need to be saved during serialization.
	// Returns:
	//    true - If proxy needs to be serialized
	virtual bool NeedSerialize() = 0;
};

//////////////////////////////////////////////////////////////////////////
// Script proxy interface.
//////////////////////////////////////////////////////////////////////////
struct IEntityScriptProxy : public IEntityProxy
{
	virtual void SetScriptUpdateRate( float fUpdateEveryNSeconds ) = 0;
	virtual IScriptTable* GetScriptTable() = 0;
	virtual void CallEvent( const char *sEvent ) = 0;
	virtual void CallEvent( const char *sEvent,float fValue ) = 0;
	virtual void CallEvent( const char *sEvent,bool bValue ) = 0;
	virtual void CallEvent( const char *sEvent,const char *sValue ) = 0;
	virtual void CallEvent( const char *sEvent,const Vec3 &vValue ) = 0;
	virtual void CallEvent( const char *sEvent,EntityId nEntityId ) = 0;

	// Description:
	//    Change current state of the entity script.
	// Return:
	//    If state was succesfully set.
	virtual bool GotoState( const char *sStateName ) = 0;

	// Description:
	//    Change current state of the entity script.
	// Return:
	//    If state was succesfully set.
	virtual bool GotoStateId( int nStateId ) = 0;

	// Description:
	//    Check if entity is in specified state.
	// Arguments:
	//    sStateName - Name of state table within entity script (case sensetive).
	// Return:
	//    If entity script is in specified state.
	virtual bool IsInState( const char *sStateName ) = 0;

	// Description:
	//    Retrieves name of the currently active entity script state.
	// Return:
	//    Name of current state.
	virtual const char* GetState() = 0;

	// Description:
	//    Retrieves the id of the currently active entity script state.
	// Return:
	//    Index of current state.
	virtual int GetStateId() = 0;

	// Description:
	//     Fires an event in the entity script.
	//     This will call OnEvent(id,param) Lua function in entity script, so that script can handle this event.
	// See Also:
	//     EScriptEventId, IScriptObject
	virtual void SendScriptEvent( int Event, IScriptTable *pParamters, bool *pRet=NULL) = 0;
	virtual void SendScriptEvent( int Event, const char *str, bool *pRet=NULL )=0;
	virtual void SendScriptEvent( int Event, int nParam, bool *pRet=NULL )=0;
};

//////////////////////////////////////////////////////////////////////////
// Parameters passed to IEntity::Physicalize function.
//////////////////////////////////////////////////////////////////////////
struct SEntityPhysicalizeParams
{
	//////////////////////////////////////////////////////////////////////////
	SEntityPhysicalizeParams() : type(0),density(-1),mass(-1),nSlot(-1),nFlagsOR(0),nFlagsAND(UINT_MAX),
		pAttachToEntity(NULL),nAttachToPart(-1),fStiffnessScale(0),
		pParticle(NULL),pBuoyancy(NULL),pPlayerDimensions(NULL),pPlayerDynamics(NULL),pCar(NULL),pAreaDef(NULL),nLod(0) {};
	//////////////////////////////////////////////////////////////////////////
	// Physicalization type must be one of pe_type ennums.
	// See Also: pe_type
	int type; // Always must be filled.

	// Index of object slot. -1 if all slots should be used.
	int nSlot;
	// Only one either density or mass must be set, parameter set to 0 is ignored.
	float density;
	float mass;
	// Optional physical flags.
	int nFlagsAND;
	int nFlagsOR;
	// When physicalizing geometry can specify to use physics from different LOD.
	// Used for characters that have ragdoll physics in Lod1
	int nLod;

	// Physical entity to attach this physics object (Only for Soft physical entity).
	IPhysicalEntity *pAttachToEntity;
	// Part ID in entity to attach to (Only for Soft physical entity).
	int nAttachToPart;
	// Used for character physicalization (Scale of force in character joint's springs).
	float fStiffnessScale;

	struct pe_params_particle   *pParticle;
	struct pe_params_buoyancy   *pBuoyancy;
	struct pe_player_dimensions *pPlayerDimensions;
	struct pe_player_dynamics   *pPlayerDynamics;
	struct pe_params_car        *pCar;     

	//////////////////////////////////////////////////////////////////////////
	// This parameters are only used when type == PE_AREA
	//////////////////////////////////////////////////////////////////////////
	struct AreaDefinition
	{
		enum EAreaType {
			AREA_SPHERE,        // Physical area will be sphere.
			AREA_BOX,						// Physical area will be box.
			AREA_GEOMETRY,			// Physical area will use geometry from the specified slot.
			AREA_SHAPE,			    // Physical area will points to specify 2D shape.
			AREA_CYLINDER,      // Physical area will be a cylinder
			AREA_SPLINE,				// Physical area will be a spline-tube
		};

		EAreaType areaType;
		float fRadius;      // Must be set when using AREA_SPHERE or AREA_CYLINDER area type or an AREA_SPLINE.
		Vec3 boxmin,boxmax; // Min,Max of bounding box, must be set when using AREA_BOX area type.
		Vec3 *pPoints;      // Must be set when using AREA_SHAPE area type or an AREA_SPLINE.
		int nNumPoints;     // Number of points in pPoints array.
		float zmin,zmax;    // Min/Max of points.
		Vec3 center;
		Vec3 axis;
		
		// pGravityParams must be a valid pointer to the area gravity params structure.
		struct pe_params_area *pGravityParams;

		AreaDefinition() : areaType(AREA_SPHERE),fRadius(0),boxmin(0,0,0),boxmax(0,0,0),
			pPoints(NULL),nNumPoints(0),pGravityParams(NULL),zmin(0),zmax(0),center(0,0,0),axis(0,0,0) {}
	};
	// When physicalizing with type == PE_AREA this must be a valid pointer to the AreaDefinition structure.
	AreaDefinition *pAreaDef;
};

//////////////////////////////////////////////////////////////////////////
// Physical proxy interface.
//////////////////////////////////////////////////////////////////////////
struct IEntityPhysicalProxy : public IEntityProxy
{
	// Description:
	//    Assign a pre-created physical entity to this proxy.
	// Arguments:
	//    pPhysEntity - The pre-created physical entity.
	//		nSlot - Slot Index to which the new position will be taken from.
	virtual void AssignPhysicalEntity( IPhysicalEntity *pPhysEntity, int nSlot=-1 ) = 0;
	// Description:
	//    Get world bounds of physical object.
	// Arguments:
	//    bounds - Returns Bounding box in world space.
	virtual void GetWorldBounds( AABB &bounds ) = 0;

	// Description:
	//    Get local space physical bounding box.
	// Arguments:
	//    bounds - Returns Bounding box in local entity space.
	virtual void GetLocalBounds( AABB &bounds ) = 0;

	virtual void Physicalize( SEntityPhysicalizeParams &params ) = 0;
	virtual IPhysicalEntity* GetPhysicalEntity() const = 0;

	virtual void SerializeTyped( TSerialize ser, int type, int flags ) = 0;
	
	// Enable or disable physical simulation.
	virtual void EnablePhysics( bool bEnable ) = 0;
	// Add impulse to physical entity.
	virtual void AddImpulse( int ipart, const Vec3 &pos,const Vec3 &impulse,bool bPos,float fAuxScale ) = 0;

	// Description:
	//    Creates a trigger bounding box.
	//    When physics will detect collision with this bounding box it will send an events to the entity.
	//    If entity have script OnEnterArea and OnLeaveArea events will be called.
	// Arguments:
	//    bbox - Axis aligned bounding box of the trigger in entity local space (Rotation and scale of the entity is ignored).
	//           Set empty bounding box to disable trgger.
	virtual void SetTriggerBounds( const AABB &bbox ) = 0;
	
	// Description:
	//    Retrieve trigger bounding box in local space.
	// Return:
	//    Axis aligned bounding box of the trigger in the local space.
	virtual void GetTriggerBounds( AABB &bbox ) = 0;

	// Description:
	//		physicalizes the foliage of StatObj in slot iSlot
	virtual bool PhysicalizeFoliage( int iSlot ) = 0;

	// Description:
	//		dephysicalizes the foliage in slot iSlot
  virtual void DephysicalizeFoliage( int iSlot ) = 0;

  // Description:
  //    retrieve current partid offset
  virtual int GetPartId0() = 0;

	// Description:
	//   Flags internally as restricted rag-doll.
	virtual void EnableRestrictedRagdoll( bool enable ) = 0;

	// Description:
	// ...
	virtual bool IsRestrictedRagdoll() const = 0;

};

//////////////////////////////////////////////////////////////////////////
// Proximity trigger proxy interface.
//////////////////////////////////////////////////////////////////////////
struct IEntityTriggerProxy : public IEntityProxy
{
	// Description:
	//    Creates a trigger bounding box.
	//    When physics will detect collision with this bounding box it will send an events to the entity.
	//    If entity have script OnEnterArea and OnLeaveArea events will be called.
	// Arguments:
	//    bbox - Axis aligned bounding box of the trigger in entity local space (Rotation and scale of the entity is ignored).
	//           Set empty bounding box to disable trgger.
	virtual void SetTriggerBounds( const AABB &bbox ) = 0;

	// Description:
	//    Retrieve trigger bounding box in local space.
	// Return:
	//    Axis aligned bounding box of the trigger in the local space.
	virtual void GetTriggerBounds( AABB &bbox ) = 0;

	// Description:
	//    Forward enter/leave events to this entity
	virtual void ForwardEventsTo( EntityId id ) = 0;

	// Description:
	//    Invalidate the trigger, so it gets recalculated and catches things which are already inside
	//		when it gets enabled.
	virtual void InvalidateTrigger( ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Sound proxy interface.
//////////////////////////////////////////////////////////////////////////
struct IEntitySoundProxy : public IEntityProxy
{
	virtual ISound* GetSound( tSoundID nSoundId ) = 0;
	virtual ISound* GetSoundByIndex(uint32 nIndex) = 0; // will return NULL on invalid index
	virtual bool PlaySound( ISound *pSound, const Vec3 &vPos=Vec3(0,0,0),const Vec3 &vDirection=FORWARD_DIRECTION,float fSoundScale=1.0f, bool bLipSync=true ) = 0;
	// new calls
	virtual tSoundID PlaySound( const char *sSoundOrEventName, const Vec3 &vOffset,const Vec3 &vDirection, uint32 nSoundFlags, ESoundSemantic eSemantic, EntityId *pSkipEnts,int nSkipEnts) = 0;
	virtual tSoundID PlaySound( const char *sSoundOrEventName, const Vec3 &vOffset,const Vec3 &vDirection, uint32 nSoundFlags, ESoundSemantic eSemantic) = 0;
	virtual tSoundID PlaySoundEx( const char *sSoundOrEventName, const Vec3 &vOffset,const Vec3 &vDirection, uint32 nSoundFlags, float fVolume, float fMinRadius,float fMaxRadius, ESoundSemantic eSemantic) = 0;
	virtual tSoundID PlaySoundEx( const char *sSoundOrEventName, const Vec3 &vOffset,const Vec3 &vDirection, uint32 nSoundFlags, float fVolume, float fMinRadius,float fMaxRadius, ESoundSemantic eSemantic, EntityId *pSkipEnts,int nSkipEnts) = 0;
	virtual bool SetStaticSound(tSoundID nSoundId, bool bStatic) = 0;
	virtual bool GetStaticSound(const tSoundID nSoundId) = 0;
	virtual void StopSound( tSoundID nSoundId, ESoundStopMode eStopMode=ESoundStopMode_EventFade ) = 0;
	virtual void PauseSound( tSoundID nSoundId,bool bPause ) = 0;
	virtual void StopAllSounds() = 0;
	virtual void SetSoundPos( tSoundID nSoundId,const Vec3 &vPos ) = 0;
	virtual Vec3 GetSoundPos( tSoundID nSoundId ) = 0;
	virtual void SetSoundDirection( tSoundID nSoundId,const Vec3 &dir ) = 0;
	virtual Vec3 GetSoundDirection( tSoundID nSoundId ) = 0;
	virtual void SetEffectRadius( float fEffectRadius ) = 0;
	virtual float GetEffectRadius() = 0;
	virtual const char* GetTailName() = 0;
	virtual void CheckVisibilityForTailName(const float fLength, const float fDistanceToRecalculate) = 0;

	virtual void UpdateSounds() = 0;

	virtual IEntity* GetEntity() const = 0;
};


//////////////////////////////////////////////////////////////////////////
// Description:
//    Flags the can be set on each of the entity object slots.
//////////////////////////////////////////////////////////////////////////
enum EEntitySlotFlags
{
	ENTITY_SLOT_RENDER = 0x0001,  // Draw this slot.
	ENTITY_SLOT_RENDER_NEAREST = 0x0002,  // Draw this slot as nearest.
	ENTITY_SLOT_RENDER_WITH_CUSTOM_CAMERA = 0x0004,  // Draw this slot using custom camera passed as a Public ShaderParameter to the entity.
	ENTITY_SLOT_IGNORE_PHYSICS = 0x0010,  // This slot will ignore physics events sent to it.
	ENTITY_SLOT_BREAK_AS_ENTITY = 0x020
};

// Description of the contents of the entity slot.
struct SEntitySlotInfo
{
	// Slot flags.
	int nFlags;
	// Index of parent slot, (-1 if no parent)
	int nParentSlot;
	// Slot local transformation matrix.
	const Matrix34 *pLocalTM;
	// Slot world transformation matrix.
	const Matrix34 *pWorldTM;
	// Objects that can binded to the slot.
	EntityId                   entityId;
	struct IStatObj*           pStatObj;
	struct ICharacterInstance*   pCharacter;
	struct IParticleEmitter*   pParticleEmitter;
	struct IRenderNode*      pLight;
	struct IRenderNode*      pChildRenderNode;
	// Custom Material used for the slot.
	IMaterial* pMaterial;
};

//////////////////////////////////////////////////////////////////////////
// Render proxy interface.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Interface to the entity Render proxy.
//////////////////////////////////////////////////////////////////////////
struct IEntityRenderProxy : public IEntityProxy
{
	// Description:
	//    Get world bounds of render proxy.
	// Arguments:
	//    bounds - Returns Bounding box in world space.
	virtual void GetWorldBounds( AABB &bounds ) = 0;

	// Description:
	//    Get local space int the entity bounds of render proxy.
	// Arguments:
	//    bounds - Returns Bounding box in local entity space.
	virtual void GetLocalBounds( AABB &bounds ) = 0;

	// Description:
	//    Force local bounds.
	// Arguments:
	//    bounds - Bounding box in local space.
	//    bDoNotRecalculate - when set to true entity will never try to recalculate local bounding box set by this call.
	virtual void SetLocalBounds( const AABB &bounds,bool bDoNotRecalculate ) = 0;

	// Invalidates local or world space bounding box.
	virtual void InvalidateLocalBounds() = 0;

	// Description:
	//     Retrieve an actual material used for rendering specified slot.
	///    Will return custom applied material or if custom material not set will return an actual material assigned to the slot geometry.
	// Arguments:
	//     nSlot - Slot to query used material from, if -1 material will be taken from the first renderable slot.
	// Return:
	//     Material used for rendering, or NULL if slot is not rendered.
	virtual IMaterial* GetRenderMaterial( int nSlot=-1 ) = 0;

	// Description:
	//     Assign custom material to the slot.
	// Arguments:
	//     nSlot - Slot to apply material to.
	virtual void SetSlotMaterial( int nSlot,IMaterial *pMaterial ) = 0;

	// Description:
	//     Retrieve slot's custom material (This material Must have been applied before with the SetSlotMaterial).
	// Arguments:
	//     nSlot - Slot to query custom material from.
	// Return:
	//     Custom material applied on the slot.
	virtual IMaterial* GetSlotMaterial( int nSlot ) = 0;

	// Description:
	//    Retrieve engine render node, used to render this entity.
	virtual IRenderNode* GetRenderNode() = 0;

	// Description:
	//    Retrieve and optionally create a shaders public params for this render proxy.
	// Arguments:
	//    bCreate - If Shader public params are not created for this entity, they will be created.
	//virtual IShaderPublicParams* GetShaderPublicParams( bool bCreate=true ) = 0;

	// Description:
	//    updates indirect lighting for children
	virtual void UpdateIndirLightForChildren(){};

	// Description:
	//    toggles updating of characters before or after physics
	virtual void UpdateCharactersBeforePhysics( bool update ) = 0;
	virtual bool IsCharactersUpdatedBeforePhysics() = 0;

  // Description:
  //    sets material layers masks
  virtual void SetMaterialLayersMask( uint8 nMtlLayersMask ) = 0;
  virtual uint8 GetMaterialLayersMask( ) const = 0;

  // Description:
  //    overrides material layers blend amount
  virtual void SetMaterialLayersBlend( uint32 nMtlLayersBlend ) = 0;
  virtual uint32 GetMaterialLayersBlend( ) const = 0;

  // Description:
  //    set/get motion blur multiplier
  virtual void SetMotionBlurAmount(float fAmount) = 0;
  virtual float GetMotionBlurAmount() const = 0;

  // Description:
  //    set/get vision params (actually used for silhouette rendering)
  virtual void SetVisionParams( float r, float g, float b, float a ) = 0;
  virtual uint32 GetVisionParams() const = 0;

  // Description:
  //    set/get opacity
  virtual void SetOpacity(float fAmount) = 0;
  virtual float GetOpacity() const = 0;

	// Description:
	//	return the last time (as set by the system timer) when the renderproxy was last seen.
	virtual	float	GetLastSeenTime() const = 0;

	// Description:
	//	return true if entity visarea was visible during last frames
	virtual	bool IsRenderProxyVisAreaVisible() const = 0;
};

//////////////////////////////////////////////////////////////////////////
// Type of an area managed by IEntityAreaProxy.
enum EEntityAreaType
{
	ENTITY_AREA_TYPE_SHAPE,     // Area type is a closed set of points forming shape.
	ENTITY_AREA_TYPE_BOX,       // Area type is a oriented bounding box.
	ENTITY_AREA_TYPE_SPHERE,    // Area type is a sphere.
	ENTITY_AREA_TYPE_GRAVITYVOLUME,    // Area type is a volume around a bezier curve.
};

//////////////////////////////////////////////////////////////////////////
// Description.
//     Area proxy allow for entity to host an area trigger.
//     Area can be shape, box or sphere, when marked entities cross this area border,
//     it will send ENTITY_EVENT_ENTERAREA,ENTITY_EVENT_LEAVEAREA,ENTITY_EVENT_AREAFADE, 
//     events to the target entities.
//////////////////////////////////////////////////////////////////////////
struct IEntityAreaProxy : public IEntityProxy
{
	enum EAreaProxyFlags
	{
		FLAG_NOT_UPDATE_AREA = BIT(1), // When set points in the area will not be updated.
		FLAG_NOT_SERIALIZE   = BIT(2)  // Areas with this flag will not be serialized
	};

	// Area flags.
	virtual void SetFlags( int nAreaProxyFlags ) = 0;
	// Area flags.
	virtual int  GetFlags() = 0;

	// Description:
	//    Retrieve area type.
	// Return:
	//    One of EEntityAreaType enumerated types.
	virtual EEntityAreaType GetAreaType() const = 0;

	// Description:
	//    Sets area to be a shape, and assign points to this shape.
	//    Points are specified in local entity space, shape will always be constructed in XY plane,
	//    lowest Z of specified points will be used as a base Z plane.
	//    If fHeight parameter is 0, shape will be considered 2D shape, and during intersection Z will be ignored
	//    If fHeight is not zero shape will be considered 3D and will accept intersection within vertical range from baseZ to baseZ+fHeight.
	// Arguments:
	//    vPoints - Array of 3D vectors defining shape vertices.
	//    nPoints - Number of vertices in vPoints array.
	//    fHeight - Height of the shape .
	// See Also: SetSphere,SetBox,GetSphere,GetBox,GetPoints
	virtual void	SetPoints( const Vec3* const vPoints, int nPointsCount,float fHeight ) = 0;

	// Description:
	//    Sets area to be a Box, min and max must be in local entity space.
	//    Host entity orientation will define the actual world position and orientation of area box.
	// See Also: SetSphere,SetPoints,GetSphere,GetBox,GetPoints
	virtual void	SetBox( const Vec3& min,const Vec3& max ) = 0;

	// Description:
	//    Sets area to be a Sphere, center and radius must be specified in local entity space.
	//    Host entity world position will define the actual world position of the area sphere.
	// See Also: SetBox,SetPoints,GetSphere,GetBox,GetPoints
	virtual void	SetSphere( const Vec3& vCenter,float fRadius ) = 0;

	// Description:
	//    Retrieve number of points for shape area, return 0 if not area type is not shape.
	// See Also: SetPoints,GetPoints
	virtual int   GetPointsCount() = 0;
	// Description:
	//    Retrieve array of points for shape area, will return NULL for all other area types.
	// See Also: SetSphere,SetBox,SetPoints,GetSphere,GetBox
	virtual const Vec3* GetPoints() = 0;
	// Description:
	//    Retrieve shape area height, if height is 0 area is 2D.
	// See Also: SetPoints
	virtual float GetHeight() = 0;
	// Description:
	//    Retrieve min and max in local space of area box.
	// See Also: SetSphere,SetBox,SetPoints,GetSphere,GetBox,GetPoints
	virtual void	GetBox( Vec3& min,Vec3& max ) = 0;
	// Description:
	//    Retrieve center and radius of the sphere area in local space.
	// See Also: SetSphere,SetBox,SetPoints,GetSphere,GetBox,GetPoints
	virtual void	GetSphere( Vec3& vCenter,float &fRadius ) = 0;

	virtual void SetGravityVolume(const Vec3 * pPoints, int nNumPoints, float fRadius, float fGravity, bool bDontDisableInvisible, float fFalloff, float fDamping) = 0;

	// Description:
	//    Set area ID, this id will be provided to the script callback OnEnterArea , OnLeaveArea.
	// See Also: GetID
	virtual void	SetID( const int id ) = 0;
	// Description:
	//    Retrieve area ID.
	// See Also: SetID
	virtual int		GetID() const = 0;

	// Description:
	//    Set area group id, areas with same group id act as an exclusive areas,
	//    If 2 areas with same group id overlap, entity will be considered in the most internal area (closest to entity).
	// See Also: GetGroup
	virtual void	SetGroup( const int id) = 0;

	// Description:
	//    Retrieve area group id.
	// See Also: SetGroup
	virtual int		GetGroup( ) const = 0;

	// Description:
	//    Set priority defines the individual priority of an area,
	//    Area with same group ideas will depend on which has the higher priority
	// See Also: GetPriority
	virtual void	SetPriority( const int nPriority) = 0;

	// Description:
	//    Retrieve area priority.
	// See Also: SetPriority
	virtual int		GetPriority( ) const = 0;

	// Description:
	//    Sets if the Area should fully obstruct it's Roof or Floor (on Box or Shape)
	virtual void	SetObstruction( bool bRoof, bool bFloor ) = 0;

	// Description:
	//    Add target entity to the area.
	//    When someone enters/leaves an area, it will send ENTERAREA,LEAVEAREA,AREAFADE, events to these target entities.
	// See Also: ClearEntities
	virtual void	AddEntity( EntityId id ) = 0;

	// Description:
	//    Removes all added target entities.
	// See Also: AddEntity
	virtual void	ClearEntities() = 0;

	// Description:
	//    Set area proximity region near the border.
	//    When someone is moving within this proximity region from the area outside border
	//    Area will generate ENTITY_EVENT_AREAFADE event to the target entity, with a fade ratio from 0, to 1.
	//    Where 0 will be at he area outside border, and 1 inside the area in distance fProximity from the outside area border.
	// See Also: GetProximity
	virtual void	SetProximity( float fProximity ) = 0;

	// Description:
	//    Retrieves area proximity.
	// See Also: SetProximity
	virtual float	GetProximity() = 0;

	// Description:
	//    computes and returned squared distance to a point which is outside
	//    OnHull3d is the closest point on the hull of the area
	virtual float PointNearDistSq(const Vec3 &Point3d, Vec3 &OnHull3d) = 0;

	// Description:
	//    computes and returned squared distance from a point to the hull of the area
	//    OnHull3d is the closest point on the hull of the area
	//		This function is not sensitive of if the point is inside or outside the area
	virtual float ClosestPointOnHullDistSq(const Vec3 &Point3d, Vec3 &OnHull3d) = 0;

	// Description:
	//    checks if a given point is inside the area
	//    ignoring the height speeds up the check
	virtual bool	IsPointWithin(const Vec3& Point3d, bool bIgnoreHeight=false) const = 0;
};

//////////////////////////////////////////////////////////////////////////
// Description.
//     Boids proxy allow entity to host flocks of birds or fishes.
//////////////////////////////////////////////////////////////////////////
struct IEntityBoidsProxy : public IEntityProxy
{

};

//////////////////////////////////////////////////////////////////////////
// Description.
//     Flow Graph proxy allows entity to host reference to the flow graph.
//////////////////////////////////////////////////////////////////////////
struct IEntityFlowGraphProxy : public IEntityProxy
{
	virtual void SetFlowGraph( IFlowGraph *pFlowGraph ) = 0;
	virtual IFlowGraph* GetFlowGraph() = 0;

	virtual void AddEventListener( IEntityEventListener *pListener ) = 0;
	virtual void RemoveEventListener( IEntityEventListener *pListener ) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Description.
//     Susbstitution proxy remembers IRenderNode this entity substitutes and unhides it upon deletion
/////////////////////////////////////////////////////////////////////////////////////////////////////
struct IEntitySubstitutionProxy : public IEntityProxy
{
	virtual void SetSubstitute(IRenderNode *pSubstitute) = 0;
	virtual IRenderNode *GetSubstitute() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Description.
//     Represents entity camera.
/////////////////////////////////////////////////////////////////////////////////////////////////////
struct IEntityCameraProxy : public IEntityProxy
{
	virtual void SetCamera( CCamera &cam ) = 0;
	virtual CCamera& GetCamera() = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Description.
//     Proxy for the entity rope.
/////////////////////////////////////////////////////////////////////////////////////////////////////
struct IEntityRopeProxy : public IEntityProxy
{
	virtual struct IRopeRenderNode* GetRopeRendeNode() = 0;
};

#endif // __IEntityProxy_h__

