// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IWEAPON_H__
#define __IWEAPON_H__

#pragma once

struct IWeapon;

struct SSpreadModParams;
struct SRecoilModParams;

enum EPhysicalizationType
{
	ePT_None = 0,
	ePT_Particle,
	ePT_Rigid,
	ePT_Static,
	ePT_StuckToEntity
};

struct IFireMode
{
	virtual void Init(IWeapon *pWeapon, const struct IItemParamsNode *params) = 0;
	virtual void Update(float frameTime, uint frameId) = 0;
	virtual void PostUpdate(float frameTime) = 0;
	virtual void UpdateFPView(float frameTime) = 0;
	virtual void Release() = 0;
	virtual void GetMemoryStatistics(ICrySizer * s) = 0;

	virtual void ResetParams(const struct IItemParamsNode *params) = 0;
	virtual void PatchParams(const struct IItemParamsNode *patch) = 0;

	virtual void Activate(bool activate) = 0;

	virtual int GetAmmoCount() const = 0;
	virtual int GetClipSize() const = 0;

	virtual bool OutOfAmmo() const = 0;
	virtual bool CanReload() const = 0;
	virtual void Reload(int zoomed) = 0;
	virtual bool IsReloading() = 0;
	virtual void CancelReload() = 0;
	virtual bool CanCancelReload() = 0;

	virtual float GetRecoil() const = 0;
	virtual float GetSpread() const = 0;
	virtual float GetMinSpread() const = 0;
	virtual float GetMaxSpread() const = 0;
	virtual float GetHeat() const = 0;
	virtual const char *GetCrosshair() const = 0;
	virtual bool	CanOverheat() const = 0;

	virtual void SetRecoilMultiplier(float recoilMult) = 0;
	virtual float GetRecoilMultiplier() const = 0;
	
	virtual bool CanFire(bool considerAmmo=true) const = 0;
	virtual void StartFire() = 0;
	virtual void StopFire() = 0;
	virtual bool IsFiring() const = 0;
	virtual bool AllowZoom() const = 0;
	virtual void Cancel() = 0;

	virtual void NetShoot(const Vec3 &hit, int predictionHandle) = 0;
	virtual void NetShootEx(const Vec3 &pos, const Vec3 &dir, const Vec3 &vel, const Vec3 &hit, float extra, int predictionHandle) = 0;
	virtual void NetEndReload() = 0;

	virtual void NetStartFire() = 0;
	virtual void NetStopFire() = 0;

	virtual EntityId GetProjectileId() const = 0;
	virtual void SetProjectileId(EntityId id) = 0;
	virtual EntityId RemoveProjectileId() = 0;

	virtual const char *GetType() const = 0;
	virtual IEntityClass* GetAmmoType() const = 0;
	virtual int GetDamage(float distance=0.0f) const = 0;
	virtual const char *GetDamageType() const = 0;

	virtual float GetSpinUpTime() const = 0;
	virtual float GetSpinDownTime() const = 0;
  virtual float GetNextShotTime() const = 0;
	virtual void SetNextShotTime(float time) = 0;
  virtual float GetFireRate() const = 0;

	virtual Vec3 GetFiringPos(const Vec3 &probableHit) const = 0;
	virtual Vec3 GetFiringDir(const Vec3 &probableHit, const Vec3& firingPos) const = 0;

	virtual void Enable(bool enable) = 0;
	virtual bool IsEnabled() const = 0;

	virtual void SetName(const char *name) = 0;
	virtual const char* GetName() = 0;

  virtual bool HasFireHelper() const = 0;
  virtual Vec3 GetFireHelperPos() const = 0;
  virtual Vec3 GetFireHelperDir() const = 0;
  
  virtual int GetCurrentBarrel() const = 0;

	virtual void Serialize(TSerialize ser) = 0;
	virtual void PostSerialize() = 0;

	virtual void PatchSpreadMod(SSpreadModParams &sSMP) = 0;
	virtual void ResetSpreadMod() = 0;

	virtual void PatchRecoilMod(SRecoilModParams &sRMP) = 0;
	virtual void ResetRecoilMod() = 0;

	virtual void ResetLock() = 0;
	virtual void StartLocking(EntityId targetId, int partId) = 0;
	virtual void Lock(EntityId targetId, int partId) = 0;
	virtual void Unlock() = 0;
};


