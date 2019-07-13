// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _CRY_COMMON_CRY_CHAR_ANIMATION_PARAMS_HDR_
#define _CRY_COMMON_CRY_CHAR_ANIMATION_PARAMS_HDR_


#include "ISerialize.h"


#define MAX_LMG_ANIMS (33)



// the flags used in the nFlags member
enum CA_AssetFlags
{
	// if this is true, then the asset has valid info data (has been loaded at least once)
	CA_ASSET_LOADED			= 0x001,
	// if this is true, then its possible to use this asset with the loop-flag
	CA_ASSET_CYCLE			= 0x002,
	// if this is true, then the asset is a locomotion group
	CA_ASSET_LMG				= 0x008,
	// if this is true, then the asset has been processed at loading time
	CA_ASSET_LMG_VALID	= 0x020,
	// if this is true, then the asset has been processed at loading time
	CA_ASSET_PROCESSED	= 0x040,
	// if this true, then the asset is created but not loaded
	CA_ASSET_CREATED    = 0x080,
	// if this true, then the asset is already requested for loading
	CA_ASSET_REQUESTED  =	0x100,
	// if this true, then the asset has on-demand loading
	CA_ASSET_ONDEMAND   = 0x200,
	// if this true, then we had an aimpose that we removed from memory
	CA_AIMPOSE_UNLOADED = 0x400
};


enum CA_AnimationFlags
{

	//-------------------------------------------------------------
	//FLAGS TO HANDLE ANIMATION PLAYBACK
	//-------------------------------------------------------------
	//	All playback flags are exclusive. Only one of them can be enabled for an animation at a time. Order of priotity:
	//	CA_MANUAL_UPDATE, (disables looping and repeat) 
	//	CA_LOOP_ANIMATION, (disables repeat) 
	//	CA_REPEAT_LAST_KEY, 

	//with this flag the animation is not being updated automatically. The user needs to set the time manually. This is used for steering-wheels and mounted-weapons, where we convert the rotation of an object into animation-keytimes 
	CA_MANUAL_UPDATE			=0x0001, 

	//Plays an animations in an endless loop till we stop this animation or start a new animation.
	CA_LOOP_ANIMATION			=0x0002, 

	//Plays an animation once and then repeats the last keyframe. Without this flag we remove the 
	//animation from the FIFO.  
	CA_REPEAT_LAST_KEY		=0x0004, 


	//-------------------------------------------------------------
	//FLAGS TO HANDLE TRANSITIONS
	//-------------------------------------------------------------

	//linear time-warping of animations to align animation with similar properties
	CA_TRANSITION_TIMEWARPING	=0x0008, 

	//Don't start a transition immediately. Wait till the previous animation is passing a specified key-time
	CA_START_AT_KEYTIME				=0x0010, 

	//can be simulated by using the flags "CA_START_AT_KEYTIME" and "CA_REPEAT_LAST_KEY".
	CA_START_AFTER	  				=0x0020, 

	//When playing an Idel2Move transition, we would like to find the best transition point 
	//to start a locomotion-animation 
	CA_IDLE2MOVE							=0x0040, 

	//By default it is not possible to start the same animation twice. In some special cases this is sometimes necessary (e.g. recoil-animations). By enabling this flag we can restart the same animation using the previously described transition-rules. 
	CA_ALLOW_ANIM_RESTART			=0x0080, 




	//-------------------------------------------------------------
	//FLAGS TO HANDLE CONTROLLER USAGE
	//-------------------------------------------------------------
	//When we play an animation we update every bone in the skeleton, even if a certain joint has no controller. 
	//That means, that full-skeleton animation is enabled by default. Animators are told to create animation-assets 
	//by using the smallest amount of controllers (=animation-channels per joint) that is possible to safe memory. 
	//For fast animations (e.g. a sprint) you don’t need finger animations. If a bone has no controller, we take 
	//the default value stored in the rig. For some mechanical objects (weapons, vehicles) or special animation (e.g. upper-body animations) 
	//we want to use only the the exiting controllers in the animations. This flag is disabling full-body animation update.   
	CA_PARTIAL_SKELETON_UPDATE	=0x0100, //Update just bones with controller. Don't use default value for missing controllers.

	//don't allow multilayer animations when this flag is set in Layer0
	CA_DISABLE_MULTILAYER       =0x0200,  

