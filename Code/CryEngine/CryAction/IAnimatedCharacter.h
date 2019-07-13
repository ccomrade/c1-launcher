////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2006.
// -------------------------------------------------------------------------
//  File name:   CryName.h
//  Version:     v1.00
//  Created:     Craig Tiller.
//  Compilers:   Visual Studio.NET
//  Description: Handles locomotion/physics integration for objects
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IANIMATEDCHARACTER_H__
#define __IANIMATEDCHARACTER_H__

#pragma once

#include "IGameObject.h"
#include "IAnimationGraph.h"
#include "IFacialAnimation.h"
#include "SerializeFwd.h"

#include "CryCharAnimationParams.h"

//--------------------------------------------------------------------------------

enum EAnimationGraphLayerID
{
	eAnimationGraphLayer_FullBody = 0,
	eAnimationGraphLayer_UpperBody,

	eAnimationGraphLayer_COUNT
};

//--------------------------------------------------------------------------------

enum ECharacterMoveType
{
	eCMT_None = 0,
	eCMT_Normal,
	eCMT_Fly,
	eCMT_Swim,
	eCMT_ZeroG,

	eCMT_Impulse,
	eCMT_JumpInstant,
	eCMT_JumpAccumulate

};

//--------------------------------------------------------------------------------

enum EAnimatedCharacterFlags
{
	// these flags are mutually exclusive, and define the game/blend mode override
	eACF_AlwaysAnimation           = 0x00000001,
	eACF_AlwaysPhysics             = 0x00000002,
	eACF_PerAnimGraph              = 0x00000004,

	// humans moving on a plane should take the z-coordinate from physics, other objects should not
	eACF_ZCoordinateFromPhysics    = 0x00000008,
	// should we process movement in the character, or just follow it?
	eACF_EnableMovementProcessing  = 0x00000010,
	//use or not the transRot2000
	eACF_NoTransRot2k	 						 = 0x00000020,
	// should we constrain desired movement to the x-y plane?
	eACF_ConstrainDesiredSpeedToXY = 0x00000040,
	//this is used when the character is attached to something (vehicles for example) 
	//and there is no need to correct the gap between animation and physics.
	eACF_NoLMErrorCorrection			= 0x00000080,
	// change stances immediately
	eACF_ImmediateStance					= 0x00000100,
	// use human blending system
	eACF_UseHumanBlending         = 0x00000200,
	eACF_LockToEntity             = 0x00000400,
  // limit entity movement according to animation
  eACF_AllowEntityClampingByAnimation = 0x00000800,

	eACF_Frozen										= 0x00001000,
};

//--------------------------------------------------------------------------------

/*
struct SPredictedCharacterState
{
	Vec3 position; // 
	Quat orientation; // 

	Vec3 velocity; // linear velocity
	Quat rotation; // angular velocity (currently not so important, may be kept at zero).

	float deltatime; // relative time of prediction 
};
*/

//--------------------------------------------------------------------------------

struct SCharacterMoveRequest
{
	SCharacterMoveRequest()
	: type(eCMT_None)
	, velocity(ZERO)
	, rotation(Quat::CreateIdentity())
	, allowStrafe(false)
	, proceduralLeaning(false)
	, jumping(false)
	{}

	ECharacterMoveType type;

	Vec3 velocity; // meters per second (world space)
	Quat rotation; // relative, but immediate (per frame, not per second)

	SPredictedCharacterStates prediction;

	bool allowStrafe;
	bool proceduralLeaning;
	bool jumping;

	// TODO: pass left/right turn intent
};

//--------------------------------------------------------------------------------

struct SAnimationBlendingParams
{
	SAnimationBlendingParams()
	{
		m_yawAngle = 0;
		m_speed = 0;
		m_strafeParam = 0;
		m_StrafeVector = ZERO;
		m_turnAngle = 0;
		m_diffBodyMove = 0;
		m_fBlendedDesiredSpeed = 0;
		m_fUpDownParam = 0;
	}
	f32 m_yawAngle; 
	f32 m_speed; 
	f32 m_strafeParam; 
	Vec3 m_StrafeVector; 
	f32 m_turnAngle;
	f32 m_diffBodyMove;
	f32 m_fBlendedDesiredSpeed;
	f32 m_fUpDownParam;
};

//--------------------------------------------------------------------------------

struct IAnimationBlending
{
	virtual ~IAnimationBlending(){}
	//
	virtual SAnimationBlendingParams *Update(IEntity *pIEntity, Vec3 DesiredBodyDirection, Vec3 DesiredMoveDirection, f32 fDesiredMovingSpeed) = 0;
	virtual void GetMemoryStatistics(ICrySizer * s) = 0;
};

