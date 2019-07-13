/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Control pipe between AI and game objects.
  
 -------------------------------------------------------------------------
  History:
  - 29:11:2005: Created by Craig Tiller

*************************************************************************/

#ifndef __IMOVEMENTCONTROLLER_H__
#define __IMOVEMENTCONTROLLER_H__

#pragma once

#include "IAgent.h"
#include "IAnimationGraph.h"

// IMPORTANT NOTE: wherever "Target" is mentioned in this file, we talk
// about a target position, *not* direction.

struct SActorTargetParams
{
	SActorTargetParams() : 
		location(ZERO), 
		direction(FORWARD_DIRECTION), 
		vehicleSeat(0), 
		speed(-1.0f), 
		directionRadius(gf_PI), 
		locationRadius(0.5f), 
		startRadius(-1.0f),
		signalAnimation(true), 
		projectEnd(false),
		navSO(false),
		stance(STANCE_NULL), 
		pQueryStart(NULL), 
		pQueryEnd(NULL),
  		triggerUser(eAGTU_AI)
	{
	}

	Vec3 location; // target location
	Vec3 direction;
	string vehicleName;
	int vehicleSeat;
	float speed;
	float directionRadius; // amount of 'slop' allowed in direction (meters)
	float locationRadius; // amount of 'slop' allowed in target location (meters)
	float startRadius; // radius around the start direction
	bool signalAnimation; // true if we're to send a signal at a target, false if we're to set an action
	bool projectEnd;
	bool navSO;
	string animation;
	EStance stance;
	TAnimationGraphQueryID * pQueryStart;
	TAnimationGraphQueryID * pQueryEnd;
	EAnimationGraphTriggerUser triggerUser;
};

class CMovementRequest
{
public:
	CMovementRequest()
		: m_flags(0),
			m_lookTarget(0), m_aimTarget(0), m_fireTarget(0), m_moveTarget(0),
			m_desiredSpeed(0), m_lookImportance(0),
			m_deltaMovement(0),m_deltaRotation(ZERO),
			m_desiredLean(0),
			m_pseudoSpeed(0),
			m_forcedNavigation(0),
			m_desiredBodyDirectionAtTarget(0), m_distanceToPathEnd(-1.0f), 
			m_alertness(0), m_alertnessLast(0),
			m_stance(STANCE_NULL)
	{
	}

	// was anything set at all in this request?`
	bool IsEmpty()
	{
		return m_flags == 0;
	}

	void SetAlertness(int alertness)
	{
		m_alertness = alertness;
	}

	ILINE int GetAlertness()
	{
		return m_alertness;
	}

	ILINE bool AlertnessChanged()
	{
		bool ret(m_alertnessLast!=m_alertness);
		m_alertnessLast = m_alertness;
		return ret;
	}

	void SetActorTarget( const SActorTargetParams& params )
	{
		m_actorTarget = params;
		SetFlag(eMRF_ActorTarget);
		ClearFlag(eMRF_RemoveActorTarget);
	}

	void ClearActorTarget()
	{
		ClearFlag(eMRF_ActorTarget);
		SetFlag(eMRF_RemoveActorTarget);
	}

	void SetLookTarget( const Vec3& position, float importance = 1.0f )
	{
		m_lookTarget = position;
		m_lookImportance = importance;
		SetFlag(eMRF_LookTarget);
		ClearFlag(eMRF_RemoveLookTarget);
	}

	void ClearLookTarget()
	{
		SetFlag(eMRF_RemoveLookTarget);
		ClearFlag(eMRF_LookTarget);
	}

	void SetAimTarget( const Vec3& position )
	{
		m_aimTarget = position;
		SetFlag(eMRF_AimTarget);
		ClearFlag(eMRF_RemoveAimTarget);
	}

	void ClearAimTarget()
	{
		ClearFlag(eMRF_AimTarget);
		SetFlag(eMRF_RemoveAimTarget);
	}

	void SetBodyTarget( const Vec3& position )
	{
		m_aimTarget = position;
		SetFlag(eMRF_BodyTarget);
		ClearFlag(eMRF_RemoveBodyTarget);
	}

