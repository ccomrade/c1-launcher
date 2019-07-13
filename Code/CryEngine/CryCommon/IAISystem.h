// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _IAISYSTEM_H_
#define _IAISYSTEM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerializeFwd.h"
#include "Cry_Math.h"
#include "TimeValue.h"
#include "AIFormationDescriptor.h"
#include "AITrackPatternDescriptor.h"
#include <list>
#include <map>
#include "IAIRecorder.h"

struct IPhysicalEntity;
struct IPhysicalWorld;
class	ICrySizer;
struct IRenderer;
struct ISystem;
struct IGame;
struct IEntity;
struct IClient;
struct IAgentProxy;
struct IAgent;
struct IAIObject;
struct IAIActor;
struct ILeader;
struct IAIGroup;
struct IGoalPipe;
struct IGraph;
struct IVisArea;
struct ObstacleData;
struct IAISignalExtraData;
struct IFireCommandDesc;
struct IFireCommandHandler;

struct IEntityClass;
struct SmartObjectCondition;
class CSmartObjectClass;
struct IAIAction;
struct IAIRecorder;
class ICentralInterestManager;

struct IStatObj;

typedef std::list<IAIObject*> TAIObjectList;
typedef std::vector<int> TSubActionList;

enum EnumAreaType
{
	AREATYPE_PATH,
	AREATYPE_FORBIDDEN,
	AREATYPE_FORBIDDENBOUNDARY,
	AREATYPE_NAVIGATIONMODIFIER,
	AREATYPE_OCCLUSION_PLANE,
	AREATYPE_EXTRALINKCOST,
	AREATYPE_GENERIC,
};

/// The first word refers to how the nodes are initially connected
/// The second word refers to how the node connections are subsequently modified - partial means that links only get disabled.
enum EWaypointConnections 
{
  WPCON_DESIGNER_NONE, 
  WPCON_DESIGNER_PARTIAL, 
  WPCON_AUTO_NONE, 
  WPCON_AUTO_PARTIAL, 
  WPCON_MAXVALUE = WPCON_AUTO_PARTIAL
};

enum ENavModifierType
{
	NMT_INVALID = -1,
	NMT_WAYPOINTHUMAN = 0,
	NMT_VOLUME,
	NMT_FLIGHT,
	NMT_WATER,
	NMT_WAYPOINT_3DSURFACE,
	NMT_EXTRA_NAV_COST,
	NMT_DYNAMIC_TRIANGULATION,
	NMT_FREE_2D,
  NMT_TRIANGULATION
};

enum EAILightLevel
{
	AILL_NONE,		// No affect.
	AILL_LIGHT,		// Light
	AILL_MEDIUM,	// Medium	
	AILL_DARK,		// Dark
	AILL_LAST,		// this always has to be the last one
};

struct SNavigationShapeParams
{
  SNavigationShapeParams(
    const char *szPathName = 0,
    EnumAreaType areaType = AREATYPE_PATH,
    bool pathIsRoad = true,
		bool closed = false,
    const Vec3* points = 0,
    unsigned nPoints = 0,
    float fHeight = 0,
		int nNavType = 0,
		int nAuxType = 0,
		EAILightLevel lightLevel = AILL_NONE,
    float fNodeAutoConnectDistance = 0,
    EWaypointConnections waypointConnections = WPCON_DESIGNER_NONE,
    bool bVehiclesInHumanNav =false,
    bool bCalculate3DNav = true,
    float f3DNavVolumeRadius = 10.0f,
    float extraLinkCostFactor = 0.0f,
    float triangulationSize = 1.0f) 
    : szPathName(szPathName), areaType(areaType), pathIsRoad(pathIsRoad), closed(closed), points(points), nPoints(nPoints), fHeight(fHeight),
    nNavType(nNavType), nAuxType(nAuxType), fNodeAutoConnectDistance(fNodeAutoConnectDistance),
    waypointConnections(waypointConnections), bVehiclesInHumanNav(bVehiclesInHumanNav), bCalculate3DNav(bCalculate3DNav), f3DNavVolumeRadius(f3DNavVolumeRadius), 
    extraLinkCostFactor(extraLinkCostFactor), triangulationSize(triangulationSize), lightLevel(lightLevel) {}

  const char *szPathName;
  EnumAreaType areaType;
  bool pathIsRoad;
	bool closed;
  const Vec3* points;
  unsigned nPoints;
  float fHeight;
	int	nNavType;
	int nAuxType;
  float fNodeAutoConnectDistance;
  EWaypointConnections waypointConnections;
  bool bVehiclesInHumanNav;
  bool bCalculate3DNav;
	EAILightLevel lightLevel;
  float f3DNavVolumeRadius;
  /// Cost of links going through this shape gets multiplied by (1 + extraCostScale) - should be >= 0
  /// for A* heuristic to be valid.
  float extraLinkCostFactor;
  // size of the triangles to create when it's a nav modifier that adds extra triangles for
  // dynamic object navigation
  float triangulationSize;
};

#define	AIFAF_VISIBLE_FROM_REQUESTER	0x0001 	//! Requires whoever is requesting the anchor to also have a line of sight to it
//#define AIFAF_VISIBLE_TARGET					0x0002 	//! Requires that there is a line of sight between target and anchor
#define AIFAF_INCLUDE_DEVALUED 				0x0004 	//! don't care if the object is devalued
#define AIFAF_INCLUDE_DISABLED				0x0008 	//! don't care if the object is disabled
#define AIFAF_HAS_FORMATION						0x0010 	//! want only formations owners
#define AIFAF_LEFT_FROM_REFPOINT			0x0020 	//! Requires that the anchor is left from the target.
#define AIFAF_RIGHT_FROM_REFPOINT			0x0040 	//! Requires that the anchor is right from the target.
#define AIFAF_INFRONT_OF_REQUESTER		0x0080 	//! Requires that the anchor is withing a 30 degree cone in front of requiester.
#define AIFAF_SAME_GROUP_ID						0x0100 	//! want same group id
#define AIFAF_DONT_DEVALUE 						0x0200 	//! do not devalue the object after having found it
#define AIFAF_USE_REFPOINT_POS				0x0400	//! Use ref point position instead of ai object pos.


