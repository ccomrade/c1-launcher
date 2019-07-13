////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ParticleParams.h
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef _PARTICLEPARAMS_H_
#define _PARTICLEPARAMS_H_ 1

#include <ISplines.h>
#include <Cry_Color.h>

#define LOOKUP_SPLINE	1

enum EParticleFacing
{
	ParticleFacing_Camera,
	ParticleFacing_Free,
	ParticleFacing_Velocity,
	ParticleFacing_Horizontal,
	ParticleFacing_Water,
	ParticleFacing_Terrain,
};

enum EParticlePhysicsType
{
	ParticlePhysics_None,
	ParticlePhysics_SimpleCollision,
	ParticlePhysics_SimplePhysics,
	ParticlePhysics_RigidBody
};

enum EParticleForceType
{
	ParticleForce_None,
	ParticleForce_Wind,
	ParticleForce_Gravity,
	ParticleForce_Target,
};

enum ETrinary
{
	Trinary_Both,
	Trinary_If_True,
	Trinary_If_False
};

inline bool TrinaryMatch(ETrinary t1, ETrinary t2)
{
	return (t1 | t2) != 3;
}

inline bool TrinaryMatch(ETrinary t, bool b)
{
	return t != (Trinary_If_True + b);
}

enum EConfigSpecBrief
{
	ConfigSpec_Low,
	ConfigSpec_Medium,
	ConfigSpec_High,
	ConfigSpec_VeryHigh,
};

// Pseudo-random number generation, from a key.
class CChaosKey
{
public:
	// Initialize with an int.
	explicit inline CChaosKey(uint32 uSeed)
		: m_Key(uSeed) {}

	explicit inline CChaosKey(float fSeed)
		: m_Key((uint32)(fSeed * float(0xFFFFFFFF))) {}

	CChaosKey Jumble(CChaosKey key2) const
	{
		return CChaosKey( Jumble(m_Key ^ key2.m_Key) );
	}
	CChaosKey Jumble(void const* ptr) const
	{
		return CChaosKey( Jumble(m_Key ^ (uint32)ptr) );
	}

	// Scale input range.
	inline float operator *(float fRange) const
	{
		return (float)m_Key / float(0xFFFFFFFF) * fRange;
	}
	inline uint operator *(uint nRange) const
	{
		return m_Key % nRange;
	}

	uint32 GetKey() const
	{
		return m_Key;
	}

	// Jumble with a range variable to produce a random value.
	template<class T>
	inline T Random(T const* pRange) const
	{
		return Jumble(CChaosKey(uint32(pRange))) * *pRange;
	}

private:
	uint32	m_Key;

	static inline uint32 Jumble(uint32 key)
	{
		key += ~rot_left(key, 15);
		key ^= rot_right(key, 10);
		key += rot_left(key, 3);
		key ^= rot_right(key, 6);
		key += ~rot_left(key, 11);
		key ^= rot_right(key, 16);
		return key;
	}

	static inline uint32 rot_left(uint32 u, int n)
	{
		return (u<<n) | (u>>(32-n));
	}
	static inline uint32 rot_right(uint32 u, int n)
	{
		return (u>>n) | (u<<(32-n));
	}
};

//
// Custom behavior for param types.
//

template<class T> struct TVarTraits
{
	typedef float	TRandom;

	// Default small storage class for scalars.
	typedef uint8	TStorage;

	static TStorage ToStorage( T const& val )
		{ return float_to_ufrac8(val); }
	static void FromStorage( T& val, TStorage const& store )
		{ val = ufrac8_to_float(store); }
	static void InterpFromStorage( T& val, TStorage const& a, TStorage const& b, float fLerp )
	{ 
		val = ufrac8_to_float( a*(1.f-fLerp) + b*fLerp );
	}
};


//
// Color specialisations.
//

inline ColorF min( const ColorF& a, const ColorF& b )
{
	return ColorF( min(a.r,b.r), min(a.g,b.g), min(a.b,b.b), min(a.a,b.a) );
}

inline ColorF max( const ColorF& a, const ColorF& b )
{
	return ColorF( max(a.r,b.r), max(a.g,b.g), max(a.b,b.b), max(a.a,b.a) );
}

inline ColorF minmag( const ColorF& a, const ColorF& b )
{
	return ColorF( minmag(a.r,b.r), minmag(a.g,b.g), minmag(a.b,b.b), minmag(a.a,b.a) );
}