	void ClearBodyTarget()
	{
		ClearFlag(eMRF_BodyTarget);
		SetFlag(eMRF_RemoveBodyTarget);
	}

	void SetFireTarget( const Vec3& position )
	{
		m_fireTarget = position;
		SetFlag(eMRF_FireTarget);
		ClearFlag(eMRF_RemoveFireTarget);
	}

	void ClearFireTarget()
	{
		ClearFlag(eMRF_FireTarget);
		SetFlag(eMRF_RemoveFireTarget);
	}

	void SetLean( float lean )
	{
		m_desiredLean = lean;
		SetFlag(eMRF_DesiredLean);
		ClearFlag(eMRF_RemoveDesiredLean);
	}

	void ClearLean()
	{
		ClearFlag(eMRF_DesiredLean);
		SetFlag(eMRF_RemoveDesiredLean);
	}

	void SetDesiredSpeed( float speed )
	{
		m_desiredSpeed = speed;
		SetFlag(eMRF_DesiredSpeed);
		ClearFlag(eMRF_RemoveDesiredSpeed);
	}

	void ClearDesiredSpeed()
	{
		ClearFlag(eMRF_DesiredSpeed);
		SetFlag(eMRF_RemoveDesiredSpeed);
	}

	void SetPseudoSpeed( float speed )
	{
		m_pseudoSpeed = speed;
		SetFlag(eMRF_PseudoSpeed);
		ClearFlag(eMRF_RemovePseudoSpeed);
	}

	void ClearPseudoSpeed()
	{
		ClearFlag(eMRF_PseudoSpeed);
		SetFlag(eMRF_RemovePseudoSpeed);
	}

	void SetPrediction( const SPredictedCharacterStates& prediction)
	{
		m_prediction = prediction;
		SetFlag(eMRF_Prediction);
	}

	const SPredictedCharacterStates& GetPrediction() const
	{
		assert( HasPrediction() );
		return m_prediction;
	}

	void ClearPrediction()
	{
		ClearFlag(eMRF_Prediction);
		SetFlag(eMRF_RemovePrediction);
	}

	void AddDeltaMovement( const Vec3& direction )
	{
		if (!CheckFlag(eMRF_DeltaMovement))
			m_deltaMovement = direction;
		else
			m_deltaMovement += direction;
		SetFlag(eMRF_DeltaMovement);
	}

	void RemoveDeltaMovement()
	{
		ClearFlag(eMRF_DeltaMovement);
	}

	void AddDeltaRotation( const Ang3& rotation )
	{
		if (!CheckFlag(eMRF_DeltaRotation))
			m_deltaRotation = rotation;
		else
			m_deltaRotation += rotation;
		SetFlag(eMRF_DeltaRotation);
	}

	void RemoveDeltaRotation()
	{
		ClearFlag(eMRF_DeltaRotation);
	}

	void SetDistanceToPathEnd( float dist )
	{
		m_distanceToPathEnd = dist;
	}

	void ClearDistanceToPathEnd()
	{
		m_distanceToPathEnd = -1.0f;
	}

	void SetJump()
	{
		SetFlag(eMRF_Jump);
	}

	void ClearJump()
	{
		ClearFlag(eMRF_Jump);
	}

	void SetAllowStrafing()
	{
		SetFlag(eMRF_AllowStrafing);
	}

	void SetAllowStrafing( bool allow )
	{
		if (allow)
			SetAllowStrafing();
		else
			ClearAllowStrafing();
	}

	void ClearAllowStrafing()
	{
		ClearFlag(eMRF_AllowStrafing);
	}

	bool AllowStrafing() const
	{
		return CheckFlag(eMRF_AllowStrafing);
	}

	void SetStance( EStance stance )
	{
		m_stance = stance;
		SetFlag(eMRF_Stance);
		ClearFlag(eMRF_RemoveStance);
	}

	void ClearStance()
	{
		ClearFlag(eMRF_Stance);
		SetFlag(eMRF_RemoveStance);
	}

	void SetMoveTarget( const Vec3& pos )
	{
		m_moveTarget = pos;
		SetFlag(eMRF_MoveTarget);
		ClearFlag(eMRF_RemoveMoveTarget);
	}