// AI sound event types. They are roughly in the order of priority.
// The priority is not enforced but may be used as a hint when handling the sound event.
enum EAISoundEventType
{
	AISE_GENERIC,					// Generic sound event
	AISE_COLLISION,				// Sound event from collisions.
	AISE_COLLISION_LOUD,	// Sound event from collisions, loud.
	AISE_MOVEMENT,				// Movement related sound event
	AISE_MOVEMENT_LOUD,		// Movement related sound event, very loud, like walking in water or a vehicle sound.
	AISE_WEAPON,					// Weapon firing related sound event
	AISE_EXPLOSION,				// Explosion related sound event
};

// Different grenade events reported into the AIsystem
enum EAIGrenadeEventType
{
	AIGE_GRENADE_THROWN,
	AIGE_GRENADE_COLLISION,
	AIGE_FLASH_BANG,
	AIGE_SMOKE,
};

// Different light events reported into the AIsystem
enum EAILightEventType
{
	AILE_GENERIC,
	AILE_MUZZLE_FLASH,
	AILE_FLASH_LIGHT,
	AILE_LASER,
	AILE_LAST,
};


#define SMART_OBJECTS_XML "/Libs/SmartObjects.xml"
#define AI_ACTIONS_PATH "Libs/ActionGraphs"



struct AIBalanceStats
{
	int nAllowedDeaths;
	int nTotalPlayerDeaths;
	int nEnemiesKilled;
	int nShotsFires;
	int nShotsHit;
	int nCheckpointsHit;
	int nVehiclesDestroyed;
	int nTotalEnemiesInLevel;
	int nSilentKills;
	
	float fAVGEnemyLifetime;
	float fAVGPlayerLifetime;
	float fTotalTimeSeconds;

	bool bFinal;

	AIBalanceStats()
	{
		bFinal = false;
		nAllowedDeaths=0;
		nTotalPlayerDeaths=0;
		nEnemiesKilled=0;
		nShotsFires=0;
		nShotsHit=0;
		nCheckpointsHit=0;
		nVehiclesDestroyed=0;
		nTotalEnemiesInLevel=0;
		nSilentKills=0;

		fAVGPlayerLifetime = 0;
		fAVGEnemyLifetime = 0;
		fTotalTimeSeconds =0;
	}

};

struct IAutoBalance
{
	virtual ~IAutoBalance() {}
	virtual void RegisterPlayerDeath()=0;
	virtual void RegisterPlayerFire(int nShots)=0;
	virtual void RegisterPlayerHit()=0;
	virtual void RegisterEnemyLifetime(float fLifeInSeconds)=0;
	virtual void RegisterVehicleDestroyed(void)=0;
	virtual void SetAllowedDeathCount(int nDeaths)=0;
	virtual void Checkpoint()=0;

	virtual void GetAutobalanceStats(AIBalanceStats & stats)=0;

	virtual void SetMultipliers(float fAccuracy, float fAggression, float fHealth)=0;
	virtual void GetMultipliers(float & fAccuracy, float & fAggression, float & fHealth)=0;

};


enum EActionType
{
	eAT_None = 0,
	eAT_Action,
	eAT_PriorityAction,
	eAT_Approach,
	eAT_PriorityApproach,
	eAT_ApproachAction,
	eAT_PriorityApproachAction,
	eAT_AISignal,
	eAT_AnimationSignal,
	eAT_AnimationAction,
};


class CSmartObjectClass;
class CSmartObjectAction;
struct SmartObjectCondition
{
	string	sUserClass;
	string	sUserState;
	string	sUserHelper;

	string	sObjectClass;
	string	sObjectState;
	string	sObjectHelper;

	float	fDistanceFrom;
	float	fDistanceTo;
	float	fOrientationLimit;
	float	fOrientationToTargetLimit;

	float	fMinDelay;
	float	fMaxDelay;
	float	fMemory;

	float	fProximityFactor;
	float	fOrientationFactor;
	float	fVisibilityFactor;
	float	fRandomnessFactor;

	float	fLookAtOnPerc;
	string	sUserPreActionState;
	string	sObjectPreActionState;
	EActionType	eActionType;
	string	sAction;
	bool	bEarlyPathRegeneration;
	string	sUserPostActionState;
	string	sObjectPostActionState;

	int		iMaxAlertness;
	bool	bEnabled;
	string	sName;
	string	sFolder;
	string	sDescription;
	int		iOrder;

	int		iRuleType; // 0 - normal rule; 1 - navigational rule;
	string	sEvent;
	string	sChainedUserEvent;
	string	sChainedObjectEvent;
	string	sEntranceHelper;
	string	sExitHelper;

	int		iTemplateId;

	// exact positioning related
	float	fApproachSpeed;
	int		iApproachStance;
	string	sAnimationHelper;
	string	sApproachHelper;
	float	fStartRadiusTolerance;
	float	fDirectionTolerance;
	float	fTargetRadiusTolerance;