class RandomColor
{
public:
	RandomColor(float f = 0.f)
		: m_fVarRandom(f), m_bRandomHue(false)
	{}
	operator float() const
	{
		return m_fVarRandom;
	}

	ColorF GetRandom() const
	{
		if (m_bRandomHue)
		{
			ColorB clr(cry_rand32());
			float fScale = m_fVarRandom / 255.f;
			return ColorF(clr.r * fScale, clr.g * fScale, clr.b * fScale, 0.f);
		}
		else
		{
			return ColorF(Random(m_fVarRandom));
		}
	}

	AUTO_STRUCT_INFO_LOCAL

protected:
	float	m_fVarRandom;				// $<Min=0> $<Max=1>
	bool	m_bRandomHue;
};

inline ColorF Random(RandomColor const& rc)
{
	return rc.GetRandom();
}


template<> struct TVarTraits<ColorF>
{
	typedef RandomColor	TRandom;

	typedef ColorB TStorage;
	static TStorage ToStorage( ColorF const& val )
	{
		return TStorage
		(
			float_to_ufrac8(val.r),
			float_to_ufrac8(val.g), 
			float_to_ufrac8(val.b)
		);
	}
	static void FromStorage( ColorF& val, TStorage const& store )
	{ 
		val.r = ufrac8_to_float(store.r); 
		val.g = ufrac8_to_float(store.g); 
		val.b = ufrac8_to_float(store.b); 
	}
	static void InterpFromStorage( ColorF& val, TStorage const& a, TStorage const& b, float fLerp )
	{
		float fILerp = 1.f - fLerp;
		val.r = ufrac8_to_float(a.r*fILerp + b.r*fLerp);
		val.g = ufrac8_to_float(a.g*fILerp + b.g*fLerp);
		val.b = ufrac8_to_float(a.b*fILerp + b.b*fLerp);
	}
};

//
// Spline implementation class.
//

template<class T>
class TCurveSpline: public spline::CBaseSplineInterpolator< 
#if LOOKUP_SPLINE
	T, spline::LookupTableSpline< T, TVarTraits<T>, spline::SplineKey<T> > 
#else
	T, spline::CatmullRomSpline< T, spline::SplineKey<T>, true > 
#endif
>
{
public:

	// Implement serialisation for particles.
	string ToString( int flags ) const;
	void FromString( const char* str );
	virtual void SerializeSpline( XmlNodeRef &node, bool bLoading );
};

template<class T>
class TCurve
{
public:

	TCurve()
		: m_pSpline(NULL) 
	{}

	~TCurve()
	{
		delete m_pSpline;
	}

	TCurve(TCurve<T> const& c)
	{
		m_pSpline = c.m_pSpline ? new TSpline(*c.m_pSpline) : NULL;
	}

	TCurve<T>& operator= (TCurve<T> const& c)
	{
		if (c.m_pSpline != m_pSpline)
		{
			delete m_pSpline;
			m_pSpline = c.m_pSpline ? new TSpline(*c.m_pSpline) : NULL;
		}
		return *this;
	}

	// Spline interface.
	ISplineInterpolator* GetSpline(bool bCreate)
	{
		if (!m_pSpline && bCreate)
			m_pSpline = new TSpline;
		return m_pSpline;
	}

	//
	// Read access
	//

	// Get the value at any key point from 0..1.
	inline T GetValue(float fTime) const
	{
		T Val(1.f);
		if (m_pSpline)
			m_pSpline->fast_interpolate( fTime, Val );
		return Val;
	}
	inline void ModValue(T& Val, float fTime) const
	{
		if (m_pSpline)
		{
			T CVal(1.f);
			m_pSpline->fast_interpolate( fTime, CVal );
			Val *= CVal;
		}
	}

	T GetMaxValue() const
	{
		return T(1.f);
	}
	T GetMinValue() const
	{
		T fMinVal(1.f);
		if (m_pSpline)
		{
			int nKeys = m_pSpline->num_keys();
			for (int i = 0; i < nKeys; i++)
			{
				T fVal = m_pSpline->value(i);
				fMinVal = min(fMinVal, fVal);
			}
		}
		return fMinVal;
	}
	bool IsIdentity() const
	{
		return m_pSpline == NULL;
	}

	//
	// Write access.
	//

