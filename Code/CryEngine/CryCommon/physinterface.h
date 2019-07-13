//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//	File: physinterface.h
//	Description : declarations of all physics interfaces and structures
//
//	History:
//	-:Created by Anton Knyazev
//
//////////////////////////////////////////////////////////////////////

#ifndef physinterface_h
#define physinterface_h

#if defined(LINUX)
	//#include "Stream.h"
	//#include "validator.h"
#endif
#include <SerializeFwd.h>

#include "Cry_Geo.h"

//////////////////////////////////////////////////////////////////////////
// Physics defines.
//////////////////////////////////////////////////////////////////////////
typedef int index_t;
//////////////////////////////////////////////////////////////////////////


enum pe_type { PE_NONE=0, PE_STATIC=1, PE_RIGID=2, PE_WHEELEDVEHICLE=3, PE_LIVING=4, PE_PARTICLE=5, PE_ARTICULATED=6, PE_ROPE=7, PE_SOFT=8, PE_AREA=9 };
enum sim_class { SC_STATIC=0, SC_SLEEPING_RIGID=1, SC_ACTIVE_RIGID=2, SC_LIVING=3, SC_INDEPENDENT=4, SC_TRIGGER=6, SC_DELETED=7 };
struct IGeometry;
struct IPhysicalEntity;
struct IGeomManager;
struct IPhysicalWorld;
struct IPhysRenderer;
class ICrySizer;
struct ILog;
struct GeomQuery;
IPhysicalEntity *const WORLD_ENTITY = (IPhysicalEntity*)-10;


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// IPhysicsStreamer Interface /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

struct IPhysicsStreamer {
	virtual int CreatePhysicalEntity(void *pForeignData,int iForeignData,int iForeignFlags) = 0;
	virtual int DestroyPhysicalEntity(IPhysicalEntity *pent) = 0;
	virtual int CreatePhysicalEntitiesInBox(const Vec3 &boxMin, const Vec3 &boxMax) = 0;
	virtual int DestroyPhysicalEntitiesInBox(const Vec3 &boxMin, const Vec3 &boxMax) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// IPhysRenderer Interface /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

struct IPhysRenderer {
	virtual void DrawGeometry(IGeometry *pGeom, struct geom_world_data *pgwd, int idxColor=0, int bSlowFadein=0, const Vec3 &sweepDir=Vec3(0)) = 0;
	virtual void DrawLine(const Vec3& pt0, const Vec3& pt1, int idxColor=0, int bSlowFadein=0) = 0;
	virtual const char *GetForeignName(void *pForeignData,int iForeignData,int iForeignFlags) = 0;
	virtual void DrawText(const Vec3 &pt, const char *txt, int idxColor, float saturation=0) = 0;
};


class CMemStream { // for "fastload" serialization; hopefully it can be made global for the project
public:
	CMemStream(bool swap) {
		Prealloc(); m_iPos=0; bDeleteBuf=true; bSwapEndian=swap;
	}
	CMemStream(void *pbuf, int sz, bool swap) {
		m_pBuf=(char*)pbuf; m_nSize=sz; m_iPos=0; bDeleteBuf=false; bSwapEndian=swap;
	}
	virtual ~CMemStream() {
		if (bDeleteBuf)
			delete[] m_pBuf;
	}

	virtual void Prealloc() { m_pBuf = new char[m_nSize=0x1000]; }

	void *GetBuf() { return m_pBuf; }
	int GetUsedSize() { return m_iPos; }
	int GetAllocatedSize() { return m_nSize; }

	template<class ftype> void Write(const ftype &op) { Write(&op, sizeof(op)); }
	void Write(const void *pbuf, int sz) {
		if (m_iPos+sz>m_nSize)
			GrowBuf(sz);
		if (sz==4)
			*(int*)(m_pBuf+m_iPos) = *(int*)pbuf;
		else
			memcpy(m_pBuf+m_iPos,pbuf,(unsigned int)sz);
		m_iPos += sz;
	}

	virtual void GrowBuf(int sz) {
		int prevsz = m_nSize; char *prevbuf = m_pBuf;
		m_pBuf = new char[m_nSize = (m_iPos+sz-1 & ~0xFFF)+0x1000];
		memcpy(m_pBuf, prevbuf, (unsigned int)prevsz);
		delete[] prevbuf;
	}

	template<class ftype> void Read(ftype &op)
	{ 
		ReadRaw(&op, sizeof(op));
		#ifdef NEED_ENDIAN_SWAP
			if (bSwapEndian)
				SwapEndian(op);
		#endif
	}
	template<class ftype> ftype Read()
	{
		ftype val;
		Read(val);
		return val;
	}
	template<class ftype> void ReadType(ftype* op, int count = 1)
	{
		ReadRaw(op, sizeof(*op)*count);
		#ifdef NEED_ENDIAN_SWAP
			if (bSwapEndian) while (count-- > 0)
				SwapEndian(*op++);
		#endif
	}
	void ReadRaw(void *pbuf, int sz) 
	{
		if (sz==4)
			*(int*)pbuf = *(int*)(m_pBuf+m_iPos);
		else
			memcpy(pbuf,(m_pBuf+m_iPos),(unsigned int)sz);
		m_iPos += sz;
	}

	char *m_pBuf;
	int m_iPos,m_nSize;
	bool bDeleteBuf;
	bool bSwapEndian;
};


class unused_marker {
public:
	unused_marker() {}
	unused_marker& operator,(float &x) { *(int*)&x = 0xFFBFFFFF; return *this; }
	unused_marker& operator,(double &x) { *((int*)&x+1) = 0xFFF7FFFF; return *this; }
	unused_marker& operator,(int &x) { x=1<<31; return *this; }
	unused_marker& operator,(unsigned int &x) { x=1u<<31; return *this; }
	template<class ref> unused_marker& operator,(ref *&x) { x=(ref*)-1; return *this; }
	template<class F> unused_marker& operator,(Vec3_tpl<F> &x) { return *this,x.x; }
	template<class F> unused_marker& operator,(Quat_tpl<F> &x) { return *this,x.w; }
	template<class F> unused_marker& operator,(strided_pointer<F> &x) { return *this,x.data; }
};
inline bool is_unused(const float &x) { return (*(int*)&x & 0xFFA00000) == 0xFFA00000; }
inline bool is_unused(int x) { return x==1<<31; }
inline bool is_unused(unsigned int x) { return x==1u<<31; }
template<class ref> bool is_unused(ref *x) { return x==(ref*)-1; }
template<class ref> bool is_unused(strided_pointer<ref> x) { return is_unused(x.data); }
template<class F> bool is_unused(const Ang3_tpl<F> &x) { return is_unused(x.x); }
template<class F> bool is_unused(const Vec3_tpl<F> &x) { return is_unused(x.x); }
template<class F> bool is_unused(const Quat_tpl<F> &x) { return is_unused(x.w); }
inline bool is_unused(const double &x) { return (*((int*)&x+1) & 0xFFF40000) == 0xFFF40000; }
#define MARK_UNUSED unused_marker(),


#if !defined(VALIDATOR_LOG)
#define VALIDATOR_LOG(pLog,str)
#define VALIDATORS_START
#define VALIDATOR(member)
#define VALIDATOR_NORM(member)
#define VALIDATOR_NORM_MSG(member,msg,member1)
#define VALIDATOR_RANGE(member,minval,maxval)
#define VALIDATOR_RANGE2(member,minval,maxval)
#define VALIDATORS_END
#endif


// in physics interface [almost] all parameters are passed via structures
// this allows having stable interface methods and flexible default arguments system

////////////////////////// Params structures /////////////////////

////////// common params
struct pe_params {
	int type;
};

struct pe_params_pos : pe_params { //	Sets postion and orientation of entity
	enum entype { type_id=0 };
	pe_params_pos() {
		type=type_id; MARK_UNUSED pos,scale,q,iSimClass; pMtx3x4=0;pMtx3x3=0; bRecalcBounds=1;
	}

	Vec3 pos;
	quaternionf q;
	float scale;
	Matrix34 *pMtx3x4;
	Matrix33 *pMtx3x3;	// optional orientation via 3x3 matrix
	int iSimClass;
	int bRecalcBounds;

	VALIDATORS_START
		VALIDATOR(pos)
		VALIDATOR_NORM_MSG(q,"(perhaps non-uniform scaling was used?)",pos)
		VALIDATOR(scale)
	VALIDATORS_END
};

struct pe_params_bbox : pe_params {
	enum entype { type_id=14 };
	pe_params_bbox() { type=type_id; MARK_UNUSED BBox[0],BBox[1]; }
	Vec3 BBox[2];

	VALIDATORS_START
		VALIDATOR(BBox[0])
		VALIDATOR(BBox[1])
	VALIDATORS_END
};

// If entity represents an interior volume this allows to set outer entity, which will be skipped during tests against
// objects that are inside this entity
struct pe_params_outer_entity : pe_params {
	enum entype { type_id=9 };
	pe_params_outer_entity() { type=type_id; pOuterEntity=0; pBoundingGeometry=0; }

	IPhysicalEntity *pOuterEntity; //	outer entity for this one (outer entities can form chains)
	IGeometry *pBoundingGeometry;	// optional geometry to test containment
};

struct ITetrLattice;

struct pe_params_part : pe_params {	// Sets geometrical parameters of entity part
	enum entype { type_id=6 };
	pe_params_part() {
		type=type_id; 
		MARK_UNUSED pos,q,scale,partid,ipart,mass,density,pPhysGeom,pPhysGeomProxy,idmatBreakable,pLattice,pMatMapping,minContactDist,flagsCond;
		pMtx3x4=0;pMtx3x3=0;
		bRecalcBBox=1; bAddrefGeoms=0; flagsOR=flagsColliderOR=0; flagsAND=flagsColliderAND=(unsigned)-1;
	}

	int partid;	// partid identifier of part
	int ipart; // optionally, part slot number
	int bRecalcBBox; // whether entity's bounding box should be recalculated
	Vec3 pos;
	quaternionf q;
	float scale;
	Matrix34 *pMtx3x4;
	Matrix33 *pMtx3x3;	// optional orientation via 3x3 matrix
	unsigned int flagsCond; // if partid and ipart are not specified, check for parts with flagsCond set
	unsigned int flagsOR,flagsAND; // new flags = (flags & flagsAND) | flagsOR
	unsigned int flagsColliderOR,flagsColliderAND;
	float mass;
	float density;
	float minContactDist;
	struct phys_geometry *pPhysGeom,*pPhysGeomProxy;
	int idmatBreakable;
	ITetrLattice *pLattice;
	int *pMatMapping;
	int nMats;
	int bAddrefGeoms;

	VALIDATORS_START
		VALIDATOR(pos)
		VALIDATOR_NORM(q)
		VALIDATOR(scale)
	VALIDATORS_END
};

struct pe_params_sensors : pe_params { // Attaches optional sensors to entity (sensors raytrace enviroment around entity)
	enum entype { type_id=7 };
	pe_params_sensors() { type=type_id; nSensors=0; pOrigins=0; pDirections=0; }

	int nSensors;	// nSensors number of sensors
	const Vec3 *pOrigins; // pOrigins sensors origins in entity CS
	const Vec3 *pDirections;	// pDirections sensors directions (dir*ray length) in entity CS
};

struct pe_simulation_params : pe_params { // Sets gravity and maximum time step
	enum entype { type_id=10 };
	pe_simulation_params() { type=type_id; MARK_UNUSED maxTimeStep,gravity,minEnergy,damping,iSimClass,
		softness,softnessAngular,dampingFreefall,gravityFreefall,mass,density,maxLoggedCollisions; }

	int iSimClass;
	float maxTimeStep; // maximum time step that entity can accept (larger steps will be split)
	float minEnergy; // minimun of kinetic energy below which entity falls asleep (divided by mass!)
	float damping;
	Vec3 gravity;
	float dampingFreefall; // damping and gravity used when there are no collisions,
	Vec3 gravityFreefall; // NOTE: if left unused, gravity value will be substituted (if provided)
	float mass;
	float density;
	float softness,softnessAngular;
	float softnessGroup,softnessAngularGroup;
	int maxLoggedCollisions;
};

struct pe_params_foreign_data : pe_params {
	enum entype { type_id=11 };
	pe_params_foreign_data() { type=type_id; MARK_UNUSED pForeignData,iForeignData,iForeignFlags; iForeignFlagsAND=-1;iForeignFlagsOR=0; }

	void *pForeignData;
	int iForeignData;
	int iForeignFlags;
	int iForeignFlagsAND,iForeignFlagsOR;
};

struct pe_params_buoyancy : pe_params {
	enum entype { type_id=12 };
	pe_params_buoyancy() {
		type=type_id; iMedium=0; MARK_UNUSED waterDensity,kwaterDensity,waterDamping,
			waterPlane.n,waterPlane.origin,waterEmin,waterResistance,kwaterResistance,waterFlow,flowVariance;
	};