	bool operator == ( const SmartObjectCondition& other ) const
	{
		return
			iTemplateId == other.iTemplateId &&

			iOrder == other.iOrder &&
			sUserClass == other.sUserClass &&
			sUserState == other.sUserState &&
			sUserHelper == other.sUserHelper &&
			
			sObjectClass == other.sObjectClass &&
			sObjectState == other.sObjectState &&
			sObjectHelper == other.sObjectHelper &&

			iRuleType == other.iRuleType &&
			sEvent == other.sEvent &&
			sEntranceHelper == other.sEntranceHelper &&
			sExitHelper == other.sExitHelper &&

			fDistanceFrom == other.fDistanceFrom &&
			fDistanceTo == other.fDistanceTo &&
			fOrientationLimit == other.fOrientationLimit &&
			fOrientationToTargetLimit == other.fOrientationToTargetLimit &&
			
			fMinDelay == other.fMinDelay &&
			fMaxDelay == other.fMaxDelay &&
			fMemory == other.fMemory &&
			
			fProximityFactor == other.fProximityFactor &&
			fOrientationFactor == other.fOrientationFactor &&
			fVisibilityFactor == other.fVisibilityFactor &&
			fRandomnessFactor == other.fRandomnessFactor &&
			
			fLookAtOnPerc == other.fLookAtOnPerc &&
			sUserPreActionState == other.sUserPreActionState &&
			sObjectPreActionState == other.sObjectPreActionState &&
			eActionType == other.eActionType &&
			sAction == other.sAction &&
			bEarlyPathRegeneration == other.bEarlyPathRegeneration &&
			sUserPostActionState == other.sUserPostActionState &&
			sObjectPostActionState == other.sObjectPostActionState &&
			
			iMaxAlertness == other.iMaxAlertness &&
			bEnabled == other.bEnabled &&
			sName == other.sName &&
			sFolder == other.sFolder &&
			sDescription == other.sDescription &&
			
			fApproachSpeed == other.fApproachSpeed &&
			iApproachStance == other.iApproachStance &&
			sAnimationHelper == other.sAnimationHelper &&
			sApproachHelper == other.sApproachHelper &&
			fStartRadiusTolerance == other.fStartRadiusTolerance &&
			fDirectionTolerance == other.fDirectionTolerance &&
			fTargetRadiusTolerance == other.fTargetRadiusTolerance;
	}

	bool operator < ( const SmartObjectCondition& other ) const
	{
		return iOrder < other.iOrder;
	}
};
typedef	std::list< SmartObjectCondition > SmartObjectConditions;


enum ESORuleType
{
	eSORuleType_TimeBased,
	eSORuleType_Navigation,
};

struct CSmartObjectRuleData
{
	string	sUserClass;
	string	sUserState;
	string	sUserHelper;

	string	sObjectClass;
	string	sObjectState;
	string	sObjectHelper;

	float	fDistanceFrom;
	float	fDistanceTo;
	float	fOrientationLimit;
	float	fOrientationToTargetLimit;

	float	fMinDelay;
	float	fMaxDelay;
	float	fMemory;

	float	fProximityFactor;
	float	fOrientationFactor;
	float	fVisibilityFactor;
	float	fRandomnessFactor;

	float	fLookAtOnPerc;
	string	sUserPreActionState;
	string	sObjectPreActionState;
	EActionType eActionType;
	string	sAction;
	bool	bEarlyPathRegeneration;
	string	sUserPostActionState;
	string	sObjectPostActionState;

	int		iMaxAlertness;

	ESORuleType	eSORuleType;
	string	sEvent;
	string	sChainedUserEvent;
	string	sChainedObjectEvent;

	string	sEntranceHelper;
	string	sExitHelper;

	// exact positioning related
	float	fApproachSpeed;
	int		iApproachStance;
	string	sAnimationHelper;
	string	sApproachHelper;
	float	fStartRadiusTolerance;
	float	fDirectionTolerance;
	float	fTargetRadiusTolerance;

	bool operator == ( const CSmartObjectRuleData& other ) const
	{
		return
			sUserClass == other.sUserClass &&
			sUserState == other.sUserState &&
			sUserHelper == other.sUserHelper &&

			sObjectClass == other.sObjectClass &&
			sObjectState == other.sObjectState &&
			sObjectHelper == other.sObjectHelper &&

			eSORuleType == other.eSORuleType &&
			sEvent == other.sEvent &&
			sEntranceHelper == other.sEntranceHelper &&
			sExitHelper == other.sExitHelper &&

			fDistanceFrom == other.fDistanceFrom &&
			fDistanceTo == other.fDistanceTo &&
			fOrientationLimit == other.fOrientationLimit &&
			fOrientationToTargetLimit == other.fOrientationToTargetLimit &&

			fMinDelay == other.fMinDelay &&
			fMaxDelay == other.fMaxDelay &&
			fMemory == other.fMemory &&

			fProximityFactor == other.fProximityFactor &&
			fOrientationFactor == other.fOrientationFactor &&
			fVisibilityFactor == other.fVisibilityFactor &&
			fRandomnessFactor == other.fRandomnessFactor &&

			fLookAtOnPerc == other.fLookAtOnPerc &&
			sUserPreActionState == other.sUserPreActionState &&
			sObjectPreActionState == other.sObjectPreActionState &&
			eActionType == other.eActionType &&
			sAction == other.sAction &&
			bEarlyPathRegeneration == other.bEarlyPathRegeneration &&
			sUserPostActionState == other.sUserPostActionState &&
			sObjectPostActionState == other.sObjectPostActionState &&

			iMaxAlertness == other.iMaxAlertness &&
			
			fApproachSpeed == other.fApproachSpeed &&
			iApproachStance == other.iApproachStance &&
			sAnimationHelper == other.sAnimationHelper &&
			sApproachHelper == other.sApproachHelper &&
			fStartRadiusTolerance == other.fStartRadiusTolerance &&
			fDirectionTolerance == other.fDirectionTolerance &&
			fTargetRadiusTolerance == other.fTargetRadiusTolerance;
	}
};
typedef	std::list< CSmartObjectRuleData >	VectorSORuleData;