struct IZoomMode
{
	virtual void Init(IWeapon *pWeapon, const struct IItemParamsNode *params) = 0;
	virtual void Update(float frameTime, uint frameId) = 0;
	virtual void Release() = 0;
	virtual void GetMemoryStatistics(ICrySizer * s) = 0;

	virtual void ResetParams(const struct IItemParamsNode *params) = 0;
	virtual void PatchParams(const struct IItemParamsNode *patch) = 0;

	virtual float GetRecoilScale() const = 0;

	virtual void Activate(bool activate) = 0;

	virtual bool CanZoom() const = 0;
	virtual bool StartZoom(bool stayZoomed = false, bool fullZoomOut = true, int zoomStep = 1) = 0;
	virtual void StopZoom() = 0;
	virtual void ExitZoom() = 0;

	virtual int GetCurrentStep() const = 0;
	virtual float GetZoomFoVScale(int step) const = 0;

	virtual void ZoomIn() = 0;
	virtual bool ZoomOut() = 0;

	virtual bool IsZoomed() const = 0;
	virtual bool IsZooming() const = 0;

	virtual void Enable(bool enable) = 0;
	virtual bool IsEnabled() const = 0;

	virtual void Serialize(TSerialize ser) = 0;

	virtual void UpdateFPView(float frameTime) = 0;
	virtual void FilterView(struct SViewParams &viewParams) = 0;
	virtual void PostFilterView(struct SViewParams &viewParams) = 0;

	virtual int  GetMaxZoomSteps() = 0;

	virtual void ApplyZoomMod(IFireMode *pFM) = 0;

	//! zoom mode is activated by toggling
	virtual bool IsToggle() = 0;
};


struct IWeaponFiringLocator
{ 
  virtual bool GetProbableHit(EntityId weaponId, const IFireMode* pFireMode, Vec3& hit) = 0;  // probable hit position
	virtual bool GetFiringPos(EntityId weaponId, const IFireMode* pFireMode, Vec3& pos) = 0;		// position of the projectile
	virtual bool GetFiringDir(EntityId weaponId, const IFireMode* pFireMode, Vec3& dir, const Vec3& probableHit, const Vec3& firingPos) = 0;		// this is target pos - firing pos
	virtual bool GetActualWeaponDir(EntityId weaponId, const IFireMode* pFireMode, Vec3& dir, const Vec3& probableHit, const Vec3& firingPos) = 0;		// this is acutal weapon direction
	virtual bool GetFiringVelocity(EntityId weaponId, const IFireMode* pFireMode, Vec3& vel, const Vec3& firingDir) = 0; // velocity to add up to the projectile (usually host velocity)  
  virtual void WeaponReleased() = 0;  // called when weapon is deleted
};


struct IWeaponEventListener
{
	virtual void OnShoot(IWeapon *pWeapon, EntityId shooterId, EntityId ammoId, IEntityClass* pAmmoType,
		const Vec3 &pos, const Vec3 &dir, const Vec3 &vel) = 0;
	virtual void OnStartFire(IWeapon *pWeapon, EntityId shooterId) = 0;
	virtual void OnStopFire(IWeapon *pWeapon, EntityId shooterId) = 0;
	virtual void OnStartReload(IWeapon *pWeapon, EntityId shooterId, IEntityClass* pAmmoType) = 0;
	virtual void OnEndReload(IWeapon *pWeapon, EntityId shooterId, IEntityClass* pAmmoType) = 0;
	virtual void OnOutOfAmmo(IWeapon *pWeapon, IEntityClass* pAmmoType) = 0;
	virtual void OnReadyToFire(IWeapon *pWeapon) = 0;

	virtual void OnPickedUp(IWeapon *pWeapon, EntityId actorId, bool destroyed) = 0;
	virtual void OnDropped(IWeapon *pWeapon, EntityId actorId) = 0;

	virtual void OnMelee(IWeapon* pWeapon, EntityId shooterId) = 0;

	virtual void OnStartTargetting(IWeapon *pWeapon) = 0;
	virtual void OnStopTargetting(IWeapon *pWeapon) = 0;

	virtual void OnSelected(IWeapon *pWeapon, bool selected) = 0;
};

struct AIWeaponDescriptor;

// Summary
//   Interface to implement a Weapon class
struct IWeapon
{
	// Summary
	//   Receives an action from the client
	// Parameters
	//   actorId - EntityId of the actor who sent the action
	//   actionId - 'name' of the action performed
	//   activationMode - one of the activation mode defined in EActionActivationMode
	//   value - value which quantified the action performed
	virtual void OnAction(EntityId actorId, const ActionId& actionId, int activationMode, float value) = 0;