	float waterDensity,kwaterDensity;
	float waterDamping;
	float waterResistance,kwaterResistance;
	Vec3 waterFlow;
	float flowVariance;
	primitives::plane waterPlane;
	float waterEmin;
	int iMedium; // 0 for water, 1 for air
};

enum phentity_flags {
	particle_single_contact=0x01,particle_constant_orientation=0x02,particle_no_roll=0x04,particle_no_path_alignment=0x08,particle_no_spin=0x10,
	lef_push_objects=0x01, lef_push_players=0x02,	lef_snap_velocities=0x04,	lef_loosen_stuck_checks=0x08, lef_report_sliding_contacts=0x10,
	rope_findiff_attached_vel=0x01, rope_no_solver=0x02, rope_ignore_attachments=0x4,	rope_target_vtx_rel0=0x08, rope_target_vtx_rel1=0x10,
	rope_subdivide_segs=0x100,
	se_skip_longest_edges=0x01,
	ref_use_simple_solver=0x01,	ref_no_splashes=0x04, ref_checksum_received=0x04, ref_checksum_outofsync=0x08,
	aef_recorded_physics = 0x02,
	wwef_fake_inner_wheels = 0x10,
	pef_disabled=0x20, pef_never_break=0x40, pef_deforming=0x80, pef_pushable_by_players=0x200,	
	pef_traceable=0x400, particle_traceable=0x400, rope_traceable=0x400, pef_update=0x800,
	pef_monitor_state_changes=0x1000, pef_monitor_collisions=0x2000, pef_monitor_env_changes=0x4000, pef_never_affect_triggers=0x8000,
	pef_invisible=0x10000, pef_ignore_ocean=0x20000, pef_fixed_damping=0x40000,	pef_custom_poststep=0x80000, pef_monitor_poststep=0x80000,
	pef_always_notify_on_deletion=0x100000,
	rope_collides=0x200000, rope_collides_with_terrain=0x400000, rope_no_stiffness_when_colliding=0x10000000,
	pef_override_impulse_scale=0x200000, pef_players_can_break=0x400000, pef_cannot_squash_players=0x10000000,
	pef_ignore_areas=0x800000,
	pef_log_state_changes=0x1000000, pef_log_collisions=0x2000000, pef_log_env_changes=0x4000000, pef_log_poststep=0x8000000,
};

struct pe_params_flags : pe_params {
	enum entype { type_id=15 };
	pe_params_flags() { type=type_id; MARK_UNUSED flags,flagsOR,flagsAND; }
	unsigned int flags;
	unsigned int flagsOR;
	unsigned int flagsAND;
};

struct pe_params_ground_plane : pe_params {
	enum entype { type_id=20 };
	pe_params_ground_plane() { type=type_id; iPlane=0; MARK_UNUSED ground.origin,ground.n; }
	int iPlane; // index of the plane to be set (-1 removes existing planes)
	primitives::plane ground;
};

struct pe_params_structural_joint : pe_params {
	enum entype { type_id=21 };
	pe_params_structural_joint() { 
		type=type_id; id=0; bReplaceExisting=0; 
		MARK_UNUSED idx,partid[0],partid[1],pt,n,maxForcePush,maxForcePull,maxForceShift,maxTorqueBend,maxTorqueTwist,
			bBreakable,szSensor,bBroken,partidEpicenter;
	}

	int id,idx;
	int bReplaceExisting;
	int partid[2];
	Vec3 pt;
	Vec3 n;
	float maxForcePush,maxForcePull,maxForceShift;
	float maxTorqueBend,maxTorqueTwist;
	int bBreakable;
	float szSensor;
	int bBroken;
	int partidEpicenter;
};

struct pe_params_timeout : pe_params {
	enum entype { type_id=23 };
	pe_params_timeout() { type=type_id; MARK_UNUSED timeIdle,maxTimeIdle; }
	float timeIdle,maxTimeIdle;
};


////////// articulated entity params
enum joint_flags { angle0_locked=1, all_angles_locked=7, angle0_limit_reached=010, angle0_auto_kd=0100, joint_no_gravity=01000,
									 joint_isolated_accelerations=02000, joint_expand_hinge=04000, angle0_gimbal_locked=010000, 
									 joint_dashpot_reached=0100000, joint_ignore_impulses=0200000 };

struct pe_params_joint : pe_params {
	enum entype { type_id=5 };
	pe_params_joint() {
		type=type_id;
		for(int i=0;i<3;i++)
			MARK_UNUSED limits[0][i],limits[1][i],qdashpot[i],kdashpot[i],bounciness[i],q[i],qext[i],ks[i],kd[i],qtarget[i];
		bNoUpdate=0; pMtx0=0; flagsPivot=3;
		MARK_UNUSED flags,q0,pivot,ranimationTimeStep,nSelfCollidingParts,animationTimeStep;
	}

	unsigned int flags; // should be a combination of angle0,1,2_locked, angle0,1,2_auto_kd, joint_no_gravity
	int flagsPivot; // if bit 0 is set, update pivot point in parent frame, if bit 1 - in child
	Vec3 pivot; // joint pivot in entity CS
	quaternionf q0;	// orientation of child in parent coordinates that corresponds to angles (0,0,0)
	Matrix33 *pMtx0; // same as 3x3 row major matrix
	Vec3 limits[2];	// limits for each angle
	Vec3 bounciness; // bounciness for each angle (applied when limit is reached)
	Vec3 ks,kd; // stiffness and damping koefficients for each angle angular spring
	Vec3 qdashpot; // limit vicinity where joints starts resisting movement
	Vec3 kdashpot; // when dashpot is activated, this is roughly the angular speed, stopped in 2 sec
	Ang3 q;	// angles values
	Ang3 qext; // additional angles values (angle[i] = q[i]+qext[i]; only q[i] is taken into account
								 // while calculating spring torque
	Ang3 qtarget; 
	int op[2]; // body identifiers of parent and child respectively
	int nSelfCollidingParts,*pSelfCollidingParts; // part ids of only parts that should be checked for self-collision
	int bNoUpdate; // omit recalculation of body parameters after changing this joint
	float animationTimeStep; // used to calculate joint velocities of animation
	float ranimationTimeStep;	// 1/animation time step, can be not specified (specifying just saves extra division operation)

	VALIDATORS_START
		VALIDATOR(pivot)
		VALIDATOR_NORM(q0)
		VALIDATOR(q)
		VALIDATOR(qext)
	VALIDATORS_END
};

struct pe_params_articulated_body : pe_params {
	enum entype { type_id=8 };
	pe_params_articulated_body() {
		type=type_id;
		MARK_UNUSED bGrounded,bInheritVel,bCheckCollisions,bCollisionResp,bExpandHinges;
		MARK_UNUSED bGrounded,bInheritVel,bCheckCollisions,bCollisionResp, a,wa,w,v,pivot, scaleBounceResponse,posHostPivot,qHostPivot;
		MARK_UNUSED bAwake,pHost,nCollLyingMode, gravityLyingMode,dampingLyingMode,minEnergyLyingMode,iSimType,iSimTypeLyingMode,nRoots;
		bApply_dqext=0;	bRecalcJoints=1;
	}

	int bGrounded; // whether body's pivot is firmly attached to something or free
	int bCheckCollisions;
	int bCollisionResp;
	Vec3 pivot; // attachment position for grounded bodies
	Vec3 a; // acceleration of ground for grounded bodies
	Vec3 wa; // angular acceleration of ground for grounded bodies
	Vec3 w; // angular velocity of ground for grounded bodies
	Vec3 v;
	float scaleBounceResponse; // scales impulsive torque that is applied at a joint that has just reached its limit
	int bApply_dqext;	// adds current dqext to joints velocities. dqext is the speed of external animation and is calculated each time
										// qext is set for joint (as difference between new value and current value, multiplied by inverse of animation timestep)
	int bAwake;

	IPhysicalEntity *pHost;
	Vec3 posHostPivot;
	quaternionf qHostPivot;
	int bInheritVel;

	int nCollLyingMode;
	Vec3 gravityLyingMode;
	float dampingLyingMode;
	float minEnergyLyingMode;
	int iSimType;
	int iSimTypeLyingMode;
	int bExpandHinges;
	int nRoots; // only used in GetParams

	int bRecalcJoints;
};

////////// living entity params

struct pe_player_dimensions : pe_params {
	enum entype { type_id=1 };
	pe_player_dimensions() : dirUnproj(0,0,1),maxUnproj(0) { 
		type=type_id; MARK_UNUSED sizeCollider,heightPivot,heightCollider,heightEye,heightHead,headRadius,bUseCapsule; 
	}

	float heightPivot; // offset from central ground position that is considered entity center
	float heightEye; // vertical offset of camera
	Vec3 sizeCollider; // collision cylinder dimensions
	float heightCollider;	// vertical offset of collision geometry center
	float headRadius;
	float heightHead;
	Vec3 dirUnproj;
	float maxUnproj;
	int bUseCapsule;

	VALIDATORS_START
		VALIDATOR(heightPivot)
		VALIDATOR(heightEye)
		VALIDATOR_RANGE2(sizeCollider,0,100)
	VALIDATORS_END
};

struct pe_player_dynamics : pe_params {
	enum entype { type_id=4 };
	pe_player_dynamics() {
		type=type_id; MARK_UNUSED kInertia,kInertiaAccel,kAirControl,gravity,gravity.z,nodSpeed,mass, bSwimming,surface_idx,bActive,collTypes;
		MARK_UNUSED minSlideAngle,maxClimbAngle,maxJumpAngle,minFallAngle,kAirResistance,bNetwork,maxVelGround,timeImpulseRecover,iRequestedTime; }

	float kInertia;	// inertia koefficient, the more it is, the less inertia is; 0 means no inertia
	float kInertiaAccel; // inertia on acceleration
	float kAirControl; // air control koefficient 0..1, 1 - special value (total control of movement)
	float kAirResistance;
	Vec3 gravity; // gravity vector
	float nodSpeed;
	int bSwimming; // whether entity is swimming (is not bound to ground plane)
	float mass;	// mass (in kg)
	int surface_idx; // surface identifier for collisions
	float minSlideAngle; // if surface slope is more than this angle, player starts sliding (angle is in radians)
	float maxClimbAngle; // player cannot climb surface which slope is steeper than this angle
	float maxJumpAngle; // player is not allowed to jump towards ground if this angle is exceeded
	float minFallAngle;	// player starts falling when slope is steeper than this
	float maxVelGround; // player cannot stand of surfaces that are moving faster than this
	float timeImpulseRecover; // forcefully turns on inertia for that duration after receiving an impulse
	int collTypes; // entity types to check collisions against
	int bNetwork;
	int bActive;
	int iRequestedTime; // requests that the player rolls back to that time and re-exucutes pending actions during the next step
};

////////// particle entity params

struct pe_params_particle : pe_params {
	enum entype { type_id=3 };
	pe_params_particle() {
		type=type_id;
		MARK_UNUSED mass,size,thickness,wspin,accThrust,kAirResistance,kWaterResistance, velocity,heading,accLift,accThrust,gravity,waterGravity;
		MARK_UNUSED surface_idx, normal,q0,minBounceVel, flags,pColliderToIgnore, iPierceability, areaCheckPeriod;
	}

	unsigned int flags; // see entity flags
	float mass;
	float size; // pseudo-radius
	float thickness; // thickness when lying on a surface (if left unused, size will be used)
	Vec3 heading; // direction of movement
	float velocity;	// velocity along "heading"
	float kAirResistance; // air resistance koefficient, F = kv
	float kWaterResistance; // same for water
	float accThrust; // acceleration along direction of movement
	float accLift; // acceleration that lifts particle with the current speed
	int surface_idx;
	Vec3 wspin; // angular velocity
	Vec3 gravity;
	Vec3 waterGravity;
	Vec3 normal;
	quaternionf q0;	// initial orientation (zero means x along direction of movement, z up)
	float minBounceVel;
	IPhysicalEntity *pColliderToIgnore;	// physical entity to ignore during collisions
	int iPierceability;
	int areaCheckPeriod; // how often (in frames) world area checks are made

	VALIDATORS_START
		VALIDATOR(mass)
		VALIDATOR(size)
		VALIDATOR(thickness)
		VALIDATOR_NORM(heading)
		VALIDATOR_NORM(normal)
		VALIDATOR_NORM(q0)
	VALIDATORS_END
};

////////// vehicle entity params

struct pe_params_car : pe_params {
	enum entype { type_id=2 };
	pe_params_car() {
		type=type_id;
		MARK_UNUSED engineMaxRPM,iIntegrationType,axleFriction,enginePower,maxSteer,maxTimeStep,minEnergy,damping,brakeTorque;
		MARK_UNUSED engineMinRPM,engineShiftUpRPM,engineShiftDownRPM,engineIdleRPM,engineStartRPM,clutchSpeed,nGears,gearRatios,kStabilizer;
		MARK_UNUSED slipThreshold,gearDirSwitchRPM,kDynFriction,minBrakingFriction,maxBrakingFriction,steerTrackNeutralTurn,maxGear,minGear,pullTilt;
	}