// Obsolete data structure - to be removed soon!!!
	struct SmartObjectHelper
	{
		SmartObjectHelper() : templateHelperIndex(-1) {}
		QuatT qt;
		string name;
		string description;
		int templateHelperIndex;
	};

	// MapSOHelpers contains all smart object helpers sorted by name of the smart object class to which they belong
	typedef std::multimap<string, SmartObjectHelper > MapSOHelpers;


struct CSmartObjectHelperData
{
	string className;
	QuatT qt;
	string name;
};

// VectorSmartObjectHelperData contains all smart object helpers sorted by name of their smart object class
typedef std::vector< CSmartObjectHelperData > VectorSmartObjectHelperData;


// AI object iterator interface, see IAISystem::GetFirst.
struct IAIObjectIter
{
	// Advance to next object.
	virtual void Next() = 0;
	// Returns the current object.
	virtual IAIObject* GetObject() = 0;
	// Delete the iterator.
	virtual void Release() = 0;
};

// Helper class for AI object iterator.
class AutoAIObjectIter
{
public:
	AutoAIObjectIter() : m_iter(0) {}
	AutoAIObjectIter(IAIObjectIter* it) : m_iter(it) {}
	~AutoAIObjectIter() { SAFE_RELEASE(m_iter); }
	IAIObjectIter* operator->() { return m_iter; }
private:
	IAIObjectIter*	m_iter;
};


enum EAIEventType
{
	AIEVENT_SOUND				= 0x0001,
	AIEVENT_EXPLOSION		= 0x0002,
	AIEVENT_BULLET			= 0x0004,
	AIEVENT_COLLISION		= 0x0008,
};

// AI event listener
struct IAIEventListener
{
	virtual void OnAIEvent(EAIEventType type, const Vec3& pos, float radius, float threat, EntityId sender) = 0;
};

struct SAIDetectionLevels
{
	SAIDetectionLevels(float puppetExposure, float puppetThreat, float vehicleExposure, float vehicleThreat) :
		puppetExposure(puppetExposure),
		puppetThreat(puppetThreat),
		vehicleExposure(vehicleExposure),
		vehicleThreat(vehicleThreat)
	{
	}

	SAIDetectionLevels()
	{
		Reset();
	}

	inline void Append(const SAIDetectionLevels& levels)
	{
		puppetExposure = max(puppetExposure, levels.puppetExposure);
		puppetThreat = max(puppetThreat, levels.puppetThreat);
		vehicleExposure = max(vehicleExposure, levels.vehicleExposure);
		vehicleThreat = max(vehicleThreat, levels.vehicleThreat);
	}

	inline void Reset()
	{
		puppetExposure = 0;
		puppetThreat = 0;
		vehicleExposure = 0;
		vehicleThreat = 0;
	}

	float puppetExposure;
	float puppetThreat;
	float vehicleExposure;
	float vehicleThreat;
};


/*! Interface to AI system. Defines functions to control the ai system.
*/

struct IAISystem
{
	/// Flags used by the GetGroupCount.
	enum EGroupFlags
	{
		GROUP_ALL =			0x01,		// Returns all agents in the group (default).
		GROUP_ENABLED =	0x02,		// Returns only the count of enabled agents (exclusive with all).
		GROUP_MAX =			0x04,		// Returns the maximum number of agents during the game (can be combined with all or enabled).
	};

	/// Flags used by the GetDangerSpots.
	enum EDangerSpots
	{
		DANGER_DEADBODY =	0x01,		// Include dead bodies.
		DANGER_EXPLOSIVE = 0x02,	// Include explosives - unexploded grenades.
		DANGER_EXPLOSION_SPOT = 0x04,	// Include recent explosions.
		DANGER_ALL = DANGER_DEADBODY | DANGER_EXPLOSIVE,
	};

  /// Indication of (a) what a graph node represents and (b) what kind of graph node an AI entity
  /// can navigate. In the latter case it can be used as a bit mask.
  enum ENavigationType {
    NAV_UNSET              = 1 << 0,
    NAV_TRIANGULAR         = 1 << 1,
    NAV_WAYPOINT_HUMAN     = 1 << 2,
    NAV_WAYPOINT_3DSURFACE = 1 << 3,
    NAV_FLIGHT             = 1 << 4,
    NAV_VOLUME             = 1 << 5,
    NAV_ROAD               = 1 << 6,
    NAV_SMARTOBJECT        = 1 << 7,
    NAV_FREE_2D            = 1 << 8,
    NAV_MAX_VALUE          = NAV_FREE_2D
  };
	enum {NAV_TYPE_COUNT = 9};

  /// Bit mask using ENavigationType
  typedef unsigned tNavCapMask;

  /// two masks that summarise the basic abilities
  enum {
    NAVMASK_SURFACE = NAV_TRIANGULAR | NAV_WAYPOINT_HUMAN | NAV_ROAD | NAV_SMARTOBJECT,
    NAVMASK_AIR = NAV_FLIGHT | NAV_VOLUME | NAV_SMARTOBJECT,
    NAVMASK_ALL = NAV_TRIANGULAR | NAV_WAYPOINT_HUMAN | NAV_WAYPOINT_3DSURFACE | NAV_ROAD | NAV_FLIGHT | NAV_VOLUME | NAV_SMARTOBJECT | NAV_FREE_2D
  };

	/// Filter flags for IAISystem::GetFirstAIObject
	enum EGetFirstFilter
	{
		OBJFILTER_TYPE,			// Include only objects of specified type (type 0 means all objects).
		OBJFILTER_GROUP,		// Include only objects of specified group id.
		OBJFILTER_SPECIES,	// Include only objects of specified species.
	};

