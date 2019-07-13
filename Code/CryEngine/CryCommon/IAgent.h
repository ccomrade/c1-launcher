// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _IAGENT_H_
#define _IAGENT_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Cry_Math.h"
#include "AgentParams.h"
#include "IScriptSystem.h"
#include "IAISystem.h"
#include "SerializeFwd.h"
#include "IPhysics.h"
#include <algorithm>
#include <limits>


#ifdef LINUX
#	include "platform.h"
#endif

#ifdef max
#undef max
#endif

struct IWeapon;
struct IEntity;
struct IUnknownProxy;
struct GraphNode;

class CSmartObject;
class IPersonalInterestManager;

//! Defines for AIObject types

#define	AIOBJECT_NONE 				200
#define AIOBJECT_DUMMY				0

#define AIOBJECT_AIACTOR			1
#define AIOBJECT_CAIACTOR			2
#define AIOBJECT_PIPEUSER			3
#define AIOBJECT_CPIPEUSER		4
#define	AIOBJECT_PUPPET				5
#define AIOBJECT_CPUPPET			6
#define AIOBJECT_VEHICLE			7
#define AIOBJECT_CVEHICLE			8

#define AIOBJECT_AWARE				10
#define AIOBJECT_ATTRIBUTE		11
#define AIOBJECT_WAYPOINT			12
#define AIOBJECT_HIDEPOINT		13
#define AIOBJECT_SNDSUPRESSOR	14
#define AIOBJECT_HELICOPTER			40
#define AIOBJECT_CAR				50
#define AIOBJECT_BOAT				60
#define AIOBJECT_AIRPLANE			70
#define AIOBJECT_2D_FLY				80
#define AIOBJECT_MOUNTEDWEAPON		90
#define AIOBJECT_GLOBALALERTNESS	94
#define AIOBJECT_LEADER				95
#define AIOBJECT_ORDER				96
#define	AIOBJECT_PLAYER 			100

#define	AIOBJECT_GRENADE 		150
#define	AIOBJECT_RPG 			151

#define AI_USE_HIDESPOTS	(1<<14)


// signal ids
#define AISIGNAL_INCLUDE_DISABLED 0
#define AISIGNAL_DEFAULT 1
#define AISIGNAL_PROCESS_NEXT_UPDATE 3
#define AISIGNAL_NOTIFY_ONLY 9
#define AISIGNAL_ALLOW_DUPLICATES 10
#define AISIGNAL_RECEIVED_PREV_UPDATE 30 // internal AI system use only, like AISIGNAL_DEFAULT but used for logging/recording
																				 // a signal sent in the previous update and processed in the current (AISIGNAL_PROCESS_NEXT_UPDATE) 
#define AISIGNAL_INITIALIZATION -100

// anchors
#define	AIANCHOR_FIRST												200
#define	AIANCHOR_COMBAT_HIDESPOT							320
#define	AIANCHOR_COMBAT_HIDESPOT_SECONDARY		330
#define	AIANCHOR_REINFORCEMENT_SPOT						400
#define	AIANCHOR_NOGRENADE_SPOT								405


//! Event types
#define AIEVENT_ONBODYSENSOR		1
#define AIEVENT_ONVISUALSTIMULUS	2
#define AIEVENT_ONPATHDECISION	3
#define AIEVENT_ONSOUNDEVENT		4
#define AIEVENT_AGENTDIED				5
#define AIEVENT_SLEEP						6
#define AIEVENT_WAKEUP					7
#define AIEVENT_ENABLE					8
#define AIEVENT_DISABLE					9
#define AIEVENT_REJECT					10
#define AIEVENT_PATHFINDON			11
#define AIEVENT_PATHFINDOFF			12
#define AIEVENT_CLEAR						15
//#define AIEVENT_MOVEMENT_CONTROL	16		// based on parameters lets object know if he is allowed to move.
#define AIEVENT_DROPBEACON			17
#define AIEVENT_USE							19
#define AIEVENT_CLEARACTIVEGOALS	22
#define AIEVENT_DRIVER_IN				23		// to enable/disable AIVehicles
#define AIEVENT_DRIVER_OUT			24		// to enable/disable AIVehicles
#define AIEVENT_FORCEDNAVIGATION	25
#define AIEVENT_ADJUSTPATH			26
#define AIEVENT_LOWHEALTH				27

#define AIEVENT_PLAYER_STUNT_SPRINT 101
#define AIEVENT_PLAYER_STUNT_JUMP 102
#define AIEVENT_PLAYER_STUNT_PUNCH 103
#define AIEVENT_PLAYER_STUNT_THROW 104
#define AIEVENT_PLAYER_STUNT_THROW_NPC 105
#define AIEVENT_PLAYER_THROW 106
#define AIEVENT_PLAYER_STUNT_CLOAK 107
#define AIEVENT_PLAYER_STUNT_UNCLOAK 108

//! System Events types
#define AISYSEVENT_DISABLEMODIFYER	1


#define AIREADIBILITY_INTERESTING		5
#define AIREADIBILITY_SEEN				10
#define AIREADIBILITY_LOST				20
#define AIREADIBILITY_NORMAL			30
#define AIREADIBILITY_NOPRIORITY		1

#define AIGOALPIPE_LOOP				0
#define AIGOALPIPE_RUN_ONCE			1 //TO DO: not working yet - see PipeUser.cpp
#define AIGOALPIPE_NOTDUPLICATE	2
#define AIGOALPIPE_HIGHPRIORITY		4	// it will be not removed when a goal pipe is selected
#define AIGOALPIPE_SAMEPRIORITY		8	// sets the priority to be the same as active goal pipe
#define AIGOALPIPE_DONT_RESET_AG	16	// don't reset the AG Input (by default AG Action input is reset to idle)
#define AIGOALPIPE_KEEP_LAST_SUBPIPE	32	// keeps the last inserted subpipe

#define SIGNALFILTER_SENDER					0
#define SIGNALFILTER_LASTOP					1
#define SIGNALFILTER_GROUPONLY			2
#define SIGNALFILTER_SPECIESONLY		3
#define SIGNALFILTER_ANYONEINCOMM		4
#define SIGNALFILTER_TARGET					5
#define SIGNALFILTER_SUPERGROUP			6
#define SIGNALFILTER_SUPERSPECIES		7
#define SIGNALFILTER_SUPERTARGET		8
#define SIGNALFILTER_NEARESTGROUP		9
#define SIGNALFILTER_NEARESTSPECIES	10
#define SIGNALFILTER_NEARESTINCOMM	11
#define SIGNALFILTER_HALFOFGROUP		12
#define SIGNALFILTER_LEADER					13
#define SIGNALFILTER_GROUPONLY_EXCEPT	14
#define SIGNALFILTER_ANYONEINCOMM_EXCEPT	15
#define SIGNALFILTER_LEADERENTITY		16
#define SIGNALFILTER_NEARESTINCOMM_SPECIES	17
#define SIGNALFILTER_NEARESTINCOMM_LOOKING	18
#define SIGNALFILTER_FORMATION	19
#define SIGNALFILTER_FORMATION_EXCEPT 20
#define SIGNALFILTER_READIBILITY		100
#define SIGNALFILTER_READIBILITYAT	101		// readability anticipation
#define SIGNALFILTER_READIBILITYRESPONSE	102		// readability response

// AI Object filters: define them as one-bit binary masks (1,2,4,8 etc..)
#define AIOBJECTFILTER_SAMESPECIES		1 // AI objects of the same species of the querying object
#define AIOBJECTFILTER_SAMEGROUP		2 // AI objects of the same group of the querying object or with no group
#define AIOBJECTFILTER_NOGROUP			4 // AI objects with Group ID ==AI_NOGROUP
#define AIOBJECTFILTER_INCLUDEINACTIVE	8 // AI objects and Inactive AI Objects

#define AI_NOGROUP	-1 // group id == AI_NOGROUP -> the entity has no group

// AI Object search methods.
//-------------------- for anchors -----------------------------
#define	AIANCHOR_NEAREST										0
#define	AIANCHOR_NEAREST_IN_FRONT						1
#define	AIANCHOR_RANDOM_IN_RANGE						2
#define	AIANCHOR_RANDOM_IN_RANGE_FACING_AT	3
#define	AIANCHOR_NEAREST_FACING_AT					4
#define	AIANCHOR_NEAREST_TO_REFPOINT				5
#define	AIANCHOR_FARTHEST										6
#define	AIANCHOR_BEHIND_IN_RANGE						7
#define	AIANCHOR_LEFT_TO_REFPOINT						8
#define	AIANCHOR_RIGHT_TO_REFPOINT					9
#define	AIANCHOR_HIDE_FROM_REFPOINT					10
//-------------------- bit flags -------------------------------
#define AIANCHOR_SEES_TARGET				(1 << 6)
#define	AIANCHOR_BEHIND							(1 << 7)
//-------------------- anchors over ----------------------------


#define HM_NEAREST							0
#define HM_FARTHEST_FROM_TARGET				1
#define HM_NEAREST_TO_TARGET				2
#define HM_FARTHEST_FROM_GROUP				3
#define HM_NEAREST_TO_GROUP					4							
#define HM_LEFTMOST_FROM_TARGET				5	
#define HM_RIGHTMOST_FROM_TARGET			6	
#define HM_RANDOM							7	
#define HM_FRONTLEFTMOST_FROM_TARGET		8	
#define HM_FRONTRIGHTMOST_FROM_TARGET		9	
#define HM_NEAREST_TO_FORMATION				10						
#define HM_FARTHEST_FROM_FORMATION			11					
#define HM_NEAREST_TOWARDS_TARGET				12 // same as HM_NEAREST_TO_TARGET, but forces the selection amongst obstacles towards the target only
#define HM_NEAREST_BACKWARDS						13
#define HM_NEAREST_PREFER_SIDES		14
//#define HM_NEAREST_HALF_SIDE			15		// consider half searh distance on sides and back
#define HM_NEAREST_TO_ME			16		// same as HM_NEAREST, but the distance considered is to the puppet itself even if point is searched with HM_AROUND_LASTOP
#define HM_NEAREST_TOWARDS_TARGET_PREFER_SIDES		17
#define HM_NEAREST_TOWARDS_TARGET_LEFT_PREFER_SIDES		18
#define HM_NEAREST_TOWARDS_TARGET_RIGHT_PREFER_SIDES		19
#define HM_NEAREST_TOWARDS_REFPOINT		20
//------------------------------------- the onces below all use lastOp result

#define HM_BEHIND_VEHICLES					29
#define HM_USEREFPOINT						30					
#define HM_ASKLEADER						31
#define HM_ASKLEADER_NOSAME					32