	virtual void SetFiringLocator(IWeaponFiringLocator *pLocator) = 0;
	virtual IWeaponFiringLocator *GetFiringLocator() const = 0;

	virtual void AddEventListener(IWeaponEventListener *pListener, const char *who) = 0;
	virtual void RemoveEventListener(IWeaponEventListener *pListener) = 0;
	
	// Summary
	//   Sets the position of the destination target
	// Parameters
	//   pos - position in world space
  virtual void SetDestination(const Vec3& pos) = 0;

	// Summary
	//   Sets an entity as the destination target
	// Parameters
	//   pos - position in world space
  virtual void SetDestinationEntity(EntityId targetId) = 0;

	// Summary
	//   Retrieves the destination position
	virtual const Vec3& GetDestination() = 0;      
  
	// Summary
	//   Retrieves the firing position
	// See Also
	//   GetFiringDir
	virtual Vec3 GetFiringPos(const Vec3 &probableHit) const = 0;

	// Summary
	//   Retrieves the firing direction
	// Remarks
	//   It's important to pass the firing position in order to get an accurate 
	//   firing direction.
	// Parameters
	//   firingPos - the firing position
	// See Also
	//   GetFiringPos
	virtual Vec3 GetFiringDir(const Vec3 &probableHit, const Vec3& firingPos) const = 0;

	// Summary
	//   Requests the weapon to start firing
	// See Also
	//   CanFire
	virtual void StartFire() = 0;

	// Summary
	//   Requests the weapon to stop firing
	virtual void StopFire() = 0;

	// Summary
	//   Determines if the weapon can shoot
	virtual bool CanFire() const = 0;

	// Summary
	//   Requests the weapon to start its zoom mode
	// Parameters
	//   shooterId - EntityId of the actor who uses this weapon.
	// See Also
	//   CanZoom
	virtual void StartZoom(EntityId shooterId, int zoomed = 1) = 0;

	// Summary
	//   Requests the weapon to stops its zoom mode
	// Parameters
	//   shooterId - EntityId of the actor who uses this weapon.
	virtual void StopZoom(EntityId shooterId) = 0;
	
	// Summary
	//   Determines if the weapon can zoom
	virtual bool CanZoom() const = 0;

	// Summary
	//   Requests the weapon to immediately stops its zoom mode
	virtual void ExitZoom() = 0;

	// Summary
	//   Performs reloading of the weapon
	// Parameters
	//   force - when enabled, this will force the weapon to reload even if some 
	//           conditions could normally block this from happening
	// See Also
	//   CanReload
	virtual void Reload(bool force) = 0;

	// Summary
	//   Determines if the weapon can reload
	virtual bool CanReload() const = 0;

	// Summary
	//   Determines if the weapon has no more ammunition
	// Parameters
	//   allFireModes - Unless this option is enabled, the ammunition status 
	//                  will only be applicable for the current firemode
	virtual bool OutOfAmmo(bool allFireModes) const = 0;

	// Summary
	//   Retrieves the ammunition count
	// Parameters
	//   pAmmoType - EntityClass pointer of the ammo type
	// Returns
	//   The ammo count if the type was properly specified, or 0 if the 
	//   specified ammo type wasn't found
	// See Also
	//   SetAmmoCount
	virtual int GetAmmoCount(IEntityClass* pAmmoType) const = 0;

	// Summary
	//   Sets the ammunition count
	// Parameters
	//   pAmmoType - EntityClass pointer of the ammo type
	//   count - Specifies the desired ammo count
	// See Also
	//   GetAmmoCount
	virtual void SetAmmoCount(IEntityClass* pAmmoType, int count) = 0;

	// Summary
	//   Indicates if the crosshair is visible
	virtual bool GetCrosshairVisibility() const = 0;

	// Summary
	//   Retrieves the crosshair opacity
	virtual float GetCrosshairOpacity() const = 0;

	// Summary
	//   Retrieves the fire mode count
	virtual int GetNumOfFireModes() const = 0;

	// Summary
	//   Retrieves a specified fire mode, by index
	// Parameters
	//   idx - index of the specified fire mode
	// See Also
	//   GetNumOfFireModes, GetFireModeIdx
	virtual IFireMode *GetFireMode(int idx) const = 0;