	//When an object is not visible, then we just update the animation but NOT the skeleton. The skeleton will stay in the last updated pose. 
	//This can can be a problem in some situations, for example when an animation is used to open a door, or when we use skeleton animation 
	//to move physical objects in the game. A good example is the moving platform in MP to lift up the VTOL and the helicopters).  
	//The only way to enforce the skeleton-update is to use this flag per animation.      
	CA_FORCE_SKELETON_UPDATE    =0x0400,  

	//-------------------------------------------------------------
	//FLAGS FOR SPECIAL ANIMATION MODES
	//-------------------------------------------------------------
	CA_TRACK_VIEW_EXCLUSIVE     =	0x0800, //Play this animation only when you're in Track-View 
	CA_SUPPRESS_WARNINGS        =	0x1000, //Suppress warnings for flocks

	//When playing an locomotion-animation, we would like to find the best transition point to start a Move2Idle 
	CA_MOVE2IDLE								=	0x2000, 

	//Usually we always update animations, even when the object is not visible. For simple objects (e.g. boids) we want to 
	//avoid even animation-update. This can lead to an overflow in the FIFO-queue. 
	//To avoid the overflow, we remove the first animation from the FIFO when there are more then 16 animation in the queue.
	CA_REMOVE_FROM_FIFO	= 0x4000,  

	CA_FULL_ROOT_PRIORITY	= 0x8000,  

};

static const int NUM_ANIMATION_USER_DATA_SLOTS = 8;



struct OldBlendSpace
{
	f32 m_speed;
	Vec2 m_strafe;
	f32 m_turn;
	f32 m_slope;


	
	f32 m_fAllowLeaningSmooth;		//just for debugging
	f32 m_fAllowLeaningSmoothRate;//just for debugging
	uint32 m_updates;							//just for debugging
	QuatT m_AnimChar;							//just for debugging

	OldBlendSpace()
	{	
		m_speed=0.0f;
		m_strafe=Vec2(0,0); 
		m_turn=0.0f; 
		m_slope=0.0f;	
		m_fAllowLeaningSmooth=1.0f;
		m_fAllowLeaningSmoothRate=1.0f;
		m_updates=0;
		m_AnimChar.SetIdentity();
	};

	OldBlendSpace( f32 a, f32 b, f32 c, f32 d, f32 e ) 
	{ 
		m_speed		=a; 
		m_strafe	=Vec2(b,c); 
		m_turn		=d; 
		m_slope		=e; 
	};

	void Init() 
	{ 
		m_speed		=0; 
		m_strafe	=Vec2(0,0); //forward direction
		m_turn		=0; 
		m_slope		=0; 
	};

};


//--------------------------------------------------------------------------------

enum EWeaponRaisedPose
{
	eWeaponRaisedPose_None				= 0x00,
	eWeaponRaisedPose_Fists				= 0x10,
	eWeaponRaisedPose_Rifle				= 0x20,
	eWeaponRaisedPose_Pistol			= 0x30,
	eWeaponRaisedPose_Rocket			= 0x40,
	eWeaponRaisedPose_MG					= 0x50,

	eWeaponRaisedPose_DualLft			= 0x01,
	eWeaponRaisedPose_DualRgt			= 0x02,

	eWeaponRaisedPose_PistolLft		= eWeaponRaisedPose_Pistol | eWeaponRaisedPose_DualLft,
	eWeaponRaisedPose_PistolRgt		= eWeaponRaisedPose_Pistol | eWeaponRaisedPose_DualRgt,
	eWeaponRaisedPose_PistolBoth	= eWeaponRaisedPose_Pistol | eWeaponRaisedPose_DualLft | eWeaponRaisedPose_DualRgt,
};

//--------------------------------------------------------------------------------

enum EMotionParamID
{
	eMotionParamID_TravelAngle = 0,
	eMotionParamID_TravelDistScale,
	eMotionParamID_TravelSpeed,
	eMotionParamID_TravelDist, 
	eMotionParamID_TravelSlope,
	eMotionParamID_TurnSpeed,
	eMotionParamID_TurnAngle,
	eMotionParamID_Duration,
	eMotionParamID_Curving,

	eMotionParamID_COUNT,
};
extern char* g_szMotionParamID[eMotionParamID_COUNT];

//--------------------------------------------------------------------------------

enum EMotionParamUsage
{
	eMotionParamUsage_None = 0,
	eMotionParamUsage_Interval,
	eMotionParamUsage_Cyclic,
};
extern char* g_szMotionParamUsage[eMotionParamID_COUNT];

//--------------------------------------------------------------------------------