	float axleFriction; // friction torque at axes divided by mass of vehicle
	float enginePower; // power of engine (about 10,000 - 100,000)
	float maxSteer;	// maximum steering angle
	float engineMaxRPM;	// engine torque decreases to 0 after reaching this rotation speed
	float brakeTorque;
	int iIntegrationType; // for suspensions; 0-explicit Euler, 1-implicit Euler
	float maxTimeStep; // maximum time step when vehicle has only wheel contacts
	float minEnergy; // minimum awake energy when vehicle has only wheel contacts
	float damping; // damping when vehicle has only wheel contacts
  float minBrakingFriction; // limits the the tire friction when handbraked
	float maxBrakingFriction; // limits the the tire friction when handbraked
	float kStabilizer; // stabilizer force, as a multiplier for kStiffness of respective suspensions
	int nWheels; // the number of wheels
	float engineMinRPM;
	float engineShiftUpRPM;
	float engineShiftDownRPM;
	float engineIdleRPM;
	float engineStartRPM;
	float clutchSpeed;
	int nGears;
	float *gearRatios;
	int maxGear,minGear;
	float slipThreshold;
	float gearDirSwitchRPM;
	float kDynFriction;
	float steerTrackNeutralTurn;
  float pullTilt; // tilt angle of pulling force towards ground [rad]	
};

struct pe_params_wheel : pe_params {
	enum entype { type_id=16 };
	pe_params_wheel() {
		type=type_id; iWheel=0; MARK_UNUSED bDriving,iAxle,suspLenMax,suspLenInitial,minFriction,maxFriction,surface_idx,bCanBrake,bBlocked,
			bRayCast,kStiffness,kDamping,kLatFriction,Tscale;
	}

	int iWheel;
	int bDriving;
	int iAxle;
	int bCanBrake;
  int bBlocked;
	float suspLenMax;
	float suspLenInitial;
	float minFriction;
	float maxFriction;
	int surface_idx;
	int bRayCast;
	float kStiffness;
	float kDamping;
  float kLatFriction;
	float Tscale;
};

////////// rope entity params

struct pe_params_rope : pe_params {
	enum entype { type_id=13 };
	pe_params_rope() {
		type=type_id; MARK_UNUSED length,mass,bCheckCollisions,collDist,surface_idx,friction,nSegments,pPoints.data,pVelocities.data;
		MARK_UNUSED pEntTiedTo[0],ptTiedTo[0],idPartTiedTo[0],pEntTiedTo[1],ptTiedTo[1],idPartTiedTo[1],stiffnessAnim,maxForce,
			flagsCollider,nMaxSubVtx,stiffnessDecayAnim,dampingAnim,bTargetPoseActive,wind,windVariance,airResistance,waterResistance,density,
			jointLimit,sensorRadius,frictionPull,stiffness,collisionBBox[0];
		bLocalPtTied = 0;
	}

	float length;
	float mass;
	int bCheckCollisions;
	float collDist;
	int surface_idx;
	float friction;
	float frictionPull;
	float stiffness;
	float stiffnessAnim;
	float stiffnessDecayAnim;
	float dampingAnim;
	int bTargetPoseActive;
	Vec3 wind;
	float windVariance;
	float airResistance;
	float waterResistance;
	float density;
	float jointLimit;
	float sensorRadius;
	float maxForce;
	int nSegments;
	int flagsCollider;
	int nMaxSubVtx;
	Vec3 collisionBBox[2];
	strided_pointer<Vec3> pPoints;
	strided_pointer<Vec3> pVelocities;

	IPhysicalEntity *pEntTiedTo[2];
	int bLocalPtTied;
	Vec3 ptTiedTo[2];
	int idPartTiedTo[2];
};

////////// soft entity params

struct pe_params_softbody : pe_params {
	enum entype { type_id=17 };
	pe_params_softbody() { type=type_id; MARK_UNUSED thickness,maxSafeStep,ks,kdRatio,airResistance,wind,windVariance,nMaxIters,
		accuracy,friction,impulseScale,explosionScale,collisionImpulseScale,maxCollisionImpulse,collTypes,waterResistance,massDecay; }

	float thickness;
	float maxSafeStep;
	float ks;
	float kdRatio;
	float friction;
	float waterResistance;
	float airResistance;
	Vec3 wind;
	float windVariance;
	int nMaxIters;
	float accuracy;
	float impulseScale;
	float explosionScale;
	float collisionImpulseScale;
	float maxCollisionImpulse;
	int collTypes;
	float massDecay;
};

/////////// area params

struct pe_params_area : pe_params {
	enum entype { type_id=18 };
	pe_params_area() { type=type_id; MARK_UNUSED gravity,size,bUniform,damping,falloff0,bUseCallback,pGeom; }

	Vec3 gravity;
	Vec3 size; // ellipsoidal falloff dimensions; 0,0,0 if no falloff
	float falloff0;
	int bUniform; // same direction in every point or always point to the center
	int bUseCallback;
	float damping;
	IGeometry *pGeom;
};


////////// water manager params

struct pe_params_waterman : pe_params {
	enum entype { type_id=22 };
	pe_params_waterman() {
		type=type_id; MARK_UNUSED posViewer,nExtraTiles,nCells,tileSize,timeStep,waveSpeed,
			dampingCenter,dampingRim,minhSpread,minVel;
	}

	Vec3 posViewer;
	int nExtraTiles;
	int nCells;
	float tileSize;
	float timeStep;
	float waveSpeed;
	float dampingCenter;
	float dampingRim;
	float minhSpread;
	float minVel;
};


////////////////////////// Action structures /////////////////////

////////// common actions
struct pe_action {
	int type;
};

struct pe_action_impulse : pe_action {
	enum entype { type_id=2 };
	pe_action_impulse() { type=type_id; impulse.Set(0,0,0); MARK_UNUSED point,angImpulse,partid,ipart; iApplyTime=2; iSource=0; }

	Vec3 impulse;
	Vec3 angImpulse;	// optional
	Vec3 point; // point of application, in world CS, optional
	int partid;	// receiver part identifier
	int ipart; // alternatively, part index can be used
	int iApplyTime; // 0-apply immediately, 1-apply before the next time step, 2-apply after the next time step
	int iSource; // reserved for internal use

	VALIDATORS_START
		VALIDATOR_RANGE2(impulse,0,1E8f)
		VALIDATOR_RANGE2(angImpulse,0,1E8f)
		VALIDATOR_RANGE2(point,0,1E6f)
		VALIDATOR_RANGE(ipart,0,10000)
	VALIDATORS_END
};

struct pe_action_reset : pe_action { // Resets dynamic state of an entity
	enum entype { type_id=4 };
	pe_action_reset() { type=type_id; bClearContacts=1; }
	int bClearContacts;
};

enum constrflags { local_frames=1, world_frames=2, constraint_inactive=0x100, constraint_ignore_buddy=0x200 };

struct pe_action_add_constraint : pe_action {
	enum entype { type_id=5 };
	pe_action_add_constraint() {
		type=type_id; pBuddy=0; flags=world_frames;
		MARK_UNUSED id,pt[0],pt[1],partid[0],partid[1],qframe[0],qframe[1],xlimits[0],yzlimits[0],
			pConstraintEntity,damping,sensorRadius,maxPullForce,maxBendTorque; 
	}
	int id;
	IPhysicalEntity *pBuddy;
	Vec3 pt[2];
	int partid[2];
	quaternionf qframe[2];
	float xlimits[2];
	float yzlimits[2];
	unsigned int flags;
	float damping;
	float sensorRadius;
	float maxPullForce,maxBendTorque;
	IPhysicalEntity *pConstraintEntity;
};

struct pe_action_update_constraint : pe_action {
	enum entype { type_id=6 };
	pe_action_update_constraint() { type=type_id; MARK_UNUSED idConstraint,pt[0],pt[1]; flagsOR=0;flagsAND=(unsigned int)-1;bRemove=0; flags=world_frames; }
	int idConstraint;
	unsigned int flagsOR;
	unsigned int flagsAND;
	int bRemove;
	Vec3 pt[2];
	int flags;
};

struct pe_action_register_coll_event : pe_action {
	enum entype { type_id=7 };
	pe_action_register_coll_event() { type=type_id; MARK_UNUSED vSelf; }

	Vec3 pt;
	Vec3 n;
	Vec3 v, vSelf;
	float collMass;
	IPhysicalEntity *pCollider;
	int partid[2];
	int idmat[2];
};

struct pe_action_awake : pe_action {
	enum entype { type_id=8 };
	pe_action_awake() { type=type_id; bAwake=1; }
	int bAwake;
};

struct pe_action_remove_all_parts : pe_action {
	enum entype { type_id=9 };
	pe_action_remove_all_parts() { type=type_id; }
};

struct pe_action_reset_part_mtx : pe_action {
	enum entype { type_id=13 };
	pe_action_reset_part_mtx() { type=type_id; MARK_UNUSED ipart,partid; }
	int ipart;
	int partid;
};

struct pe_action_set_velocity : pe_action {
	enum entype { type_id=10 };
	pe_action_set_velocity() { type=type_id; MARK_UNUSED ipart,partid,v,w; }
	int ipart;
	int partid;
	Vec3 v,w;

	VALIDATORS_START
		VALIDATOR_RANGE2(v,0,1E5f)
		VALIDATOR_RANGE2(w,0,1E5f)
	VALIDATORS_END
};

struct pe_action_notify : pe_action {
	enum entype { type_id=14 };
	enum encodes { ParentChange=0 };
	pe_action_notify() { type=type_id; iCode=ParentChange; }
	int iCode;
};

struct pe_action_auto_part_detachment : pe_action {
	enum entype { type_id=15 };
	pe_action_auto_part_detachment() { type=type_id; MARK_UNUSED threshold,autoDetachmentDist; }
	float threshold;
	float autoDetachmentDist;
};

struct pe_action_move_parts : pe_action {
	enum entype { type_id=16 };
	int idStart,idEnd;
	int idOffset;
	IPhysicalEntity *pTarget;
	Matrix34 mtxRel;
	pe_action_move_parts() { type=type_id; idStart=0; idEnd=1<<30; idOffset=0; mtxRel.SetIdentity(); pTarget=0; }
};

////////// living entity actions

struct pe_action_move : pe_action { // movement request for living entities
	enum entype { type_id=1 };
	pe_action_move() { type=type_id; iJump=0; dt=0; MARK_UNUSED dir; }

	Vec3 dir; // dir requested velocity vector
	int iJump; // jump mode - 1-instant velocity change, 2-just adds to current velocity
	float dt;	// time interval for this action

	VALIDATORS_START
		VALIDATOR_RANGE2(dir,0,1000)
		VALIDATOR_RANGE(dt,0,2)
	VALIDATORS_END
};

////////// vehicle entity actions

struct pe_action_drive : pe_action {
	enum entype { type_id=3 };
	pe_action_drive() { type=type_id; MARK_UNUSED pedal,dpedal,steer,dsteer,bHandBrake,clutch,iGear; }

	float pedal; // engine pedal absolute value
	float dpedal; // engine pedal delta
	float steer; // steering angle absolute value
	float dsteer; // steering angle delta
	float clutch;
	int bHandBrake;
	int iGear;
};

////////// rope entity actions

struct pe_action_target_vtx : pe_action {
	enum entype { type_id=12 };
	pe_action_target_vtx() { type=type_id; MARK_UNUSED points,nPoints; }

	int nPoints;
	Vec3 *points;
};

////////// soft entity actions

struct pe_action_attach_points : pe_action {
	enum entype { type_id=11 };
	pe_action_attach_points() { type=type_id; MARK_UNUSED partid,points; nPoints=1; pEntity=WORLD_ENTITY; }

	IPhysicalEntity *pEntity;
	int partid;
	int *piVtx;
	Vec3 *points;
	int nPoints;
};

////////////////////////// Status structures /////////////////////

////////// common statuses
struct pe_status {
	int type;
};

enum status_pos_flags { status_local=1,status_thread_safe=2,status_addref_geoms=4 };

struct pe_status_pos : pe_status {
	enum entype { type_id=1 };
	pe_status_pos() { type=type_id; ipart=partid=-1; flags=0; pMtx3x4=0;pMtx3x3=0; iSimClass=0; timeBack=0; }

	int partid; // part identifier, -1 for entire entity
	int ipart; // optionally, part slot index
	unsigned int flags; // status_local if part coordinates should be returned in entity CS rather than world CS
	unsigned int flagsOR; // boolean OR for all parts flags of the object (or just flags for the selected part)
	unsigned int flagsAND; // boolean AND for all parts flags of the object (or just flags for the selected part)
	Vec3 pos; // position of center
	Vec3 BBox[2]; // bounding box relative to pos (bbox[0]-min, bbox[1]-max)
	quaternionf q;
	float scale;
	int iSimClass;
	Matrix34* pMtx3x4;
	Matrix33* pMtx3x3;	// optional 3x3 matrix buffer that receives transformation
	IGeometry *pGeom,*pGeomProxy;
	float timeBack; // can retrieve pervious position; only supported by rigid entities; pos and q; one step back
};

struct pe_status_extent : pe_status {	// Caches eForm extent of entity in pGeo
	enum entype { type_id=24 };
	pe_status_extent() { type=type_id; eForm=EGeomForm(-1); pGeo=0; }