	enum EResetReason
	{
		RESET_INTERNAL,			// Called by the AI system itself
		RESET_ENTER_GAME,
		RESET_EXIT_GAME,
		RESET_INTERNAL_LOAD,	// Called by the AI system itself
	};

	virtual ~IAISystem() {}

	//! Initialize the ai system 
	virtual bool Init() = 0;
	//! Initialize the Smart Objects system
	virtual bool InitSmartObjects() = 0;
	//! Release the system
	virtual void ShutDown() = 0;
	//! Update system. This function is called every frame
	//! currentTime is the current AI time (since game start, in seconds). frameTime is the time since the
	//! last update. These must be used (via GetCurrentTime and GetFrameTime) by AI to support multiple
	//! timesteps per frame.
	virtual void Update(CTimeValue currentTime, float frameTime) = 0;

	// to if disabled - not updated, nothing is loaded, no AIObjects created for entities
	virtual void Enable(bool enable=true) = 0;

  /// Indicates if the auto-disable for distant AIs should be over-ridden
  virtual bool GetUpdateAllAlways() const = 0;

  /// Gets called when AI updates are about to stop (e.g. stopping AI/physics or leaving physics mode
  /// in editor) to let the AI system restore any run-time changes etc
  virtual void StoppingAIUpdates() = 0;

	/// Returns the current time (seconds since game began) that AI should be working with - 
	/// This may be different from the system so that we can support multiple updates per
	/// game loop/update.
	virtual CTimeValue GetFrameStartTime() const = 0;

	/// Time interval between this and the last update
	virtual float GetFrameDeltaTime() const = 0;

	/// returns the basic AI system update interval
	virtual float GetUpdateInterval() const = 0;

	/// this called before loading (level load/serialization)
	virtual void FlushSystem(void) = 0;
	virtual void FlushSystemNavigation(void) = 0;

	virtual const ObstacleData GetObstacle(int nIndex) = 0;

	//! Create an ai representation for an object
	virtual IAIObject *CreateAIObject(unsigned short type, IAIObject *pAssociation) = 0;

	virtual IGoalPipe *CreateGoalPipe(const char *pName) = 0;
	virtual IGoalPipe *OpenGoalPipe(const char *pName) = 0;

	virtual IAIObject *GetAIObjectByName(unsigned short type, const char *pName) const = 0;
	virtual IAIObject *GetNearestObjectOfTypeInRange(const Vec3 &pos, unsigned int type, float fRadiusMin, float fRadiusMax, IAIObject* pSkip=NULL, int nOption = 0) = 0;
	virtual IAIObject *GetNearestObjectOfTypeInRange(IAIObject *pObject , unsigned int type, float fRadiusMin, float fRadiusMax, int nOption = 0) = 0;
	virtual IAIObject *GetRandomObjectInRange(IAIObject * pRef, unsigned short nType, float fRadiusMin, float fRadiusMax, bool bFaceAttTarget=false) = 0;
	virtual IAIObject *GetNearestToObjectInRange( IAIObject *pRef , unsigned short nType, float fRadiusMin, float fRadiusMax, float inCone=-1, bool bFaceAttTarget=false, bool bSeesAttTarget=false, bool bDevalue=true) = 0;
	virtual IAIObject *GetBehindObjectInRange(IAIObject * pRef, unsigned short nType, float fRadiusMin, float fRadiusMax) = 0;
	virtual bool GetNearestPunchableObjectPosition(IAIObject* pRef, const Vec3& searchPos, float searchRad, const Vec3& targetPos, float minSize, float maxSize, float minMass, float maxMass, Vec3& posOut, Vec3& dirOut, IEntity** objEntOut) = 0;

	// Devalues an AI object for the refence object only or for whole group.
	virtual void			Devalue( IAIObject* pRef, IAIObject* pObject, bool group, float fDevalueTime=20.f ) = 0;

	virtual ILeader*	GetILeader(int groupid) = 0;

	virtual IAIGroup*	GetIAIGroup(int groupid) = 0;

	virtual void Release() = 0;

	virtual void Reset(EResetReason reason) = 0;

	virtual void RemoveObject(IAIObject *pObject) = 0;
	virtual void RemoveSmartObject(IEntity *pEntity) = 0;

	virtual void SoundEvent(const Vec3 &pos, float fRadius, EAISoundEventType type, IAIObject *pObject) = 0;
	virtual void CollisionEvent(const Vec3& pos, const Vec3& vel, float mass, float radius, float impulse, IEntity* pCollider, IEntity* pTarget) = 0;
	virtual void IgnoreCollisionEventsFrom(EntityId ent, float time) = 0;
	virtual void BreakEvent(const Vec3& pos, float breakSize, float radius, float impulse, IEntity* pCollider) = 0;
	virtual	void ExplosionEvent(const Vec3& pos, float radius, IAIObject* pShooter) = 0;
	virtual	void BulletHitEvent(const Vec3& pos, float radius, float soundRadius, IAIObject* pShooter) = 0;
	virtual	void BulletShotEvent(const Vec3& pos, const Vec3& dir, float range, IAIObject* pShooter) = 0;
	virtual	void DynOmniLightEvent(const Vec3& pos, float radius, EAILightEventType type, IAIObject* pShooter, float time = 5.0f) = 0;
	virtual	void DynSpotLightEvent(const Vec3& pos, const Vec3& dir, float radius, float fov, EAILightEventType type, IAIObject* pShooter, float time = 5.0f) = 0;
	virtual void GrenadeEvent(const Vec3& pos, float radius, EAIGrenadeEventType type, IEntity* pGrenade, IEntity* pShooter) = 0;