struct MotionParamDesc
{
	// Defined how the parameter value should be used.
	uint32 m_nUsage;
	bool m_bLocked; // =(maxChangeRate == 0.0f)

	// These are the total limits, including procedural scaling.
	// None: not parameterizable, only a static property (min==max)
	// Interval: Absolute minimum parameter value
	// Cyclic: The calculated parameter is shifted to be within this cyclic range.
	f32 m_fMin; 
	f32 m_fMax;

	// These are the real limits of the asset.
	f32 m_fMinAsset;
	f32 m_fMaxAsset;

	// Parameter value change rate per seconds
	f32 m_fMaxChangeRate;
	f32 m_fAllowProcTurn;

	MotionParamDesc()
	{
		m_nUsage = eMotionParamUsage_None;
		m_bLocked = false;

		m_fMin = 0.0f;
		m_fMax = 0.0f;
		m_fMinAsset = 0.0f;
		m_fMaxAsset = 0.0f;
		m_fMaxChangeRate = 0.0f;
		m_fAllowProcTurn = 0.0f;
	}
};

//--------------------------------------------------------------------------------

struct LMGCapabilities
{
	uint8 m_bIsValid;			// by default not valid
	uint8 m_bIsLMG;				// 1=LMG / 0=simple asset
	int8 m_bHasStrafingAsset; // 1=has strafing assets, 0=don't have but don't care, -1=don't like at all
	int8 m_bHasTurningAsset;  // 1=has turning assets, 0=don't have but don't care, -1=don't like at all
	int8 m_bHasTurningDistAsset;  // 1=has turning assets, 0=don't have but don't care, -1=don't like at all
	int8 m_bHasSlopeAsset;    // 1=supports uphill/downhill, 0=don't have but don't care, -1=don't like at all
	int8 m_bHasVelocity; // 1=has parameterized speed, 0=don't have but don't care, -1=don't like at all
	int8 m_bHasDistance; // 1=has parameterized speed, 0=don't have but don't care, -1=don't like at all

	int8 m_bHasCurving;

	uint8 m_bpad1;
	uint8 m_bpad2;

	uint32 m_BlendType;	    //blend-type for the LMG

	f32 m_travelAngleChangeRate;
	f32 m_travelSpeedChangeRate;
	f32 m_turnSpeedChangeRate;
	f32 m_curvingChangeRate;

	f32 m_fSlowDuration;			//slow duration of the LMG with these settings
	f32 m_fFastDuration;			//fast duration of the LMG with these settings
	Vec3 m_vMinVelocity;
	Vec3 m_vMaxVelocity;

	f32 m_fSlowTurnLeft;
	f32 m_fSlowTurnRight;
	f32 m_fFastTurnLeft;
	f32 m_fFastTurnRight;
	f32 m_fAllowDesiredTurning; //is a value between [0...1] 

	f32 m_fDesiredLocalLocationLookaheadTime;

	ILINE LMGCapabilities() 
	{
		m_bIsValid=0;			// by default not valid
		m_bIsLMG=0;				// 1=LMG / 0=simple asset
		m_bHasStrafingAsset=0;	// 1=has strafing assets    0=don't have, don't care  -1=don't have, don't want at all (prevent)
		m_bHasTurningAsset=0;	// 1=has turning assets    0=don't have, don't care  -1=don't have, don't want at all (prevent)
		m_bHasTurningDistAsset=0;	// 1=has turning assets    0=don't have, don't care  -1=don't have, don't want at all (prevent)
		m_bHasSlopeAsset=0;    // 1=supports uphill/downhill
		m_bHasVelocity=0;	// 1=has parameterized speed    0=don't have, don't care  -1=don't have, don't want at all (prevent)
		m_bHasDistance=0;	// 1=has parameterized speed    0=don't have, don't care  -1=don't have, don't want at all (prevent)
		m_bHasCurving=0;

		m_bpad1=0;
		m_bpad2=0;

		m_BlendType=0;

		m_travelAngleChangeRate = 0.0f;
		m_travelSpeedChangeRate = 0.0f;
		m_turnSpeedChangeRate = 0.0f;
		m_curvingChangeRate = 0.0f;

		m_fSlowDuration=-1.0f;
		m_fFastDuration=-1.0f;

		m_vMinVelocity.Set(0,0,0);
		m_vMaxVelocity.Set(0,0,0);

		m_fSlowTurnLeft=0;
		m_fSlowTurnRight=0;
		m_fFastTurnLeft=0;
		m_fFastTurnRight=0;
		m_fAllowDesiredTurning=0;

		m_fDesiredLocalLocationLookaheadTime = 0.0f;
	}