	EGeomForm eForm;
	GeomQuery* pGeo;
};

struct pe_status_random : pe_status_extent {	// Generates random pos on entity, also caches extent
	enum entype { type_id=25 };
	pe_status_random() { type=type_id; ran.vPos.zero(); ran.vNorm.zero(); }
	RandomPos ran;
};

struct pe_status_sensors : pe_status { // Requests status of attached to the entity sensors
	enum entype { type_id=18 };
	pe_status_sensors() { type=type_id; }

	Vec3 *pPoints;	// pointer to array of points where sensors touch environment (assigned by physical entity)
	Vec3 *pNormals; // pointer to array of surface normals at points where sensors touch environment
	unsigned int flags; // bitmask of flags, bit==1 - sensor touched environment
};

struct pe_status_dynamics : pe_status {
	enum entype { type_id=8 };
	pe_status_dynamics() : v(ZERO),w(ZERO),a(ZERO),wa(ZERO),centerOfMass(ZERO) {
		MARK_UNUSED partid,ipart; type=type_id; mass=energy=0; nContacts=0; time_interval=0; submergedFraction=0; 
	}

	int partid;
	int ipart;
	Vec3 v; // velocity
	Vec3 w; // angular velocity
	Vec3 a; // linear acceleration
	Vec3 wa; // angular acceleration
	Vec3 centerOfMass;
	float submergedFraction;
	float mass;
	float energy;
	int nContacts;
	float time_interval;
};

struct coll_history_item {
	Vec3 pt; // collision area center
	Vec3 n; // collision normal in entity CS
	Vec3 v[2]; // velocities of contacting bodies at the point of impact
	float mass[2]; // masses of contacting bodies
	float age; // age of collision event
	int idCollider; // id of collider (not a pointer, since collider can be destroyed before history item is queried)
	int partid[2];
	int idmat[2];	// 0-this body material, 1-collider material
};

struct pe_status_collisions : pe_status {
	enum entype { type_id=9 };
	pe_status_collisions() { type=type_id; age=0; len=1; pHistory=0; bClearHistory=0; }

	coll_history_item *pHistory; // pointer to a user-provided array of history items
	int len; // length of this array
	float age; // maximum age of collision events (older events are ignored)
	int bClearHistory;
};

struct pe_status_id : pe_status {
	enum entype { type_id=10 };
	pe_status_id() { type=type_id; ipart=partid=-1; bUseProxy=1; }

	int ipart;
	int partid;
	int iPrim;
	int iFeature;
	int bUseProxy;
	int id; // usually id means material
};

struct pe_status_timeslices : pe_status {
	enum entype { type_id=11 };
	pe_status_timeslices() { type=type_id; pTimeSlices=0; sz=1; precision=0.0001f; MARK_UNUSED time_interval; }

	float *pTimeSlices;
	int sz;
	float precision; // time surplus below this threshhold will be discarded
	float time_interval; // if unused, time elapsed since the last action will be used
};

struct pe_status_nparts : pe_status {
	enum entype { type_id=12 };
	pe_status_nparts() { type=type_id; }
};

struct pe_status_awake : pe_status {
	enum entype { type_id=7 };
	pe_status_awake() { type=type_id; lag=0; }
	int lag;
};

struct pe_status_contains_point : pe_status {
	enum entype { type_id=13 };
	pe_status_contains_point() { type=type_id; }
	Vec3 pt;
};

struct pe_status_placeholder : pe_status {
	enum entype { type_id=16 };
	pe_status_placeholder() { type=type_id; }
	IPhysicalEntity *pFullEntity;
};

struct pe_status_sample_contact_area : pe_status {
	enum entype { type_id=19 };
	pe_status_sample_contact_area() { type=type_id; }
	Vec3 ptTest;
	Vec3 dirTest;
};

struct pe_status_caps : pe_status {
	enum entype { type_id=20 };
	pe_status_caps() { type=type_id; }
	unsigned int bCanAlterOrientation; // can change orientation that is explicitly set from outside
};

struct pe_status_constraint : pe_status {
	enum entype { type_id=26 };
	pe_status_constraint() { type=type_id; }
	int id;
	int flags;
	Vec3 pt[2];
	Vec3 n;
};

////////// area status

struct pe_status_area : pe_status {
	enum entype { type_id=23 };
	pe_status_area() { type=type_id; bUniformOnly=false; ctr.zero(); size.zero(); vel.zero(); MARK_UNUSED gravity, pb; pLockUpdate=0; }

	// inputs.
	Vec3 ctr, size;							// query bounds
	Vec3 vel;	
	bool bUniformOnly;

	// outputs.
	Vec3 gravity;
	pe_params_buoyancy pb;
	volatile int *pLockUpdate;
};

////////// living entity statuses

struct pe_status_living : pe_status {
	enum entype { type_id=2 };
	pe_status_living() { type=type_id; }

	int bFlying; // whether entity has no contact with ground
	float timeFlying; // for how long the entity was flying
	Vec3 camOffset; // camera offset
	Vec3 vel; // actual velocity (as rate of position change)
	Vec3 velUnconstrained; // 'physical' movement velocity
	Vec3 velRequested;	// velocity requested in the last action
	Vec3 velGround; // velocity of the object entity is standing on
	float groundHeight; // position where the last contact with the ground occured
	Vec3 groundSlope;
	int groundSurfaceIdx;
	int groundSurfaceIdxAux;
	IPhysicalEntity *pGroundCollider;
	int iGroundColliderPart;
	float timeSinceStanceChange;
	int bOnStairs;
	int bStuck;
	volatile int *pLockStep;
	int iCurTime;
	int bSquashed;
};

struct pe_status_check_stance : pe_status {
	enum entype { type_id=21 };
	pe_status_check_stance() : dirUnproj(0,0,1),unproj(0) { type=type_id; MARK_UNUSED pos,q,sizeCollider,heightCollider,bUseCapsule; }

	Vec3 pos;
	quaternionf q;
	Vec3 sizeCollider;
	float heightCollider;
	Vec3 dirUnproj;
	float unproj;
	int bUseCapsule;
};

////////// vehicle entity statuses

struct pe_status_vehicle : pe_status {
	enum entype { type_id=4 };
	pe_status_vehicle() { type=type_id; }

	float steer; // current steering angle
	float pedal; // current engine pedal
	int bHandBrake;	// nonzero if handbrake is on
	float footbrake; // nonzero if footbrake is pressed (range 0..1)
	Vec3 vel;
	int bWheelContact; // nonzero if at least one wheel touches ground
	int iCurGear;
	float engineRPM;
	float clutch;
  float drivingTorque;
	int nActiveColliders;
};

struct pe_status_wheel : pe_status {
	enum entype { type_id=5 };
	pe_status_wheel() { type=type_id; iWheel=0; MARK_UNUSED partid; }
	int iWheel;
	int partid;

	int bContact;	// nonzero if wheel touches ground
	Vec3 ptContact; // point where wheel touches ground
	float w; // rotation speed
	int bSlip;
	Vec3 velSlip; // slip velocity
	int contactSurfaceIdx;  
  float friction; // current friction applied
	float suspLen; // current suspension spring length
	float suspLenFull; // relaxed suspension spring length
	float suspLen0; // initial suspension spring length
	float r; // wheel radius  
  float torque; // driving torque
  IPhysicalEntity* pCollider;
};

struct pe_status_vehicle_abilities : pe_status {
	enum entype { type_id=15 };
	pe_status_vehicle_abilities() { type=type_id; MARK_UNUSED steer; }

	float steer; // should be set to requested steering angle
	Vec3 rotPivot;	// returns turning circle center
	float maxVelocity; // calculates maximum velocity of forward movement along a plane (steer is ignored)
};

////////// articulated entity statuses

struct pe_status_joint : pe_status {
	enum entype { type_id=6 };
	pe_status_joint() { type=type_id; MARK_UNUSED partid,idChildBody; }

	int idChildBody; // requested joint is identified by child body id
	int partid; // ..or, alternatively, by any of parts that belong to it
	unsigned int flags; // joint flags
	Ang3 q;	// current joint angles (controlled by physics)
	Ang3 qext; // external angles (from animation)
	Ang3 dq; // current joint angular velocities
	quaternionf quat0;
};

////////// rope entity statuses

struct pe_status_rope : pe_status {
	enum entype { type_id=14 };
	pe_status_rope() { type=type_id; pPoints=pVelocities=pVtx=pContactNorms=0; nCollStat=nCollDyn=bTargetPoseActive=0; 
		pContactEnts=0; stiffnessAnim=timeLastActive=0; nVtx=0; }

	int nSegments;
	Vec3 *pPoints;
	Vec3 *pVelocities;
	int nCollStat,nCollDyn;
	int bTargetPoseActive;
	float stiffnessAnim;
	strided_pointer<IPhysicalEntity*> pContactEnts;
	int nVtx;
	Vec3 *pVtx;
	Vec3 *pContactNorms;
	float timeLastActive;
};

////////// soft entity statuses

enum ESSVFlags { eSSV_LockPos=1, eSSV_UnlockPos=2 };

struct pe_status_softvtx : pe_status {
	enum entype { type_id=17 };
	pe_status_softvtx() { type=type_id; pVtx=pNormals=0; pVtxMap=0; flags=0; }

	int nVtx;
	strided_pointer<Vec3> pVtx;
	strided_pointer<Vec3> pNormals;
	int *pVtxMap;
	int flags;
};

////////// waterman statuses

struct SWaterTileBase {
	int bActive;
	float *ph;
	float *pvel;
};

struct pe_status_waterman : pe_status {
	enum entype { type_id=22 };
	pe_status_waterman() { type=type_id; }

	int bActive;
	Matrix33 R;
	Vec3 origin;
	int nTiles,nCells; // number of tiles and cells in one dimension
	SWaterTileBase **pTiles; // nTiles^2 entries
};

////////////////////////// Geometry structures /////////////////////

////////// common geometries
enum geom_flags { geom_colltype0=0x0001, geom_colltype1=0x0002, geom_colltype2=0x0004, geom_colltype3=0x0008, geom_colltype4=0x0010,
									geom_colltype5=0x0020, geom_colltype6=0x0040, geom_colltype7=0x0080, geom_colltype8=0x0100, geom_colltype9=0x0200,
									geom_colltype10=0x0400,geom_colltype11=0x0800,geom_colltype12=0x1000,geom_colltype13=0x2000,geom_colltype14=0x4000,
									geom_colltype_ray=0x8000, geom_floats=0x10000, 
									geom_proxy=0x20000, geom_structure_changes=0x40000, geom_can_modify=0x80000, 
									geom_squashy=0x100000, geom_log_interactions=0x200000,
									geom_monitor_contacts=0x400000, geom_manually_breakable=0x800000,
									geom_no_coll_response=0x1000000, geom_mat_substitutor=0x2000000,
									geom_break_approximation=0x4000000,
									// mnemonic group names
									geom_colltype_player=geom_colltype1, geom_colltype_explosion=geom_colltype2, 
									geom_colltype_vehicle=geom_colltype3, geom_colltype_foliage=geom_colltype4, geom_colltype_debris=geom_colltype5, 
									geom_colltype_foliage_proxy=geom_colltype13, geom_colltype_obstruct=geom_colltype14,
									geom_colltype_solid=0x0FFF&~geom_colltype_explosion, geom_collides=0xFFFF
								};

struct pe_geomparams {
	enum entype { type_id=0 };
	pe_geomparams() {
		type=type_id; density=mass=0; pos.Set(0,0,0); q.SetIdentity(); bRecalcBBox=1;
		flags = geom_colltype_solid|geom_colltype_ray|geom_floats|geom_colltype_explosion; flagsCollider = geom_colltype0;
		pMtx3x4=0;pMtx3x3=0; scale=1.0f; pLattice=0; pMatMapping=0; nMats=0;
		MARK_UNUSED surface_idx,minContactDist,idmatBreakable;
	}

	int type;
	float density; // 0 if mass is used
	float mass; // 0 if density is used
	Vec3 pos; // offset from object's geometrical pivot
	quaternionf q; // orientation relative to object
	float scale;
	Matrix34 *pMtx3x4;
	Matrix33 *pMtx3x3;	// optional 3x3 orintation+scale matrix
	int surface_idx; // surface identifier (used if corresponding CGeometry does not contain materials)
	unsigned int flags,flagsCollider;
	float minContactDist;
	int idmatBreakable;
	ITetrLattice *pLattice;
	int *pMatMapping;
	int nMats;
	int bRecalcBBox;

