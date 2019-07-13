////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IMaterialEffects.h
//  Version:     v1.00
//  Created:     26/7/2006 by JohnN/AlexL.
//  Compilers:   Visual Studio.NET 2003
//  Description: Interface to the Material Effects System
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IMATERIALEFFECTS_H__
#define __IMATERIALEFFECTS_H__
#pragma once

#include "ISound.h"

struct IEntityClass;
struct ISurfaceType;

//////////////////////////////////////////////////////////////////////////
enum EMFXPlayFlags
{
	MFX_PLAY_SOUND = BIT(1),
	MFX_PLAY_DECAL = BIT(2),
	MFX_PLAY_PARTICLES = BIT(3),
	MFX_DISABLE_DELAY = BIT(4),
	MFX_PLAY_FLOWGRAPH = BIT(5),
	MFX_PLAY_ALL = (MFX_PLAY_SOUND | MFX_PLAY_DECAL | MFX_PLAY_PARTICLES | MFX_PLAY_FLOWGRAPH),
};

#define MFX_INVALID_ANGLE (gf_PI2+1)

//////////////////////////////////////////////////////////////////////////
struct SMFXSoundEffectParam
{
	SMFXSoundEffectParam()
	{
		paramName = "";
		paramValue = 0.0f;
	}
	const char *paramName;
	float paramValue;
};

//////////////////////////////////////////////////////////////////////////
struct SMFXParticleEffectParams
{  
  SMFXParticleEffectParams()
  {
    minscale = 0.f;
    maxscale = 0.f;
    maxscaledist = 0.f;
  }
  bool Filled() { return (minscale * maxscale * maxscaledist) > 0.f; }

	float minscale;
	float maxscale;
	float maxscaledist;
};

//////////////////////////////////////////////////////////////////////////
struct SMFXRunTimeEffectParams 
{
	static const int MAX_SOUND_PARAMS = 4;
	SMFXRunTimeEffectParams() :
		inWater(false),
		inZeroG(false),
		playSoundFP(false),
		soundNoObstruction(false),
		playflags(MFX_PLAY_ALL),
		fLastTime(0.0f),
		src(0),
		trg(0),
		srcSurfaceId(0),
		trgSurfaceId(0),
		srcRenderNode(0),
		trgRenderNode(0),
		partID(0),
		pos(ZERO),
		decalPos(ZERO),
		normal(0.0f,0.0f,1.0f),
		angle(MFX_INVALID_ANGLE),			
		scale(1.0f),
		soundSemantic(eSoundSemantic_Physics_General),
		soundProxyEntityId(0),
		soundProxyOffset(ZERO),
		soundDistanceMult(1.0f),
		numSoundParams(0)
	{
		dir[0].Set(0.0f,0.0f,-1.0f);
		dir[1].Set(0.0f,0.0f, 1.0f);
	}

	bool AddSoundParam(const char* name, float val)
	{
		if (numSoundParams < MAX_SOUND_PARAMS)
		{
			soundParams[numSoundParams].paramName = name;
			soundParams[numSoundParams].paramValue = val;
			++numSoundParams;
			return true;
		}
		return false;
	}

	void ResetSoundParams()
	{
		numSoundParams = 0;
	}

public:
	uint16 inWater : 1; // effect medium in in water
	uint16 inZeroG : 1; // effect medium is in zeroG
	uint16 playSoundFP : 1; // play sound as 2D (temp until Tomas fixes!), appends "_fp" to soundname
	uint16 soundNoObstruction : 1; // don't use obstruction on sound
	uint16 padding : 12;

	uint16 playflags;     // see EMFXPlayFlags
	float	 fLastTime;		  // last time this effect was played

	EntityId     src;
	EntityId     trg;
	int          srcSurfaceId;
	int          trgSurfaceId;
	IRenderNode* srcRenderNode;
	IRenderNode* trgRenderNode;
	int          partID;

	Vec3  pos;
	Vec3  decalPos;
	Vec3  dir[2];
	Vec3  normal;
	float angle;  
	float scale;

	// particle related
	SMFXParticleEffectParams particleParams;

	// sound related
	ESoundSemantic soundSemantic;
	EntityId soundProxyEntityId;  // if set, uses this Entity's sound proxy to play the sound. otherwise creates independent sound
	Vec3     soundProxyOffset;        // in case of sound proxy, uses this offset
	float    soundDistanceMult;
	int      numSoundParams;
	SMFXSoundEffectParam soundParams[MAX_SOUND_PARAMS];
};