	ILINE LMGCapabilities operator * (f32 k) const 
	{ 
		LMGCapabilities caps;
		caps.m_fSlowDuration		=	m_fSlowDuration*k;
		caps.m_fFastDuration		=	m_fFastDuration*k;

		caps.m_vMinVelocity		=	m_vMinVelocity*k;
		caps.m_vMaxVelocity		=	m_vMaxVelocity*k;

		caps.m_fFastTurnLeft					=	m_fFastTurnLeft*k;
		caps.m_fFastTurnRight				=	m_fFastTurnRight*k;
		caps.m_fAllowDesiredTurning	=	m_fAllowDesiredTurning*k;
		return caps;
	}
	ILINE LMGCapabilities operator + (const LMGCapabilities& c) const 
	{ 
		LMGCapabilities caps;

		caps.m_fSlowDuration	=	m_fSlowDuration+c.m_fSlowDuration;
		caps.m_fFastDuration	=	m_fFastDuration+c.m_fFastDuration;

		caps.m_vMinVelocity	=	m_vMinVelocity+c.m_vMinVelocity;
		caps.m_vMaxVelocity	=	m_vMaxVelocity+c.m_vMaxVelocity;

		caps.m_fFastTurnLeft					=	m_fFastTurnLeft+c.m_fFastTurnLeft;
		caps.m_fFastTurnRight				=	m_fFastTurnRight+c.m_fFastTurnRight;
		caps.m_fAllowDesiredTurning	=	m_fAllowDesiredTurning+c.m_fAllowDesiredTurning;
		return caps;

	}

	ILINE static LMGCapabilities Merge(const LMGCapabilities& a, const LMGCapabilities& b, float blend)
	{
		LMGCapabilities merged;

		if (a.m_bIsValid && !b.m_bIsValid)
			return a;

		if (b.m_bIsValid && !a.m_bIsValid)
			return b;

		if (!a.m_bIsValid && !a.m_bIsValid)
			return merged;

		merged.m_bIsValid = true;
		merged.m_bIsLMG = a.m_bIsLMG || b.m_bIsLMG;

		#define GETBLEND(flag) (((a.flag*b.flag) < 0) ? -1.0f : ((a.flag) ? ((b.flag) ? blend : 0.0f) : ((b.flag) ? 1.0f : -1.0f)))
		#define BLEND(member, t) { merged.member = LERP(a.member, b.member, t); }
		#define IFBLEND(flag, t) float t = GETBLEND(flag); merged.flag = (t != -1.0f); if (merged.flag)

		IFBLEND(m_bHasStrafingAsset, strafeBlend)
		{
			BLEND(m_travelAngleChangeRate, strafeBlend);
		}
		
		IFBLEND(m_bHasTurningAsset, turnBlend)
		{
			BLEND(m_fFastTurnLeft, turnBlend);
			BLEND(m_fFastTurnRight, turnBlend);
			BLEND(m_fAllowDesiredTurning, turnBlend);
		}

		IFBLEND(m_bHasVelocity, veloBlend)
		{
			BLEND(m_vMinVelocity, veloBlend);
			BLEND(m_vMaxVelocity, veloBlend);
			BLEND(m_travelSpeedChangeRate, veloBlend);
		}

		
		IFBLEND(m_bHasDistance, distBlend)
		{
			BLEND(m_vMinVelocity, distBlend);
			BLEND(m_vMaxVelocity, distBlend);
			BLEND(m_fSlowDuration, distBlend);
			BLEND(m_fFastDuration, distBlend);
		}
		else
		{
			BLEND(m_fSlowDuration, blend);
			BLEND(m_fFastDuration, blend);
		} 

		IFBLEND(m_bHasSlopeAsset, slopeBlend)
		{
		}

		merged.m_BlendType = (blend < 0.5f) ? a.m_BlendType : b.m_BlendType;


		return merged;
	}

};

//--------------------------------------------------------------------------------

