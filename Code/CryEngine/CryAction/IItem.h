/*************************************************************************
Crytek Source File.
Copyright (C), Crytek Studios, 2001-2004.
-------------------------------------------------------------------------
$Id$
$DateTime$
Description: Item interface.

-------------------------------------------------------------------------
History:
- 29:11:2005   13:52 : Created by Márcio Martins

*************************************************************************/
#ifndef __IITEM_H__
#define __IITEM_H__

#pragma once


struct IWeapon;

// Summary
//   Interface to implement a new Item class
struct IItem : public IGameObjectExtension
{
	// Summary
	//   Enumeration which defines the hand values
	enum eItemHand
	{
		eIH_Right = 0, // indicates the right hand of the actor
		eIH_Left, // indicates the left hand of the actor
		eIH_Last,
	};

	// Summary
	//   Registers the owner of the item
	virtual void SetOwnerId(EntityId ownerId) = 0;
	
	// Summary
	//   Retrieves the ownwer id
	virtual EntityId GetOwnerId() const = 0;

	// Summary
	//   Receives an action from the client
	// Parameters
	//   actorId - EntityId of the actor who sent the action
	//   actionId - 'name' of the action performed
	//   activationMode - one of the activation mode defined in EActionActivationMode
	//   value - value which quantified the action performed
	virtual void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value) = 0;

	// Summary
	//   
	// Parameters
	//   hand - Values defined in eItemHand should be used
	virtual void SetHand(int hand) = 0;

	// Summary
	//   Performs the selection of the item
	// Parameters
	//   select - status of the selection
	virtual void Select(bool select) = 0;

	// Summary
	//   Callback used when the parent item has been selected
	// Parameters
	//   select - status of the selection
	virtual void OnParentSelect(bool select) = 0;

	// Summary
	//   Performs the action to drop the item
	// Parameters
	//   impulseScale - the scale of the impulse, 1.0 by default
	//   selectNext - requests the next item to be selected after the current 
	//                one is dropped
	//   byDeath - indicates that the item is dropped because its owner has died
	virtual void Drop(float impulseScale=1.0f, bool selectNext=true, bool byDeath=false) = 0;

	// Summary
	//   Performs the action to pickup the item
	// Parameters
	//   picker - EntityId of the actor who pick this item
	//   sound - enables a sound to be played
	//   select - will allow the item to be selected after being picked up
	//   keepHistory - instructs the item system to keep track of the previously 
	//                 selected item
	virtual void PickUp(EntityId picker, bool sound, bool select=true, bool keepHistory=true) = 0;

	// Summary
	//   Performs the physicalization of the item
	// Parameters
	//   enable - Enables or disables the physicalization
	//   rigid - indicates that the item should be physicalized as rigid body
	virtual void Physicalize(bool enable, bool rigid) = 0;

	// Summary
	//   Specifies the item pickable status
	// Paramters
	//   enable - enables the ability of the item of being possible to pick
	//   dropped - sets the item to be dropped
	virtual void Pickalize(bool enable, bool dropped) = 0;


	virtual void Impulse(const Vec3 &position, const Vec3 &direction, float impulse) = 0;
	virtual bool CanDrop() const = 0;

	// Summary
	//   Retrieves the mountable property
	virtual bool IsMountable() const = 0;

	virtual bool SupportsDualWield(const char *itemName) const = 0;
	virtual void ResetDualWield() = 0;
	virtual IItem *GetDualWieldSlave() const = 0;
	virtual EntityId GetDualWieldSlaveId() const = 0;
	virtual IItem *GetDualWieldMaster() const = 0;
	virtual EntityId GetDualWieldMasterId() const = 0;
	virtual void SetDualWieldMaster(EntityId masterId) = 0;
	virtual void SetDualWieldSlave(EntityId slaveId) = 0;
	virtual bool IsDualWield() const = 0;
	virtual bool IsDualWieldMaster() const = 0;
	virtual bool IsDualWieldSlave() const = 0;
	virtual bool IsTwoHand() const = 0;
	virtual void SetDualSlaveAccessory(bool noNetwork = false) = 0;

	// Summary
	//   Specifies the item to be selectable
	// Paramters
	//   enable - enables the ability of the item of being selected
	virtual void EnableSelect(bool enable) = 0;

	// Summary
	//   Retrieves selection ability
	virtual bool CanSelect() const = 0;

	// Summary
	//   Enables sounds of the item
	virtual void EnableSound(bool enable) = 0;
	
	// Summary
	//   Retrieves status of the sound of the item
	virtual bool IsSoundEnabled() const = 0;
	virtual void TakeAccessories(EntityId receiver) = 0;

	// Summary
	//   Sets the mount position
	// Parameters
	//   pos - world position to mount the item
	// See Also
	//   MountAtEntity
	virtual void MountAt(const Vec3 &pos) = 0;

	// Summary
	//   Sets the mount position relative to an entity
	// Parameters
	//   entityId - Id of the entity which will mount the item
	//   pos - local position to mount the item
	//   angles - local angles to mount the item
	// See Also
	//   MountAt
	virtual void MountAtEntity(EntityId entityId, const Vec3 &pos, const Ang3 &angles) = 0;
	
	// Summary
	//   Sets the item to be in use
	// Parameters
	//   EntityId of the actor who use the weapon
	// See Also
	//   StopUse
	virtual void StartUse(EntityId userId) = 0;

	// Summary
	//   Sets the item to cease being in use
	// Parameters
	//   EntityId of the actor who use the weapon
	// See Also
	//   StartUse
	virtual void StopUse(EntityId userId) = 0;

	// Summary
	//   Sets the animation to play
	virtual void EnableAnimations(bool enable) = 0;

	virtual void EnterWater(bool enter) = 0;

	// Summary
	//   Sets the item status as being busy
	// Description
	//   The busy status is used to prevent the item to be requested any action 
	//   when it's currently performing some actions. For example, the reloading 
	//   action of a weapon will sets the weapon as busy to prevent it from 
	//   shooting.
	// See Also
	//   IsBusy
	virtual void SetBusy(bool busy) = 0;

	// Summary
	//   Retrieves the busy status
	// See Also
	//   SetBusy
	virtual bool IsBusy() const = 0;
	virtual void Cloak(bool cloak, IMaterial *cloakMat = 0) = 0;

	virtual void UpdateFPView(float frameTime) = 0;
  virtual bool FilterView(struct SViewParams &viewParams) = 0;
  virtual void PostFilterView(struct SViewParams &viewParams) = 0;

	virtual bool CheckAmmoRestrictions(EntityId pickerId) = 0;

	// Summary
	//   Retrieves the weapon interface, if available
	// Returns
	//   A pointer to the IWeapon implementation
	virtual IWeapon *GetIWeapon() = 0;

	//used to serialize item attachment when loading next level
	virtual void SerializeLTL( TSerialize ser ) = 0;

	//removes all attachments
	virtual void RemoveAllAccessories() = 0;
	
	// gets the angle limits for a mounted weapon item
	virtual Vec3 GetMountedAngleLimits() const = 0;

	// gets the original (not current) direction vector of a mounted weapon
	virtual Vec3 GetMountedDir() const = 0;

};

#endif // __IITEM_H__