	VALIDATORS_START
		VALIDATOR_RANGE(density,-1E8,1E8)
		VALIDATOR_RANGE(mass,-1E8,1E8)
		VALIDATOR(pos)
		VALIDATOR_NORM(q)
	VALIDATORS_END
};

////////// articulated entity geometries

struct pe_articgeomparams : pe_geomparams {
	enum entype { type_id=2 };
	pe_articgeomparams() { type=type_id; idbody=0; }
	pe_articgeomparams(pe_geomparams &src) {
		type=type_id;	density=src.density; mass=src.mass; pos=src.pos; q=src.q; scale=src.scale; surface_idx=src.surface_idx;	
		pLattice=src.pLattice; pMatMapping=src.pMatMapping; nMats=src.nMats;
		pMtx3x4=src.pMtx3x4;pMtx3x3=src.pMtx3x3; flags=src.flags; flagsCollider=src.flagsCollider;
		idbody=0;	idmatBreakable=src.idmatBreakable; bRecalcBBox=src.bRecalcBBox;
		if (!is_unused(src.minContactDist)) minContactDist=src.minContactDist; else MARK_UNUSED minContactDist;
	}
	int idbody; // id of the subbody this geometry is attached to, the 1st add geometry specifies frame CS of this subbody
};

////////// vehicle entity geometries

const int NMAXWHEELS = 18;
struct pe_cargeomparams : pe_geomparams {
	enum entype { type_id=1 };
	pe_cargeomparams() : pe_geomparams() { type=type_id; MARK_UNUSED bDriving,minFriction,maxFriction,bRayCast,kLatFriction; bCanBrake=1; }
	pe_cargeomparams(pe_geomparams &src) {
		type=type_id;	density=src.density; mass=src.mass; pos=src.pos; q=src.q; surface_idx=src.surface_idx; 
		idmatBreakable=src.idmatBreakable; pLattice=src.pLattice; pMatMapping=src.pMatMapping; nMats=src.nMats;
		pMtx3x4=src.pMtx3x4;pMtx3x3=src.pMtx3x3; flags=src.flags; flagsCollider=src.flagsCollider;
		MARK_UNUSED bDriving,minFriction,maxFriction,bRayCast; bCanBrake=1;
	}
	int bDriving;	// whether wheel is driving, -1 - geometry os not a wheel
	int iAxle; // wheel axle, currently not used
	int bCanBrake; // whether the wheel is locked during handbrakes
	int bRayCast;	// whether the wheel use simple raycasting instead of geometry sweep check
	Vec3 pivot; // upper suspension point in vehicle CS
	float lenMax;	// relaxed suspension length
	float lenInitial; // current suspension length (assumed to be length in rest state)
	float kStiffness; // suspension stiffness, if 0 - calculate from lenMax, lenInitial, and vehicle mass and geometry
	float kDamping; // suspension damping, if <0 - calculate as -kdamping*(approximate zero oscillations damping)
	float minFriction,maxFriction; // additional friction limits for tire friction
  float kLatFriction; // coefficient for lateral friction
};

///////////////// tetrahedra lattice params ////////////////////////

struct pe_tetrlattice_params : pe_params {
	enum entype { type_id=19 };
	pe_tetrlattice_params() { 
		type = type_id; 
		MARK_UNUSED nMaxCracks,maxForcePush,maxForcePull,maxForceShift, maxTorqueTwist,maxTorqueBend,crackWeaken,density; 
	}

	int nMaxCracks;
	float maxForcePush,maxForcePull,maxForceShift;
	float maxTorqueTwist,maxTorqueBend;
	float crackWeaken;
	float density;
};

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// IGeometry Interface ////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

struct geom_world_data {
	geom_world_data() {
		v.Set(0,0,0);
		w.Set(0,0,0);
		offset.Set(0,0,0);
		R.SetIdentity();
		centerOfMass.Set(0,0,0);
		scale=1.0f; iStartNode=0;
	}
	Vec3 offset;
	Matrix33 R;
	float scale;
	Vec3 v,w;
	Vec3 centerOfMass;
	int iStartNode;
};

struct intersection_params {
	intersection_params() {
		iUnprojectionMode=0;
		vrel_min=1E-6f;
		time_interval=100.0f;
		maxSurfaceGapAngle=1.0f*float(g_PI/180);
		pGlobalContacts=0;
		minAxisDist=0;
		bSweepTest=false;
		centerOfRotation.Set(0,0,0);
		axisContactNormal.Set(0,0,1);
		unprojectionPlaneNormal.Set(0,0,0);
		axisOfRotation.Set(0,0,0);
		bKeepPrevContacts=false;
		bStopAtFirstTri=false;
		ptOutsidePivot[0].Set(1E11f,1E11f,1E11f);
		ptOutsidePivot[1].Set(1E11f,1E11f,1E11f);
		maxUnproj=1E10f;
		bNoAreaContacts=false;
		bNoBorder=false;
		bNoIntersection=0;
		bExactBorder=0;
		bThreadSafe=bThreadSafeMesh=0;
		plock=(volatile int*)&bThreadSafe;
	}
	int iUnprojectionMode;
	Vec3 centerOfRotation;
	Vec3 axisOfRotation;
	float time_interval;
	float vrel_min;
	float maxSurfaceGapAngle;
	float minAxisDist;
	Vec3 unprojectionPlaneNormal;
	Vec3 axisContactNormal;
	float maxUnproj;
	Vec3 ptOutsidePivot[2];
	bool bSweepTest;
	bool bKeepPrevContacts;
	bool bStopAtFirstTri;
	bool bNoAreaContacts;
	bool bNoBorder;
	int bExactBorder;
	int bNoIntersection;
	int bBothConvex;
	int bThreadSafe; // only set if it's known that no other thread will contend for the internal intersection data
	int bThreadSafeMesh;
	geom_contact *pGlobalContacts;
	volatile int *plock;
};

struct phys_geometry {
	IGeometry *pGeom;
	Vec3 Ibody;
	quaternionf q;
	Vec3 origin;
	float V;
	int nRefCount;
	int surface_idx;
	int *pMatMapping;
	int nMats;
	void *pForeignData;

	AUTO_STRUCT_INFO
};

struct bop_newvtx {
	int idx;			 // vertex index in the resulting A phys mesh
	int iBvtx;     // -1 if intersection vertex, >=0 if B vertex
	int idxTri[2]; // intersecting triangles' foreign indices 
};

struct bop_newtri {
	int idxNew;  // a newly generated foreign index (can be remapped later)
	int iop;     // triangle source 0=A, 1=B
	int idxOrg;  // original (triangulated) triangle's foreign index
	int iVtx[3]; // for each vertex, existing vertex index if >=0, -(new vertex index+1) if <0
	float areaOrg; // original triangle's area
	Vec3 area[3]; // areas of compementary triangles for each vertex (divide by original tri area to get barycentric coords)
};

struct bop_vtxweld {
	void set(int _ivtxDst,int _ivtxWelded) { ivtxDst=_ivtxDst; ivtxWelded=_ivtxWelded; }
	int ivtxDst : 16; // ivtxWelded is getting replaced with ivtxDst
	int ivtxWelded : 16;
};

struct bop_TJfix {
	void set(int _iACJ,int _iAC, int _iABC,int _iCA, int _iTJvtx) { iACJ=_iACJ;iAC=_iAC;iABC=_iABC;iCA=_iCA;iTJvtx=_iTJvtx; }
	// A _____J____ C	 (ACJ is a thin triangle on top of ABC; J is 'the junction vertex')
	//   \		.	  /		 in ABC: set A->Jnew
	//		 \	. /			 in ACJ: set J->Jnew, A -> A from original ABC, C -> B from original ABC
	//			 \/
	//			 B
	int iABC; // big triangle's foreign idx
	int iACJ; // small triangle's foreign idx
	int iCA; // CA edge number in ABC
	int iAC; // AC edge number in ACJ
	int iTJvtx; // J vertex index
};

struct bop_meshupdate_thunk {
	bop_meshupdate_thunk() { prevRef=nextRef = this; }
	~bop_meshupdate_thunk() { prevRef->nextRef = nextRef; nextRef->prevRef = prevRef; }
	virtual void Release() { prevRef=nextRef = this; }
	bop_meshupdate_thunk *prevRef,*nextRef;
};

struct bop_meshupdate : bop_meshupdate_thunk {
	bop_meshupdate() { Reset(); }
	~bop_meshupdate() { Release(); }

	void Reset() {
		pRemovedVtx=0; pRemovedTri=0; pNewVtx=0; pNewTri=0;	pWeldedVtx=0; pTJFixes=0;	pMovedBoxes=0;
		nRemovedVtx=nRemovedTri=nNewVtx=nNewTri=nWeldedVtx=nTJFixes=nMovedBoxes = 0; next = 0;
		pMesh[0]=pMesh[1]=0; relScale=1.0f;
	}
	virtual void Release();

	IGeometry *pMesh[2]; // 0-dst (A), 1-src (B)
	int *pRemovedVtx;
	int nRemovedVtx;
	int *pRemovedTri;
	int nRemovedTri;
	bop_newvtx *pNewVtx;
	int nNewVtx;
	bop_newtri *pNewTri;
	int nNewTri;
	bop_vtxweld *pWeldedVtx;
	int nWeldedVtx;
	bop_TJfix *pTJFixes;
	int nTJFixes;
	bop_meshupdate *next;
	primitives::box *pMovedBoxes;
	int nMovedBoxes;
	float relScale;
};

struct trinfo {
	trinfo &operator=(trinfo &src) { ibuddy[0]=src.ibuddy[0]; ibuddy[1]=src.ibuddy[1]; ibuddy[2]=src.ibuddy[2]; return *this; }
	index_t ibuddy[3];
};

struct mesh_island {
	int itri;
	int nTris;
	int iParent,iChild,iNext;
	float V;
	Vec3 center;
	int bProcessed;
};

struct tri2isle {
	unsigned int inext : 16;
	unsigned int isle	 : 15;
	unsigned int bFree : 1;
};

struct mesh_data : primitives::primitive {
	index_t *pIndices;
	char *pMats;
	int *pForeignIdx;
	strided_pointer<Vec3> pVertices;
	Vec3 *pNormals;
	int *pVtxMap;
	trinfo *pTopology;
	int nTris,nVertices;
	mesh_island *pIslands;
	int nIslands;
	tri2isle *pTri2Island;
};

const int BOP_NEWIDX0 = 0x8000000;

enum geomtypes { GEOM_TRIMESH=primitives::triangle::type, GEOM_HEIGHTFIELD=primitives::heightfield::type, GEOM_CYLINDER=primitives::cylinder::type,
								 GEOM_CAPSULE=primitives::capsule::type, GEOM_RAY=primitives::ray::type, GEOM_SPHERE=primitives::sphere::type, 
								 GEOM_BOX=primitives::box::type, GEOM_VOXELGRID=primitives::voxelgrid::type };
enum foreigntypes { DATA_MESHUPDATE=-1 };

enum meshflags { mesh_shared_vtx=1, mesh_shared_idx=2, mesh_shared_mats=4, mesh_shared_foreign_idx=8,	mesh_shared_normals=0x10,
								 mesh_OBB=0x20, mesh_AABB=0x40, mesh_SingleBB=0x80, mesh_multicontact0=0x100, mesh_multicontact1=0x200, 
								 mesh_multicontact2=0x400, mesh_approx_cylinder=0x800, mesh_approx_box=0x1000, mesh_approx_sphere=0x2000, 
								 mesh_keep_vtxmap=0x8000, mesh_keep_vtxmap_for_saving=0x10000, mesh_no_vtx_merge=0x20000, mesh_AABB_rotated=0x40000,
								 mesh_VoxelGrid=0x80000, mesh_always_static=0x100000, mesh_approx_capsule=0x200000 };
enum meshAuxData { mesh_data_materials=1, mesh_data_foreign_idx=2, mesh_data_vtxmap=4 };

struct IGeometry {
	virtual int GetType() = 0;
	virtual int AddRef() = 0;
	virtual void Release() = 0;
	virtual void Lock(int bWrite=1) = 0;
	virtual void Unlock(int bWrite=1) = 0;
	virtual void GetBBox(primitives::box *pbox) = 0;
	virtual int CalcPhysicalProperties(phys_geometry *pgeom) = 0;
	virtual int PointInsideStatus(const Vec3 &pt) = 0;
	virtual int Intersect(IGeometry *pCollider, geom_world_data *pdata1,geom_world_data *pdata2, intersection_params *pparams, geom_contact *&pcontacts) = 0;
	virtual int FindClosestPoint(geom_world_data *pgwd, int &iPrim,int &iFeature, const Vec3 &ptdst0,const Vec3 &ptdst1,
		Vec3 *ptres, int nMaxIters=10) = 0;
	virtual void CalcVolumetricPressure(geom_world_data *gwd, const Vec3 &epicenter,float k,float rmin,
		const Vec3 &centerOfMass, Vec3 &P,Vec3 &L) = 0;
	virtual float CalculateBuoyancy(const primitives::plane *pplane, const geom_world_data *pgwd, Vec3 &submergedMassCenter) = 0;
	virtual void CalculateMediumResistance(const primitives::plane *pplane, const geom_world_data *pgwd, Vec3 &dPres,Vec3 &dLres) = 0;
	virtual void DrawWireframe(IPhysRenderer *pRenderer, geom_world_data *gwd, int iLevel, int idxColor) = 0;
	virtual int GetPrimitiveId(int iPrim,int iFeature) = 0;
	virtual int GetPrimitive(int iPrim, primitives::primitive *pprim) = 0;
	virtual int GetForeignIdx(int iPrim) = 0;
	virtual Vec3 GetNormal(int iPrim, const Vec3 &pt) = 0;
	virtual int GetFeature(int iPrim,int iFeature, Vec3 *pt) = 0;
	virtual int IsConvex(float tolerance) = 0;
	virtual void PrepareForRayTest(float raylen) = 0;
	virtual float BuildOcclusionCubemap(geom_world_data *pgwd, int iMode, int *pGrid0[6],int *pGrid1[6],int nRes, float rmin,float rmax, int nGrow) = 0;
	virtual void GetMemoryStatistics(ICrySizer *pSizer) = 0;
	virtual void Save(CMemStream &stm) = 0;
	virtual void Load(CMemStream &stm) = 0;
	virtual void Load(CMemStream &stm, strided_pointer<const Vec3> pVertices, strided_pointer<unsigned short> pIndices, char *pIds) = 0;
	virtual int GetPrimitiveCount() = 0;
	virtual const primitives::primitive *GetData() = 0;
	virtual void SetData(const primitives::primitive*) = 0;
	virtual float GetVolume() = 0;
	virtual Vec3 GetCenter() = 0;
	virtual int Subtract(IGeometry *pGeom, geom_world_data *pdata1,geom_world_data *pdata2, int bLogUpdates=1) = 0;
	virtual int GetSubtractionsCount() = 0;
	virtual void *GetForeignData(int iForeignData=0) = 0;
	virtual int GetiForeignData() = 0;
	virtual void SetForeignData(void *pForeignData, int iForeignData) = 0;
	virtual int GetErrorCount() = 0;
	virtual void DestroyAuxilaryMeshData(int idata) = 0; // see meshAuxData enum
	virtual void RemapForeignIdx(int *pCurForeignIdx, int *pNewForeignIdx, int nTris) = 0;
	virtual void AppendVertices(Vec3 *pVtx,int *pVtxMap, int nVtx) = 0;
	virtual float ComputeExtent(GeomQuery& geo, EGeomForm eForm) = 0;
	virtual void GetRandomPos(RandomPos& ran, GeomQuery& geo, EGeomForm eForm) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// IGeometryManager Interface /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

struct SMeshBVParams {};

struct SBVTreeParams : SMeshBVParams {
	int nMinTrisPerNode,nMaxTrisPerNode;
	float favorAABB;
};

struct SVoxGridParams : SMeshBVParams {
	Vec3 origin;
	Vec3 step;
	Vec3i size;
};

struct ITetrLattice {
	virtual int SetParams(pe_params *) = 0;
	virtual int GetParams(pe_params *) = 0;
	virtual void DrawWireframe(IPhysRenderer *pRenderer, geom_world_data *gwd, int idxColor) = 0;
	virtual void Release() = 0;
};

struct IBreakableGrid2d {
	virtual int *BreakIntoChunks(const vector2df &pt, float r, vector2df *&ptout, int maxPatchTris,float jointhresh,int seed=-1) = 0;
	virtual primitives::grid *GetGridData() = 0;
	virtual bool IsEmpty() = 0;
	virtual void Release() = 0;
};


struct IGeomManager {
	virtual void InitGeoman() = 0;
	virtual void ShutDownGeoman() = 0;