struct SAnimationSelectionProperties
{
	f32 m_fDurationMin;
	f32 m_fDurationMax;
	f32 m_fStartTravelSpeedMin;
	f32 m_fStartTravelSpeedMax;
	f32 m_fEndTravelSpeedMin;
	f32 m_fEndTravelSpeedMax;
	f32 m_fTravelDistanceMin;
	f32 m_fTravelDistanceMax;
	f32 m_fStartTravelAngleMin; // Relative to StartBodyAngle
	f32 m_fStartTravelAngleMax; // Relative to StartBodyAngle
	f32 m_fEndTravelAngleMin; // Relative to StartBodyAngle
	f32 m_fEndTravelAngleMax; // Relative to StartBodyAngle
	f32 m_fEndBodyAngleMin;  // Relative to StartBodyAngle
	f32 m_fEndBodyAngleMax;  // Relative to StartBodyAngle
	// StartBodyAngle is zero, by definition.
	f32 m_fTravelAngleChangeMin;
	f32 m_fTravelAngleChangeMax;
	f32 m_fEndTravelToBodyAngleMin;
	f32 m_fEndTravelToBodyAngleMax;
	f32 m_fUrgencyMin;
	f32 m_fUrgencyMax;
	bool m_bPredicted; // Indicates this is a fancy animation that relies on prediction and should not be preferred (or used at all) if no prediction is available.
	bool m_bGuarded; // Optimization, indicates that AG pathfinding should apply animation dependent guard on states using this animation.
	bool m_bComplexBodyTurning; // Indicates that the body angle will change either quickly or complex (and should not be clamped to the entity, as much).
	bool m_bComplexTravelPath; // Indicates that the path changes either quickly or complex (and should not be clamped to the entity, as much).
	bool m_bLocomotion;
	float m_fEndBodyAngleThreshold;
	float m_fTravelDistanceThreshold;

//#ifdef _DEBUG
	uint32 DebugCapsCode;
//#endif

	SAnimationSelectionProperties()
	{
		init();
	}

	void init()
	{
//#ifdef _DEBUG
		DebugCapsCode = 0;
//#endif

		m_fDurationMin = 0.0f;
		m_fDurationMax = 0.0f;
		m_fStartTravelSpeedMin = 0.0f;
		m_fStartTravelSpeedMax = 0.0f;
		m_fEndTravelSpeedMin = 0.0f;
		m_fEndTravelSpeedMax = 0.0f;
		m_fTravelDistanceMin = 0.0f;
		m_fTravelDistanceMax = 0.0f;
		m_fStartTravelAngleMin = 0.0f;
		m_fStartTravelAngleMax = 0.0f;
		m_fEndTravelAngleMin = 0.0f;
		m_fEndTravelAngleMax = 0.0f;
		m_fEndBodyAngleMin = 0.0f;
		m_fEndBodyAngleMax = 0.0f;
		m_fTravelAngleChangeMin = 0.0f;
		m_fTravelAngleChangeMax = 0.0f;
		m_fEndTravelToBodyAngleMin = 0.0f;
		m_fEndTravelToBodyAngleMax = 0.0f;
		m_fUrgencyMin = 0.0f;
		m_fUrgencyMax = 0.0f;
		m_bPredicted = false;
		m_bGuarded = false;
		m_bComplexBodyTurning = false;
		m_bComplexTravelPath = false;
		m_bLocomotion = false;
		m_fEndBodyAngleThreshold = 0.0f;
		m_fTravelDistanceThreshold = 0.0f;
	}

	void expand(const SAnimationSelectionProperties* other)
	{
		if (other == NULL)
			return;

		#define EXPAND(name)		{ m_f##name##Min = min(m_f##name##Min, other->m_f##name##Min); m_f##name##Max = max(m_f##name##Max, other->m_f##name##Max); }

//#ifdef _DEBUG
		DebugCapsCode = 0xFFFF;
//#endif

		EXPAND(Urgency);
		EXPAND(Duration);
		EXPAND(StartTravelSpeed);
		EXPAND(EndTravelSpeed);
		EXPAND(TravelDistance);
		EXPAND(StartTravelAngle);
		EXPAND(EndTravelAngle);
		EXPAND(EndBodyAngle);
		EXPAND(TravelAngleChange);
		EXPAND(EndTravelToBodyAngle);
		m_bPredicted = m_bPredicted || other->m_bPredicted;
		m_bGuarded = m_bGuarded || other->m_bGuarded;
		m_bComplexBodyTurning = m_bComplexBodyTurning || other->m_bComplexBodyTurning;
		m_bComplexTravelPath = m_bComplexTravelPath || other->m_bComplexTravelPath;
	}

};

//--------------------------------------------------------------------------------

struct CryAnimationPath
{
	QuatT m_key0;
	QuatT m_key1;
};