	void ClearMoveTarget()
	{
		ClearFlag(eMRF_MoveTarget);
		SetFlag(eMRF_RemoveMoveTarget);
	}

	void SetForcedNavigation( const Vec3& pos )
	{
		m_forcedNavigation = pos;
		SetFlag(eMRF_ForcedNavigation);
		ClearFlag(eMRF_RemoveForcedNavigation);
	}

	void ClearForcedNavigation()
	{
		m_forcedNavigation = ZERO;
		ClearFlag(eMRF_ForcedNavigation);
		SetFlag(eMRF_RemoveForcedNavigation);
	}

	bool HasForcedNavigation() const
	{
		return CheckFlag(eMRF_ForcedNavigation);
	}

	bool HasLookTarget() const
	{
		return CheckFlag(eMRF_LookTarget);
	}

	bool RemoveLookTarget() const
	{
		return CheckFlag(eMRF_RemoveLookTarget);
	}

	const Vec3& GetLookTarget() const
	{
		assert( HasLookTarget() );
		return m_lookTarget;
	}

	float GetLookImportance() const
	{
		assert( HasLookTarget() );
		return m_lookImportance;
	}

	bool HasAimTarget() const
	{
		return CheckFlag(eMRF_AimTarget);
	}

	bool RemoveAimTarget() const
	{
		return CheckFlag(eMRF_RemoveAimTarget);
	}

	const Vec3& GetAimTarget() const
	{
		assert( HasAimTarget() );
		return m_aimTarget;
	}

	bool HasBodyTarget() const
	{
		return CheckFlag(eMRF_BodyTarget);
	}

	bool RemoveBodyTarget() const
	{
		return CheckFlag(eMRF_RemoveBodyTarget);
	}

	const Vec3& GetBodyTarget() const
	{
		assert( HasBodyTarget() );
		return m_aimTarget;
	}

	bool HasFireTarget() const
	{
		return CheckFlag(eMRF_FireTarget);
	}

	bool RemoveFireTarget() const
	{
		return CheckFlag(eMRF_RemoveFireTarget);
	}

	const Vec3& GetFireTarget() const
	{
		assert( HasFireTarget() );
		return m_fireTarget;
	}

	bool HasDesiredSpeed() const
	{
		return CheckFlag(eMRF_DesiredSpeed);
	}

	bool RemoveDesiredSpeed() const
	{
		return CheckFlag(eMRF_RemoveDesiredSpeed);
	}

	float GetDesiredSpeed() const
	{
		assert( HasDesiredSpeed() );
		return m_desiredSpeed;
	}

	bool HasPseudoSpeed() const
	{
		return CheckFlag(eMRF_PseudoSpeed);
	}

	bool RemovePseudoSpeed() const
	{
		return CheckFlag(eMRF_RemovePseudoSpeed);
	}

	bool HasPrediction() const
	{
		return CheckFlag(eMRF_Prediction);
	}

	bool RemovePrediction() const
	{
		return CheckFlag(eMRF_RemovePrediction);
	}

	float GetPseudoSpeed() const
	{
		assert( HasPseudoSpeed() );
		return m_pseudoSpeed;
	}

	bool ShouldJump() const
	{
		return CheckFlag(eMRF_Jump);
	}

	bool HasStance() const
	{
		return CheckFlag(eMRF_Stance);
	}

	bool RemoveStance() const
	{
		return CheckFlag(eMRF_RemoveStance);
	}

	EStance GetStance() const
	{
		assert( HasStance() );
		return m_stance;
	}

	bool HasMoveTarget() const
	{
		return CheckFlag(eMRF_MoveTarget);
	}

	bool RemoveMoveTarget() const
	{
		return CheckFlag(eMRF_RemoveMoveTarget);
	}

	const Vec3& GetMoveTarget() const
	{
		assert( HasMoveTarget() );
		return m_moveTarget;
	}

	const Vec3& GetForcedNavigation() const
	{
		assert( HasForcedNavigation() );
		return m_forcedNavigation;
	}