	// Registers AI event listener. Only events overlapping the sphere will be sent.
	// Register can be called again to update the listener position, radius and flags.
	// If pointer to the listener is specified it will be used instead of the pointer to entity.
	virtual void RegisterAIEventListener(IAIEventListener* pListener, const Vec3& pos, float rad, int flags) = 0;
	virtual void UnregisterAIEventListener(IAIEventListener* pListener) = 0;

	virtual bool CreateNavigationShape(const SNavigationShapeParams &params) = 0;
	virtual void DeleteNavigationShape(const char *szPathName) = 0;
	virtual bool DoesNavigationShapeExists(const char * szName, EnumAreaType areaType, bool road = false) = 0;

	virtual void SetUseAutoNavigation(const char *szPathName, EWaypointConnections waypointConnections) = 0;


	virtual	float GetNearestPointOnPath( const char * szPathName, const Vec3& vPos, Vec3& vResult, bool& bLoopPath, float& totalLength ) = 0 ;
	virtual void GetPointOnPathBySegNo( const char * sPathName, Vec3& vResult, float segNo ) = 0;

	virtual const char*	GetNearestPathOfTypeInRange(IAIObject* requester, const Vec3& pos, float range, int type, float devalue, bool useStartNode) = 0;

	virtual const char*	GetEnclosingGenericShapeOfType(const Vec3& pos, int type, bool checkHeight) = 0;
	virtual bool IsPointInsideGenericShape(const Vec3& pos, const char* shapeName, bool checkHeight) = 0;
	virtual float DistanceToGenericShape(const Vec3& pos, const char* shapeName, bool checkHeight) = 0;
	virtual bool ConstrainInsideGenericShape(Vec3& pos, const char* shapeName, bool checkHeight) = 0;
	virtual const char*	CreateTemporaryGenericShape(Vec3* points, int npts, float height, int type) = 0;
	virtual void EnableGenericShape(const char* shapeName, bool state) = 0;

	// Returns specified number of nearest hidespots. It considers the hidespots in graph and anchors.
	// Any of the pointers to return values can be null. Returns number of hidespots found.
	virtual unsigned int GetHideSpotsInRange(IAIObject* requester, const Vec3& reqPos,
		const Vec3& hideFrom, float minRange, float maxRange, bool collidableOnly, bool validatedOnly,
		unsigned int maxPts, Vec3* coverPos, Vec3* coverObjPos, Vec3* coverObjDir, float* coverRad, bool* coverCollidable) = 0;

	// Returns a point which is a valid distance away from a wall in front of the point.
	virtual void	AdjustDirectionalCoverPosition(Vec3& pos, const Vec3& dir, float agentRadius, float testHeight) = 0;

	/// The AI system will automatically reconnect nodes in this building ID 
	/// and recreate area/vertex information (only affects automatically
	/// generated links)
	virtual void ReconnectWaypointNodesInBuilding(int nBuildingID) = 0;

	struct SBuildingInfo
	{
		EWaypointConnections waypointConnections;
		float fNodeAutoConnectDistance;
	};
	virtual bool GetBuildingInfo(int nBuildingID, SBuildingInfo& info) = 0;
	virtual bool IsPointInBuilding(const Vec3& pos, int nBuildingID) = 0;

	/// Automatically reconnect all waypoint nodes in all buildings,
	/// and disconnects nodes not in buildings (only affects automatically
	/// generated links)
	virtual void ReconnectAllWaypointNodes() = 0;

	virtual Vec3 IsFlightSpaceVoid(const Vec3 &vPos, const Vec3 &vFwd, const Vec3 &vWng, const Vec3 &vUp )= 0;
	virtual Vec3 IsFlightSpaceVoidByRadius(const Vec3 &vPos, const Vec3 &vFwd, float radius )= 0;

	/// Returns true if a point is in a forbidden region. When two forbidden regions
	/// are nested then it is just the region between them that is forbidden. This
	/// only checks the forbidden areas, not the boundaries.
	virtual bool IsPointInForbiddenRegion(const Vec3 & pos, bool checkAutoGenRegions = true) const = 0;

	virtual bool IsSegmentValid(tNavCapMask navCap, float rad, const Vec3& posFrom, Vec3& posTo, ENavigationType& navTypeFrom) = 0;

	virtual void SendSignal(unsigned char cFilter, int nSignalId,const char *szText,  IAIObject *pSenderObject, IAISignalExtraData* pData=NULL) = 0;

	virtual void SendAnonymousSignal(int nSignalId,const char *szText, const Vec3 &pos, float fRadius, IAIObject *pSenderObject,IAISignalExtraData* pData=NULL) = 0;

	virtual void LoadNavigationData(const char * szLevel, const char * szMission) = 0;
	// Gets called after loading the mission
	virtual void OnMissionLoaded() = 0;
	virtual void GenerateTriangulation(const char * szLevel, const char * szMission) = 0;
	virtual void Generate3DVolumes(const char * szLevel, const char * szMission) = 0;
	virtual void GenerateFlightNavigation(const char * szLevel, const char * szMission) = 0;

  /// Modifies the additional cost multiplier of a named cost nav modifier. If factor < 0 then
  /// the cost is made infinite. If >= 0 then the cost is multiplied by 1 + factor.
  /// The original value gets reset when leaving/entering game mode etc.
  virtual void ModifyNavCostFactor(const char *navModifierName, float factor) = 0;

  /// Generates voxels for 3D nav regions and stores them (can cause memory problems) for subsequent
  /// debug viewing using ai_debugdrawvolumevoxels
  virtual void Generate3DDebugVoxels() = 0;

	virtual void	ValidateNavigation() = 0;

  /// returns the names of the region files generated during volume generation
  virtual const std::vector<string> & GetVolumeRegionFiles(const char * szLevel, const char * szMission) = 0;