enum MFXParticleDirection {
	MFX_PART_DIRECTION_NORMAL = 0,
	MFX_PART_DIRECTION_RICOCHET
};

struct SMFXBreakageParams 
{
	enum EBreakageRequestFlags
	{
		eBRF_Matrix							= BIT(0),
		eBRF_HitPos							= BIT(1),
		eBRF_HitImpulse					= BIT(2),
		eBRF_Velocity           = BIT(3),
		eBRF_ExplosionImpulse		= BIT(4),
		eBRF_Mass               = BIT(5),
		eBFR_Entity             = BIT(6),
	};

	SMFXBreakageParams() : 
		m_flags(0),
		m_worldTM(IDENTITY),
		m_vHitPos(ZERO),
		m_vHitImpulse(IDENTITY),
		m_vVelocity(ZERO),
		m_fExplosionImpulse(1.0f),
		m_fMass(0.0f),
		m_nEntityId(0)
	{
	}


	// Matrix
	void SetMatrix(const Matrix34& worldTM)
	{
		m_worldTM = worldTM;
		SetFlag(eBRF_Matrix);
	}

	const Matrix34& GetMatrix() const
	{
		return m_worldTM;
	}

	// HitPos
	void SetHitPos(const Vec3& vHitPos)
	{
		m_vHitPos = vHitPos;
		SetFlag(eBRF_HitPos);
	}

	const Vec3& GetHitPos() const
	{
		return m_vHitPos;
	}

	// HitImpulse
	void SetHitImpulse(const Vec3& vHitImpulse)
	{
		m_vHitImpulse = vHitImpulse;
		SetFlag(eBRF_HitImpulse);
	}

	const Vec3& GetHitImpulse() const
	{
		return m_vHitImpulse;
	}

	// Velocity
	void SetVelocity(const Vec3& vVelocity)
	{
		m_vVelocity = vVelocity;
		SetFlag(eBRF_Velocity);
	}

	const Vec3& GetVelocity() const
	{
		return m_vVelocity;
	}

	// Explosion Impulse
	void SetExplosionImpulse(float fExplosionImpulse)
	{
		m_fExplosionImpulse = fExplosionImpulse;
		SetFlag(eBRF_ExplosionImpulse);
	}

	float GetExplosionImpulse() const
	{
		return m_fExplosionImpulse;
	}

	// Mass
	void SetMass(float fMass)
	{
		m_fMass = fMass;
		SetFlag(eBRF_Mass);
	}

	float GetMass() const
	{
		return m_fMass;
	}

	// Entity
	void SetEntityId(EntityId id)
	{
		m_nEntityId = id;
		SetFlag(eBFR_Entity);
	}

	EntityId GetEntityId() const
	{
		return m_nEntityId;
	}

	// Checking for flags
	bool CheckFlag(EBreakageRequestFlags flag) const
	{
		return (m_flags & flag) != 0;
	}

protected:
	void SetFlag(EBreakageRequestFlags flag)
	{
		m_flags |= flag;
	}

	void ClearFlag(EBreakageRequestFlags flag)
	{
		m_flags &= ~flag;
	}

	uint32   m_flags;
	Matrix34 m_worldTM;
	Vec3     m_vHitPos;
	Vec3     m_vHitImpulse;
	Vec3     m_vVelocity;
	float    m_fExplosionImpulse;
	float    m_fMass;
	EntityId m_nEntityId;
};

class IMFXParticleParams 
{
public:
	IMFXParticleParams()
	{
		name = NULL;
	}

	const char *name;
  const char *userdata;
	float scale;
	MFXParticleDirection directionType;
};

class SMFXParticleListNode 
{
public:
	static SMFXParticleListNode * Create();
	void Destroy();

	IMFXParticleParams m_particleParams;
	SMFXParticleListNode *pNext;

private:
	SMFXParticleListNode() 
	{
		pNext = NULL;
	}
	~SMFXParticleListNode() {}
};

class IMFXSoundParams 
{
public:
	IMFXSoundParams()
	{
		name = NULL;
	}
	const char *name;
};

class SMFXSoundListNode 
{
public:
	static SMFXSoundListNode * Create();
	void Destroy();