#define HM_INCLUDE_SOFTCOVERS		(1 << 6)
#define HM_IGNORE_ENEMIES				(1 << 7)
#define HM_BACK									(1 << 8)
#define HM_AROUND_LASTOP				(1 << 9)
#define HM_FROM_LASTOP					(1 << 10)
#define HM_USE_LASTOP_RADIUS		(1 << 11)
#define HM_ON_SIDE							(1 << 12)		// to choose spot on left/right side of cover, not behind

#define HM_NEAREST_TO_LASTOPRESULT			(HM_NEAREST+HM_AROUND_LASTOP)
#define HM_RANDOM_AROUND_LASTOPRESULT		(HM_RANDOM+HM_AROUND_LASTOP)


#define BODYPOS_NONE		-1
#define BODYPOS_STAND		0
#define BODYPOS_CROUCH		1
#define BODYPOS_PRONE		2
#define BODYPOS_RELAX		3
#define BODYPOS_STEALTH		4

#define PERCEPTION_MAX_VALUE 30.f
#define PERCEPTION_TARGET_SEEN_VALUE 20.f
#define PERCEPTION_SOMETHING_SEEN_VALUE 10.f

#define	AISPEED_ZERO    0.0f
#define	AISPEED_SLOW    0.21f
#define	AISPEED_WALK    0.4f
#define	AISPEED_RUN     1.0f
#define AISPEED_SPRINT  1.4f

#define AI_JUMP_CHECK_COLLISION (1<<0)
#define AI_JUMP_ON_GROUND				(1<<1)
#define AI_JUMP_RELATIVE				(1<<2)
#define AI_JUMP_MOVING_TARGET		(1<<3)

#define AI_LOOKAT_CONTINUOUS (1<<0)
#define AI_LOOKAT_USE_BODYDIR (1<<1)

enum EJumpAnimType
{
	JUMP_ANIM_FLY,
	JUMP_ANIM_LAND
};


//===================================================================
// MovementUrgencyToIndex
//===================================================================
inline int MovementUrgencyToIndex(float urgency)
{
	int sign = urgency < 0.0f ? -1 : 1;
	if(urgency < 0) urgency = -urgency;

	if(urgency < (AISPEED_ZERO+AISPEED_SLOW)/2)
		return 0;
	else if(urgency < (AISPEED_SLOW+AISPEED_WALK)/2)
		return 1 * sign;	// slow
	else if(urgency < (AISPEED_WALK+AISPEED_RUN)/2)
		return 2 * sign;	// walk
	else if(urgency < (AISPEED_RUN+AISPEED_SPRINT)/2)
		return 3 * sign;	// run
	else
		return 4 * sign;	// sprint
}

//===================================================================
// IndexToMovementUrgency
//===================================================================
inline float IndexToMovementUrgency(int idx)
{
	float sign = idx < 0 ? -1.0f : 1.0f;
	if(idx < 0) idx = -idx;
	switch(idx)
	{
	case 0: return AISPEED_ZERO * sign;
	case 1: return AISPEED_SLOW * sign;
	case 2: return AISPEED_WALK * sign;
	case 3: return AISPEED_RUN * sign;
	default: return AISPEED_SPRINT * sign;
	}
}

/// The coverage half-angle of a hidespot anchor. The total coverage angle is twice this angle.
#define	HIDESPOT_COVERAGE_ANGLE_COS			0.70710678118654f		// cos(45 degrees)

//	path finder blockers types
enum ENavigationBlockers
{
	PFB_NONE,
	PFB_ATT_TARGET,
	PFB_REF_POINT,
	PFB_BEACON,
	PFB_DEAD_BODIES,
	PFB_EXPLOSIVES,
	PFB_PLAYER,
	PFB_BETWEEN_NAV_TARGET,
};

enum EOPWaitType
{
	WAIT_ALL = 0,		// wait for all group to be finished
	WAIT_ANY = 1,		// wait for any goal in the group to be finished
	WAIT_ANY_2 = 2,		// wait for any goal in the group to be finished
	WAIT_LAST,
};

enum EOPBranchType
{
	IF_ACTIVE_GOALS = 0,			// default as it was in CryEngine1 - jumps if there are active (not finished) goal operations
	IF_ACTIVE_GOALS_HIDE,		// default as it was in CryEngine1 - jumps if there are active goals or hide spot wasn't found
	IF_NO_PATH,				// jumps if there wasn't a path in the last "pathfind" goal operation
	IF_PATH_STILL_FINDING, // jumps if the current pathfind request hasn't been completed yet
	IF_IS_HIDDEN,			// jumps if the last "hide" goal operation has succeed and distance to hide point is small
	IF_CAN_HIDE,			// jumps if the last "hide" goal operation has succeed
	IF_CANNOT_HIDE,			// jumps if the last "hide" goal operation has failed
	IF_STANCE_IS,			// jumps if the stance of this PipeUser is equal to value specified as 5-th argument
	IF_FIRE_IS,				// jumps if the argument of the last "firecmd" on this PipeUser is equal to 5-th argument
	IF_HAS_FIRED,			// jumps if the PipeUser just fired - fire flag passed to actor
	IF_NO_LASTOP,			// jumps if m_pLastOpResult is NULL (for example if "locate" goalOp returned NULL)
	IF_SEES_LASTOP,			// jumps if m_pLastOpResult is visible from here
	IF_SEES_TARGET,			// jumps if attention target is visible from here
	IF_CAN_SHOOT_TARGET,	// jumps if nothing between weapon and attention target (can shoot)
	IF_CAN_MELEE,					// if curren weapon has melee fire mode
	IF_NO_ENEMY_TARGET,// jumps if it hasn't got an enemy attention target
	IF_PATH_LONGER,			// jumps if current path is longer than 5-th argument
	IF_PATH_SHORTER,			// jumps if current path is shoter than 5-th argument
	IF_PATH_LONGER_RELATIVE, // jumps if current path is longer than (5-th argument) times the distance to requested destination
	IF_NAV_WAYPOINT_HUMAN,		// jumps if the current navigation graph is waypoint (use for checking indoor)
	IF_NAV_TRIANGULAR,		// jumps if the current navigation graph is triangular (use for checking outdoor)
	IF_TARGET_DIST_LESS,		// jumps if the distance to target is less
	IF_TARGET_DIST_LESS_ALONG_PATH,		// jumps if the distance along path to target is less
	IF_TARGET_DIST_GREATER,		// jumps if the distance to target is more
	IF_TARGET_IN_RANGE,		// jumps if the distance to target is less than the attackRannge
	IF_TARGET_OUT_OF_RANGE,		// jumps if the distance to target is more than the attackRannge
	IF_TARGET_TO_REFPOINT_DIST_LESS,		// jumps if the distance between target and refpoint is less
	IF_TARGET_TO_REFPOINT_DIST_GREATER,		// jumps if the distance between target and refpoint is more
	IF_TARGET_LOST_TIME_MORE,		// jumps if target lost time is more
	IF_TARGET_LOST_TIME_LESS,		// jumps if target lost time is less
	IF_LASTOP_DIST_LESS,		// jumps if the distance to last op result is less
	IF_LASTOP_DIST_LESS_ALONG_PATH,		// jumps if the distance to last op result along path is less
	IF_TARGET_MOVED_SINCE_START,		// jumps if the distance between current targetPos and targetPos when pipe started more than threshold
	IF_TARGET_MOVED,		// jumps if the distance between current targetPos and targetPos when pipe started (or last time it was checked) more than threshold
	IF_EXPOSED_TO_TARGET,	// jumps if the upper torso of the agent is visible towards the target.
	IF_COVER_COMPROMISED,	// jumps if the current cover cannot be used for hiding or if the hide spots does not exists.
	IF_COVER_NOT_COMPROMISED,	// negated version of IF_COVER_COMPROMISED.
	IF_COVER_SOFT,							// if current cover is soft cover.
	IF_COVER_NOT_SOFT,					// if curren cover is not soft cover.
	IF_CAN_SHOOT_TARGET_CROUCHED,
	IF_COVER_FIRE_ENABLED,			// if cover fire is not enabled 
	IF_RANDOM,
	IF_LASTOP_FAILED,
	IF_LASTOP_SUCCEED,
	BRANCH_ALWAYS,		// unconditional jump
	NOT = 0x100,
};

enum EAnimationMode
{
	AIANIM_SIGNAL = 1,		// 
	AIANIM_ACTION,		
};

enum EStance
{
	STANCE_NULL = -1,
	STANCE_STAND = 0,
	STANCE_CROUCH = 1,
	STANCE_PRONE = 2,
	STANCE_RELAXED = 3,
	STANCE_STEALTH = 4,
	STANCE_SWIM = 5,
	STANCE_ZEROG = 6,
	// this value must be last
	STANCE_LAST
};

enum EFireMode
{
	FIREMODE_OFF = 0,					// Do not fire
	FIREMODE_BURST = 1,				// Fire in bursts - living targets only.
	FIREMODE_CONTINUOUS = 2,	// Fire continuously - living targets only.
	FIREMODE_FORCED = 3,			// Fire continuously - allow any target.
	FIREMODE_AIM = 4,					// Aim target only - allow any target.
	FIREMODE_SECONDARY = 5,		// Fire secondary weapon (grenades,....).
	FIREMODE_SECONDARY_SMOKE = 6,		// Fire smoke grenade.
	FIREMODE_MELEE = 7,				// Melee
	FIREMODE_KILL = 8,				// no missing, shoot directly at the target, no matter what aggression/attackRange/accuracy is
	FIREMODE_BURST_WHILE_MOVING = 9, // [mikko] to be renamed.
	FIREMODE_PANIC_SPREAD = 10,
	FIREMODE_BURST_DRAWFIRE = 11,
	FIREMODE_MELEE_FORCED = 12,	// Melee, without distance restrictions.
	FIREMODE_BURST_SNIPE = 13,
	FIREMODE_AIM_SWEEP = 14,
};

