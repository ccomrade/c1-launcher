// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _AGENTPARAMS_H_
#define _AGENTPARAMS_H_

#include "SerializeFwd.h"


//
//this structure should contain all the relevant information for perception/visibility determination
struct AgentPerceptionParameters
{
	// Perception abilities

	// How far can the agent see
	float	sightRange;
	// How far can the agent see if the target is vehicle
	float	sightRangeVehicle;

	// Sign envelope threshold percentage on normal and alarmed conditions.
	float sightEnvScaleNormal;
	float sightEnvScaleAlarmed;

	// how movement velocity affects perception
	//VELmultyplier = (velBase + velScale*CurrentVel^2);
	//current priority gets scaled by VELmultyplier
	//example: if agent should see only moving objects, set velBase=0; velScale=3
	float		velBase;					
	float		velScale;
	// Normal and peripheral fov (degrees)
	float		FOVPrimary;
	float		FOVSecondary;
	// How heights of the target affects visibility
	float		stanceScale;
	// The sensitivity to sounds. 0=deaf, 1=normal.
	float		audioScale;
	// Flag indicating that the agent has thermal vision.
	bool		bThermalVision;

	// Perceptible parameters
	
	// how much am I visible (example: sniper .2; scientist 5)
	float		camoScale;
	// Equivalent to camo scale, used with thermal vision.
	float		heatScale;

	float		targetPersistence;

	float		collisionReactionScale;
	float		stuntReactionTimeOut;

	float		forgetfulness;	// overall scaling, controlled by FG. 
	float		forgetfulnessTarget;		//	when above PERCEPTION_TARGET_SEEN_VALUE
	float		forgetfulnessSeek;	//	when below PERCEPTION_TARGET_SEEN_VALUE
	float		forgetfulnessMemory;		//	when below PERCEPTION_SOMETHING_SEEN_VALUE

	bool		isAffectedByLight;		// flag indicating if the agent perception is affected by light conditions.
	float		minAlarmLevel;

	float bulletHitRadius; // radius for perceiving bullet hits nearby
	struct { float visual; float audio; } perceptionScale;

	AgentPerceptionParameters():sightRange(0),sightRangeVehicle(-1),FOVPrimary(-1),FOVSecondary(-1),camoScale(1),velBase(1.0f),velScale(.3f),audioScale(1),
		stanceScale(1.0f), bThermalVision(false),heatScale(1),targetPersistence(0.f),forgetfulness(1.f),
		forgetfulnessTarget(1.f),forgetfulnessSeek(1.f),forgetfulnessMemory(1.f), collisionReactionScale(1.0f), stuntReactionTimeOut(3.0f),
		isAffectedByLight(false),bulletHitRadius(0.f),minAlarmLevel(0.0f),sightEnvScaleNormal(0.2f),sightEnvScaleAlarmed(0.5f)

	{
		perceptionScale.visual=1.f;perceptionScale.audio=1.f;
	}

	void Serialize(TSerialize ser);

};

typedef struct AgentParameters
{
	AgentPerceptionParameters	m_PerceptionParams;
	//-------------
	// combat class of this agent
	//----------------
	int m_CombatClass;

	//-------------
	float m_fAccuracy;
	float	m_fPassRadius;
	float m_fStrafingPitch;		//if this > 0, will do a strafing draw line firing. 04/12/05 Tetsuji
	float m_fDistanceToHideFrom;

	// behaviour
	float m_fAttackRange;
	float m_fCommRange;
	float m_fAttackZoneHeight;
	float m_fPreferredCombatDistance;
	float m_fGrenadeThrowDistScale;	// Controls the preferred throw distance of a grenade. In range [0..1].

	int	m_weaponAccessories;

	//-----------
	// hostility data
	//------------
	bool m_bSpeciesHostility;
	float m_fGroupHostility;
	float m_fMeleeDistance;

	//-------------
	// grouping data
	//--------------
	int		m_nSpecies;
	int		m_nGroup;
	int		m_nRank;

	//-------------
	// track pattern
	//--------------
	string	m_trackPatternName;			// The pattern name to use, empty string will disable the pattern.
	int			m_trackPatternContinue;	// Signals to continue to advance on the pattern, this is a three state flag, 0 means no change (default), 1=continue, 2=stop

	bool  m_bAiIgnoreFgNode;
	bool  m_bPerceivePlayer;
	float m_fAwarenessOfPlayer;
	bool  m_bSpecial;
	//--------------------------
	//	cloaking/invisibility
	//--------------------------
	bool	m_bInvisible;
	float	m_fCloakScale;	// 0- no cloaked, 1- complitly cloaked (invisible)
	float	m_fCloakScaleTarget;	// cloak scale should fade to this value

	//--------------------------
	//	Turn speed
	//--------------------------
	float m_lookIdleTurnSpeed;	// How fast the character turn towards target when looking (radians/sec). -1=immediate
	float m_lookCombatTurnSpeed;	// How fast the character turn towards target when looking (radians/sec). -1=immediate
	float m_aimTurnSpeed;		// How fast the character turn towards target when aiming (radians/sec). -1=immediate
	float m_fireTurnSpeed;	// How fast the character turn towards target when aiming and firing (radians/sec). -1=immediate

	AgentParameters()
	{
		Reset();
	}

	void Serialize(TSerialize ser);

	void Reset()
	{
		m_CombatClass = -1;
		m_fAccuracy = 0.0f;
		m_fPassRadius = 0.4f;
		m_fStrafingPitch = 0.0f;
		m_fDistanceToHideFrom = 0.0f;
		m_fAttackRange = 0.0f;
		m_fCommRange = 0.0f;
		m_fAttackZoneHeight = 0.0f;
		m_fPreferredCombatDistance = 0.0f;
		m_bSpeciesHostility = true;
		m_fGroupHostility = 0.0f;
		m_fMeleeDistance = 2.0f;
		m_nSpecies = -1;
		m_nRank = 0;
		m_nGroup = 0;
		m_trackPatternName = "";
		m_trackPatternContinue = 0;
		m_bAiIgnoreFgNode = false;
		m_bPerceivePlayer = true;
		m_fAwarenessOfPlayer = 0;
		m_bSpecial = false;
		m_bInvisible = false;
		m_fCloakScale = 0.0f;
		m_fCloakScaleTarget = 0.0f;
		m_weaponAccessories = 0;
		m_lookIdleTurnSpeed = -1;
		m_lookCombatTurnSpeed = -1;
		m_aimTurnSpeed = -1;
		m_fireTurnSpeed = -1;
		m_fGrenadeThrowDistScale = 1.0f;
	}
	
} AgentParameters;


#endif _AGENTPARAMS_H_