struct SAnimRoot
{
	f32 m_NormalizedTimeAbs;
	f32 m_KeyTimeAbs;
	QuatT m_TransRot;

	SAnimRoot() 
	{
		m_NormalizedTimeAbs	=	0;
		m_KeyTimeAbs	=	0;
		m_TransRot.SetIdentity();
	};

};

struct AnimTransRotParams
{
	//output: if valid then its "1"
	uint32 m_Valid;
	//output (clamped if animation is shorter) in seconds. Any negative values (probably only -1) should return the movement of the latest update between the previous and current frames.
	f32 m_DeltaTime;
	//output, relative trans/rot at requested (clamped) deltatime.
	QuatT m_TransRot;
	//output: this is the total time of the animation based on the current parameters till it loops 
	f32 m_KeyTimeAbs;  
	//output: for this LMG/asset as "normalized time"
	f32 m_NormalizedTimeAbs;

	AnimTransRotParams() 
	{
		m_Valid = 0;
		m_DeltaTime	=	-1;
		m_TransRot.SetIdentity();
		m_KeyTimeAbs	=	-1;
		m_NormalizedTimeAbs =	-1;
	};

};







struct AnimEventInstance
{
	f32 m_time; 
	uint32 m_nAnimNumberInQueue;
	f32 m_fAnimPriority; 
	const char* m_AnimPathName; 
	int m_AnimID;
	const char* m_EventName; 
	const char* m_CustomParameter; // Meaning depends on event - sound: sound path, effect: effect name
	const char* m_BonePathName;
	Vec3 m_vOffset;
	Vec3 m_vDir;

	AnimEventInstance()
	{
		m_time=0; 
		m_nAnimNumberInQueue=0; 
		m_fAnimPriority=0; 
		m_AnimPathName=0; 
		m_AnimID=0;
		m_EventName=0; 
		m_CustomParameter=0;
		m_BonePathName=0;
		m_vOffset = Vec3(0, 0, 0);
		m_vDir = Vec3(0, 0, 0);
	}
};

//////////////////////////////////////////////////////////////////////////
// This structure describes the parameters used to start an animation
// on a character.
struct CryCharAnimationParams
{
	CryCharAnimationParams( int _nLayerID=0, uint32 _nFlags=0) : m_fTransTime(-1.0f),
		m_fKeyTime(-1.0f), m_fInterpolation(0.5f), m_fPlaybackSpeed(1.0f),
		m_fAllowMultilayerAnim(1.0f), m_nLayerID( _nLayerID), m_fLayerBlendIn(0.3f),
		m_nFlags(_nFlags), m_nUserToken(0) 
	{
		for (int i=0; i<NUM_ANIMATION_USER_DATA_SLOTS; i++)
			m_fUserData[i] = 0.0f;
	}
	// transition time between two animations.
	f32 m_fTransTime;
	// keytime[0-1]. can be used to start a transition animation.
	f32 m_fKeyTime;
	// interpolation in case we play 2 animation per layer.
	f32	m_fInterpolation; 
	// multiplier for animation-update.
	f32	m_fPlaybackSpeed; 
	// if this is '1' then we can play animation on higher layers and they overwrite the channels on lower layers. 
	f32	m_fAllowMultilayerAnim; 

	// Specify the layer where to start the animation.
	int32 m_nLayerID;
	// every layer can override the previous layer. this is the blend in/out value
	f32 m_fLayerBlendIn;
	// Combination of flags defined above.
	uint32 m_nFlags;

	// Token specified by the animation calling code for it's own benefit.
	uint32 m_nUserToken;
	// a set of weights that are blended together just like the animation is, for calling code's benefit
	f32 m_fUserData[NUM_ANIMATION_USER_DATA_SLOTS];



	void Serialize(TSerialize ser)
	{
		if (ser.GetSerializationTarget() != eST_Network)
		{
			ser.BeginGroup("CharAnimationParams");
			for (int32 i=0; i<NUM_ANIMATION_USER_DATA_SLOTS; i++)
			{
				ser.BeginGroup("UserData");
				ser.Value("fUserData", m_fUserData[i]);
				ser.EndGroup();
			}

			ser.Value("fTransTime", m_fTransTime);
			ser.Value("fKeyTime", m_fKeyTime);
			ser.Value("fInterpolation", m_fInterpolation); 
			ser.Value("fPlaybackSpeed", m_fPlaybackSpeed); 
			ser.Value("fAllowMultilayerAnim", m_fAllowMultilayerAnim); 
			ser.Value("nLayerID", m_nLayerID);
			ser.Value("fLayerBlendIn", m_fLayerBlendIn);
			ser.Value("nUserToken", m_nUserToken);
			ser.Value("nFlags", m_nFlags);
			ser.EndGroup();
		}
	}

};