	virtual IGeometry *CreateMesh(strided_pointer<const Vec3> pVertices, strided_pointer<unsigned short> pIndices, char *pMats,int *pForeignIdx,int nTris, 
		int flags, float approx_tolerance=0.05f, int nMinTrisPerNode=2,int nMaxTrisPerNode=4, float favorAABB=1.0f) = 0;
	virtual IGeometry *CreateMesh(strided_pointer<const Vec3> pVertices, strided_pointer<unsigned short> pIndices, char *pMats,int *pForeignIdx,int nTris, 
		int flags, float approx_tolerance, SMeshBVParams *pParams) = 0;
	virtual IGeometry *CreatePrimitive(int type, const primitives::primitive *pprim) = 0;
	virtual void DestroyGeometry(IGeometry *pGeom) = 0;

	// defSurfaceIdx will be used (until overwritten in entity part) if the geometry doesn't have per-face materials
	virtual phys_geometry *RegisterGeometry(IGeometry *pGeom,int defSurfaceIdx=0, int *pMatMapping=0,int nMats=0) = 0;
	virtual int AddRefGeometry(phys_geometry *pgeom) = 0;
	virtual int UnregisterGeometry(phys_geometry *pgeom) = 0;
	virtual void SetGeomMatMapping(phys_geometry *pgeom, int *pMatMapping, int nMats) = 0;

	virtual void SaveGeometry(CMemStream &stm, IGeometry *pGeom) = 0;
	virtual IGeometry *LoadGeometry(CMemStream &stm, strided_pointer<const Vec3> pVertices, strided_pointer<unsigned short> pIndices, char *pMats) = 0;
	virtual void SavePhysGeometry(CMemStream &stm, phys_geometry *pgeom) = 0;
	virtual phys_geometry *LoadPhysGeometry(CMemStream &stm, strided_pointer<const Vec3> pVertices, 
		strided_pointer<unsigned short> pIndices, char *pIds) = 0;
	virtual IGeometry *CloneGeometry(IGeometry *pGeom) = 0;

	virtual ITetrLattice *CreateTetrLattice(const Vec3 *pt,int npt, const int *pTets,int nTets) = 0;
	virtual int RegisterCrack(IGeometry *pGeom, Vec3 *pVtx, int idmat) = 0;
	virtual void UnregisterCrack(int id) = 0;
	virtual IGeometry *GetCrackGeom(const Vec3 *pt,int idmat, geom_world_data *pgwd) = 0;

	virtual IBreakableGrid2d *GenerateBreakableGrid(vector2df *ptsrc,int npt, const vector2di &nCells, int bStatic=1, int seed=-1) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// IPhysUtils Interface /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

struct IPhysUtils {
	virtual int CoverPolygonWithCircles(strided_pointer<vector2df> pt,int npt,bool bConsecutive, const vector2df &center,
		vector2df *&centers,float *&radii, float minCircleRadius) = 0;
	virtual int qhull(strided_pointer<Vec3> pts, int npts, index_t*& pTris) = 0;
	virtual void DeletePointer(void *pdata) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// IPhysicalEntity Interface //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

enum snapshot_flags { ssf_compensate_time_diff=1, ssf_checksum_only=2, ssf_no_update=4, ssf_from_child_class=8 };

struct IPhysicalEntity {
	/*! Retrieves entity type
		@returb entity type enum
	*/
	virtual pe_type GetType() = 0;

	virtual int AddRef() = 0;
	virtual int Release() = 0;

	/*! Sets parameters
		@param params pointer to parameters structure
		@return nonzero if success
	*/
	virtual int SetParams(pe_params* params, int bThreadSafe=0) = 0;
	virtual int GetParams(pe_params* params) = 0;
	/*! Retrieves status
		@param status pointer to status structure
		@retuan nonzero if success
	*/
	virtual int GetStatus(pe_status* status) = 0;
	/*! Performes action
		@param action pointer to action structure
		@return nonzero if success
	*/
	virtual int Action(pe_action*, int bThreadSafe=0) = 0;

	/*! Adds geometry
		@param pgeom geometry identifier (obtained from RegisterXXX function)
		@param params pointer to geometry parameters structure
		@param id requested geometry id, if -1 - assign automatically
		@return geometry id (0..some number), -1 means error
	*/
	virtual int AddGeometry(phys_geometry *pgeom, pe_geomparams* params,int id=-1, int bThreadSafe=0) = 0;
	/*! Removes geometry
		@param params pointer to parameters structure
		@return nonzero if success
	*/
	virtual void RemoveGeometry(int id, int bThreadSafe=0) = 0;

	/*! Retrieves foreign data passed during creation (can be pointer to the corresponding engine entity, for instance)
		@param iforeigndata requested foreign data type
		@return foreign data (void*) if itype==iforeigndata of this entity, 0 otherwise
	*/
	virtual void *GetForeignData(int itype=0) = 0;

	/*! Retrieves iforeigndata of the entity (usually it will be a type identifier for pforeign data
		@return iforeigndata
	*/
	virtual int GetiForeignData() = 0;

	/*! Writes state into snapshot
		@param stm stream
		@param time_back requests previous state (only supported by living entities)
		@params flags a combination of snapshot_flags
		@return non0 if successful
	*/
	virtual int GetStateSnapshot(class CStream &stm, float time_back=0, int flags=0) = 0;
	virtual int GetStateSnapshot(TSerialize ser, float time_back=0, int flags=0) = 0;
	/*! Reads state from snapshot
		@param stm stream
		@return size of snapshot
	*/
	virtual int SetStateFromSnapshot(class CStream &stm, int flags=0) = 0;
	virtual int SetStateFromSnapshot(TSerialize ser, int flags=0) = 0;
	virtual int SetStateFromTypedSnapshot(TSerialize ser, int type, int flags=0) = 0;
	virtual int PostSetStateFromSnapshot() = 0;
	virtual int GetStateSnapshotTxt(char *txtbuf,int szbuf, float time_back=0) = 0;
	virtual void SetStateFromSnapshotTxt(const char *txtbuf,int szbuf) = 0;
	virtual unsigned int GetStateChecksum() = 0;
	/*! Evolves entity in time. Normally this is called from PhysicalWorld::TimeStep
		@param time_interval time step
	*/
	virtual int DoStep(float time_interval, int iCaller=1) = 0;
	/*! Restores previous entity state that corresponds to time -time_interval from now, interpolating when
			necessary. This can be used for	manual client-server synchronization. Outside of PhysicalWorld::TimeStep
			should be called only for living entities
		@param time_interval time to trace back
	*/
	virtual void StepBack(float time_interval) = 0;
	/*! Returns physical world this entity belongs to
		@return physical world interface
	*/
	virtual IPhysicalWorld *GetWorld() = 0;