	float GetDistanceToPathEnd() const
	{
		return m_distanceToPathEnd;
	}

	void SetNoAiming()
	{
		SetFlag(eMRF_NoAiming);
	}

	bool HasNoAiming() const
	{
		return CheckFlag(eMRF_NoAiming);
	}

	bool HasDeltaMovement() const
	{
		return CheckFlag(eMRF_DeltaMovement);
	}

	const Vec3& GetDeltaMovement() const
	{
		assert( HasDeltaMovement() );
		return m_deltaMovement;
	}

	bool HasDeltaRotation() const
	{
		return CheckFlag(eMRF_DeltaRotation);
	}

	const Ang3& GetDeltaRotation() const
	{
		assert( HasDeltaRotation() );
		return m_deltaRotation;
	}

	bool HasLean() const
	{
		return CheckFlag(eMRF_DesiredLean);
	}

	float GetLean() const
	{
		assert( HasLean() );
		return m_desiredLean;
	}

	bool RemoveLean() const
	{
		return CheckFlag(eMRF_RemoveDesiredLean);
	}

	bool RemoveActorTarget() const
	{
		return CheckFlag(eMRF_RemoveActorTarget);
	}

	bool HasActorTarget() const
	{
		return CheckFlag(eMRF_ActorTarget);
	}

	const SActorTargetParams& GetActorTarget() const
	{
		assert(CheckFlag(eMRF_ActorTarget));
		return m_actorTarget;
	}

private:
	enum EMovementRequestFlags
	{
		// do we have some parameter
		eMRF_LookTarget                         = 0x00000001,
		eMRF_AimTarget                          = 0x00000002,
		eMRF_DesiredSpeed                       = 0x00000004,
		eMRF_Stance                             = 0x00000008,
		eMRF_MoveTarget                         = 0x00000010,
		eMRF_DeltaMovement                      = 0x00000020,
		eMRF_DeltaRotation                      = 0x00000040,
		eMRF_DesiredLean                        = 0x00000100,
		eMRF_ActorTarget                        = 0x00000200,
		eMRF_FireTarget                         = 0x00000400,
		eMRF_PseudoSpeed                        = 0x00000800,
		eMRF_Prediction													= 0x00001000,
		eMRF_ForcedNavigation										= 0x00002000,
		eMRF_BodyTarget                         = 0x00004000,

		// other flags
		eMRF_Jump                               = 0x80000000,
		eMRF_NoAiming                           = 0x40000000,
		eMRF_AllowStrafing											= 0x20000000,

		eMRF_RemoveLookTarget                   = 0x08000000,
		eMRF_RemoveAimTarget                    = 0x04000000,
		eMRF_RemoveDesiredSpeed                 = 0x02000000,
		eMRF_RemoveStance                       = 0x01000000,
		eMRF_RemoveMoveTarget                   = 0x00800000,
		eMRF_RemoveDesiredLean                  = 0x00100000,
		eMRF_RemoveActorTarget                  = 0x00080000,
		eMRF_RemoveFireTarget                   = 0x00040000,
		eMRF_RemovePseudoSpeed                  = 0x00020000,
		eMRF_RemovePrediction										= 0x00010000,
		eMRF_RemoveForcedNavigation							= 0x00008000,
		eMRF_RemoveBodyTarget                   = 0x10000000,
	};

	void ClearFlag( EMovementRequestFlags flag )
	{
		m_flags &= ~flag;
	}
	void SetFlag( EMovementRequestFlags flag )
	{
		m_flags |= flag;
	}
	bool CheckFlag( EMovementRequestFlags flag ) const
	{
		return (m_flags & flag) != 0;
	}

	unsigned m_flags;
	Vec3 m_lookTarget;
	Vec3 m_aimTarget;
	Vec3 m_fireTarget;
	float m_desiredSpeed;
	float m_lookImportance;
	float m_desiredSpeedAtTarget;
	float	m_distanceToPathEnd;
	EStance m_stance;
	Vec3 m_moveTarget;
	Vec3 m_desiredBodyDirectionAtTarget;
	Vec3 m_deltaMovement;
	Ang3 m_deltaRotation;
	float m_desiredLean;
	SActorTargetParams m_actorTarget;
	int	m_alertness;
	int m_alertnessLast;
	float m_pseudoSpeed;
	SPredictedCharacterStates	m_prediction;
	Vec3 m_forcedNavigation;

};