struct MotionParamBlendSpace
{
	f32 m_fAssetBlend;
	f32 m_fProceduralOffset;
	f32 m_fProceduralScale;
	f32 m_fAllowPrceduralTurn;

	MotionParamBlendSpace()
	{
		m_fAssetBlend				  = 0.5f;
		m_fProceduralOffset		= 0.0f;
		m_fProceduralScale		= 1.0f;
		m_fAllowPrceduralTurn = 0.0f;
	}

};

struct MotionParam 
{
	MotionParamDesc desc;
	MotionParamBlendSpace blendspace;
	bool initialized;
	f32 value;
	f32 delta;

	MotionParam()
	{
		initialized = false;
		value = 0.0f;
		delta = 0.0f;
	}
	void Serialize(TSerialize ser)
	{
		if (ser.GetSerializationTarget() != eST_Network)
		{
			ser.BeginGroup("MotionParam");
			ser.Value("initialized", initialized);
			ser.Value("value", value);
			ser.Value("delta", delta);
			ser.EndGroup();
		}
	}

};

struct SLocoGroup
{
	int32		m_nLMGID;												//if >=0 then this is a "locomotion group"
	int32		m_nGlobalLMGID;									//if >=0 then this is a "locomotion group"

	int32		m_numAnims;											//number of used animations per locomotion group
	int32		m_nAnimID[MAX_LMG_ANIMS];
	int32		m_nGlobalID[MAX_LMG_ANIMS];
	int32		m_nSegmentCounter[MAX_LMG_ANIMS];
	f32			m_fDurationQQQ[MAX_LMG_ANIMS];		//duration of the motion0 in seconds 
	f32			m_fBlendWeight[MAX_LMG_ANIMS];		//percentage blend-value for this motion 
	f32			m_fFootPlants[MAX_LMG_ANIMS];			//footplants   1-has footplants / 0-no foot-plants 
	OldBlendSpace	m_BlendSpace;								//interpolation weights for blend-space 

	float m_fDesiredLocalLocationLookaheadTime;
	MotionParam m_params[eMotionParamID_COUNT];

	SLocoGroup()
	{
		m_nLMGID				= -3;
		m_nGlobalLMGID	= -3;
		m_numAnims			= 0;
		for (uint32 i=0; i<MAX_LMG_ANIMS; i++)
		{
			m_nAnimID[i]				= -7;
			m_nGlobalID[i]			= -7;
			m_nSegmentCounter[i]= 0;
			m_fDurationQQQ[i]		= 0.0f; 
			m_fBlendWeight[i]		= 0.0f;	 
			m_fFootPlants[i]		= 0.0f;	 
		}

		m_fDesiredLocalLocationLookaheadTime = 0.0f;
	}

	void Serialize(TSerialize ser)
	{
		if (ser.GetSerializationTarget() != eST_Network)
		{
			
			ser.BeginGroup("Params");
			for (int32 i=0; i<eMotionParamID_COUNT; i++)
				m_params[i].Serialize(ser);
			ser.EndGroup();

			ser.Value("nLMGID", m_nLMGID);
			ser.Value("nGlobalLMGID", m_nGlobalLMGID);
			ser.Value("numAnims", m_numAnims);
			for (int32 i=0; i<m_numAnims; i++)
			{
				ser.BeginGroup("ID");
				ser.Value("nAnimIDs", m_nAnimID[i]);
				ser.Value("nGlobalIDs", m_nGlobalID[i]);
				ser.Value("nSegmentCounter", m_nSegmentCounter[i]);
				ser.Value("m_fDurationQQQ", m_fDurationQQQ[i]);
				ser.EndGroup();
			}

			ser.Value("m_fDesiredLocalLocationLookaheadTime", m_fDesiredLocalLocationLookaheadTime);
		}
	}

};


struct CAnimation
{
	SLocoGroup m_LMG0;
	SLocoGroup m_LMG1;
	
	const char*	m_strAimPosName0;	// the name of the aim-pose for the first frame
	int32 m_nAnimAimID0;								
	int32 m_nGlobalAimID0;							

	const char*	m_strAimPosName1;	// the name of the aim-pose for the last frame
	int32 m_nAnimAimID1;				
	int32 m_nGlobalAimID1;