	IMFXSoundParams m_soundParams;
	SMFXSoundListNode *pNext;

private:
	SMFXSoundListNode() 
	{
		pNext = NULL;
	}
	~SMFXSoundListNode() {}
};

class IMFXFlowGraphParams 
{
public:
	IMFXFlowGraphParams()
	{
		name = NULL;
	}
	const char *name;
};

class SMFXFlowGraphListNode 
{
public:
	static SMFXFlowGraphListNode * Create();
	void Destroy();

	IMFXFlowGraphParams m_flowGraphParams;
	SMFXFlowGraphListNode *pNext;

private:
	SMFXFlowGraphListNode() 
	{
		pNext = NULL;
	}
	~SMFXFlowGraphListNode() {}
};

class IMFXDecalParams 
{
public:
	IMFXDecalParams()
	{
		filename = 0;
		material = 0;
    minscale = 1.f;
    maxscale = 1.f;
		lifetime = 10.0f;
	}
	const char *filename;
	const char *material;
	float minscale;
  float maxscale;
	float lifetime;
};

class SMFXDecalListNode 
{
public:
	static SMFXDecalListNode * Create();
	void Destroy();

	IMFXDecalParams m_decalParams;
	SMFXDecalListNode *pNext;

private:
	SMFXDecalListNode() 
	{
		pNext = 0;
	}
	~SMFXDecalListNode() {}
};

class SMFXResourceList;
typedef _smart_ptr<SMFXResourceList> SMFXResourceListPtr;

class SMFXResourceList
{
public:
	SMFXParticleListNode	*m_particleList;
	SMFXSoundListNode		*m_soundList;
	SMFXDecalListNode		*m_decalList;
	SMFXFlowGraphListNode *m_flowGraphList;

	void AddRef() { ++m_refs; }
	void Release() { if (--m_refs <= 0) Destroy(); }

	static SMFXResourceListPtr Create();

private:
	int m_refs;

	virtual void Destroy();

	SMFXResourceList() : m_refs(0)
	{
		m_particleList = 0;
		m_soundList = 0;
		m_decalList = 0;
		m_flowGraphList = 0;
	}
	~SMFXResourceList()
	{
		while (m_particleList != 0)
		{
			SMFXParticleListNode *next = m_particleList->pNext;
			m_particleList->Destroy();
			m_particleList = next;
		}
		while (m_soundList != 0)
		{
			SMFXSoundListNode *next = m_soundList->pNext;
			m_soundList->Destroy();
			m_soundList = next;
		}
		while (m_decalList != 0)
		{
			SMFXDecalListNode *next = m_decalList->pNext;
			m_decalList->Destroy();
			m_decalList = next;
		}
		while (m_flowGraphList != 0)
		{
			SMFXFlowGraphListNode *next = m_flowGraphList->pNext;
			m_flowGraphList->Destroy();
			m_flowGraphList = next;
		}
	}
};

typedef uint16 TMFXEffectId;
static const TMFXEffectId InvalidEffectId = 0;

//////////////////////////////////////////////////////////////////////////
struct IMaterialEffects 
{
public:
	virtual void Reset() = 0;
	virtual TMFXEffectId GetEffectIdByName(const char* libName, const char* effectName) = 0;
	virtual TMFXEffectId GetEffectId(int surfaceIndex1, int surfaceIndex2) = 0;
	virtual TMFXEffectId GetEffectId(const char* customName, int surfaceIndex2) = 0;
	virtual TMFXEffectId GetEffectId(IEntityClass* pEntityClass, int surfaceIndex2) = 0;
	virtual SMFXResourceListPtr GetResources(TMFXEffectId effectId) = 0;
	virtual void PreLoadAssets() = 0;
	virtual bool ExecuteEffect(TMFXEffectId effectId, SMFXRunTimeEffectParams& runtimeParams) = 0;
	virtual void StopEffect(TMFXEffectId effectId) = 0;
	virtual	int	GetDefaultSurfaceIndex()=0;
	virtual	int	GetDefaultCanopyIndex()=0;

	virtual bool PlayBreakageEffect(ISurfaceType* pSurfaceType, const char* breakageType, const SMFXBreakageParams& mfxBreakageParams) = 0;
};

#endif // __IMATERIALEFFECTS_H__