enum EAIGoalFlags
{ // to do: names are not coherent, but they can used together; rename all of them?
	AILASTOPRES_USE			= 0x01,
	AILASTOPRES_LOOKAT	= 0x02,
	AI_LOOK_FORWARD			= 0x04, 
	AI_DONT_STEER_AROUND_TARGET = 0x08, // used in stick goal (not using AI_MOVE_BACKWARD) 
	AI_MOVE_BACKWARD = 0x08, // default direction 
	AI_MOVE_RIGHT = 0x10,
	AI_MOVE_LEFT =0x20,
	AI_MOVE_FORWARD = 0x40,
	AI_MOVE_BACKRIGHT =0x80,
	AI_MOVE_BACKLEFT =0x100,
	AI_MOVE_TOWARDS_GROUP = 0x200,	// towards the other group members.
	AI_REQUEST_PARTIAL_PATH = 0x400,
	AI_BACKOFF_FROM_TARGET = 0x800,
	AI_BREAK_ON_LIVE_TARGET = 0x1000,
	AI_RANDOM_ORDER = 0x2000,
	AI_CONSTANT_SPEED = 0x2000, // used in stick goal (not using AI_RANDOM_ORDER) 
	AI_USE_TIME = 0x4000, // when param can be interpreted as time or distance, use time
	AI_STOP_ON_ANIMATION_START = 0x8000,
	AI_USE_TARGET_MOVEMENT = 0x10000, // when requesting a direction (AI_MOVE_LEFT/RIGHT etc), make it relative to target movement direction
	AI_ADJUST_SPEED = 0x20000, // used in stick goal forces to adjust the speed based on the target.
	AI_CHECK_SLOPE_DISTANCE = 0x40000, // used in backoff goal, checks the actual distance on slopes rather than horizontal
};

// Unit properties for group behavior (see CLeader/CLeaderAction)
// These bit flags can be combined based on the unit capabilities.
enum EUnitProperties
{
	UPR_COMBAT_GROUND = 1,		// The unit can take part in ground actions.
	UPR_COMBAT_FLIGHT	= 2,		// The unit can take part in flight actions.
	UPR_COMBAT_MARINE	= 4,		// The unit can take part in marine actions.
	UPR_COMBAT_RECON	= 8,		// The unit can take part in recon actions.
	UPR_ALL						= 0xffffffff,
};

enum EAIUseAction
{
	AIUSEOP_NONE,
	AIUSEOP_PLANTBOMB,
	AIUSEOP_VEHICLE,
	AIUSEOP_RPG
};

enum	ELeaderAction{
	LA_NONE,
	LA_HIDE,
	LA_HOLD,
	LA_ATTACK,
	LA_SEARCH,
	LA_FOLLOW,
	LA_USE,
	LA_USE_VEHICLE,
	LA_LAST,	// make sure this one is always the last!
};

enum	ELeaderActionSubType{
	LAS_DEFAULT=0,
	LAS_ATTACK_ROW,
	LAS_ATTACK_CIRCLE,
	LAS_ATTACK_FLANK,
	LAS_ATTACK_FLANK_HIDE,
	LAS_ATTACK_FOLLOW_LEADER,
	LAS_ATTACK_FUNNEL,
	LAS_ATTACK_LEAPFROG,
	LAS_ATTACK_HIDE_COVER,
	LAS_ATTACK_FRONT,
	LAS_ATTACK_CHAIN,
	LAS_ATTACK_COORDINATED_FIRE1,
	LAS_ATTACK_COORDINATED_FIRE2,
	LAS_ATTACK_USE_SPOTS,
	LAS_ATTACK_HIDE,
	LAS_ATTACK_SWITCH_POSITIONS,
	LAS_ATTACK_CHASE,
	LAS_SEARCH_DEFAULT,
	LAS_SEARCH_COVER,
	LAS_LAST	// make sure this one is always the last!
};

enum EAIAlertStatus
{
	AIALERTSTATUS_SAFE,
	AIALERTSTATUS_UNSAFE,
	AIALERTSTATUS_READY,
	AIALERTSTATUS_ACTION
};

enum EAITargetThreat
{
	AITHREAT_NONE,
	AITHREAT_INTERESTING,
	AITHREAT_THREATENING,
	AITHREAT_AGGRESSIVE,
	AITHREAT_LAST	// for serialization.
};

enum EAITargetType
{
	// Atomic types.
	AITARGET_NONE,				// No target
	AITARGET_SOUND,				// Primary sensory from sound event.
	AITARGET_MEMORY,			// Primary sensory from vis event, not visible.
	AITARGET_VISUAL,			// Primary sensory from vis event, visible.

	// Backwards compatibility for scriptbind
	AITARGET_ENEMY,
	AITARGET_FRIENDLY,
	AITARGET_BEACON,
	AITARGET_GRENADE,
	AITARGET_RPG,

	// for serialization.
	AITARGET_LAST	
};

enum EAIGoalType
{
	AIGOALTYPE_UNDEFINED,
	AIGOALTYPE_GOTO,
	AIGOALTYPE_PATH,
	AIGOALTYPE_ATTACK,
	AIGOALTYPE_TRANSPORT,
	AIGOALTYPE_REINFORCEMENT,
	AIGOALTYPE_FOLLOW
};

enum EAIWeaponAccessories
{
	AIWEPA_NONE = 0,
	AIWEPA_LASER = 0x0001,
	AIWEPA_COMBAT_LIGHT = 0x0002,
	AIWEPA_PATROL_LIGHT = 0x0004,
};

struct IAIObject;
struct IAIActor;
struct IAIGroup;

/// In practice, rather than modifying/setting all these values explicitly when
/// making a pathfind request, I suggest you set up bunch of constant sets
/// of properties, and use them

struct AgentPathfindingProperties
{
	AgentPathfindingProperties(
		IAISystem::tNavCapMask navCapMask, 
		float triangularResistanceFactor,
		float waypointResistanceFactor,
		float flightResistanceFactor,
		float volumeResistanceFactor,
		float roadResistanceFactor,
		float waterResistanceFactor,
		float maxWaterDepth, 
		float minWaterDepth,
		float exposureFactor,
		float dangerCost,
		float zScale) : 
	navCapMask(navCapMask), triangularResistanceFactor(triangularResistanceFactor), waypointResistanceFactor(waypointResistanceFactor),
		flightResistanceFactor(flightResistanceFactor), volumeResistanceFactor(volumeResistanceFactor), roadResistanceFactor(roadResistanceFactor),
		waterResistanceFactor(waterResistanceFactor), maxWaterDepth(maxWaterDepth), minWaterDepth(minWaterDepth),
		exposureFactor(exposureFactor), dangerCost(dangerCost),
		zScale(zScale) {if (maxWaterDepth < minWaterDepth) maxWaterDepth = minWaterDepth;}

	AgentPathfindingProperties() {SetToDefaults();}

	void SetToDefaults() {
		navCapMask = IAISystem::NAVMASK_ALL;
		triangularResistanceFactor = 0.0f;
		waypointResistanceFactor = 0.0f;
		flightResistanceFactor = 0.0f;
		volumeResistanceFactor = 0.0f;
		roadResistanceFactor = 0.0f;
		waterResistanceFactor = 0.0f;
		maxWaterDepth = 10000.0f;
		minWaterDepth = -10000.0f;
		exposureFactor = 0.0f;
		dangerCost = 0.0f; ///< Cost (in meters) per dead body in the destination node!
		zScale = 1.0f;
	}

	/// What kind of nodes can this agent traverse - expect this to normally be 
	/// set just once when the agent is created.
	IAISystem::tNavCapMask navCapMask;

	/// The basic extra costs associated with traversing each node type
	/// Everything below here could be reasonably set/modified each time
	/// a pathfind request is made
	float triangularResistanceFactor;
	float waypointResistanceFactor;
	float flightResistanceFactor;
	float volumeResistanceFactor;
	float roadResistanceFactor;
	/// additional cost modifiers
	float waterResistanceFactor;
	/// only travel if water depth is between min and max values
	float maxWaterDepth;
	float minWaterDepth;
	float exposureFactor;
	float dangerCost;
	// How much the z component should be scaled by when calculating distances
	float zScale;
};

struct AgentMovementSpeeds
{
	enum EAgentMovementUrgency {AMU_SLOW, AMU_WALK, AMU_RUN, AMU_SPRINT, AMU_NUM_VALUES};
	enum EAgentMovementStance {AMS_RELAXED, AMS_COMBAT, AMS_STEALTH, AMS_CROUCH, AMS_PRONE, AMS_SWIM, AMS_NUM_VALUES};

	struct SSpeedRange
	{
		float def, min, max;
	};

	AgentMovementSpeeds()
	{
		memset(this, 0, sizeof(*this));
	}

	void SetBasicSpeeds(float slow, float walk, float run, float sprint, float maneuver)
	{
		for (int s = 0 ; s < AMS_NUM_VALUES; ++s)
		{
			speedRanges[s][AMU_SLOW].def = slow;
			speedRanges[s][AMU_SLOW].min = maneuver;
			speedRanges[s][AMU_SLOW].max = slow;

			speedRanges[s][AMU_WALK].def = walk;
			speedRanges[s][AMU_WALK].min = maneuver;
			speedRanges[s][AMU_WALK].max = walk;

			speedRanges[s][AMU_RUN].def = run;
			speedRanges[s][AMU_RUN].min = maneuver;
			speedRanges[s][AMU_RUN].max = run;

			speedRanges[s][AMU_SPRINT].def = sprint;
			speedRanges[s][AMU_SPRINT].min = maneuver;
			speedRanges[s][AMU_SPRINT].max = sprint;
		}

	}

	void SetRanges(int stance, int urgency, float sdef, float smin, float smax)
	{
		assert(stance >= 0 && stance < AMS_NUM_VALUES);
		assert(urgency >= 0 && urgency < AMU_NUM_VALUES);
		speedRanges[stance][urgency].def = sdef;
		speedRanges[stance][urgency].min = smin;
		speedRanges[stance][urgency].max = smax;
	}

	void CopyRanges(int stance, int toUrgency, int fromUrgency)
	{
		assert(stance >= 0 && stance < AMS_NUM_VALUES);
		assert(toUrgency >= 0 && toUrgency < AMU_NUM_VALUES);
		assert(fromUrgency >= 0 && fromUrgency < AMU_NUM_VALUES);
		speedRanges[stance][toUrgency] = speedRanges[stance][fromUrgency];
	}

	const SSpeedRange& GetRange(int stance, int urgency) const
	{
		assert(stance >= 0 && stance < AMS_NUM_VALUES);
		assert(urgency >= 0 && urgency < AMU_NUM_VALUES);
		return speedRanges[stance][urgency];
	}

private:
	SSpeedRange speedRanges[AMS_NUM_VALUES][AMU_NUM_VALUES];
};