	// Add or replace a key/value pair.
	void AddPoint( int nDegree, float fTime, T val )
	{
		if (!m_pSpline)
			m_pSpline = new TSpline;
		int nKey = m_pSpline->FindKey(fTime, .001f);
		if (nKey >= 0)
			m_pSpline->value(nKey) = val;
		else
		{
			typename TSpline::key_type key;
			memset(&key, 0, sizeof(key));
			key.time = fTime;
			key.value = val;
			key.flags = (nDegree < 2) ? SPLINE_KEY_NONCONTINUOUS_SLOPE : 0;
			m_pSpline->insert_key( key );
			m_pSpline->sort_keys();
		}
		m_pSpline->SetModified(true);
		m_pSpline->update();
	}

	void ClearPoints()
	{
		delete m_pSpline;
		m_pSpline = NULL;
	}

	// Serialisation.
	string ToString(int flags) const
	{
		if (IsIdentity())
			return string();
		return m_pSpline->ToString(flags);
	}

	void FromString( const char* str )
	{
		delete m_pSpline;
		if (!*str)
			m_pSpline = NULL;
		else
		{
			m_pSpline = new TSpline;
			m_pSpline->FromString( str );
			CleanUp();
		}
	}

	size_t GetMemoryUsage() const
	{
		if (m_pSpline)
			return sizeof(*m_pSpline) + m_pSpline->mem_size();
		else
			return 0;
	}

	// TypeInfo explicitly implemented in ParticleParamsTypeInfo.cpp.
	STRUCT_INFO

protected:

	void CleanUp()
	{
		if (GetMinValue() == T(1.f))
		{
			delete m_pSpline;
			m_pSpline = NULL;
		}
	}

	typedef TCurveSpline<T> TSpline;
	TSpline*	m_pSpline;
};

//
// Composite parameter types, incorporating base value, randomness, and lifetime curves.
//

template<class T>
class TVarParam
{
protected:
	typedef typename TVarTraits<T>::TRandom TRandom;
	typedef typename TVarTraits<T>::TStorage TStorage;

public:
	// Construction.
	TVarParam()
		: m_Base(), m_VarRandom(0.f)
	{
	}

	TVarParam(const T& tBase)
		: m_Base(tBase), m_VarRandom(0.f)
	{
	}

	inline void SetBase(const T& t)
	{ 
		m_Base = t; 
	}

	// Convenient function to set a linear random variation curve.
	void SetRandomVar(TRandom const& RandVar)
	{
		m_VarRandom = RandVar;
	}

	//
	// Value extraction.
	//

	// Uses a chaos key for randomisation in every dimension of T.
	inline T GetBase() const
	{
		return m_Base;
	}

	inline TRandom const& GetRandom() const
	{
		return m_VarRandom;
	}

	// Uses a chaos key for randomisation in every dimension of T.
	inline T GetVarValue(CChaosKey keyRan) const
	{
		if ((float)m_VarRandom == 0.f)
			return m_Base;
		else
			return m_Base - keyRan.Jumble(this) * m_VarRandom * m_Base;
	}

	inline T GetMaxValue() const
	{
		return m_Base;
	}

	T GetMinValue() const
	{
		return m_Base * (1.f-(float)m_VarRandom);
	}

	AUTO_STRUCT_INFO_LOCAL

protected:
	// Base value.
	T						m_Base;									

	// Variations, all multiplicative.
	TRandom			m_VarRandom;						// $<Inline=1> $<Min=0> $<Max=1>
};


template<class T>
class TVarEParam: public TVarParam<T>
{
public:

	// Construction.
	TVarEParam()
	{
	}

	TVarEParam(const T& tBase)
		: TVarParam<T>(tBase)
	{
	}

	//
	// Value extraction.
	//

	inline T GetVarMod(float fELife) const
	{
		T val = T(1.f);
		if ((float)m_VarRandom != 0.f)
			val -= Random(m_VarRandom) * val;
		m_VarEmitterLife.ModValue(val, fELife);
		return val;
	}

	inline T GetVarValue(float fELife) const
	{
		return GetVarMod(fELife) * m_Base;
	}

	inline T GetVarValue(float fMinMax, float fELife) const
	{
		T val = m_Base;
		val -= (1.f-fMinMax) * m_VarRandom * val;
		m_VarEmitterLife.ModValue(val, fELife);
		return val;
	}

	inline T GetVarValue(CChaosKey keyRan, float fELife, T Abs = T(0.f)) const
	{
		T val = m_Base;
		if ((float)m_VarRandom != 0.f)
		{
			if (Abs != T(0.f))
				val += keyRan.Jumble(this) * m_VarRandom * Abs;
			else
				val -= keyRan.Jumble(this) * m_VarRandom * val;
		}
		m_VarEmitterLife.ModValue(val, fELife);
		return val;
	}