	// Prompt the exporting of data from AI - these should require not-too-much processing - the volume
	// stuff requires more processing so isn't done for a normal export 
	virtual void ExportData(const char * navFileName, const char * hideFileName, const char * areasFileName, const char* roadsFileName, const char *waypoint3DSurfaceFileName) = 0;

  /// Register a spherical region that causes damage (so should be avoided in pathfinding). pID is just
  /// a unique identifying - so if this is called multiple times with the same pID then the damage region
  /// will simply be moved. If radius <= 0 then the region is disabled.
  virtual void RegisterDamageRegion(const void *pID, const struct Sphere &sphere) = 0;

	virtual int GetGroupCount(int nGroupID, int flags = GROUP_ALL, int type = 0) = 0;
	virtual IAIObject* GetGroupMember(int groupID, int index, int flags = GROUP_ALL, int type = 0) = 0;

	virtual void	SetLeader(IAIObject* pObject) = 0;

	virtual IAIObject*	GetBeacon(unsigned short nGroupID) = 0;
	virtual void UpdateBeacon(unsigned short nGroupID, const Vec3 & vPos, IAIObject *pOwner = 0) = 0;

	virtual void SetAssesmentMultiplier(unsigned short type, float fMultiplier) = 0;

	virtual void SetSpeciesThreatMultiplier(int nSpeciesID, float fMultiplier) = 0;

	/// Fills the array with possible dangers, returns number of dangers.
	virtual unsigned int GetDangerSpots(const IAIObject* requester, float range, Vec3* positions, unsigned int* types, unsigned int n, unsigned int flags) = 0;

	/// Checks for each point in the array if it is inside path obstacles of the specified requester. The corresponding result
	/// value will be set to true if the point is inside obstacle. Returns number of points inside the obstacles.
	virtual unsigned int CheckPointsInsidePathObstacles(IAIObject* requester, const Vec3* points, bool* results, unsigned int n) = 0;

	virtual IGraph *GetNodeGraph() = 0;

	virtual IGraph *GetHideGraph() = 0;

	virtual ENavigationType CheckNavigationType(const Vec3 & pos, int & nBuildingID, IVisArea *&pArea, tNavCapMask navCapMask) = 0;

	virtual IAutoBalance * GetAutoBalanceInterface(void) = 0;

	virtual	void DumpStateOf(IAIObject * pObject) = 0;

  virtual bool IsAIInDevMode() = 0;

	// debug members ============= DO NOT USE
	virtual void DebugDraw(IRenderer *pRenderer) = 0;
	
	virtual void	SetPerceptionDistLookUp( float* pLookUpTable, int tableSize ) = 0;	//look up table to be used when calculating visual time-out increment

	// Returns the exposure and threat levels as perceived by the specified AIobject.
	// If the AIobject is null, the local player values are returns
	virtual void GetDetectionLevels(IAIObject *pObject, SAIDetectionLevels& levels) = 0;

	// Returns how much an AI object is detected by enemy agents in range [0..1].
//	virtual float GetDetectionValue(IAIObject *pObject) = 0;
	// Returns an estimate how much an AI object is detectable in range [0..1].
//	virtual float GetAmbientDetectionValue(IAIObject *pObject) = 0;

	virtual int GetAITickCount() = 0;

	virtual	void GetMemoryStatistics(ICrySizer *pSizer) = 0;

  enum EIFMode {IF_AREASBOUNDARIES, IF_AREAS, IF_BOUNDARIES};
	virtual bool IntersectsForbidden(const Vec3 & vStart, const Vec3 & vEnd, Vec3 & vClosestPoint, const string * nameToSkip = 0,Vec3* pNormal = NULL, EIFMode mode = IF_AREASBOUNDARIES, bool bForceNormalOutwards = false) const = 0;

	// Returns AIObject iterator for first match, see EGetFirstFilter for the filter options.
	// The parameter 'n' specifies the type, group id or species based on the selected filter.
	// It is possible to iterate over all objects by setting the filter to OBJFILTER_TYPE
	// passing zero to 'n'.
	virtual IAIObjectIter* GetFirstAIObject(EGetFirstFilter filter, short n) = 0;

	// Iterates over AI objects within specified range.
	// Parameter 'pos' and 'rad' specify the enclosing sphere, for other parameters see GetFirstAIObject.
	virtual IAIObjectIter* GetFirstAIObjectInRange(EGetFirstFilter filter, short n, const Vec3& pos, float rad, bool check2D) = 0;
	// Iterates over AI objects within specified shape.
	// Parameter 'name' specify the enclosing shape and parameter 'checkHeight' specifyes if hte height of the shape is taken into account too,
	// for other parameters see GetFirstAIObject.
	virtual IAIObjectIter* GetFirstAIObjectInShape(EGetFirstFilter filter, short n, const char* shapeName, bool checkHeight) = 0;

	virtual void Event( int eventT, const char *) = 0;

	virtual void CreateFormationDescriptor(const char *name) = 0;

	virtual bool ChangeFormation(IAIObject* pOwner, const char * szFormationName,float fScale) = 0;

	virtual bool ScaleFormation(IAIObject* pOwner, float fScale) = 0;

	virtual bool SetFormationUpdate(IAIObject* pOwner, bool bUpdate) = 0;

	virtual void AddFormationPoint(const char *name, const FormationNode& nodeDescriptor) = 0;

	virtual IAIObject* GetFormationPoint(IAIObject* pObject) = 0;

	virtual int	 GetFormationPointClass(const char* descriptorName, int position) = 0;

	virtual ITrackPatternDescriptor*	CreateTrackPatternDescriptor( const char* patternName ) = 0;
	virtual ITrackPatternDescriptor*	FindTrackPatternDescriptor( const char* patternName ) = 0;