//
//this structure should contain all the relevant information describing the way agent moves (can strafe,speeds,inertia,...)
struct AgentMovementAbility
{
	bool b3DMove;
	bool bUsePathfinder;
	bool usePredictiveFollowing;
	bool allowEntityClampingByAnimation;
	float maxAccel;
	float maxDecel;
	float	minTurnRadius;
	float	maxTurnRadius;
	float avoidanceRadius;
	float pathLookAhead; // How far to look ahead when path following
	float pathRadius;    // How wide the path should be treated
	float pathSpeedLookAheadPerSpeed; // How far to look ahead to do speed control (mult this by the speed); -ve means use pathLookAhead
	float cornerSlowDown; // slow down at corners: 0 to 1
	float slopeSlowDown; // slow down on slopes uphill (0 to 1)
	float optimalFlightHeight;
	float minFlightHeight;
	float maxFlightHeight;
	float	maneuverTrh;		//cosine of forward^desired angle, after which to start maneuvering
	float	velDecay;		//how fast velocity decays with cos(fwdDir^pathDir) 
	float	pathFindPrediction;		// time in seconds how much the path finder will predict the start of the path
	/// enable/disable the algorithm to attempt to handle the agent getting stuck during tracing
	bool  resolveStickingInTrace;
	/// enable/disable the path getting regenerated during tracing (needed for crowd control and dynamic
	/// updates to graph) - regeneration interval (in seconds) - 0 disables
	float pathRegenIntervalDuringTrace;
	/// enable/disable teleporting when path following
	bool teleportEnabled;
	/// set to true, if the movement speed should be lowered in low light conditions.
	bool  lightAffectsSpeed;
	/// Adjust the movement speed based on the angel between body dir and move dir.
	float directionalScaleRefSpeedMin;
	float directionalScaleRefSpeedMax;

	AgentMovementSpeeds movementSpeeds;

	AgentPathfindingProperties pathfindingProperties;

	AgentMovementAbility():b3DMove(false),bUsePathfinder(true), usePredictiveFollowing(false), allowEntityClampingByAnimation(false),
		maxAccel(std::numeric_limits<float>::max()), maxDecel(std::numeric_limits<float>::max()),
		minTurnRadius(0), maxTurnRadius(0),
		avoidanceRadius(0), pathLookAhead(3.0f), pathRadius(1.0f), pathSpeedLookAheadPerSpeed(-1), cornerSlowDown(0.0f), slopeSlowDown(1.f),
		optimalFlightHeight(0), minFlightHeight(0), maxFlightHeight(0), maneuverTrh(0.f),
		velDecay(0.0f), pathFindPrediction(0), resolveStickingInTrace(true), pathRegenIntervalDuringTrace(0.0f),
		teleportEnabled(false), lightAffectsSpeed(false),
		directionalScaleRefSpeedMin(-1.0f), directionalScaleRefSpeedMax(-1.0f)

	{}
};


struct AIObjectParameters
{
	AgentParameters	 m_sParamStruct;
	AgentMovementAbility	m_moveAbility;
	IUnknownProxy *pProxy;
	AIObjectParameters ():
	pProxy(NULL)
	{
	}
};

struct AIWeaponDescriptor
{
	string	firecmdHandler;
	float	fSpeed;			// relevant for projectiles only
	float	fDamageRadius;	// explosion radius for rockets/grenades
	float	fChargeTime;
	float	fRangeMax;
	float	fRangeMin;
	bool	bSignalOnShoot;
	int burstBulletCountMin;			// The burst bullet count is scaled between min and max based on several factors (distance, visibility).
	int burstBulletCountMax;			// Each burst is followed by a pause.
	float burstPauseTimeMin;			// The burst pause is scale between min and max.
	float burstPauseTimeMax;
	float singleFireTriggerTime;	// Interval between single shot shots. Scale randomly between 70-125%. Set to -1 for continuous fire.
	float spreadRadius;						// Miss spread radius in meters.
	float coverFireTime;					// How long the shooting should continue after the target is not visible anymore.
	float drawTime;								// how long the AI will shoot around the target before aiming it precisely 
	float sweepWidth;							// how wide is the sweep left/right around the target direction during 
	float sweepFrequency;					// how fast the sweep left/right around the target direction during 
	string smartObjectClass;


	AIWeaponDescriptor(const string& fcHandler="instant", float chargeTime=-1.0f,float speed=-1.0f, float damageRadius=-1.0f):
	firecmdHandler(fcHandler),fSpeed(speed),fDamageRadius(damageRadius),fChargeTime(chargeTime),
		fRangeMax(1000.f),fRangeMin(1.f),bSignalOnShoot(false), burstBulletCountMin(1), burstBulletCountMax(10),
		burstPauseTimeMin(0.8f), burstPauseTimeMax(3.5f), singleFireTriggerTime(-1.0f), spreadRadius(1.0f), coverFireTime(2.0f),
		drawTime(3.0f), sweepWidth(2.0f), sweepFrequency(2.0f)
	{}
};

// Fire command handler interface.
struct IFireCommandHandler
{
	// Returns the name identifier of the handler.
	virtual const char*	GetName() = 0;
	// Reset is called each time
	virtual void	Reset() = 0;
	// Update is called on each AI update when the target is valid.
	virtual bool	Update(IAIObject* pTarget, bool canFire, EFireMode fireMode, const AIWeaponDescriptor& descriptor, Vec3& outShootTargetPos) = 0;
	// checks if it's ok to shoot with this weapon in fireDir - to see if no friends are hit, no ostacles nearby hit, etc
	virtual bool	ValidateFireDirection(const Vec3& fireVector, bool mustHit) = 0;
	// Deletes the handler.
	virtual void Release() = 0;
	// Draws debug information.
	virtual void DebugDraw(struct IRenderer *pRenderer) = 0;
	// Return true if default effect should be used for special firemode.
	virtual bool UseDefaultEffectFor(EFireMode fireMode) const = 0;
	// called whenever weapon is reloaded
	virtual void OnReload() = 0;
	// called whenever shot is done (if weapon descriptor defines EnableWeaponListener)
	virtual void OnShoot() = 0;
	// how many shots were done
	virtual int	 GetShotCount() const = 0;

};

// Fire command handler descriptor/factory.
struct IFireCommandDesc
{
	// Returns the name identifier of the handler.
	virtual const char*	GetName() = 0;
	// Creates new instance of a fire command handler.
	virtual IFireCommandHandler*	Create(IAIActor *pShooter) = 0;
	// Deletes the factory.
	virtual void Release() = 0;
};

// Helper class for declaring fire command descriptors.
template <class T>
class CFireCommandDescBase : public IFireCommandDesc
{
public:
	CFireCommandDescBase(const char* name) : m_name(name) {};
	virtual const char*	GetName() { return m_name.c_str(); }
	virtual IFireCommandHandler*	Create(IAIActor* pOwner) { return new T(pOwner); }
	virtual void Release() { delete this; }
protected:
	string	m_name;
};
#define CREATE_FIRECOMMAND_DESC(name, type) new CFireCommandDescBase<type>((name))


// memento used in CanTargetPointBeReached etc
class CTargetPointRequest
{
public:
	CTargetPointRequest() : result(eTS_false), pathID(-1) {}
	CTargetPointRequest(const Vec3 &targetPoint, bool continueMovingAtEnd = true) 
		: targetPoint(targetPoint), pathID(-1), continueMovingAtEnd(continueMovingAtEnd) {}
	ETriState GetResult() const {return result;}
	const Vec3& GetPosition() const {return targetPoint;}
	void SetResult(ETriState res) {result = res;}
private:
	// Data is internal to AI (CNavPath)!
	friend class CNavPath;
	Vec3 targetPoint;
	Vec3 splitPoint;
	int itIndex;
	int itBeforeIndex;
	int pathID; // used to identify the path this was valid for
	bool continueMovingAtEnd;
	ETriState result;
};

struct GoalParameters
{
	Vec3 m_vPosition;
	Vec3 m_vPositionAux;
	IAIObject *m_pTarget; 
	float fValue;
	float fValueAux;
	int nValue;
	int nValueAux;
	bool bValue;
	string szString;
	string szString2;
	//const char *szString;

	GoalParameters():
	m_pTarget(NULL),
		m_vPosition(ZERO),
		m_vPositionAux(ZERO),
		fValue(0),
		fValueAux(0),
		nValue(0),
		nValueAux(0),
		bValue(false)
	{
	}

#if defined(WIN64) && !defined(_DLL) 
	// FIX: refcounted STL with static libs (e.g. on AMD64 compiler) will crash without these
	// TODO: AMD64 port: make a robust solution
	inline GoalParameters (const GoalParameters& params)
	{
		m_vPosition = params.m_vPosition;
		m_vPositionAux = params.m_vPositionAux;
		m_pTarget   = params.m_pTarget; 
		fValue      = params.fValue;
		fValueAux   = params.fValueAux;
		nValue      = params.nValue;
		bValue      = params.bValue;
		szString    = params.szString.c_str();
		szString2   = params.szString2.c_str();
	}

	inline GoalParameters& operator = (const GoalParameters& params)
	{
		m_vPosition = params.m_vPosition;
		m_vPositionAux = params.m_vPositionAux;
		m_pTarget   = params.m_pTarget; 
		fValue      = params.fValue;
		fValueAux   = params.fValueAux;
		nValue      = params.nValue;
		bValue      = params.bValue;
		szString    = params.szString.c_str();
		szString2   = params.szString2.c_str();
		return *this;
	}
#endif
	void Serialize( TSerialize ser);
};

// ATOMIC AI OPERATIONS
enum EGoalOperations
{
	AIOP_ACQUIRETARGET,
	AIOP_LOOKAROUND,
	AIOP_APPROACH,
	AIOP_FOLLOW,
	AIOP_FOLLOWPATH,
	AIOP_BACKOFF,
	AIOP_FIRECMD,
	AIOP_STRAFE,
	AIOP_BODYPOS,
	AIOP_RUN,
	AIOP_TIMEOUT,
	AIOP_PATHFIND,
	AIOP_LOCATE,
	AIOP_TRACE,
	AIOP_SIGNAL,
	AIOP_IGNOREALL,
	AIOP_DEVALUE,
	AIOP_FORGET,
	AIOP_HIDE,
	AIOP_FORM,
	AIOP_STICK,
	AIOP_CLEAR,
	AIOP_BRANCH,
	AIOP_RANDOM,
	AIOP_LOOKAT,
	AIOP_CONTINUOUS,
	AIOP_MOVE,
	AIOP_CHARGE,
	AIOP_WAITSIGNAL,
	AIOP_ANIMATION,
	AIOP_ANIMTARGET,
	AIOP_USECOVER,
	AIOP_WAIT,
	AIOP_ADJUSTAIM,
	AIOP_SEEKCOVER,
	AIOP_PROXIMITY,
	AIOP_MOVETOWARDS,
	AIOP_DODGE,
	LAST_AIOP
};