	T GetMinValue() const
	{
		return TVarParam<T>::GetMinValue()
			* m_VarEmitterLife.GetMinValue();
	}

	AUTO_STRUCT_INFO_LOCAL

protected:
	TCurve<T>		m_VarEmitterLife;

	// Dependent name nonsense.
	using TVarParam<T>::m_Base;
	using TVarParam<T>::m_VarRandom;
};

template<class T>
class TVarEPParam: public TVarEParam<T>
{
public:
	// Construction.
	TVarEPParam()
	{
	}

	TVarEPParam(const T& tBase)
		: TVarEParam<T>(tBase)
	{
	}

	//
	// Value extraction.
	//

	inline T GetValueFromBase( T val, float fParticleAge ) const
	{
		m_VarParticleLife.ModValue(val, fParticleAge);
		return val;
	}

	inline T GetValueFromMod( T val, float fParticleAge ) const
	{
		m_VarParticleLife.ModValue(val, fParticleAge);
		return m_Base * val;
	}

	T GetMinValue() const
	{
		return TVarEParam<T>::GetMinValue()
			* m_VarParticleLife.GetMinValue();
	}

	AUTO_STRUCT_INFO_LOCAL

protected:
	TCurve<T>		m_VarParticleLife;

	// Dependent name nonsense.
	using TVarEParam<T>::m_Base;
	using TVarEParam<T>::m_VarRandom;
	using TVarEParam<T>::m_VarEmitterLife;
};

struct STextureTiling
{
	uint	nTilesX, nTilesY;		// $<Min=1> Number of tiles texture is split into 
	uint	nFirstTile;					// $<Min=0> First (or only) tile to use.
	uint	nVariantCount;			// $<Min=1> Number of randomly selectable tiles; 0 or 1 if no variation.
	uint	nAnimFramesCount;		// $<Min=1> Number of tiles (frames) of animation; 0 or 1 if no animation.
	float	fAnimFramerate;			// $<Min=0> $<SoftMax=60> Tex framerate; 0 = 1 cycle / particle life.
	bool	bAnimCycle;					// Whether animation cycles, or holds last frame.

	STextureTiling()
	{
		nFirstTile = 0;
		fAnimFramerate = 0.f;
		bAnimCycle = false;
		nTilesX = nTilesY = nVariantCount = nAnimFramesCount = 1;
	}

	uint GetFrameCount() const
	{
		return nTilesX * nTilesY - nFirstTile;
	}

	bool IsValid() const
	{
		return nTilesX >= 1 && nTilesY >= 1 && nVariantCount >= 1 && nAnimFramesCount >= 1
			&& nVariantCount * nAnimFramesCount <= GetFrameCount();
	}

	void Correct()
	{
		nTilesX = max(nTilesX, 1U);
		nTilesY = max(nTilesY, 1U);
		nAnimFramesCount = max(nAnimFramesCount, 1U);
		nVariantCount = max(nVariantCount, 1U);
	}

	AUTO_STRUCT_INFO_LOCAL
};

struct Vec3_Zero: Vec3
{
	Vec3_Zero() : Vec3(ZERO) {}
	Vec3_Zero(const Vec3& v) : Vec3(v) {}
};

//! Particle system parameters
struct ParticleParams
{
	// Emitter params (eVar_ParticleLife unused).
	bool bEnabled;										// Set false to disable this effect.
	bool bContinuous;									// Emit particles gradually until nCount reached

	TVarEParam<float> fCount;					// $<Min=0> $<SoftMax=1000> Number of particles in system at once.
	TVarParam<float> fEmitterLifeTime;// $<Min=0> Min lifetime of the emitter, 0 if infinite. Always emits at least nCount particles.
	TVarParam<float> fSpawnDelay;			// $<SoftMin=0> $<SoftMax=10> Delay the actual spawn time by this value	
	TVarParam<float> fPulsePeriod;		// $<Min=0> Time between auto-restarts of finite systems. 0 if just once, or continuous.

	// Particle timing
	TVarEParam<float> fParticleLifeTime;	// $<Min=0> Lifetime of particle
	bool bRemainWhileVisible;					// Particles will only die when not rendered (by any viewport).