struct SStanceState
{
	SStanceState() :
		pos(0,0,0),
		bodyDirection(FORWARD_DIRECTION),
		upDirection(0,0,1),
		weaponPosition(0,0,0),
		aimDirection(FORWARD_DIRECTION),
		fireDirection(FORWARD_DIRECTION),
		eyePosition(0,0,0),
		eyeDirection(FORWARD_DIRECTION),
		lean(0.0f)
	{
		m_StanceSize.min=Vec3(ZERO);		// Game logic bounds of the character related to the 'pos'.
		m_StanceSize.max=Vec3(ZERO);		// Game logic bounds of the character related to the 'pos'.
		m_ColliderSize.min=Vec3(ZERO);			// The size of only the collider in this stance.
		m_ColliderSize.max=Vec3(ZERO);			// The size of only the collider in this stance.
	}

	// Note: All positions a directions are in worldspace.
	Vec3 pos;								// Position of the character.
	Vec3 bodyDirection;			// Direction of the body of the character.
	Vec3 upDirection;				// Up direction of the character.
	Vec3 weaponPosition;		// Game logic position of the weapon of the character.
	Vec3 aimDirection;			// Direction from the weapon position to aim target, used for representation.
	Vec3 fireDirection;			// Direction from the weapon position to the fire target, used for emitting the bullets.
	Vec3 eyePosition;				// Game logic position of the eye of the character.
	Vec3 eyeDirection;			// Direction from the eye position to the lookat or aim-at target.
	float lean;							// The amount the character is leaning -1 = left, 1 = right;
	AABB m_StanceSize;				// Game logic bounds of the character related to the 'pos'.
	AABB m_ColliderSize;			// The size of only the collider in this stance.
};

struct SMovementState : public SStanceState
{
	SMovementState() :
		SStanceState(),
		fireTarget(0,0,0),
		stance(STANCE_NULL),
		animationEyeDirection(FORWARD_DIRECTION),
		movementDirection(0,0,0),
		desiredSpeed(0.0f),
		minSpeed(0.2f),
		normalSpeed(1.0f),
		maxSpeed(2.0f),
		slopeAngle(0.0f),
		atMoveTarget(false),
		isAlive(true),
		isAiming(false),
		isFiring(false),
		isVisible(false)
	{
	}

	Vec3 fireTarget;						// Target position to fire at, note the direction from weapon to the fire target
															// can be different than aim direction. This value is un-smoothed target set by AI.
	EStance stance;							// Stance of the character.
	Vec3 animationEyeDirection; // Eye direction reported from Animation [used for cinematic look-ats]
	Vec3 movementDirection;
	float desiredSpeed;
	float minSpeed;
	float normalSpeed;
	float maxSpeed;
	float slopeAngle; // Degrees of ground below character (positive when facing uphill, negative when facing downhill).
	bool atMoveTarget;
	bool isAlive;
	bool isAiming;
	bool isFiring;
	bool isVisible;
};

struct IMovementController
{
	// Description:
	//    Request some movement; 
	//    If the request cannot be fulfilled, returns false, and request
	//    is updated to be a similar request that could be fulfilled
	//    (calling code is then free to inspect this, and call RequestMovement
	//    again to set a new movement)
	virtual bool RequestMovement( CMovementRequest& request ) = 0;
	// Description:
	//    Fetch the current movement state of the entity
	virtual void GetMovementState( SMovementState& state ) = 0;
	// Description:
	//    Returns the description of the stance as if the specified stance would be set right now.
	//		If the parameter 'defaultPose' is set to false, the current aim and look target info is used to
	//		calculate the stance info, else a default pose facing along positive Y-axis is returned.
	//		Returns false if the description cannot be queried.
	virtual bool GetStanceState( EStance stance, float lean, bool defaultPose, SStanceState& state ) = 0;
};

#endif