	f32		  m_fIWeight;					  //interpolation weights between blend-spaces 

	f32			m_fAnimTimePrev;        //this is a percentage value between 0-1
	f32			m_fAnimTime;						//this is a percentage value between 0-1
	f32			m_fTransitionPriority;	//motion priority: a value of '1' is overwriting all other motion in the queue
	f32			m_fTransitionWeight;		//this is the real percentage value for all active motions in the transition queue
	f32			m_fCurrentPlaybackSpeed; //here only for debugging
	f32			m_fCurrentDuration;
	f32			m_fCurrentDeltaTime;

	uint16  m_nRepeatCount;
	uint16  m_nLoopCount;
	uint16  m_nKeyPassed;
	uint16  m_bActivated;				//this anim is currently playing

	uint16  m_bRemoveFromQueue; //this anim is not need any more in the queue
	uint8		m_bTWFlag;
	uint8		m_bEndOfCycle;

	uint16  m_nSegCounter;
	uint16  m_nSegHighest;

	CryCharAnimationParams m_AnimParams;

	CAnimation()
	{
		m_strAimPosName0=0;								
		m_nAnimAimID0=-4;								
		m_nGlobalAimID0=-4;							

		m_strAimPosName1=0;							
		m_nAnimAimID1=-4;								
		m_nGlobalAimID1=-4;							

		m_fIWeight		=	0.0f;						 

		m_fAnimTimePrev					= 0.0f;
		m_fAnimTime							= 0.0f;

		m_fTransitionPriority		= 0.0f;
		m_fTransitionWeight			= -1.0f;
		m_fCurrentPlaybackSpeed	= 0.0f;
		m_fCurrentDuration			= -1.0f;
		m_fCurrentDeltaTime	    = -1.0f;

		m_nRepeatCount					= 0;
		m_nLoopCount						= 0;
		m_nKeyPassed    				= 0;
		m_bActivated						= false;

		m_bRemoveFromQueue			=	false;
		m_bTWFlag								=	false;
		m_bEndOfCycle						=	false;

		m_nSegCounter						=	0;
		m_nSegHighest           = 0xffff;

		m_AnimParams.m_nLayerID	=	0;
		m_AnimParams.m_nFlags		=	0;
	};


	void Serialize(TSerialize ser)
	{
		if (ser.GetSerializationTarget() != eST_Network)
		{
			ser.BeginGroup("CAnimation");

			ser.Value("nAnimAimID0", m_nAnimAimID0);
			ser.Value("nGlobalAimID0", m_nGlobalAimID0);

			ser.Value("nAnimAimID1", m_nAnimAimID1);
			ser.Value("nGlobalAimID1", m_nGlobalAimID1);

			ser.Value("fIWeight", m_fIWeight);

			ser.Value("fAnimTimePrev", m_fAnimTimePrev);
			ser.Value("fAnimTime", m_fAnimTime);

			ser.Value("fTransitionPriority", m_fTransitionPriority);
			ser.Value("fTransitionWeight", m_fTransitionWeight);
			ser.Value("fCurrentPlaybackSpeed", m_fCurrentPlaybackSpeed);
			ser.Value("fCurrentDuration", m_fCurrentDuration);
			ser.Value("fCurrentDeltaTime", m_fCurrentDeltaTime);

			ser.Value("nRepeatCount", m_nRepeatCount);
			ser.Value("nLoopCount", m_nLoopCount);
			ser.Value("nKeyPassed", m_nKeyPassed);
			ser.Value("bActivated", m_bActivated);

			ser.Value("bRemoveFromQueue", m_bRemoveFromQueue);
			ser.Value("m_bTWFlag",m_bTWFlag);
			ser.Value("m_bEndOfCycle",m_bEndOfCycle);

			ser.Value("nSegCounter", m_nSegCounter);
			ser.Value("nSegHighest", m_nSegHighest);
			ser.EndGroup();

			ser.BeginGroup("SLocoGroup0");
			m_LMG0.Serialize(ser);
			ser.EndGroup();

			ser.BeginGroup("SLocoGroup1");
			m_LMG1.Serialize(ser);
			ser.EndGroup();

			ser.BeginGroup("AnimParams");
			m_AnimParams.Serialize(ser);
			ser.EndGroup();
		}
	}

};

static const int ANIM_FUTURE_PATH_LOOKAHEAD = 25;


struct Joint
{
	int32 m_idxParent;
	const char* m_strJointName;
};


#endif