	// Appearance
	EParticleFacing eFacing;					// $<Group="Appearance"> The basic particle shape type.
	bool bOrientToVelocity;						// Rotate particle X axis in velocity direction.
	EParticleBlendType eBlendType;		// The blend parameters.
	string sTexture;									// Texture for particle.
	STextureTiling TextureTiling;		// Animation etc.
	string sMaterial;									// Material (overrides texture).
	string sGeometry;									// Geometry for 3D particles.
	bool bGeometryInPieces;						// Spawn geometry pieces separately.
	bool bSoftParticle;								// Enable soft particle processing in the shader.
  bool bOceanParticle;								// Enable soft particle processing in the shader.

	// Lighting
	TVarEPParam<float> fAlpha;				// $<Group="Lighting"> $<Min=0> $<Max=1> Alpha value (fade parameters).
	TVarEPParam<ColorF> cColor;				// Color modulation.
	float fDiffuseLighting;						// $<Min=0> $<SoftMax=1> Multiplier for particle dynamic lighting.
	float fDiffuseBacklighting;				// $<Min=0> $<Max=1> Fraction of diffuse lighting applied in all directions.
	float fEmissiveLighting;					// $<Min=0> $<SoftMax=1> Multiplier for particle emissive lighting.
	float fEmissiveHDRDynamic;				// $<Min=0> $<SoftMax=2> Power to apply to engine HDR multiplier for emissive lighting in HDR.
	bool bReceiveShadows;							// Shadows will cast on these particles.
	bool bCastShadows;								// Particles will cast shadows (currently only geom particles).
  TVarEPParam<float> fLightSourceIntensity;		// $<Min=0> $<SoftMax=16>
  TVarEPParam<float> fLightHDRDynamic;				// $<Min=-2> $<SoftMax=2>
  TVarEPParam<float> fLightSourceRadius;			// $<Min=0> $<SoftMax=1>

	// Size
	TVarEPParam<float> fSize;					// $<Group="Size"> $<Min=0> $<SoftMax=10> Particle size.
	TVarEPParam<float> fTailLength;		// $<Min=0> $<SoftMax=10> Delay of tail ( 0 - no tail, 1 meter if speed is 1 meter/sec
	uint nTailSteps;									// $<Min=0> $<SoftMax=16> How many tail steps particles have.
	TVarEPParam<float> fStretch;			// $<Min=0> $<SoftMax=10> Stretch particle into moving direction
	float fStretchOffsetRatio;				// Move particle center this fraction in direction of stretch.
	float fMinPixels;									// $<Min=0> $<SoftMax=10> Augment true size with this many pixels.

	// Spawning
	bool bSecondGeneration;						// $<Group="Spawning"> Emitters tied to each parent particle.
	bool bSpawnOnParentCollision;			// (Second Gen only) Spawns when parent particle collides.
	float fInheritVelocity;						// $<SoftMin=0> $<SoftMax=1> Fraction of emitter's velocity to inherit.
	EGeomType eAttachType;						// Where to emit from attached geometry.
	EGeomForm eAttachForm;						// How to emit from attached geometry.
	Vec3_Zero vPositionOffset;				// Spawn Position offset from the effect spawn position
	Vec3_Zero vRandomOffset;					// Random offset of the particle relative position to the spawn position
	float fPosRandomOffset;						// Radial random offset.

	// Movement
	TVarEParam<float> fSpeed;					// $<Group="Movement"> Initial speed of a particle.
	TVarEPParam<float> fAirResistance;// $<Min=0> $<SoftMax=10> Air drag value, in inverse seconds.
	TVarEPParam<float> fGravityScale;	// $<SoftMin=0> $<SoftMax=1> Multiplier for world gravity.
	Vec3_Zero vAcceleration;					// Specific world-space acceleration vector.
	TVarEPParam<float> fTurbulence3DSpeed;	// $<Min=0> $<SoftMax=10> 3D random turbulence force
	TVarEPParam<float> fTurbulenceSize;			// $<Min=0> $<SoftMax=10> Radius of turbulence
	TVarEPParam<float> fTurbulenceSpeed;		// $<SoftMin=-360> $<SoftMax=360> Speed of rotation

	// Angles
	TVarEParam<float>	fFocusAngle;		// $<Group="Angles"> $<Min=0> $<Max=180> Angle to vary focus from default (Y axis), for variation.
	TVarEParam<float>	fFocusAzimuth;	// $<SoftMax=360> Angle to rotate focus about default, for variation. 0 = Z axis.
	bool bFocusGravityDir;						// Uses negative gravity dir, rather than emitter Y, as focus dir.
	TVarEParam<float> fEmitAngle;			// $<Min=0> $<Max=180> Angle from focus dir (emitter Y), in degrees. RandomVar determines min angle.
	Vec3_Zero vInitAngles;						// $<SoftMin=-180> $<SoftMax=180> Initial rotation in symmetric angles (degrees).
	Vec3_Zero vRandomAngles;					// $<Min=0> $<Max=180> Bidirectional random angle variation.
	Vec3_Zero vRotationRate;					// $<SoftMin=-360> $<SoftMax=360> Rotation speed (degree/sec).
	Vec3_Zero vRandomRotationRate;		// $<Min=0> $<SoftMax=360> Random variation.

