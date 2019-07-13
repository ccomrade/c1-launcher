/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Actor System interfaces.
  
 -------------------------------------------------------------------------
  History:
  - 26:8:2004   14:55 : Created by Márcio Martins

*************************************************************************/
#ifndef __IACTORSYSTEM_H__
#define __IACTORSYSTEM_H__

#if _MSC_VER > 1000
# pragma once
#endif


#include <IEntity.h>
#include <IEntitySystem.h>
#include <IScriptSystem.h>
#include "IGameObjectSystem.h"
#include "IGameObject.h"

enum EActorPhysicalization
{
	eAP_NotPhysicalized,
	eAP_Alive,
	eAP_Ragdoll,
	eAP_Sleep,
	eAP_Frozen,
	eAP_Linked,
	eAP_Spectator,
};

struct IActor;
struct IActionListener;
struct IAnimationGraphState;
struct SViewParams;
class CGameObject;
struct IGameObjectExtension;
struct IInventory;
struct IAnimatedCharacter;
struct ICharacterInstance;
struct AnimEventInstance;

struct IActor:
	public IGameObjectExtension
{
	virtual void	Release() = 0;

	virtual void InitLocalPlayer() = 0;

	virtual void  ResetAnimGraph() = 0;

	virtual IInventory *GetInventory() const = 0;
	virtual void	SerializeXML( XmlNodeRef& node, bool bLoading ) = 0;
	virtual void  SerializeLevelToLevel( TSerialize &ser ) = 0;
	virtual void	ProcessEvent( SEntityEvent& event ) = 0;
	virtual int		GetHealth() const = 0;
	virtual int		GetMaxHealth() const = 0;
	virtual int		GetArmor() const = 0;
	virtual int		GetMaxArmor() const = 0;
	virtual void  SetHealth( int health ) = 0;
  virtual int   IsGod() = 0;
	virtual void	PlayAction(const char *action,const char *extension, bool looping) = 0;
	virtual bool	IsThirdPerson() const = 0;
  virtual void ToggleThirdPerson() = 0;
	virtual IItem* GetCurrentItem(bool includeVehicle=false) const = 0;
	virtual IItem* GetHolsteredItem() const = 0;
	virtual void HolsterItem(bool holster) = 0;

	// fetch the state structure for the animation graph associated with this actor
	// may return NULL if no animation graph exists
	virtual IAnimationGraphState * GetAnimationGraphState() = 0;
	virtual void SetFacialAlertnessLevel(int alertness) = 0;

	virtual void SetIKPos(const char *pLimbName, const Vec3& goalPos, int priority) = 0;

	virtual bool IsClient() const = 0;

	virtual IMaterial *GetReplacementMaterial() = 0;

	// get the current movement controller
	virtual IMovementController * GetMovementController() = 0;

	// Set rotation of the actor view.
	// Used for precisely orienting player for time demo.
	virtual void SetViewRotation( const Quat &rotation ) = 0;
	virtual Quat GetViewRotation() const = 0;

	// Enables/Disables time demo mode.
	// Some movement/view direction things work differently when running under time demo.
	virtual void EnableTimeDemo( bool bTimeDemo ) = 0;

	virtual Vec3 GetViewAngleOffset() = 0;
  virtual Vec3 GetLocalEyePos(int slot = 0) const = 0;
  
  // get currently linked vehicle, or NULL
	virtual IEntity *LinkToVehicle(EntityId vehicleId) = 0;

  virtual IVehicle* GetLinkedVehicle() const = 0;
  virtual IEntity* GetLinkedEntity() const = 0;

	// for look ik in vehicles
	virtual void SetViewInVehicle(Quat viewRotation) = 0;

	// get current item (from inventory / in hands)
	//virtual IItem *GetCurrentItem() const = 0;

	uint16 GetChannelId() const
	{
		return GetGameObject()->GetChannelId();
	}
	void SetChannelId( uint16 id )
	{
		GetGameObject()->SetChannelId( id );
	}
	bool IsPlayer() const
	{
		return GetChannelId() != 0;
	}

	virtual void SwitchDemoModeSpectator(bool activate) = 0;

	virtual void RequestFacialExpression(const char* pExpressionName = NULL) = 0;
	virtual void PrecacheFacialExpression(const char* pExpressionName) = 0;

	virtual void SetAnimTentacleParams(pe_params_rope& rope, float animBlend) = 0;

	virtual void AnimationEvent(ICharacterInstance *pCharacter, const AnimEventInstance &event) = 0;
	virtual IAnimatedCharacter * GetAnimatedCharacter() = 0;
	virtual bool IsFallen() const = 0;
	virtual void NotifyLeaveFallAndPlay() = 0;

	virtual void NotifyInventoryAmmoChange(IEntityClass* pAmmoClass, int amount) = 0;

	virtual EntityId	GetGrabbedEntityId() const = 0;

	virtual void HideAllAttachments(bool isHiding) = 0;

};


struct IActorIterator
{
	virtual size_t  Count() = 0;
	virtual IActor* Next() = 0;
	virtual void    AddRef() = 0;
	virtual void    Release() = 0;
};
typedef _smart_ptr<IActorIterator> IActorIteratorPtr;

struct IActorSystem
{
	virtual void  Reset() = 0;
	virtual IActor *GetActor(EntityId entityId) = 0;
	virtual IActor *GetActorByChannelId(uint16 channelId) = 0;
	virtual IActor *CreateActor(uint16 channelId, const char *name, const char *actorClass, const Vec3 &pos, const Quat &rot, const Vec3 &scale, EntityId id = 0) = 0;

	virtual int GetActorCount() const = 0;
	virtual IActorIteratorPtr CreateActorIterator() = 0;

	virtual void SetDemoPlaybackMappedOriginalServerPlayer(EntityId id) = 0;
	virtual EntityId GetDemoPlaybackMappedOriginalServerPlayer() const = 0;
	virtual void SwitchDemoSpectator(EntityId id = 0) = 0;
	virtual IActor *GetCurrentDemoSpectator() = 0;
	virtual IActor *GetOriginalDemoSpectator() = 0;

	virtual void AddActor(EntityId entityId, IActor *pActor) = 0;
	virtual void RemoveActor(EntityId entityId) = 0;
};


#endif //__IACTORSYSTEM_H__