//--------------------------------------------------------------------------------

struct SAnimatedCharacterParams
{
	SAnimatedCharacterParams() : flags(eACF_PerAnimGraph|eACF_EnableMovementProcessing), 
															modeChangeTime(0.1f),
															inertia(0), inertiaAccel(0), inertiaChanged(false), timeImpulseRecover(0.0f),
															physErrorInnerRadiusFactor(0.0f), physErrorOuterRadiusFactor(0.07f), 
															physErrorMinOuterRadius(0.0f), physErrorMaxOuterRadius(2.0f),
															pAnimationBlending(0) {} 

	SAnimatedCharacterParams ModifyFlags( uint32 flagsOn, uint32 flagsOff ) const
	{
		SAnimatedCharacterParams copy = *this;
		copy.flags |= flagsOn;
		copy.flags &= ~flagsOff;
		return copy;
	}

	void SetInertia(float i,float ia)
	{
		inertia = i;
		inertiaAccel = ia;
		inertiaChanged = true;
	}

	bool InertiaChanged()
	{
		bool ret(inertiaChanged);
		inertiaChanged = false;
		return ret;
	}

	uint32 flags;
	float inertia;
	float inertiaAccel;
	float timeImpulseRecover;
	float modeChangeTime;

	float physErrorInnerRadiusFactor;
	float physErrorOuterRadiusFactor;
	float physErrorMinOuterRadius;
	float physErrorMaxOuterRadius;

	IAnimationBlending *pAnimationBlending;

private:

	bool inertiaChanged;
};

//--------------------------------------------------------------------------------

struct IAnimatedCharacter : public IGameObjectExtension
{
	virtual IAnimationGraphPtr GetAnimationGraph( int layer ) = 0;
	virtual IAnimationGraphState * GetAnimationGraphState() = 0;
	virtual void PushForcedState( const char * state ) = 0;
	virtual void ClearForcedStates() = 0;
	virtual void ChangeGraph( const char * graph, int layer ) = 0;
	virtual void ResetState() = 0;

	// movement related - apply some physical impulse/movement request to an object
	// and make sure it syncs correctly with animation
	virtual void AddMovement( const SCharacterMoveRequest& ) = 0;
	virtual void SetEntityRotation(const Quat &rot) = 0;

	virtual const QuatT& GetAnimLocation() const = 0;

	virtual const SAnimatedCharacterParams& GetParams() = 0;
	virtual void SetParams( const SAnimatedCharacterParams& params ) = 0;

	virtual void SetExtraAnimationOffset( const Matrix34& offset ) = 0;
	virtual void SetExtraAnimationOffset( const QuatT& offset ) = 0;

	// stance helper functions
	virtual int GetCurrentStance() = 0;
	virtual bool InStanceTransition() = 0;
	virtual void RequestStance( int stanceID, const char * name ) = 0;

	//
	virtual bool IsAnimationControlledView() const = 0;
	virtual float FilterView(SViewParams &viewParams) const = 0;
	
	virtual EColliderMode GetPhysicalColliderMode() = 0;
	virtual void ForceRefreshPhysicalColliderMode() = 0;
	virtual void RequestPhysicalColliderMode(EColliderMode mode, EColliderModeLayer layer, const char* tag = NULL) = 0;
	virtual void SetMovementControlMethods(EMovementControlMethod horizontal, EMovementControlMethod vertical) = 0;
	virtual void SetLocationClampingOverride(float distance, float angle) = 0;

	// Used by fall and play, when transitioning from ragdoll to get up animation.
	virtual void ForceTeleportAnimationToEntity() = 0;

	//virtual void MakePushable(bool enable) = 0;

	virtual void SetFacialAlertnessLevel(int alertness) = 0;
	virtual int GetFacialAlertnessLevel() = 0;

	// look IK is allowed by default. use this to disallow it
	virtual void AllowLookIk( bool allow ) = 0;
	virtual bool IsLookIkAllowed() const = 0;

	virtual void TriggerRecoil(float duration, float distance) = 0;
	virtual void SetWeaponRaisedPose(EWeaponRaisedPose pose) = 0;

	virtual void SetNoMovementOverride(bool external) = 0;

	// Returns the angle (in degrees) of the ground below the character.
	// Zero is flat ground (along facing direction), positive when character facing uphill, negative when character facing downhill.
	virtual float GetGroundSlopeAngle() const = 0;
};

//--------------------------------------------------------------------------------

#endif