	// Physics
	EParticlePhysicsType ePhysicsType;// $<Group="Physics"> 
	bool bCollideTerrain;							// Collides with terrain (if Physics <> none)
	bool bCollideStaticObjects;				// Collides with static physics objects (if Physics <> none)
	bool bCollideDynamicObjects;			// Collides with dynamic physics objects (if Physics <> none)
	string sSurfaceType;							// $<Select="Surface"> Surface type for physicalised particles.
	float fBounciness;								// $<SoftMin=0> $<SoftMax=1> Elasticity: 0 = no bounce, 1 = full bounce, <0 = die.
	float fDynamicFriction;						// $<Min=0> $<SoftMax=10> Sliding drag value, in inverse seconds.
	float fThickness;									// $<Min=0> $<SoftMax=1> Lying thickness ratio - for physicalized particles only
	float fDensity;										// $<Min=0> $<SoftMax=2000> Mass density for physicslized particles.
	uint nMaxCollisionEvents;					// $<Min=0> $<SoftMax=10> Max # collision events generatable per frame per particle.
	EParticleForceType eForceGeneration;	// Generate physical forces if set.

	// Sound
	string sSound;										// $<Group="Sound"> Name of the sound file
	TVarEParam<float> fSoundFXParam;	// $<Min=0> $<Max=1> Custom real-time sound modulation parameter.

	// Advanced
	int nDrawLast;								// $<Group="Advanced"> Add this element into the second list and draw this list last
	bool bSortOldestFirst;				// Reverse sort order, oldest-to-newest.
	ETrinary tVisibleIndoors;			// Whether visible indoors/outdoors/both.
	ETrinary tVisibleUnderwater;	// Whether visible under/above water / both.
	bool bDisableVisClipping;			// Disable particle clipping against vis/water boundaries.
	bool bNotAffectedByFog;
	bool bIgnoreAttractor;				// Ignore ParticleTargets.
	bool bDrawNear;								// Render particle in near space (weapon)
	bool bNoOffset;								// Disable centering of static objects
	float fViewDistanceAdjust;		// $<Min=0> $<SoftMax=1> Multiplier to automatic distance fade-out.
	float fFillRateCost;					// $<Min=0> $<SoftMax=10> Adjustment to max screen fill allowed per emitter.
	float fMotionBlurScale;				// $<Min=0> $<SoftMax=2> Multiplier to motion blur.
	bool bBindEmitterToCamera;	
	float fCameraMaxDistance;			// $<Min=0> $<SoftMax=100> Max distance from camera to render particles.
	float fCameraMinDistance;			// $<Min=0> $<SoftMax=100> Min distance from camera to render particles.
	bool bBindToEmitter;					// Always keep particle position binded to position of the emitter (for rockets etc...)
	bool bMoveRelEmitter;					// Particle motion is in emitter space.
	bool bSpaceLoop;							// Lock particles in box around emitter position, use vSpaceLoopBoxSize to set box size
	bool bEncodeVelocity;					// Orient and encode velocity direction, for special shaders.

	// Configurations
	EConfigSpecBrief	eConfigMin;			// $<Group="Configuration"> 
	EConfigSpecBrief	eConfigMax;
	ETrinary tDX10;
	ETrinary tGPUComputation;
	ETrinary tMultiThread;

	ParticleParams() 
	{
		memset(this, 0, sizeof(*this));
		new(this) ParticleParams(true);
	}

	AUTO_STRUCT_INFO_LOCAL

protected:

	ParticleParams(bool)
	{
		bEnabled = true;
		fCount = 1;
		fParticleLifeTime = 1;
		fSize = 1;
		fSpeed = 1;
		cColor = ColorF(1.f);
		fAlpha = 1;
		fDiffuseLighting = 1;
		fViewDistanceAdjust = 1;
		fFillRateCost = 1;
		fDensity = 1000;
		fThickness = 1;
		fMotionBlurScale = 1;
		eConfigMax = ConfigSpec_VeryHigh;
	}
};

#endif