struct IGoalPipe
{
	enum EGroupType
	{
		GT_NOGROUP = 0,
		GT_GROUPWITHPREV = 1,
		GT_GROUPED = 2,
		GT_LAST
	};

	virtual ~IGoalPipe() {}
//	virtual void PushGoal(const string &name, bool bBlocking, EGroupType eGrouping, GoalParameters &params) = 0;
	virtual void PushGoal(EGoalOperations name, bool bBlocking, EGroupType eGrouping, GoalParameters &params) = 0;
	virtual void PushPipe(const char* szName, bool bBlocking, EGroupType eGrouping, GoalParameters &params) = 0;
	virtual void PushLabel(const char* label) = 0;
	virtual void SetDebugName(const char* name) = 0;
	virtual void HighPriority() = 0;
	virtual const char* GetName() const = 0;
	virtual EGoalOperations GetGoalOpEnum(const char* szName) = 0;
	virtual const char* GetGoalOpName(EGoalOperations op) = 0;
};

struct SAIEVENT
{
	bool bFuzzySight;
	int nDeltaHealth;
	float fThreat;
	int nType;
	IAIObject *pSeen;
	bool bPathFound;
	Vec3 vPosition;
	Vec3 vForcedNavigation;
	EntityId targetId;

	SAIEVENT():bFuzzySight(false),nDeltaHealth(0),fThreat(0.f),nType(0),
		pSeen(NULL),bPathFound(false),vForcedNavigation(ZERO),targetId(0){}

};


struct SOBJECTSTATE;
struct AISIGNAL_EXTRA_DATA;


class CAIActor;
class CAIAttribute;
class CAIPlayer;
class CLeader;
struct IPipeUser;
class CPipeUser;
struct IPuppet;
class CPuppet;
struct IAIVehicle;
class CAIVehicle;

struct IAIObject:
	public IAIRecordable
{
	enum ESubTypes{
		STP_NONE,
		STP_FORMATION,
		STP_REFPOINT,
		STP_LOOKAT,
		STP_CAR,
		STP_BOAT,
		STP_HELI,
		STP_2D_FLY,
		STP_SOUND,
		STP_MEMORY,
		STP_BEACON,
		STP_TRACKDUMMY,
		STP_SPECIAL,
		STP_ANIM_TARGET,
		STP_MAXVALUE
	};	

protected:
	union
	{
		struct 
		{
			int _fastcast_CAIActor : 1;
			int _fastcast_CAIAttribute : 1;
			int _fastcast_CAIPlayer : 1;
			int _fastcast_CLeader : 1;
			int _fastcast_CPipeUser : 1;
			int _fastcast_CPuppet : 1;
			int _fastcast_CAIVehicle : 1;
		};
		int _fastcast_any;
	};
public:
	IAIObject() : _fastcast_any(0) {}
	virtual ~IAIObject() {}

	virtual const IAIActor* CastToIAIActor() const { return NULL; }
	virtual IAIActor* CastToIAIActor() { return NULL; }
	const CAIActor* CastToCAIActor() const { return _fastcast_CAIActor ? (const CAIActor*) this : NULL; }
	CAIActor* CastToCAIActor() { return _fastcast_CAIActor ? (CAIActor*) this : NULL; }

	const CAIAttribute* CastToCAIAttribute() const { return _fastcast_CAIAttribute ? (const CAIAttribute*) this : NULL; }
	CAIAttribute* CastToCAIAttribute() { return _fastcast_CAIAttribute ? (CAIAttribute*) this : NULL; }

	const CAIPlayer* CastToCAIPlayer() const { return _fastcast_CAIPlayer ? (const CAIPlayer*) this : NULL; }
	CAIPlayer* CastToCAIPlayer() { return _fastcast_CAIPlayer ? (CAIPlayer*) this : NULL; }

	const CLeader* CastToCLeader() const { return _fastcast_CLeader ? (const CLeader*) this : NULL; }
	CLeader* CastToCLeader() { return _fastcast_CLeader ? (CLeader*) this : NULL; }

	virtual const IPipeUser* CastToIPipeUser() const { return NULL; }
	virtual IPipeUser* CastToIPipeUser() { return NULL; }
	const CPipeUser* CastToCPipeUser() const { return _fastcast_CPipeUser ? (const CPipeUser*) this : NULL; }
	CPipeUser* CastToCPipeUser() { return _fastcast_CPipeUser ? (CPipeUser*) this : NULL; }

	virtual const IPuppet* CastToIPuppet() const { return NULL; }
	virtual IPuppet* CastToIPuppet() { return NULL; }
	const CPuppet* CastToCPuppet() const { return _fastcast_CPuppet ? (const CPuppet*) this : NULL; }
	CPuppet* CastToCPuppet() { return _fastcast_CPuppet ? (CPuppet*) this : NULL; }

	virtual const IAIVehicle* CastToIAIVehicle() const { return NULL; }
	virtual IAIVehicle* CastToIAIVehicle() { return NULL; }
	const CAIVehicle* CastToCAIVehicle() const { return _fastcast_CAIVehicle ? (const CAIVehicle*) this : NULL; }
	CAIVehicle* CastToCAIVehicle() { return _fastcast_CAIVehicle ? (CAIVehicle*) this : NULL; }

	virtual void SetRadius(float fRadius) = 0;
	virtual float GetRadius() const = 0;
	virtual void SetPos(const Vec3 &pos,const Vec3 &dirForw=Vec3(1,0,0)) = 0;
	virtual const Vec3 &GetPos() const = 0;
	virtual const Vec3 &GetBodyDir() const = 0;
	virtual void SetBodyDir(const Vec3 &dir) = 0;
	virtual const Vec3 &GetMoveDir() const = 0;
	virtual void SetMoveDir(const Vec3 &dir) = 0;
	virtual const Vec3 &GetViewDir() const = 0;
	virtual void SetViewDir(const Vec3 &dir) = 0;
	virtual const Vec3 &GetFirePos( ) const = 0; ///< Returns the position weapon position (from where it will fire)
	/// Returns the velocity reported by physics of the AI object. Returns zero len vector for static objects.
	virtual Vec3	GetVelocity() const = 0;

	virtual unsigned short GetAIType() const = 0;	
	virtual ESubTypes GetSubType() const = 0;
	virtual void SetType(unsigned short type) = 0;
	virtual void Release()=0;
	virtual void SetName(const char *pName)= 0;
	virtual const char *GetName() const= 0;
	virtual bool IsEnabled() const = 0;
	virtual void Event(unsigned short, SAIEVENT *pEvent) = 0;
	virtual void EDITOR_DrawRanges(bool bEnable = true) = 0;
	virtual IUnknownProxy* GetProxy() const = 0;
	virtual bool IsMoving() const = 0;
	virtual bool IsHostile(const IAIObject* pOther, bool bUsingAIIgnorePlayer=true) const = 0;
	virtual bool CreateFormation(const char *szName, Vec3 vTargetPos=ZERO)=0;
	virtual bool ReleaseFormation()=0;

	/// Sets params used when following a predefined path
	virtual void SetPathToFollow( const char *pathName ) = 0;
	virtual void SetPathAttributeToFollow( bool bSpline ) = 0;
	virtual void SetPointListToFollow( const std::list<Vec3>& pointList,IAISystem::ENavigationType navType,bool bSpline ) = 0;
	/// Returns true if the path can be modified to use request.targetPoint, and byproducts 

	/// of the test are cached in request.
	virtual ETriState CanTargetPointBeReached(CTargetPointRequest &request) = 0;
	/// Returns true if the request is still valid/can be used, false otherwise.
	virtual bool UseTargetPointRequest(const CTargetPointRequest &request) = 0;
	/// AI will take proposedPosition as a starting point. If it is possible for the AI entity to 
	/// adopt this location, or a position very close by, true will be returned, and adjustedPosition
	/// will be set.
	/// If no suitable position can be found nearby this function returns false.
	virtual bool GetValidPositionNearby(const Vec3 &proposedPosition, Vec3 &adjustedPosition) const = 0;
	/// AI will try to find a suitable location to teleport this AI entity to, and return true together with the location
	/// if it finds such a location.
	/// It will only return false if there is a temporary reason for failure (i.e. player is looking and we just need to wait).
	virtual bool GetTeleportPosition(Vec3 &teleportPos) const = 0;
	virtual void Following(IAIObject* pFollowed) =0;
	virtual IAIObject* GetFollowed() =0;
	virtual IAIObject* GetFollowedLeader() =0;

	virtual IAIObject* GetSpecialAIObject(const char* objName, float range = 0.0f) =0;

	// Returns trues if the AIObject is an agent (thing, acting).
	virtual bool	IsAgent() const { return false; }

	// Returns true if tho given point is in AIObject's (puppet, AIPlayer) field/range of view
	virtual bool IsPointInFOVCone(const Vec3 &pos, float distanceScale = 1.0f) = 0;

	// Description:
	//    Serialize proxy with a TSerialize
	// Arguments:
	//    ser - the object to serialize with
	virtual void Serialize( TSerialize ser, class CObjectTracker& objectTracker ) = 0;
	// Tell AI that this object what entity owns it so that AI can sort out
	// linking these objects during save/load
	virtual void SetEntityID(unsigned ID) = 0;
	virtual unsigned GetEntityID() const = 0;
	virtual IEntity* GetEntity() const = 0;
	// every AIObject can be a leader - player, puppet, etc
	//virtual IAIObject* MakeMeLeader() = 0;

	/// If non-zero then makes pathfinding with this agent avoid specified blocker by the
	/// the specified radius 
	virtual void SetPFBlockerRadius(int blockerType, float radius) = 0;

	virtual bool IsUpdatedOnce() const = 0;

	// last finished AI action sets uses this method to set its status (succeed or failed)
	virtual void SetLastActionStatus( bool bSucceed ) = 0;

	virtual void SetWeaponDescriptor(const AIWeaponDescriptor& descriptor) = 0;
	// sets a randomly rotating range for the AIObject's formation sight directions
	virtual void SetFormationUpdateSight(float range,float minTime,float maxTime) = 0;

	virtual bool HasFormation() = 0;

	virtual void SetGroupId(int id) = 0;	
	virtual int GetGroupId() const = 0;	

};

struct IAIActor
{
	virtual ~IAIActor() {}