	// Summary
	//   Retrieves a specified fire mode, by name
	// Parameters
	//   name - name of the specified fire mode
	virtual IFireMode *GetFireMode(const char *name) const = 0;

	// Summary
	//   Retrieves the index for a specified fire mode
	// Parameters
	//   name - name of the specified fire mode
	virtual int GetFireModeIdx(const char *name) const = 0;

	// Summary
	//   Retrieves the index of the current fire mode
	virtual int GetCurrentFireMode() const = 0;

	// Summary
	//   Sets the fire mode, by index
	// Parameters
	//   idx - index of the requested fire mode
	virtual void SetCurrentFireMode(int idx) = 0;

	// Summary
	//   Sets the fire mode, by name
	// Parameters
	//   name - name of the requested fire mode
	virtual void SetCurrentFireMode(const char *name) = 0;

	// Summary
	//   Toggles to the next fire mode
	virtual void ChangeFireMode() = 0;

	// Summary
	//   Retrieves a specified zoom mode, by index
	// Parameters
	//   idx - index of the specified zoom mode
	// See Also
	//   GetNumOfZoomModes, GetZoomModeIdx
	virtual IZoomMode *GetZoomMode(int idx) const = 0;

	// Summary
	//   Retrieves a specified zoom mode, by name
	// Parameters
	//   name - name of the specified zoom mode
	virtual IZoomMode *GetZoomMode(const char *name) const = 0;

	// Summary
	//   Retrieves the index for a specified zoom mode
	// Parameters
	//   name - name of the specified zoom mode
	virtual int GetZoomModeIdx(const char *name) const = 0;

	// Summary
	//   Retrieves the index of the current zoom mode
	virtual int GetCurrentZoomMode() const = 0;

	// Summary
	//   Sets the zoom mode, by index
	// Parameters
	//   idx - index of the requested zoom mode
	virtual void SetCurrentZoomMode(int idx) = 0;

	// Summary
	//   Sets the zoom mode, by name
	// Parameters
	//   name - name of the requested zoom mode
	virtual void SetCurrentZoomMode(const char *name) = 0;

	// Summary
	//   Toggles to the next zoom mode
	virtual void ChangeZoomMode() = 0;

	virtual float GetSpinUpTime() const = 0;
	virtual float GetSpinDownTime() const = 0;

	// Summary
	//   Sets the host of the weapon
	// Description
	//   The host of the weapon can be any entity which the weapon is child of. 
	//   For example, the cannon of the tank has the tank as an host.
	// Parameters
	//   hostId - EntityId of the host
	// See Also
	//   GetHostId
	virtual void SetHostId(EntityId hostId) = 0;

	// Summary
	//   Retrieves the EntityId of the host
	virtual EntityId GetHostId() const = 0;

	// Summary
	//   Performs a melee attack
	// See Also
	//   CanMeleeAttack
	virtual void MeleeAttack() = 0;

	// Summary
	//   Determines if the weapon can perform a melee attack
	// See Also
	//   MeleeAttack
	virtual bool CanMeleeAttack() const = 0;

	virtual void PerformThrow(float speedScale) = 0;

	virtual bool	PredictProjectileHit(IPhysicalEntity *pShooter, const Vec3 &pos, const Vec3 &dir,
		const Vec3 &velocity, float speed, Vec3& predictedPosOut, float& projectileSpeedOut,
		Vec3* pTrajectory = 0, unsigned int* trajectorySizeInOut = 0) const = 0;

	virtual void ForceHitMaterial(int surfaceId) = 0;
	virtual int GetForcedHitMaterial() const = 0;

	virtual bool IsZoomed() const = 0;
	virtual bool IsZooming() const = 0;
	virtual bool IsReloading() const = 0;

	virtual	void		RaiseWeapon(bool raise, bool faster = false) = 0;

	virtual const AIWeaponDescriptor& GetAIWeaponDescriptor( ) const = 0;

	virtual bool		IsLamAttached() = 0;
	virtual bool    IsFlashlightAttached() = 0;
	virtual void    ActivateLamLaser(bool activate, bool aiRequest = true) = 0;
	virtual void		ActivateLamLight(bool activate, bool aiRequest = true) = 0;
	virtual bool	  IsLamLaserActivated() = 0;
	virtual bool		IsLamLightActivated() = 0;

};

#endif //__IWEAPON_H__