	virtual void GetMemoryStatistics(ICrySizer *pSizer) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// IPhysicsEventClient Interface //////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

struct IPhysicsEventClient {
	virtual void OnBBoxOverlap(IPhysicalEntity *pEntity, void *pForeignData,int iForeignData,
		IPhysicalEntity *pCollider, void *pColliderForeignData,int iColliderForeignData) = 0;
	virtual void OnStateChange(IPhysicalEntity *pEntity, void *pForeignData,int iForeignData, int iOldSimClass,int iNewSimClass) = 0;
	virtual void OnCollision(IPhysicalEntity *pEntity, void *pForeignData,int iForeignData, coll_history_item *pCollision) = 0;
	virtual int OnImpulse(IPhysicalEntity *pEntity, void *pForeignData,int iForeignData, pe_action_impulse *impulse) = 0;
	virtual void OnPostStep(IPhysicalEntity *pEntity, void *pForeignData,int iForeignData, float dt) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// IPhysicalWorld Interface //////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

enum draw_helper_flags { pe_helper_collisions=1, pe_helper_geometry=2, pe_helper_bbox=4, pe_helper_lattice=8 };
enum surface_flags { sf_pierceable_mask=0x0F, sf_max_pierceable=0x0F, sf_important=0x200, sf_manually_breakable=0x400, sf_matbreakable_bit=16 };
#define sf_pierceability(i) (i)
#define sf_matbreakable(i) (((i)+1)<<sf_matbreakable_bit)
enum rwi_flags { rwi_ignore_terrain_holes=0x20, rwi_ignore_noncolliding=0x40, rwi_ignore_back_faces=0x80, rwi_ignore_solid_back_faces=0x100,
								 rwi_pierceability_mask=0x0F, rwi_pierceability0=0, rwi_stop_at_pierceable=0x0F, rwi_separate_important_hits=sf_important,
								 rwi_colltype_bit=16, rwi_colltype_any=0x400, rwi_queue=0x800, rwi_force_pierceable_noncoll=0x1000,
								 rwi_reuse_last_hit=0x2000, rwi_update_last_hit=0x4000, rwi_any_hit=0x8000 };
#define rwi_pierceability(pty) (pty)
enum entity_query_flags { ent_static=1, ent_sleeping_rigid=2, ent_rigid=4, ent_living=8, ent_independent=16, ent_deleted=128, ent_terrain=0x100,
													ent_all=ent_static | ent_sleeping_rigid | ent_rigid | ent_living | ent_independent | ent_terrain,
													ent_flagged_only = pef_update, ent_skip_flagged = pef_update*2, ent_areas = 32, ent_triggers = 64,
													ent_ignore_noncolliding = 0x10000, ent_sort_by_mass = 0x20000, ent_allocate_list = 0x40000,
													ent_water = 0x200, ent_no_ondemand_activation = 0x80000 // can only be used in RayWorldIntersection
												};
enum phys_locks { PLOCK_WORLD_STEP=1, PLOCK_CALLER0, PLOCK_CALLER1, PLOCK_QUEUE, PLOCK_AREAS };

struct phys_profile_info {
	IPhysicalEntity *pEntity;
	int nTicks,nCalls;
	int nTicksAvg;
	float nCallsAvg;
	int nTicksPeak,nCallsPeak,peakAge;
	int nTicksStep;
	int id;
	const char *pName;
};

struct SolverSettings {
	int nMaxStackSizeMC;							// def 8
	float maxMassRatioMC;							// def 50
	int nMaxMCiters;									// def 1400
	int nMaxMCitersHopeless;					// def 400
	float accuracyMC;									// def 0.005
	float accuracyLCPCG;							// def 0.005
	int nMaxContacts;									// def 150
	int nMaxPlaneContacts;            // def 7
	int nMaxPlaneContactsDistress;		// def 4
	int nMaxLCPCGsubiters;						// def 120
	int nMaxLCPCGsubitersFinal;				// def 250
	int nMaxLCPCGmicroiters;
	int nMaxLCPCGmicroitersFinal;
	int nMaxLCPCGiters;								// def 5
	float minLCPCGimprovement;        // def 0.1
	int nMaxLCPCGFruitlessIters;			// def 4
	float accuracyLCPCGnoimprovement;	// def 0.05
	float minSeparationSpeed;					// def 0.02
	float maxvCG;
	float maxwCG;
	float maxvUnproj;
	int bCGUnprojVel;
	float maxMCMassRatio;
	float maxMCVel;
	int maxLCPCGContacts;
};

struct PhysicsVars : SolverSettings {
  int bFlyMode;
	int iCollisionMode;
	int bSingleStepMode;
	int bDoStep;
	float fixedTimestep;
	float timeGranularity;
	float maxWorldStep;
	int iDrawHelpers;
	float maxContactGap;
	float maxContactGapPlayer;
	float minBounceSpeed;
	int bProhibitUnprojection;
	int bUseDistanceContacts;
	float unprojVelScale;
	float maxUnprojVel;
	int bEnforceContacts;
	int nMaxSubsteps;
	int nMaxSurfaces;
	Vec3 gravity;
	int nGroupDamping;
	float groupDamping;
	int nMaxSubstepsLargeGroup;
	int nBodiesLargeGroup;
	int bBreakOnValidation;
	int bLogActiveObjects;
	int bMultiplayer;
	int bProfileEntities;
	int bProfileFunx;
	int nGEBMaxCells;
	int nMaxEntityCells;
	int nMaxAreaCells;
	float maxVel;
	float maxVelPlayers;
	float maxContactGapSimple;
	float penaltyScale;
	int bSkipRedundantColldet;
	int bLimitSimpleSolverEnergy;
	int nMaxEntityContacts;
	int bLogLatticeTension;
	int nMaxLatticeIters;
	int bLogStructureChanges;
	float tickBreakable;
	float approxCapsLen;
	int nMaxApproxCaps;
	int bPlayersCanBreak;
	float lastTimeStep;
	int bMultithreaded;
	float breakImpulseScale;
	float rtimeGranularity;
	float massLimitDebris;
	int flagsColliderDebris;
	int flagsANDDebris;
	int maxSplashesPerObj;
	float splashDist0,minSplashForce0,minSplashVel0;
	float splashDist1,minSplashForce1,minSplashVel1;
	int bDebugExplosions;
	float timeScalePlayers;
	float threadLag;
	// net-synchronization related
	float netMinSnapDist;
	float netVelSnapMul;
	float netMinSnapDot;
	float netAngSnapMul;
	float netSmoothTime;
};

struct ray_hit {
	float dist;
	IPhysicalEntity *pCollider;
	int ipart;
	int partid;
	int surface_idx;
	int idmatOrg;	// original material index, not mapped with material mapping
	int foreignIdx;
	int iNode;
	Vec3 pt;
	Vec3 n;
	int bTerrain;
	ray_hit *next; // reserved for internal use
};

struct ray_hit_cached {
	ray_hit_cached() { pCollider=0; ipart=0; }
	ray_hit_cached(const ray_hit &hit) { pCollider=hit.pCollider; ipart=hit.ipart; iNode=hit.iNode; }
	ray_hit_cached &operator=(const ray_hit &hit) { pCollider=hit.pCollider; ipart=hit.ipart; iNode=hit.iNode; return *this; }

	IPhysicalEntity *pCollider;
	int ipart;
	int iNode;
};

#ifndef PWI_NAME_TAG
#define PWI_NAME_TAG "PrimitiveWorldIntersection"
#endif
#ifndef RWI_NAME_TAG
#define RWI_NAME_TAG "RayWorldIntersection"
#endif

struct pe_explosion {
	pe_explosion() { nOccRes=0; nGrow=0; rminOcc=0.1f; holeSize=0; explDir.Set(0,0,1); iholeType=0; forceDeformEntities=false; }
	Vec3 epicenter;
	Vec3 epicenterImp;
	float rmin,rmax,r;
	float impulsivePressureAtR;
	int nOccRes;
	int nGrow;
	float rminOcc;
	float holeSize;
	Vec3 explDir;
	int iholeType;
	bool forceDeformEntities; // force deformation even if breakImpulseScale is zero
	// filled as results
	IPhysicalEntity **pAffectedEnts;
	float *pAffectedEntsExposure;
	int nAffectedEnts;
};

struct EventPhys {
	EventPhys *next;
	int idval;
};

struct EventPhysMono : EventPhys {
	IPhysicalEntity *pEntity;
	void *pForeignData;
	int iForeignData;
};

struct EventPhysStereo : EventPhys {
	IPhysicalEntity *pEntity[2];
	void *pForeignData[2];
	int iForeignData[2];
};

struct EventPhysBBoxOverlap : EventPhysStereo {
	enum entype { id=0, flagsCall=0, flagsLog=0 };
	EventPhysBBoxOverlap() { idval=id; }
};

struct EventPhysCollision : EventPhysStereo {
	enum entype { id=1, flagsCall=pef_monitor_collisions, flagsLog=pef_log_collisions };
	EventPhysCollision() { idval=id; pEntContact=0; }
	int idCollider;
	Vec3 pt;
	Vec3 n;
	Vec3 vloc[2];
	float mass[2];
	int partid[2];
	int idmat[2];
	float penetration;
	float normImpulse;
	float radius;
	void *pEntContact; // reserved for internal use
};

struct EventPhysStateChange : EventPhysMono {
	enum entype { id=2, flagsCall=pef_monitor_state_changes, flagsLog=pef_log_state_changes };
	EventPhysStateChange() { idval=id; }
	int iSimClass[2];
};

struct EventPhysEnvChange : EventPhysMono {
	enum entype { id=3, flagsCall=pef_monitor_env_changes, flagsLog=pef_log_env_changes };
	enum encode { EntStructureChange=0 };
	EventPhysEnvChange() { idval=id; }
	int iCode;
	IPhysicalEntity *pentSrc;
	IPhysicalEntity *pentNew;
};

struct EventPhysPostStep : EventPhysMono {
	enum entype { id=4, flagsCall=pef_monitor_poststep, flagsLog=pef_log_poststep };
	EventPhysPostStep() { idval=id; }
	float dt;
	Vec3 pos;
	quaternionf q;
	int idStep;
};

struct EventPhysUpdateMesh : EventPhysMono {
	enum entype { id=5, flagsCall=1, flagsLog=2 };
	enum reason { ReasonExplosion, ReasonFracture, ReasonRequest };
	EventPhysUpdateMesh() { idval=id; }
	int partid;
	int bInvalid;
	int iReason;
	IGeometry *pMesh;	// ->GetForeignData(DATA_MESHUPDATE) returns a list of bop_meshupdates
	bop_meshupdate *pLastUpdate; // the last mesh update for at moment when the event was generated
};

struct EventPhysCreateEntityPart : EventPhysMono {
	enum entype { id=6, flagsCall=1, flagsLog=2 };
	enum reason { ReasonMeshSplit, ReasonJointsBroken };
	EventPhysCreateEntityPart() { idval=id; }
	IPhysicalEntity *pEntNew;
	int partidSrc,partidNew;
	int nTotParts;
	int bInvalid; 
	int iReason;
	Vec3 breakImpulse;
	Vec3 breakAngImpulse;
	float breakSize;
	float cutRadius;
	Vec3 cutPtLoc[2];
	Vec3 cutDirLoc[2];
	IGeometry *pMeshNew;
	bop_meshupdate *pLastUpdate;
};

struct EventPhysRemoveEntityParts : EventPhysMono {
	enum entype { id=7, flagsCall=1, flagsLog=2 };
	EventPhysRemoveEntityParts() { idval=id; }
	unsigned int partIds[4];
	float massOrg;
};

struct EventPhysJointBroken : EventPhysStereo {
	enum entype { id=8, flagsCall=1, flagsLog=2 };
	EventPhysJointBroken() { idval=id; }
	int idJoint;
	int bJoint; // otherwise it's a constraint
	int partidEpicenter;
	Vec3 pt;
	Vec3 n;
	int partid[2];
	int partmat[2];
	IPhysicalEntity *pNewEntity[2];
};

struct EventPhysRWIResult : EventPhysMono {
	enum entype { id=9, flagsCall=0,flagsLog=0 };
	EventPhysRWIResult() { idval=id; }
	ray_hit *pHits;
	int nHits,nMaxHits;
	int bHitsFromPool; // 1 if hits reside in the internal physics hits pool
};

struct EventPhysPWIResult : EventPhysMono {
	enum entype { id=10, flagsCall=0,flagsLog=0 };
	EventPhysPWIResult() { idval=id; }
	float dist;
	Vec3 pt;
	Vec3 n;
	int idxMat;
};

struct EventPhysArea : EventPhysMono {
	enum entype { id=11, flagsCall=0,flagsLog=0 };
	EventPhysArea() { idval=id; }

	Vec3 pt;
	Vec3 ptref,dirref;
	pe_params_buoyancy pb;
	Vec3 gravity;
	IPhysicalEntity *pent;
};

struct EventPhysAreaChange : EventPhysMono {
	enum entype { id=12, flagsCall=0,flagsLog=0 };
	EventPhysAreaChange() { idval=id; }

	Vec3 boxAffected[2];
};

const int EVENT_TYPES_NUM = 13;


struct IPhysicalWorld {
	/*! Inits world
		@param pconsole pointer of IConsole interace
	*/
	virtual void Init() = 0;

	virtual IGeomManager* GetGeomManager() = 0;
	virtual IPhysUtils* GetPhysUtils() = 0;

	/*! Shuts the world down
	*/
	virtual void Shutdown(int bDeleteGeometries = 1) = 0;
	/*! Destroys the world
	*/
	virtual void Release() = 0;

	/*! Initializes entity hash grid
		@param axisx id of grid x axis (0-world x,1-world y,2-world z)
		@param axisy id of grid y axis (0-world x,1-world y,2-world z)
		@param origin grid (0,0) in world CS
		@param nx number of cells in grid x direciton
		@param ny number of cells in grid y direciton
		@param stepx cell x dimension
		@param stepy cell y dimension
	*/
	virtual void SetupEntityGrid(int axisz, Vec3 org, int nx,int ny, float stepx,float stepy) = 0;
	virtual void DeactivateOnDemandGrid() = 0;
	virtual void RegisterBBoxInPODGrid(const Vec3 *BBox) = 0;
	virtual int AddRefEntInPODGrid(IPhysicalEntity *pent, const Vec3 *BBox=0) = 0;
	/*! Sets heightfield data
		@param phf
	*/
	virtual IPhysicalEntity *SetHeightfieldData(const primitives::heightfield *phf,int *pMatMapping=0,int nMats=0) = 0;
	/*! Retrieves heightfield data
		@param phf
		@return pointer to heightfield entity
	*/
	virtual IPhysicalEntity *GetHeightfieldData(primitives::heightfield *phf) = 0;
	/*! Retrieves pointer to physvars structure
		@return pointer to physvar structure
	*/
	virtual PhysicsVars *GetPhysVars() = 0;

	/*! Creates physical entity
		@param type entity type
		@param params initial params (as in entity SetParams)
		@param pforeigndata entity foreign data
		@param iforeigndata entity foreign data type identifier
		@return pointer to physical entity interface
	*/
	virtual IPhysicalEntity* CreatePhysicalEntity(pe_type type, pe_params* params=0, void *pforeigndata=0, int iforeigndata=0, int id=-1) = 0;
	virtual IPhysicalEntity* CreatePhysicalEntity(pe_type type, float lifeTime, pe_params* params=0, void *pForeignData=0,int iForeignData=0,
		int id=-1,IPhysicalEntity *pHostPlaceholder=0) = 0;
	virtual IPhysicalEntity *CreatePhysicalPlaceholder(pe_type type, pe_params* params=0, void *pForeignData=0,int iForeignData=0, int id=-1) = 0;
	/*! Destroys physical entity
		@param pent entity
		@param mode 0-normal destroy, 1-suspend, 2-restore from suspended state
		@return nonzero if success
	*/
	virtual int DestroyPhysicalEntity(IPhysicalEntity *pent, int mode=0, int bThreadSafe=0) = 0;