	virtual SOBJECTSTATE * GetState() = 0;
	virtual const SOBJECTSTATE * GetState() const = 0;
	virtual IUnknownProxy* GetProxy() const = 0;
	virtual void SetSignal(int nSignalID, const char * szText, IEntity *pSender=0, IAISignalExtraData *pData=NULL, uint32 crcCode = 0) = 0;
	virtual void NotifySignalReceived(const char* szText, IAISignalExtraData* pData=NULL) = 0;
	virtual const AgentParameters& GetParameters() const = 0;
	virtual void SetParameters(AgentParameters &pParams)=0;
	virtual const AgentMovementAbility& GetMovementAbility() const = 0;
	virtual void SetMovementAbility(AgentMovementAbility &pParams)=0;
	virtual void ParseParameters(const AIObjectParameters &params)=0;
	virtual bool CanAcquireTarget(IAIObject* pOther) const = 0;
	virtual IPersonalInterestManager* GetInterestManager(void) = 0;
	virtual bool IsActive() const = 0;
};

struct IPipeUser;

enum EGoalPipeEvent
{
	ePN_OwnerRemoved,  // sent if pipeUser is removed
	ePN_Deselected,    // sent if replaced by selecting other pipe
	ePN_Finished,      // sent if reached end of pipe
	ePN_Suspended,     // sent if other pipe was inserted
	ePN_Resumed,       // sent if resumed after finishing inserted pipes
	ePN_Removed,       // sent if inserted pipe was removed with RemovePipe()

	ePN_AnimStarted,   // sent when exact positioning animation is started
	ePN_RefPointMoved, // sent to the last inserted goal pipe when the ref. point is moved
};

struct IGoalPipeListener;

// TODO: figure out better way to handle this, the structure is almost 1:1 to the SActorTargetParams.
typedef uint32 TAnimationGraphQueryID;
struct SAIActorTargetRequest
{
	SAIActorTargetRequest() :
		id(0),
		approachLocation(0,0,0),
		approachDirection(0,0,0),
		animLocation(0,0,0),
		animDirection(0,0,0),
		vehicleSeat(0),
		speed(0),
		directionRadius(0),
		locationRadius(0),
		startRadius(0),
		signalAnimation(true),
		projectEndPoint(true),
		lowerPrecision(false),
		stance(STANCE_NULL),
		pQueryStart(0),
		pQueryEnd(0)
	{
	}

	void Reset()
	{
		id = 0;
		approachLocation.Set(0,0,0);
		approachDirection.Set(0,0,0);
		animLocation.Set(0,0,0);
		animDirection.Set(0,0,0);
		vehicleSeat = 0;
		speed = 0;
		directionRadius = 0;
		locationRadius = 0;
		startRadius = 0;
		signalAnimation = true;
		projectEndPoint = true;
		lowerPrecision = false;
		stance = STANCE_NULL;
		pQueryStart = 0;
		pQueryEnd = 0;
		vehicleName = "";
		animation = "";
	}

	void Serialize(TSerialize ser, class CObjectTracker& objectTracker);

	int id;	// id=0 means invalid
	Vec3 approachLocation;
	Vec3 approachDirection;
	Vec3 animLocation;
	Vec3 animDirection;
	string vehicleName;
	int vehicleSeat;
	float speed;
	float directionRadius;
	float locationRadius;
	float startRadius;
	bool signalAnimation;
	bool projectEndPoint;
	bool lowerPrecision; // lower precision should be true when passing thru a navSO
	string animation;
	EStance stance;
	TAnimationGraphQueryID * pQueryStart;
	TAnimationGraphQueryID * pQueryEnd;
};

struct IPipeUser
{
	virtual ~IPipeUser() {}
	virtual void RegisterAttack(const char *name)=0;
	virtual void RegisterRetreat(const char *name)=0;
	virtual void RegisterWander(const char *name)=0;
	virtual void RegisterIdle(const char *name)=0;
	virtual bool SelectPipe(int id, const char *name, IAIObject *pArgument = 0, int goalPipeId = 0, bool resetAlways = false)=0;
	virtual IGoalPipe* InsertSubPipe(int id, const char *name, IAIObject *pArgument = 0, int goalPipeId = 0)=0;
	virtual bool CancelSubPipe(int goalPipeId)=0;
	virtual bool RemoveSubPipe(int goalPipeId, bool keepInserted = false)=0;
	virtual bool IsUsingPipe(const char *name)=0;
	virtual IAIObject *GetAttentionTarget() const =0;
	virtual EAITargetThreat GetAttentionTargetThreat() const = 0;
	virtual EAITargetType GetAttentionTargetType() const = 0;
	virtual IAIObject *GetLastOpResult()=0;
	virtual IAIObject* GetSpecialAIObject(const char* objName, float range = 0.0f) =0;
	virtual void MakeIgnorant(bool ignorant) = 0;
	virtual bool SetCharacter(const char *character, const char* behaviour = NULL)=0;

	//virtual void Devalue(IAIObject *pObject, bool bDevaluePuppets) = 0;
	virtual bool IsDevalued(IAIObject *pObject) = 0;
	virtual void ClearDevalued() = 0;

	virtual void RegisterGoalPipeListener( IGoalPipeListener* pListener, int goalPipeId, const char* debugClassName ) = 0;
	virtual void UnRegisterGoalPipeListener( IGoalPipeListener* pListener, int goalPipeId ) = 0;
	virtual int GetGoalPipeId() const = 0;

	virtual void ResetLookAt() = 0;
	virtual bool SetLookAtPoint( const Vec3& point, bool priority=false ) = 0;
	virtual bool SetLookAtDir( const Vec3& dir, bool priority=false ) = 0;

	virtual void SetExtraPriority( float priority ) = 0;
	virtual float GetExtraPriority(void) = 0;

	virtual IAIObject* GetRefPoint()=0;
	virtual void SetRefPointPos(const Vec3 &pos)=0; 
	virtual void SetRefPointPos(const Vec3 &pos, const Vec3 &dir)=0;
	virtual void SetRefShapeName(const char* shapeName) = 0;
	virtual const char* GetRefShapeName() const = 0;

	virtual void SetActorTargetRequest(const SAIActorTargetRequest& req) = 0;

	virtual EntityId GetLastUsedSmartObjectId() const=0;
	virtual void ClearPath( const char* dbgString )=0;

	virtual void SetFireMode(EFireMode mode) = 0;
	virtual EFireMode GetFireMode() const = 0;

	// Add the current hideobject position to unreachable list (will be avoided for some time).
	virtual void SetCurrentHideObjectUnreachable() = 0;

	// Returns most probable target position or the target if it is visible.
	virtual Vec3 GetProbableTargetPosition() = 0;

	virtual Vec3 GetLastSOExitHelperPos() = 0;
};


struct IGoalPipeListener
{
	virtual void OnGoalPipeEvent( IPipeUser* pPipeUser, EGoalPipeEvent event, int goalPipeId ) = 0;

private:
	friend class CPipeUser;

	typedef std::vector< std::pair< IPipeUser*, int > > VectorRegisteredPipes;
	VectorRegisteredPipes _vector_registered_pipes;

protected:
	virtual ~IGoalPipeListener()
	{
		assert( _vector_registered_pipes.empty() );
		while ( _vector_registered_pipes.empty() == false )
		{
			_vector_registered_pipes.back().first->UnRegisterGoalPipeListener( this, _vector_registered_pipes.back().second );
		}
	}
};


struct IPuppet 
{
	virtual ~IPuppet() {}
	virtual IAIObject* GetTargetTrackPoint()=0;
	virtual void UpTargetPriority(const IAIObject *pTarget, float fPriorityIncrement)=0;
	virtual void UpdateBeacon()=0;

	virtual IAIObject* MakeMeLeader()=0;
	// gets the owner of a dummy AI Object
	virtual IAIObject* GetEventOwner(IAIObject* pOwned) const = 0;
	// where to shoot if need to miss now (target is another AI) - select some point around target 
	virtual Vec3 ChooseMissPoint_Deprecated(const Vec3 &targetPos) const = 0;
	// Returns true and sends signal "OnFriendInWay" if there is a friendly agent in the line of fire.
	// If the cheapTest flag is set, only cone test is performed, otherwise raycasting.
	virtual bool CheckFriendsInLineOfFire(const Vec3& fireDirection, bool cheapTest) = 0;
	// Returns a specified point projected on floor/ground.
	virtual Vec3 GetFloorPosition(const Vec3& pos) = 0;
	// get the distance of an AI object to this, along this' path; must be called with bInit=true first time
	// and then false other time to avoid considering path regeneration after
	virtual float	GetDistanceAlongPath(const Vec3& pos, bool bInit) = 0;
	// Sets the shape that defines the Puppet territory.
	virtual void SetTerritoryShapeName(const char* shapeName) = 0;
	virtual const char* GetTerritoryShapeName() = 0;
	virtual void EnableCoverFire(bool enable) =0;
	virtual bool IsCoverFireEnabled() const =0;
	virtual bool GetPosAlongPath(float dist, bool extrapolateBeyond, Vec3& retPos) const = 0;
	virtual IFireCommandHandler* GetFirecommandHandler() const = 0;
};


struct IAIVehicle 
{
	virtual ~IAIVehicle() {}
};

struct ILeader
{
	virtual void				RequestAttack(uint32 unitProp,int type, float fDuration, const Vec3& defensePoint) = 0;
};

struct IAISignalExtraData
{
	virtual void Serialize( TSerialize ser ) = 0;
	virtual const char* GetObjectName() const = 0;
	virtual void SetObjectName( const char* objectName ) = 0;

	Vec3 point;
	Vec3 point2;
	ScriptHandle nID;
	float fValue;
	int iValue;
	int iValue2;
};


// Put new signal here!


struct AISIGNAL
{
	//AISIGNAL():point(0,0,0){}
	int						nSignal;
	//private:
	uint32				m_nCrcText;

	//CryString				strText;
	IEntity *					pSender;
	IAISignalExtraData*		pEData;

	// for 64b alignment
	static const int SIGNAL_STRING_LENGTH = 50;
	char					strText[SIGNAL_STRING_LENGTH];
	// for 512b alignment
	//	char					strText[498];

	inline bool Compare(uint32 crc) const 
	{
		return m_nCrcText == crc;
	}

	AISIGNAL()
	: nSignal(0)
	, pSender(NULL)
	, pEData(NULL)
	, m_nCrcText(0)
	{
		strText[0]=0;
	}
	void Serialize( TSerialize ser, class CObjectTracker& objectTracker );

};

struct PATHPOINT
{
	Vec3 vPos;
	Vec3 vDir;
	PATHPOINT(const Vec3& pos=ZERO,const Vec3& dir=ZERO):vPos(pos),vDir(dir){}
};
typedef DynArray<PATHPOINT>	PATHPOINTVECTOR;