	virtual	void RegisterFirecommandHandler(IFireCommandDesc* desc) = 0;
	virtual	IFireCommandHandler*	CreateFirecommandHandler(const char* name, IAIActor *pShooter) = 0;

	virtual IAIObject* GetLeaderAIObject(int iGroupId) = 0;

	virtual IAIObject* GetLeaderAIObject(IAIObject* pObject) = 0;

	virtual bool RequestVehicle(IAIObject* pRequestor, Vec3 &vSourcePos, IAIObject *pAIObjectTarget, const Vec3& vTargetPos, int iGoalType) = 0;
	virtual TAIObjectList* GetVehicleList(IAIObject* pRequestor) = 0;

	// current global AI alertness value (what's the most allerted puppet)
	virtual int GetAlertness() const = 0;
	
	virtual int GetAlertStatus(const IAIObject* pObject) = 0;
	// outline contains a list of points (z is ignored) within which outside navigation
	// should be disabled because the surface has been broken
	virtual void DisableNavigationInBrokenRegion(std::list<Vec3> & outline) = 0;

	// functions to let external systems (e.g. lua) access the AI logging functions. 
	// the external system should pass in an identifier (e.g. "<Lua> ")
	virtual void Warning(const char * id, const char * format, ...) PRINTF_PARAMS(3, 4) = 0;
	virtual void Error(const char * id, const char * format, ...) PRINTF_PARAMS(3, 4) = 0;
	virtual void LogProgress(const char * id, const char * format, ...) PRINTF_PARAMS(3, 4) = 0;
	virtual void LogEvent(const char * id, const char * format, ...) PRINTF_PARAMS(3, 4) = 0;
	virtual void LogComment(const char * id, const char * format, ...) PRINTF_PARAMS(3, 4) = 0;

	// save/load
	virtual void Serialize( TSerialize ser ) = 0;

	// smart objects
	virtual void ReloadSmartObjectRules() = 0;
	virtual void AddSmartObjectCondition(const SmartObjectCondition& condition) = 0;
	virtual const char* GetSmartObjectStateName( int id ) const = 0;
	virtual SmartObjectHelper* GetSmartObjectHelper( const char* className, const char* helperName ) const = 0;
	virtual void RegisterSmartObjectState( const char* sStateName ) = 0;
	virtual void SetSmartObjectState( IEntity* pEntity, const char* sStateName ) = 0;
	virtual void AddSmartObjectState( IEntity* pEntity, const char* sState ) = 0;
	virtual void RemoveSmartObjectState( IEntity* pEntity, const char* sState ) = 0;
	virtual void ModifySmartObjectStates( IEntity* pEntity, const char* listStates ) = 0;
	virtual bool CheckSmartObjectStates( IEntity* pEntity, const char* patternStates ) const = 0;

	virtual void GetSOClassTemplateIStatObj( IEntity* pEntity, IStatObj**& pStatObjects ) const = 0;
	virtual bool ValidateSOClassTemplate( IEntity* pEntity, bool bStaticOnly ) = 0;
	virtual void DrawSOClassTemplate( IEntity* pEntity ) const = 0;

	// ai actions
	virtual void ReloadActions() = 0;
	virtual IAIAction* GetAIAction( const char* name ) = 0;
	virtual IAIAction* GetAIAction( size_t index ) = 0;
	virtual const char* GetAIActionName( int index ) const = 0;
	virtual void ExecuteAIAction( const char* sActionName, IEntity* pUser, IEntity* pObject, int maxAlertness, int goalPipeId ) = 0;
	virtual void AbortAIAction( IEntity* pUser, int goalPipeId ) = 0;

	// smart object events
	virtual int SmartObjectEvent( const char* sEventName, IEntity*& pUser, IEntity*& pObject, const Vec3* pExtraPoint = NULL, bool bHighPriority = false ) = 0;

	virtual int AllocGoalPipeId() const = 0;
	virtual IAISignalExtraData* CreateSignalExtraData() const = 0;
	virtual void FreeSignalExtraData( IAISignalExtraData* pData ) const = 0;

	virtual void AddCombatClass(int combatClass, float* pScalesVector, int size, const char* szCustomSignal) = 0;
	virtual const char* GetCustomOnSeenSignal(int combatClass) = 0;

	virtual void AddObstructSphere( const Vec3& pos, float radius, float lifeTime, float fadeTime) = 0;

	// Draws a fake tracer around the player.
	virtual void DebugDrawFakeTracer(const Vec3& pos, const Vec3& dir) = 0;

	virtual float ProcessBalancedDamage(IEntity* pShooterEntity, IEntity* pTargetEntity, float damage, const char* damageType) = 0;

	//====================================================================
	// debug recorder
	//====================================================================
	virtual bool	IsRecording( const IAIObject* pTarget, IAIRecordable::e_AIDbgEvent event ) const = 0;
	virtual void	Record( const IAIObject* pTarget, IAIRecordable::e_AIDbgEvent event, const char* pString ) const = 0;
	virtual void	SetDrawRecorderRange(float start, float end, float cursor) = 0;
	virtual void	AddDebugLine( const Vec3& start, const Vec3& end, uint8 r, uint8 g, uint8 b, float time ) = 0;
	virtual void	AddDebugSphere(const Vec3& pos, float radius, uint8 r, uint8 g, uint8 b, float time) = 0;

	virtual void	DebugReportHitDamage(IEntity* pVictim, IEntity* pShooter, float damage, const char* material) = 0;
	virtual void	DebugReportDeath(IAIObject* pVictim) = 0;

	virtual IAIRecorder * GetIAIRecorder(void) = 0;

	// profiling
	virtual void TickCountStartStop( bool start=true ) = 0;

	// Get the interest system
	virtual ICentralInterestManager* GetCentralInterestManager(void) = 0;
};

#endif //_IAISYSTEM_H_