	virtual int SetPhysicalEntityId(IPhysicalEntity *pent, int id, int bReplace=1, int bThreadSafe=0) = 0;
	virtual int GetPhysicalEntityId(IPhysicalEntity *pent) = 0;
	virtual IPhysicalEntity* GetPhysicalEntityById(int id) = 0;

	/*! Sets surface parameters
		@param surface_idx surface identifier
		@param bounciness restitution coefficient (for pair of surfaces k = sum of their coefficients, clamped to [0..1]
		@param friction friction coefficient (for pair of surfaces k = sum of their coefficients, clamped to [0..inf)
		@param flags bitmask (see surface_flags enum)
		@return nonzero if success
	*/
	virtual int SetSurfaceParameters(int surface_idx, float bounciness,float friction, unsigned int flags=0) = 0;
	virtual int GetSurfaceParameters(int surface_idx, float &bounciness,float &friction, unsigned int &flags) = 0;

	/*! Perfomes a time step
		@param time_interval time interval
		@param flags entity types to update (ent_..; ent_deleted to purge deletion physics-on-demand state monitoring)
	*/
	virtual void TimeStep(float time_interval,int flags=ent_all|ent_deleted) = 0;
	/*! Returns current time of the physical world
		@return current time
	*/
	virtual float GetPhysicsTime() = 0;
	virtual int GetiPhysicsTime()  = 0;
	/*! Sets current time of the physical world
		@param time new time
	*/
	virtual void SetPhysicsTime(float time) = 0;
	virtual void SetiPhysicsTime(int itime) = 0;
	/*! Sets physical time that corresponds to the following server state snapshot
		@param time_snapshot physical time of the following server snapshot
	*/
	virtual void SetSnapshotTime(float time_snapshot,int iType=0) = 0;
	virtual void SetiSnapshotTime(int itime_snapshot,int iType=0) = 0;

	/*! Retrives list of entities that fall into a box
		@param ptmix,ptmax - box corners
		@param pList returned pointer to entity list
		@param objtypes	bitmask 0-static, 1-sleeping, 2-physical, 3-living
		@return number of entities
	*/
	virtual int GetEntitiesInBox(Vec3 ptmin,Vec3 ptmax, IPhysicalEntity **&pList, int objtypes, int szListPrealloc=0) = 0;

	/*! Shoots ray into world
		@param origin origin
		@param dir direction*(ray length)
		@param objtypes	bitmask 0-terrain 1-static, 2-sleeping, 3-physical, 4-living
		@param flags a combination of rwi_flags
		@param hits destination hits array
		@param nmaxhits size of this array
		@param pskipent entity to skip
		@param pForeignData data that is returned in EventPhysRWIResult if rwi_queue is specified
		@return number of collisions
	*/
	virtual int RayWorldIntersection(Vec3 org,Vec3 dir, int objtypes, unsigned int flags, ray_hit *hits,int nMaxHits,
		IPhysicalEntity **pSkipEnts=0,int nSkipEnts=0, void *pForeignData=0,int iForeignData=0, 
		const char *pNameTag=RWI_NAME_TAG, ray_hit_cached *phitLast=0, int iCaller=1) = 0;
	int RayWorldIntersection(Vec3 org,Vec3 dir, int objtypes, unsigned int flags, ray_hit *hits,int nMaxHits, 
		IPhysicalEntity *pSkipEnt,IPhysicalEntity *pSkipEntAux=0, void *pForeignData=0,int iForeignData=0) 
	{
		IPhysicalEntity *pSkipEnts[2];
		int nSkipEnts = 0;
		if (pSkipEnt) pSkipEnts[nSkipEnts++] = pSkipEnt;
		if (pSkipEntAux) pSkipEnts[nSkipEnts++] = pSkipEntAux;
		return RayWorldIntersection(org,dir,objtypes,flags,hits,nMaxHits, pSkipEnts,nSkipEnts, pForeignData,iForeignData);
	}
	// Traces ray requests (rwi calls with rwi_queue set); logs and calls EventPhysRWIResult for each
	// returns the number of rays traced
	virtual int TracePendingRays(int bDoActualTracing=1) = 0;

	/*! Freezes (resets velocities of) all physical, living, and detached entities
	*/
	virtual void ResetDynamicEntities() = 0;
	/*! Immediately destroys all physical, living, and detached entities; flushes the deleted entities
		All subsequent calls to DestroyPhysicalEntity for non-static entities are ignored until the next
		non-static entity is created
	*/
	virtual void DestroyDynamicEntities() = 0;
	/*! Forces deletion of all entities marked as deleted
	*/
	virtual void PurgeDeletedEntities() = 0;
	virtual int GetEntityCount(int iEntType) = 0;
	virtual int ReserveEntityCount(int nExtraEnts) = 0;

	/*! Simulates physical explosion with k/(r^2) pressure distribution
		@param epicenter epicenter used for building the occlusion map
		@param epicenterImp epicenter used for applying impulse
		@param rmin all r<rmin are set to rmin to avoid singularity in center
		@param rmax clamps entities father than rmax
		@param r radius at which impulsive pressure is spesified
		@param impulsive_pressure_at_r impulsive pressure at r
		@param nOccRes resolution of occulision cubemap (0 to skip occlusion test)
		@param nGrow inflate dynamic objects' rasterized image by this amount
		@params rmin_occ subtract cube with this size (half length of its side) during rasterization
		@params pSkipEnts pointer to array of entities to skip
		@params nSkipEnts number of entities to skip
	*/
	virtual void SimulateExplosion(pe_explosion *pexpl, IPhysicalEntity **pSkipEnts=0,int nSkipEnts=0,
		int iTypes=ent_rigid|ent_sleeping_rigid|ent_living|ent_independent, int iCaller=1) = 0;
	void SimulateExplosion(Vec3 epicenter,Vec3 epicenterImp, float rmin,float rmax, float r,float impulsivePressureAtR, 
		int nOccRes=0,int nGrow=0,float rminOcc=0.1f, IPhysicalEntity **pSkipEnts=0,int nSkipEnts=0, 
		int iTypes=ent_rigid|ent_sleeping_rigid|ent_living|ent_independent) 
	{ 
		pe_explosion expl;
		expl.epicenter=epicenter; expl.epicenterImp=epicenterImp; expl.rmin=rmin; expl.rmax=rmax; expl.r=r;
		expl.impulsivePressureAtR=impulsivePressureAtR; expl.nOccRes=nOccRes; expl.nGrow=nGrow; expl.rminOcc=rminOcc;
		SimulateExplosion(&expl,pSkipEnts,nSkipEnts,iTypes);
	}
	virtual int DeformPhysicalEntity(IPhysicalEntity *pent, const Vec3 &ptHit,const Vec3 &dirHit,float r, int flags=0) = 0;
	virtual void UpdateDeformingEntities(float time_interval=0.01f) = 0;
	virtual float CalculateExplosionExposure(pe_explosion *pexpl, IPhysicalEntity *pient) = 0;

	/*! Returns fraction of pent (0-1) that was exposed to the last explosion
	*/
	virtual float IsAffectedByExplosion(IPhysicalEntity *pent, Vec3 *impulse=0) = 0;

	virtual int AddExplosionShape(IGeometry *pGeom, float size,int idmat, float probability=1.0f) = 0;
	virtual void RemoveExplosionShape(int id) = 0;

	virtual void DrawPhysicsHelperInformation(IPhysRenderer *pRenderer,int iCaller=1) = 0;

	virtual int CollideEntityWithBeam(IPhysicalEntity *_pent, Vec3 org,Vec3 dir,float r, ray_hit *phit) = 0;
	virtual int CollideEntityWithPrimitive(IPhysicalEntity *_pent, int itype, primitives::primitive *pprim, Vec3 dir, ray_hit *phit) = 0;
	virtual int RayTraceEntity(IPhysicalEntity *pient, Vec3 origin,Vec3 dir, ray_hit *pHit, pe_params_pos *pp=0) = 0;

	// PrimitiveWorldIntersection 
	// Summary: 
	//		can perform sweep/overlap/or custom intersection check for a given primitive
	//		for sweep tests, affected entities are selected from the bounding box of the swept volume, so it's ineffective for long sweeps
	// Params:
	//		itype,pprim - primitive type and data (cannot be tri mesh or heightfield)
	//		sweepDir - self-explanatory
	//		entTypes - entity types to check; add rwi_queue flag to queue the request
	//		ppcontact - pointer to the pointer to the resulting contacts array
	//    geomFlagsAll - flags that must all be present in an entity part
	//		geomFlagsAny - flags at least one of which must be present in an entity part
	//    pip - custom intersection parameters, overrides the sweepDir setting if any.
	//			if not specified, the function performs a simple true/false overlap check if sweepDir is 0, and a sweep check otherwise
	//			if specified and pip->bThreadSafe==false, the caller must manually release the lock in pip->plock (but only if there were any contacts)
	// Returns:
	//		distance to the first hit for sweep checks and the number of hits for intersection checks (as float)
	virtual float PrimitiveWorldIntersection(int itype, primitives::primitive *pprim, const Vec3 &sweepDir=Vec3(ZERO), int entTypes=ent_all, 
		geom_contact **ppcontact=0, int geomFlagsAll=0,int geomFlagsAny=geom_colltype0|geom_colltype_player, intersection_params *pip=0,
		void *pForeignData=0, int iForeignData=0, IPhysicalEntity **pSkipEnts=0,int nSkipEnts=0, const char *pNameTag=PWI_NAME_TAG) = 0;

	virtual void GetMemoryStatistics(ICrySizer *pSizer) = 0;

	virtual void SetPhysicsStreamer(IPhysicsStreamer *pStreamer) = 0;
	virtual void SetPhysicsEventClient(IPhysicsEventClient *pEventClient) = 0;
	virtual float GetLastEntityUpdateTime(IPhysicalEntity *pent) = 0;
	virtual int GetEntityProfileInfo(phys_profile_info *&pList) = 0;
	virtual int GetFuncProfileInfo(phys_profile_info *&pList) = 0;

	virtual void AddEventClient(int type, int (*func)(const EventPhys*), int bLogged, float priority=1.0f) = 0;
	virtual int RemoveEventClient(int type, int (*func)(const EventPhys*), int bLogged) = 0;
	virtual void PumpLoggedEvents() = 0;
	virtual void ClearLoggedEvents() = 0;

	virtual IPhysicalEntity *AddGlobalArea() = 0;
	virtual IPhysicalEntity *AddArea(Vec3 *pt,int npt, float zmin,float zmax, const Vec3 &pos=Vec3(0,0,0), const quaternionf &q=quaternionf(IDENTITY),
		float scale=1.0f, const Vec3 &normal=Vec3(ZERO), int *pTessIdx=0,int nTessTris=0,Vec3 *pFlows=0) = 0;
	virtual IPhysicalEntity *AddArea(IGeometry *pGeom, const Vec3& pos,const quaternionf &q,float scale) = 0;
	virtual void RemoveArea(IPhysicalEntity *pArea) = 0;
	virtual IPhysicalEntity *AddArea(Vec3 *pt,int npt, float r, const Vec3 &pos=Vec3(0,0,0),const quaternionf &q=quaternionf(IDENTITY),float scale=1) = 0;
	// GetNextArea: iterates through all registered areas, if prevarea==0 returns the global area
	virtual IPhysicalEntity *GetNextArea(IPhysicalEntity *pPrevArea=0) = 0;
	// Checks areas for a given point
	virtual int CheckAreas(const Vec3 &ptc, Vec3 &gravity, pe_params_buoyancy *pb, int nMaxBuoys=1, const Vec3 &vec=Vec3(ZERO), 
		IPhysicalEntity *pent=0, int iCaller=1) = 0;

	virtual void SetWaterMat(int imat) = 0;
	virtual int GetWaterMat() = 0;
	virtual int SetWaterManagerParams(pe_params *params) = 0;
	virtual int GetWaterManagerParams(pe_params *params) = 0;
	virtual int GetWatermanStatus(pe_status *status) = 0;
	virtual void DestroyWaterManager() = 0;

	virtual volatile int *GetInternalLock(int idx) = 0; // returns one of phys_lock locks

	virtual int SerializeWorld(const char *fname, int bSave) = 0;
	virtual int SerializeGeometries(const char *fname, int bSave) = 0;

	virtual void SerializeGarbageTypedSnapshot( TSerialize ser, int iSnapshotType, int flags ) = 0;
};

#endif