// current phase of actor target animation
enum EActorTargetPhase
{
	eATP_None,
	eATP_Waiting,
	eATP_Starting,
	eATP_Started,
	eATP_Playing,
	eATP_StartedAndFinished,
	eATP_Finished,
	eATP_Error,
};

enum EAITargetStuntReaction
{
	AITSR_NONE,
	AITSR_SEE_STUNT_ACTION,
	AITSR_SEE_CLOAKED,
	AITSR_LAST
};

// current phase of actor target animation
enum ERequestedSecondaryType
{
	RST_ANY,
	RST_SMOKE_GRENADE,
	RST_FLASHBANG_GRENADE,
	RST_FRAG_GRENADE,
};

struct SAIPredictedCharacterState
{
	SAIPredictedCharacterState():predictionTime(0.f),position(0,0,0),velocity(0,0,0){}
	void Set(const Vec3 &pos, const Vec3 vel, float predT) {
		position = pos; velocity = vel; predictionTime = predT;}
	void Serialize( TSerialize ser );

	Vec3 position; 
	Vec3 velocity; 
	float predictionTime; // time of prediction relative to when it was made
};

struct SAIPredictedCharacterStates
{
	SAIPredictedCharacterStates():nStates(0){}
	void Serialize( TSerialize ser );

	static const int maxStates = 32;
	SAIPredictedCharacterState states[maxStates];
	int nStates;
};

struct SOBJECTSTATE
{

	//fixme	- remove it when done with tweaking of vehicles movement
	char	DEBUG_controlDirection;
	float	m_fDEBUGDirection;
	float	m_fDEBUGSteering;
	bool	m_bDEBUGBraking;

	// actor's movement\looking\firing related
	bool	jump;
	bool	fire;
	bool	fireSecondary;
	bool	fireMelee;
	bool	aimObstructed;
	bool	aimTargetIsValid;	// 
	ERequestedSecondaryType	secondaryType;
	int		weaponAccessories;	// weapon accessories to enable.
	bool	forceWeaponAlertness;
	int		bodystate;
	float	lean;
	float	fHitProbability;
	float	fProjectileSpeedScale;
	Vec3	vShootTargetPos;	// The requested position to shoot at. This value must be passed directly to weapon system,
	// the AI system has decided already if the shot should miss or hit. Note: can be (0,0,0) if not begin requested!
	Vec3	vAimTargetPos;		// The requested position to aim at. This value is slightly different from the vShootTarget, and is
	// used for visual representation to where to shoot at. Note: can be (0,0,0) if not begin requested!
	Vec3	vLookTargetPos;		// The requested position to look at. Note: can be (0,0,0) if not begin requested!
	Vec3	vMoveDir;
	Vec3	vForcedNavigation;
	// hint to animation system what will happen in the future
	SAIPredictedCharacterStates predictedCharacterStates;

	/// AISPEED_ZERO, AISPEED_SLOW, AISPEED_WALK, AISPEED_RUN, AISPEED_SPRINT. This affects animation and thus speed only indirectly,
	/// due to it affecting the available max/min speeds
	float fMovementUrgency; 
	float fDesiredSpeed; //< in m/s
	bool allowStrafing;	// Whether strafing is allowed or not.
	bool allowEntityClampingByAnimation;

	PATHPOINTVECTOR	remainingPath;
	float	fDistanceToPathEnd;

	EActorTargetPhase			curActorTargetPhase;						// return value
	Vec3									curActorTargetFinishPos;				// return value
	SAIActorTargetRequest	actorTargetReq;

	bool	bCloseContact;

	bool	bHurryNow;	// needs to skip animation transitions - going from idle to combat
	bool bReevaluate;
	bool bTakingDamage;
	bool bTargetEnabled;
	EAITargetType eTargetType;
	EAITargetThreat eTargetThreat;
	float fDistanceFromTarget;
	EAITargetStuntReaction eTargetStuntReaction;
	int nTargetType;
	DynArray<AISIGNAL> vSignals;
	int nAuxSignal;					// aux signal id
	string szAuxSignalText;	// aux signal name
	float fAuxDelay;				// aux signal delay (delay before the signal is being processed)
	int nAuxPriority;				// aux signal priority (only higher priority will interupt the current one)

	SOBJECTSTATE():
	vMoveDir(0.0f,0.0f,0.0f),
		vLookTargetPos(0.0f,0.0f,0.0f),
		fMovementUrgency(AISPEED_WALK),
		fDesiredSpeed(1.0f),
		//		bExactPositioning(false),
		fDistanceToPathEnd(-1.0f),
		allowStrafing(false),
		forceWeaponAlertness(false)
	{
		FullReset();
	}

	void Reset()
	{
		DEBUG_controlDirection = 0;
		bCloseContact = false;
		eTargetThreat = AITHREAT_NONE;
		eTargetType = AITARGET_NONE;
		eTargetStuntReaction = AITSR_NONE;
		vMoveDir(0,0,0);
		vForcedNavigation(0.0f, 0.0f, 0.0f);

		predictedCharacterStates.nStates = 0;
		fProjectileSpeedScale=1.f;
		bTargetEnabled = false;
		bHurryNow = false;
	}

	void FullReset()
	{
		Reset();

		vLookTargetPos.Set(0.0f,0.0f,0.0f);
		vShootTargetPos.Set(0.0f, 0.0f, 0.0f);
		vAimTargetPos.Set(0.0f, 0.0f, 0.0f);
		fDistanceToPathEnd = -1.0f;

		bReevaluate = false;
		bTakingDamage = false;
		jump = false;
		fire = false;
		fireSecondary = false;
		fireMelee = false;
		secondaryType = RST_ANY;
		aimObstructed = false;
		aimTargetIsValid = false;
		weaponAccessories = AIWEPA_NONE;
		allowStrafing = false;
		bodystate = 0;
		bTakingDamage = false;
		bCloseContact = false;
		fDistanceFromTarget = 0;
		actorTargetReq.Reset();
		curActorTargetPhase = eATP_None;
		curActorTargetFinishPos.Set(0,0,0);
		allowEntityClampingByAnimation = false;

		nTargetType = 0;
		fAuxDelay = 0.0f;
		nAuxPriority = 0;
		fMovementUrgency = AISPEED_WALK;
		fDesiredSpeed = 1.0f;
		fHitProbability = 1.0f;

		m_fDEBUGDirection = 0;
		m_fDEBUGSteering = 0;
		m_bDEBUGBraking = false;

		lean = 0.0f;

		vSignals.clear();
		remainingPath.clear();
		szAuxSignalText.clear();
	}

	bool operator==(SOBJECTSTATE &other)
	{
		if (fire == other.fire)
			if (bodystate == other.bodystate)
				return true;
		return false;
	}

	void Serialize( TSerialize ser, class CObjectTracker& objectTracker );
};

// PROXY TYPES
#define	AIPROXY_PUPPET	1
#define	AIPROXY_OBJECT	2
#define	AIPROXY_VEHICLE 3

// PROXY STATUS TYPES to check
#define	AIPROXYSTATUS_INVEHICLE	1

struct SAIBodyInfo
{
	Vec3		vEyePos;		
	Vec3		vEyeDir;		//direction of head	(where I'm looking at)
	Vec3		vEyeDirAnim;	//direction of head	from animation 
	Vec3		vBodyDir;		//direction of entity "body" (my forward)
	Vec3		vMoveDir;		//direction of entity movement
	Vec3		vUpDir;			//direction of entity (my up)
	Vec3		vFireDir;		//firing direction
	Vec3		vFirePos;		//firing position
	float		maxSpeed;   // maximum speed i can move at
	float		normalSpeed;// the "normal" (walking) speed
	float		minSpeed;   // minimum speed i need to move anywhere (otherwise I'll consider myself stopped)
	EStance	stance;
	AABB		stanceSize;		// approximate local bounds of the stance, relative to the entity position.
	AABB		colliderSize;	// approximate local bounds of the stance, relative to the entity position.
	bool		isAiming;
	bool		isFiring;
	float		lean;	// The amount the character is leaning.
	float		slopeAngle;

	IEntity *linkedVehicleEntity;
	IEntity *linkedDriverEntity;

	SAIBodyInfo() : vEyePos(ZERO), vEyeDir(ZERO), vEyeDirAnim(ZERO), vBodyDir(ZERO), vMoveDir(ZERO), vUpDir(ZERO), vFireDir(ZERO),
		maxSpeed(0), normalSpeed(0), minSpeed(0), stance(STANCE_NULL),
		isAiming(false), isFiring(false), linkedVehicleEntity(0), linkedDriverEntity(0), lean(0.0f), slopeAngle(0.0f)
	{
		stanceSize.min=Vec3(ZERO);
		stanceSize.max=Vec3(ZERO);
		colliderSize.min=Vec3(ZERO);
		colliderSize.max=Vec3(ZERO);
	}
};

struct IWeapon;

struct SAIWeaponInfo
{
	bool	canMelee;
	bool	outOfAmmo;
	bool	shotIsDone;
	bool	hasLightAccessory;
	bool	hasLaserAccessory;
	bool	isReloading;
	bool	isFiring;
	CTimeValue lastShotTime;

	SAIWeaponInfo() : outOfAmmo(false), shotIsDone(false), canMelee(false),
		hasLightAccessory(false), hasLaserAccessory(false), isReloading(false), isFiring(false), lastShotTime(0.0f) {}
};


struct IPhysicalEntity;

enum EAIAGInput
{
	AIAG_SIGNAL,
	AIAG_ACTION,
};

enum EObjectResetType
{
	AIOBJRESET_INIT,
	AIOBJRESET_SHUTDOWN,
};

struct IUnknownProxy
{
	virtual ~IUnknownProxy() {}
	virtual bool QueryProxy(unsigned char type, void **pProxy) = 0;
	virtual int Update(SOBJECTSTATE *) = 0;
	virtual bool CheckUpdateStatus() = 0;
	virtual void EnableUpdate(bool enable) = 0;
	virtual bool IsEnabled() const = 0;
	virtual int GetAlertnessState() const = 0;
	virtual bool IsCurrentBehaviorExclusive() const = 0;
	virtual bool SetCharacter( const char* character, const char* behaviour=NULL ) = 0;
	virtual const char* GetCharacter()  =0;
	virtual void QueryBodyInfo( SAIBodyInfo& bodyInfo ) = 0;
	virtual bool QueryBodyInfo( EStance stance, float lean, bool defaultPose, SAIBodyInfo& bodyInfo ) = 0;
	virtual	void QueryWeaponInfo( SAIWeaponInfo& weaponInfo ) = 0;
	virtual	EntityId GetLinkedDriverEntityId() = 0;
	virtual	bool IsDriver() = 0;
	virtual	EntityId GetLinkedVehicleEntityId() = 0;
	virtual void Release()=0;
	virtual void Reset(EObjectResetType type) = 0;
	virtual void Serialize( TSerialize ser ) = 0;

	virtual IPhysicalEntity* GetPhysics(bool wantCharacterPhysics=false) = 0;
	virtual void DebugDraw(struct IRenderer *pRenderer, int iParam = 0) = 0;
	virtual int& DebugValue(int idx=0) = 0;
	virtual const char* GetCurrentReadibilityName() const = 0;

	// gets the corners of the tightest projected bounding rectangle in 2D world space coordinates
	virtual void GetWorldBoundingRect(Vec3& FL, Vec3& FR, Vec3& BL, Vec3& BR, float extra=0) const = 0;
	virtual bool SetAGInput(EAIAGInput input, const char* value) = 0;
	virtual bool ResetAGInput(EAIAGInput input) = 0;
	virtual bool IsSignalAnimationPlayed( const char* value ) = 0;
	virtual bool IsActionAnimationStarted( const char* value ) = 0;
	virtual bool IsAnimationBlockingMovement() = 0;

	/// Returns the number of bullets shot since the last call to the method.
	virtual int GetAndResetShotBulletCount() = 0;

	virtual void	EnableWeaponListener(bool needsSignal) = 0;
	virtual void ClearFire() {};
	virtual void UpdateMind(SOBJECTSTATE *state) = 0;

//	virtual bool IsDead() = 0;
	virtual int GetActorHealth() = 0;
	virtual int GetActorMaxHealth() = 0;
	virtual int GetActorArmor() = 0;
	virtual int GetActorMaxArmor() = 0;
	virtual bool GetActorIsFallen() const = 0;

	virtual IWeapon *GetCurrentWeapon() const = 0;
	virtual IWeapon *GetSecWeapon( ERequestedSecondaryType prefType=RST_ANY ) const = 0;
	virtual IEntity* GetGrabbedEntity() const = 0;

	// needed for debug drawing
	virtual bool IsUpdateAlways() const = 0;
	virtual bool IfShouldUpdate() = 0;
};


struct IPuppetProxy : public IUnknownProxy
{
	virtual bool PredictProjectileHit(const Vec3& throwDir, float vel, Vec3& posOut, float& speedOut,
		Vec3* pTrajectory = 0, unsigned int* trajectorySizeInOut = 0) = 0;

	/// Plays readability sound.
	virtual int PlayReadabilitySound(const char* szReadability, bool stopPreviousSound) = 0;
	/// Toggles readability testing on/off (called from a console command).
	virtual void TestReadabilityPack(bool start, const char* szReadability) = 0;
	/// Returns the amount of time to block the readabilities of same name in a group.
	virtual void GetReadabilityBlockingParams(const char* text, float& time, int& id) = 0;

	virtual bool QueryCurrentAnimationSpeedRange(float& smin, float& smax) = 0;

	virtual bool CanJumpToPoint(Vec3& dest, float theta, float maxheight, int flags, Vec3& retVelocity, float& tDest, IPhysicalEntity* pSkipEnt, Vec3* pStartPos) = 0;

};

struct IAIVehicleProxy : public IUnknownProxy
{
	virtual Vec3	UpdateThreat( void* threat ) = 0;
	virtual Vec3	HeliAttackAdvance( SOBJECTSTATE &state ) = 0;
	virtual void SetSpeeds(float fwd, float bkw=-1) = 0;
	virtual float GetCurrentSpeed() = 0;
	virtual IAIObject* GetDriverAI()= 0;
	virtual void RequestSpeedVariation(float fVariation, float fTime) = 0;
};

enum EWaypointNodeType {WNT_UNSET, WNT_WAYPOINT, WNT_HIDE,  WNT_ENTRYEXIT, WNT_EXITONLY, WNT_HIDESECONDARY};
enum EWaypointLinkType {WLT_AUTO_PASS = 320, WLT_AUTO_IMPASS = -320, WLT_EDITOR_PASS = 321, WLT_EDITOR_IMPASS = -321, WLT_UNKNOWN_TYPE = 0};

struct IGraph {
	/// ID = 0 means assign a unique ID automatically
  virtual unsigned CreateNewNode(IAISystem::ENavigationType type, const Vec3 &pos, unsigned ID = 0)=0;
	virtual GraphNode* GetNode(unsigned nodeIndex) = 0;
  virtual void MoveNode(unsigned nodeIndex, const Vec3 &newPos) = 0;
	virtual bool RemoveEntrance(int nBuildingID, unsigned nodeIndex)=0;
	virtual void AddIndoorEntrance(int nBuildingID, unsigned nodeIndex, bool bExitOnly = false) = 0;
	/// Connects two nodes using the pass radii. Note that if the two nodes are already connected then for each radius
	/// that != 0 the connection radius is just modified. The two new (or already existing) links get optionally
	/// returned. 
	virtual void Connect(unsigned one, unsigned two, 
											 float radiusOneToTwo = 100.0f, float radiusTwoToOne = 100.0f,
											 unsigned* pLinkOneTwo = 0, unsigned* pLinkTwoOne = 0) = 0;
	virtual void Disconnect(unsigned nodeIndex,bool bDelete = true) =0;
	/// Removes an individual link from a node (and removes the reciprocal link) - 
	/// doesn't delete it.
	virtual void Disconnect(unsigned nodeIndex, unsigned iLink) = 0;
	/// Get the graph node that contains pos. if range > 0.0f then if no node is found it looks to
	/// see if pos is within range of a valid node - if it is it returns that node and sets closestValid
	/// to be the closest point in the node to pos (within range of it). [use of range is only 
	/// implemented in 3D navigation graph]
	/// If there are a number of nodes that could be returned, only nodes that have at least one 
	/// link with radius > passRadius will be returned.
	/// If returnSuspect then a node may be returned even if it may not be reachable
	virtual unsigned GetEnclosing(const Vec3 &pos, IAISystem::tNavCapMask navCapMask, float passRadius = 0.0f, unsigned startIndex = 0,
		float range = 0.0f, Vec3 * closestValid = 0, bool returnSuspect = false, const char *requesterName = "") = 0;

	/// Restores all node/links
	virtual void RestoreAllNavigation() = 0;

	virtual void MakeNodeRemovable(int nBuildingID, unsigned nodeIndex, bool bRemovable = true) = 0;
	virtual bool Validate(const char * msg, bool checkPassable) const = 0;
	/// Checks that a node exists (should be quick). If fullCheck is true it will do some further 
	/// checks which will be slower
	virtual bool ValidateNode(unsigned nodeIndex, bool fullCheck = false) const = 0;

  // fns used by the editor to get info about nodes
  virtual int GetBuildingIDFromWaypointNode(const GraphNode *pNode) = 0;
  virtual void SetBuildingIDInWaypointNode(GraphNode *pNode, unsigned nBuildingID) = 0;
  virtual void SetVisAreaInWaypointNode(GraphNode *pNode, IVisArea *pArea) = 0;
  virtual EWaypointNodeType GetWaypointNodeTypeFromWaypointNode(const GraphNode *pNode) = 0;
  virtual void SetWaypointNodeTypeInWaypointNode(GraphNode *pNode, EWaypointNodeType type) = 0;
  virtual EWaypointLinkType GetOrigWaypointLinkTypeFromLink(unsigned link) = 0;
  virtual float GetRadiusFromLink(unsigned link) = 0;
  virtual float GetOrigRadiusFromLink(unsigned link) = 0;
  virtual IAISystem::ENavigationType GetNavType(const GraphNode *pNode) = 0;
  virtual unsigned GetNumNodeLinks(const GraphNode *pNode) = 0;
  virtual unsigned GetGraphLink(const GraphNode *pNode, unsigned iLink) = 0;
  virtual Vec3 GetWaypointNodeUpDir(const GraphNode *pNode) = 0;
  virtual void SetWaypointNodeUpDir(GraphNode *pNode, const Vec3 &up) = 0;
  virtual Vec3 GetWaypointNodeDir(const GraphNode *pNode) = 0;
  virtual void SetWaypointNodeDir(GraphNode *pNode, const Vec3 &dir) = 0;
  virtual Vec3 GetNodePos(const GraphNode *pNode) = 0;
  virtual const GraphNode *GetNextNode(unsigned link) const = 0;
  virtual GraphNode *GetNextNode(unsigned link) = 0;

protected:
	virtual ~IGraph() {}
};


//	orders
#define AIORD_ATTACK 			"ORD_ATTACK"			// 
#define AIORD_SEARCH 			"ORD_SEARCH"			// 
#define AIORD_HIDE 				"ORD_HIDE"				// 
#define AIORD_HOLD 				"ORD_HOLD"				// 
#define AIORD_GOTO 				"ORD_GOTO"				// 
#define AIORD_USE 				"ORD_USE"				// 
#define AIORD_FOLLOW 			"ORD_FOLLOW"			// 
#define AIORD_IDLE	 			"ORD_IDLE"				// 
#define AIORD_REPORTDONE		"ORD_DONE"				// 
#define AIORD_REPORTFAIL		"ORD_FAIL"				// 
#define AIORD_LEAVE_VEHICLE		"ORD_LEAVE_VEHICLE"
#define USE_ORDER_ENABLED		"USE_ORDER_ENABLED"
#define USE_ORDER_DISABLED		"USE_ORDER_DISABLED"

// Helper functions to cast to certain AI object interfaces.
inline const IAIActor* CastToIAIActorSafe(const IAIObject* pAI) { return pAI ? pAI->CastToIAIActor() : 0; }
inline IAIActor* CastToIAIActorSafe(IAIObject* pAI) { return pAI ? pAI->CastToIAIActor() : 0; }

inline const IPuppet* CastToIPuppetSafe(const IAIObject* pAI) { return pAI ? pAI->CastToIPuppet() : 0; }
inline IPuppet* CastToIPuppetSafe(IAIObject* pAI) { return pAI ? pAI->CastToIPuppet() : 0; }

inline const IPipeUser* CastToIPipeUserSafe(const IAIObject* pAI) { return pAI ? pAI->CastToIPipeUser() : 0; }
inline IPipeUser* CastToIPipeUserSafe(IAIObject* pAI) { return pAI ? pAI->CastToIPipeUser() : 0; }


#endif //_IAGENT_H_
