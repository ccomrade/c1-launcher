////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   i3dengine.h
//  Version:     v1.00
//  Created:     28/5/2001 by Vladimir Kajalin
//  Compilers:   Visual Studio.NET
//  Description: 3dengine interface
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef CRY3DENGINEINTERFACE_H
#define CRY3DENGINEINTERFACE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef CRY3DENGINE_EXPORTS
	#define CRY3DENGINEENGINE_API DLL_EXPORT
#else
	#define CRY3DENGINEENGINE_API DLL_IMPORT
#endif

// !!! Do not add any headers here !!!
#include "CryEngineDecalInfo.h" 
#include "IStatObj.h"
#include "IRenderer.h"
#include "IProcess.h"
#include "IMaterial.h"
#include "ISurfaceType.h"
#include "IEntityRenderState.h"
#include "CryArray.h"
// !!! Do not add any headers here !!!

struct ISystem;
struct ICharacterInstance;
struct CVars;
struct pe_params_particle;
struct IMaterial;
struct RenderLMData;
struct AnimTexInfo;
struct ISplineInterpolator;
class CContentCGF;
struct SpawnParams;
struct ForceObject;
class I3DSampler;
struct IAutoCubeMapRenderNode;

/*!
IParticleEffect interface.
This Object with this interface is created by CreateParticleEffect method of 3d engine.
*/

enum E3DEngineParameter
{
	E3DPARAM_HDR_DYNAMIC_POWER_FACTOR,

	E3DPARAM_SUN_COLOR,
	E3DPARAM_SUN_COLOR_MULTIPLIER,

	E3DPARAM_SKY_COLOR,
	E3DPARAM_SKY_COLOR_MULTIPLIER,

	E3DPARAM_FOG_COLOR,

	E3DPARAM_VOLFOG_GLOBAL_DENSITY,
	E3DPARAM_VOLFOG_ATMOSPHERE_HEIGHT,

	E3DPARAM_SKYLIGHT_SUN_INTENSITY,
	E3DPARAM_SKYLIGHT_SUN_INTENSITY_MULTIPLIER,

	E3DPARAM_SKYLIGHT_KM,
	E3DPARAM_SKYLIGHT_KR,
	E3DPARAM_SKYLIGHT_G,

	E3DPARAM_SKYLIGHT_WAVELENGTH_R,
	E3DPARAM_SKYLIGHT_WAVELENGTH_G,
	E3DPARAM_SKYLIGHT_WAVELENGTH_B,

	E3DPARAM_NIGHSKY_HORIZON_COLOR,
	E3DPARAM_NIGHSKY_ZENITH_COLOR,
	E3DPARAM_NIGHSKY_ZENITH_SHIFT,

	E3DPARAM_NIGHSKY_STAR_INTENSITY,

	E3DPARAM_NIGHSKY_MOON_DIRECTION,
	E3DPARAM_NIGHSKY_MOON_COLOR,
	E3DPARAM_NIGHSKY_MOON_SIZE,
	E3DPARAM_NIGHSKY_MOON_INNERCORONA_COLOR,
	E3DPARAM_NIGHSKY_MOON_INNERCORONA_SCALE,
	E3DPARAM_NIGHSKY_MOON_OUTERCORONA_COLOR,
	E3DPARAM_NIGHSKY_MOON_OUTERCORONA_SCALE,

	E3DPARAM_CLOUDSHADING_SUNLIGHT_MULTIPLIER,
	E3DPARAM_CLOUDSHADING_SKYLIGHT_MULTIPLIER,

	E3DPARAM_CORONA_SIZE,

	E3DPARAM_OCEANFOG_COLOR_MULTIPLIER,

	// Sky highlight (ex. From Lightning)
	E3DPARAM_SKY_HIGHLIGHT_COLOR,
	E3DPARAM_SKY_HIGHLIGHT_SIZE,
	E3DPARAM_SKY_HIGHLIGHT_POS,

	E3DPARAM_SKY_SUNROTATION,
	E3DPARAM_SKY_MOONROTATION,

  EPARAM_SUN_SHAFTS_VISIBILITY,

	E3DPARAM_SKYBOX_MULTIPLIER,

	E3DPARAM_DAY_NIGHT_INDICATOR,

	// --------------------------------------

	E3DPARAM_EYEADAPTIONCLAMP,

  // --------------------------------------

  E3DPARAM_COLORGRADING_COLOR_SATURATION,
  E3DPARAM_COLORGRADING_FILTERS_PHOTOFILTER_COLOR,
  E3DPARAM_COLORGRADING_FILTERS_PHOTOFILTER_DENSITY,
  E3DPARAM_COLORGRADING_FILTERS_GRAIN

};

//! Particle Blend Type
enum EParticleBlendType
{
  ParticleBlendType_AlphaBased,
  ParticleBlendType_ColorBased,
  ParticleBlendType_Additive,
  ParticleBlendType_None
};

// Reference to one of several types of geometry, for particle attachment.
struct GeomRef
{
	IStatObj*	m_pStatObj;
	ICharacterInstance* m_pChar;
	IPhysicalEntity* m_pPhysEnt;

	GeomRef(): m_pStatObj(0), m_pChar(0), m_pPhysEnt(0) {}
	GeomRef(IStatObj* pObj) : m_pStatObj(pObj), m_pChar(0), m_pPhysEnt(0) {}
	GeomRef(ICharacterInstance* pChar) : m_pChar(pChar), m_pStatObj(0), m_pPhysEnt(0) {}
	GeomRef(IPhysicalEntity* pPhys) : m_pChar(0), m_pStatObj(0), m_pPhysEnt(pPhys) {}

	operator bool() const
	{
		return m_pStatObj || m_pChar || m_pPhysEnt;
	}
};

// Real-time params to control particle emitters. Some parameters override emitter params.
struct SpawnParams
{
	EGeomType	eAttachType;			// What type of object particles emitted from.
	EGeomForm	eAttachForm;			// What aspect of shape emitted from.
	int				nAttachSlot;			// Entity slot where particle emitter attaches.
	float			fCountScale;			// Multiple for particle count (on top of bCountPerUnit if set).
	float			fSizeScale;				// Multiple for all effect sizes.
	float			fSpeedScale;			// Multiple for particle emission speed.
	float			fPulsePeriod;			// How often to restart emitter.
	bool			bCountPerUnit;		// Multiply particle count also by geometry extent (length/area/volume).
	bool			bIndependent;			// Emitter should spawn independent of entity (momentary attachment).
	bool			bIgnoreLocation;	// Ignore all location-based culling tests (useful for detached rendering calls).

	inline SpawnParams( EGeomType eType = GeomType_None, EGeomForm eForm = GeomForm_Surface )
	{
		eAttachType = eType;
		eAttachForm = eForm;
		nAttachSlot = -1;
		fCountScale = 1;
		fSizeScale = 1;
		fSpeedScale = 1;
		fPulsePeriod = 0;
		bCountPerUnit = false;
		bIndependent = false;
		bIgnoreLocation = false;
	}
};

struct ParticleTarget
{
	Vec3	vTarget;				// Target location for particles.
	Vec3	vVelocity;			// Velocity of moving target.
	bool	bTarget;				// Enable targeting.
	bool	bPriority;			// Priority targeting, ignore attractor entities.

	// Extension mechanisms. Any or all can be set. If none set, particles may not reach target.
	bool	bExtendLife;		// Extend particle life as necessary to reach target at normal speed.
	bool	bExtendCount;		// Extend particle count as necessary to preserve normal density.
	bool	bExtendSpeed;		// Extend particle speed as necessary to reach target in normal lifetime.
	bool	bStretch;				// Stretch particle to compensate for increased speed.

	ParticleTarget()
	{
		memset(this, 0, sizeof(*this));
	}

	AUTO_STRUCT_INFO
};


struct ParticleParams;

// Description:
// This interface is used by I3DEngine::CreateParticleEffect to control a particle effect
struct IParticleEffect : public _i_reference_target_t
{
	static Matrix34 ParticleLoc(const Vec3& pos, const Vec3& dir = Vec3(0,0,1), float scale = 1.f)
	{
		Quat q(IDENTITY);
		if (!dir.IsZero())
		{
			// Rotate in 2 stages to avoid roll.
			Vec3 dirxy = Vec3(dir.x, dir.y, 0.f);
			if (!dirxy.IsZero())
			{
				dirxy.Normalize();
				q = Quat::CreateRotationV0V1(dirxy, dir.GetNormalized())
					* Quat::CreateRotationV0V1(Vec3(0,1,0), dirxy);
			}
			else
				q = Quat::CreateRotationV0V1(Vec3(0,1,0), dir.GetNormalized());
			Vec3 check = q * Vec3(0,1,0);
		}

		// To do: Use QuatTS directly for all particle coords.
		return Matrix34( QuatTS(q, pos, scale) );
	}

	//////////////////////////////////////////////////////////////////////////
	// Spawn this effect.
	//////////////////////////////////////////////////////////////////////////

	// Summary:
	//     Spawn the particles
	virtual IParticleEmitter* Spawn( bool bIndependent, const Matrix34& loc ) = 0;

	//! Assign name to this particle effect.

	// Summary:
	//     Set a new name
	// Arguments:
	//     sFullName - The full name of this effect, including library and group qualifiers.
	virtual void SetName( const char *sFullName ) = 0;

	//! Returns name of this particle effect.

	// Summary:
	//     Gets the name
	// Return Value:
	//     A C string which holds the fully qualified name of this effect, with parents and library.
	virtual const char* GetName() const = 0;

	// Summary:
	//     Gets the name
	// Return Value:
	//     A C string which holds the base name of this effect.
	//		 For top-level effects, this includes group qualifier.
	virtual const char* GetBaseName() const = 0;

	//! Enable or disable this effect.

	// Summary:
	//     Enable or disable the effect
	// Arguments:
	//     bEnabled - set to true to enable the effect or to false to disable it
	virtual void SetEnabled( bool bEnabled ) = 0;

	// Summary:
	//     Deternime if the effect is already enabled
	// Return Value:
	//     A boolean value which indicate the status of the effect; true if 
	//     enabled or false if disabled.
	virtual bool IsEnabled() const = 0;

	//! Return ParticleParams.

	// Summary:
	//     Sets the particle parameters
	// Return Value:
	//     An object of the type ParticleParams which contains several parameters.
	virtual void SetParticleParams( const ParticleParams &params ) = 0;

	//! Return ParticleParams.

	// Summary:
	//     Gets the particle parameters
	// Return Value:
	//     An object of the type ParticleParams which contains several parameters.
	virtual const ParticleParams& GetParticleParams() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Child particle systems.
	//////////////////////////////////////////////////////////////////////////

	//! Get number of sub Particles childs.

	// Summary:
	//     Gets the number of sub particles childs
	// Return Value:
	//     An integer representing the amount of sub particles childs
	virtual int GetChildCount() const = 0;
	
	//! Get sub Particles child by index.

	// Summary:
	//     Gets a specified particles child
	// Arguments:
	//     index - The index of a particle child
	// Return Value:
	//     A pointer to a IParticleEffect derived object.
	virtual IParticleEffect* GetChild( int index ) const = 0;
	
	//! Adds a new sub Particles.

	// Summary:
	//   Adds a child particle effect
	// Arguments:
	//   pEffect - A pointer the particle effect to add as child
	virtual void AddChild( IParticleEffect *pEffect ) = 0;

	//! Remove specific sub Particles

	// Summary:
	//   Removes a sub particle effect
	// Arguments:
	//   pEffect - A pointer to the child particle effect to be removed
	virtual void RemoveChild( IParticleEffect *pEffect ) = 0;

	//! Remove all sub Particles.

	// Summary:
	//   Removes all child particles
	virtual void ClearChilds() = 0;

	//! Insert sub particles in between other child particles.

	// Summary:
	//   Insert a child particle effect at a precise slot
	// Arguments:
	//   slot - An integer value which specify the desired slot
	//   pEffect - A pointer to the particle effect to insert
	virtual void InsertChild( int slot,IParticleEffect *pEffect ) = 0;
	
	//! Find slot where sub Particles stored.
	//! @retun slot index if Particles found, -1 if Particles not found.

	// Summary:
	//   Finds in which slot a child particle effect is stored
	// Arguments:
	//   pEffect - A pointer to the child particle effect
	// Return Value:
	//   An integer representing the slot number.
	virtual int FindChild( IParticleEffect *pEffect ) const = 0;

	// Return parent effect if any.
	virtual IParticleEffect* GetParent() const = 0;
	
	//! Load particle effect resources

	// Summary:
	//   Load all resources needed for a particle effects
	// Returns:
	//	 True if any resources loaded.
	virtual bool LoadResources() = 0;

	// Summary:
	//   Unload all resources previously loaded.
	virtual void UnloadResources() = 0;

	// Summary:
	//   Serialize particle effect to/from XML
	// Arguments:
	//   bLoading - true when loading,false for saving.
	//   bChilds - When true also recursively serailizes effect childs.
	virtual void Serialize( XmlNodeRef node,bool bLoading,bool bChilds ) = 0;
};

//DOC-IGNORE-BEGIN
TYPEDEF_AUTOPTR(IParticleEffect);
//DOC-IGNORE-END

//! Particle emitter interface

// Description:
//     An IParticleEmitter should usually be creater by 
//     I3DEngine::CreateParticleEmitter. Deleting the emitter should be done 
//     using I3DEngine::DeleteParticleEmitter.
// Summary:
//     Interface to a particle effect emitter
struct IParticleEmitter : public _i_reference_target_t, public IRenderNode
{
	// Summary: 
	//		Returns whether emitter still alive in engine.
	virtual bool IsAlive() const = 0;

	// Summary:
	//		Sets emitter state to active or inactive. Emitters are initially active.
	//		if bActive = true:
	//				Emitter updates and emits as normal, and deletes itself if limited lifetime.
	//		if bActive = false: 
	//				Stops all emitter updating and particle emission.
	//				Existing particles continue to update and render.
	//				Emitter is not deleted.
	virtual void Activate( bool bActive ) = 0;

	// Summary:
	//		 Advance the emitter to its equilibrium state.
	virtual void Prime() = 0;

	// Summary: 
	//		 Restarts the emitter from scratch (if active). 
	//		 Any existing particles are re-used oldest first.
	virtual void Restart() = 0;

	//! Set particle effect to spawn at this emitter (Do not use if you use SetParams).
	// NOTE: Do not use this function if you already call SetParams.

	// Description:
	//     Will define the effect used to spawn the particles from the emitter.
	// Note: 
	//     Never call this function if you already used SetParams.
	// See Also:
	//     SetParams
	// Arguments:
	//     pEffect - A pointer to an IParticleEffect object
	// Summary:
	//     Set the effects used by the particle emitter
	virtual void SetEffect( const IParticleEffect* pEffect ) = 0;

	// Summary:
	//		 Specify how particles are emitted from source.
	virtual void SetSpawnParams( const SpawnParams& spawnParams, GeomRef geom = GeomRef()) = 0;

	// Summary:
	//      Associate emitter with entity, for dynamic updating of positions etc.
	// Note:
	//      Must be done when entity created or serialized, entity association is not serialized.
	virtual void SetEntity( IEntity* pEntity, int nSlot ) = 0;

	// Attractors.
	virtual void SetTarget( const ParticleTarget& target ) = 0;

	virtual void Serialize(TSerialize ser) = 0;

	// Summary:
	//		Update emitter's state to current time.
	virtual void Update() = 0;

	// Summary:
	//     Programmatically add a particle to emitter for rendering.
	//		 With no arguments, spawns a particle according to emitter settings.
	//		 Specific objects can be passed for programmatic control.
	// Arguments:
	//		 pLocation - Specified location for particle. If 0, set from effect params.
	//		 pVel - Specified velocity for particle. If 0, set from effect params.
	//		 pStatObj	- The displayable geometry object for the entity. If 0, uses emitter settings for sprite or geometry.
	//     pPhysEnt - A physical entity which controls the particle. If 0, uses emitter settings to physicalise or move particle.
	virtual void EmitParticle( IStatObj* pStatObj = NULL, IPhysicalEntity* pPhysEnt = NULL, QuatTS* pLocation = NULL, Vec3* pVel = NULL ) = 0;
};

//DOC-IGNORE-BEGIN
TYPEDEF_AUTOPTR(IParticleEmitter);
//DOC-IGNORE-END

//! Physics material enumerator, allows for 3dengine to get material id from game code
struct IPhysMaterialEnumerator
{
  virtual int EnumPhysMaterial(const char * szPhysMatName) = 0;
	virtual bool IsCollidable(int nMatId) = 0;
	virtual int	GetMaterialCount() = 0;
	virtual const char* GetMaterialNameByIndex( int index ) = 0;
};

// Summary:
//     Flags used by I3DEngine::DrawLowDetail
enum EDrawLowDetailFlags
{
	DLD_DETAIL_OBJECTS            =   1<<0,
	DLD_DETAIL_TEXTURES           =   1<<1,
	DLD_TERRAIN_WATER             =   1<<2,
	DLD_FAR_SPRITES               =   1<<3,
	DLD_STATIC_OBJECTS            =   1<<4,
	DLD_PARTICLES                 =   1<<5,
	DLD_DECALS										=   1<<6,
	DLD_TERRAIN_LIGHT             =		1<<7,
	DLD_SHADOW_MAPS               =		1<<9,
	DLD_ENTITIES                  =		1<<10,
	DLD_TERRAIN                   =		1<<11,
};

// phys foreign data flags
enum EPhysForeignFlags
{
	PFF_HIDABLE										= 1,
	PFF_HIDABLE_SECONDARY					=	2,
	PFF_EXCLUDE_FROM_STATIC				= 4,
	PFF_BRUSH											= 8,
	PFF_VEGETATION								= 16,
	PFF_UNIMPORTANT								= 32,
	PFF_OUTDOOR_AREA							= 64,
};

// ocean data flags
enum EOceanRenderFlags
{
  OCR_NO_DRAW     =   1<<0,
  OCR_OCEANVOLUME_VISIBLE  =   1<<1,
};

//! structure to pass vegetation group properties
struct IStatInstGroup
{
	IStatInstGroup() 
	{ 
		pStatObj = 0;
		szFileName[0]=0;
		bHideability = 0;
		bHideabilitySecondary = 0;

    bPickable = 0;
		fBending = 0;
    bComplexBending = 0;
		bCastShadow = 0;
		bRecvShadow = 0;
		bPrecShadow = true;
		bUseAlphaBlending = 0;
		fSpriteDistRatio = 1.f;
		fShadowDistRatio = 1.f;
		fMaxViewDistRatio= 1.f;
		fBrightness = 1.f;
		pMaterial = 0;
    bUseSprites = true;
	
		fDensity=1;
		fElevationMax=4096;
		fElevationMin=8;
		fSize=1 ;
		fSizeVar=0 ;
		fSlopeMax=255 ;
		fSlopeMin=0 ;
		bRandomRotation = false;
    nMaterialLayers = 0;
		bAffectedByVoxels = bUseTerrainColor = bAlignToTerrain = false;
		minConfigSpec = (ESystemConfigSpec)0;
	}

	_smart_ptr<IStatObj> pStatObj;
	char  szFileName[256];
	bool	bHideability;
	bool	bHideabilitySecondary;
  bool  bPickable;
	float fBending;
  bool  bComplexBending;
	bool	bCastShadow;
	bool	bRecvShadow;
	bool	bPrecShadow;
	bool	bUseAlphaBlending;
	float fSpriteDistRatio;
	float fShadowDistRatio;
	float fMaxViewDistRatio;
	float	fBrightness;
  bool  bUseSprites;
	bool  bRandomRotation;
	bool  bAlignToTerrain;
  bool  bUseTerrainColor;
	bool  bAffectedByVoxels;

	float fDensity;
	float fElevationMax;
	float fElevationMin;
	float fSize;
	float fSizeVar;
	float fSlopeMax;
	float fSlopeMin;

	// Minimal configuration spec for this vegetation group.
	ESystemConfigSpec minConfigSpec;

	//! Override material for this instance group.
	_smart_ptr<IMaterial> pMaterial;

  //! Material layers bitmask -> which layers are active.
  uint8 nMaterialLayers;

	//! flags similar to entity render flags
	int m_dwRndFlags;
};

//! Interface for water volumes editing from editor


// Description:
//     Water volumes should usually be created by I3DEngine::CreateWaterVolume.
// Summary:
//     Interface to water volumes
struct IWaterVolume
{
//DOC-IGNORE-BEGIN
	virtual void UpdatePoints(const Vec3 * pPoints, int nCount, float fHeight) = 0;
	virtual void SetFlowSpeed(float fSpeed) = 0;
	virtual void SetAffectToVolFog(bool bAffectToVolFog) = 0;
	virtual void SetTriSizeLimits(float fTriMinSize, float fTriMaxSize) = 0;
//	virtual void SetMaterial(const char * szShaderName) = 0;
	virtual void SetMaterial( IMaterial *pMaterial ) = 0;
	virtual IMaterial * GetMaterial() = 0;
	virtual const char* GetName() const = 0;
	virtual void SetName(const char * szName) = 0;
//DOC-IGNORE-END

	// Description:
	//     Used to change the water level. Will assign a new Z value to all 
	//     vertices of the water geometry.
	// Arguments:
	//     vNewOffset - Position of the new water level
	// Summary:
	//     Set a new water level
	virtual void SetPositionOffset(const Vec3 & vNewOffset) = 0;
};

// Summary:
//     Provide information about the different VisArea volumes
struct IVisArea
{
	// Summary:
	//     Gets the last rendered frame id
	// Return Value:
	//     An int which contain the frame id.
	virtual int GetVisFrameId() = 0;
	
	// Description:
	//     Gets a list of all the VisAreas which are connected to the current one. 
	// Arguments:
	//     pAreas               - Pointer to an array of IVisArea*
	//     nMaxConnNum          - The maximum of IVisArea to write in pAreas
	//     bSkipDisabledPortals - Ignore portals which are disabled
	// Return Value:
	//     An integer which hold the amount of VisArea found to be connected. If 
	//     the return is equal to nMaxConnNum, it's possible that not all 
	//     connected VisAreas were returned due to the restriction imposed by the 
	//     argument.
	// Summary:
	//     Gets all the areas which are connected to the current one
	virtual	int GetVisAreaConnections(IVisArea ** pAreas, int nMaxConnNum, bool bSkipDisabledPortals = false) = 0;

	// Summary:
	//     Determine if it's connected to an outdoor area
	// Return Value:
	//     Return true if the VisArea is connected to an outdoor area.
	virtual bool IsConnectedToOutdoor() = 0;

	// Summary:
	//     Gets the name
	// Note:
	//     The name is always returned in lower case.
	// Return Value:
	//     A null terminated char array containing the name of the VisArea.
	virtual const char * GetName() = 0;

	// Summary:
	//     Determine if this VisArea is a portal
	// Return Value:
	//     true if the VisArea is a portal, or false in the opposite case.
	virtual bool IsPortal() = 0;

	// Description: 
	//     Search for a specified VisArea to see if it's connected to the current 
	//     VisArea.
	// Arguments:
	//     pAnotherArea         - A specified VisArea to find
	//     nMaxRecursion        - The maximum number of recursion to do while searching
	//     bSkipDisabledPortals - Will avoid searching disabled VisAreas
	//		 pVisitedAreas				- if not NULL - will get list of all visited areas
	// Return Value:
	//     true if the VisArea was found.
	// Summary:
	//     Search for a specified VisArea
	virtual bool FindVisArea(IVisArea * pAnotherArea, int nMaxRecursion, bool bSkipDisabledPortals) = 0;

	// Description: 
	//     Search for the surrounding VisAreas which connected to the current 
	//     VisArea.
	// Arguments:
	//     nMaxRecursion        - The maximum number of recursion to do while searching
	//     bSkipDisabledPortals - Will avoid searching disabled VisAreas
	//		 pVisitedAreas				- if not NULL - will get list of all visited areas
	// Return Value:
	//     none
	// Summary:
	//     Search for the surrounding VisAreas
	virtual void FindSurroundingVisArea( int nMaxRecursion, bool bSkipDisabledPortals, PodArray<IVisArea*> * pVisitedAreas = NULL, int nMaxVisitedAreas = 0, int nDeepness = 0) = 0;

	// Summary:
	//     Determine if it's affected by outdoor lighting
	// Return Value:
	//     Return true if the VisArea if it's affected by outdoor lighting, else
	//     false will be returned.
	virtual bool IsAffectedByOutLights() = 0;

	// Summary:
	//     Determine if the spere can be affect the VisArea
	// Return Value:
	//     Return true if the VisArea can be affected by the sphere, else
	//     false will be returned.
	virtual bool IsSphereInsideVisArea(const Vec3 & vPos, const f32 fRadius) = 0;

	// Summary:
	//     Clips geometry inside or outside a vis area.
	// Return Value:
	//     Whether geom was clipped.
	virtual bool ClipToVisArea(bool bInside, Sphere& sphere, Vec3 const& vNormal) = 0;
	
	// Summary:
	//     Give back the axis aligned bounding box of VisArea
	// Return Value:
	//     Return the pointer of a AABB
	virtual const AABB* GetAABBox() const = 0;

	// Summary:
	//     Determine if the point can be affect the VisArea
	// Return Value:
	//     Return true if the VisArea can be affected by the point, else
	//     false will be returned.
	virtual bool IsPointInsideVisArea(const Vec3 & vPos) = 0;

	// Description: 
	//     Calculate the 6colors of the Ambient cube for this AABBox
	// Arguments:
	//     pAmbientCube    - float4[6]	feeded with the color data
	//     AABBMin/AABBMax - BoundingBox
	// Return Value:
	//     none
	virtual	void	CalcAmbientCube(f32* pAmbientCube,const Vec3& rAABBMin,const Vec3& rAABBMax	)	=0;

	// Description: 
	//     Calculate the 4colors per face of the Ambient cube for this AABBox
	// Arguments:
	//     pAmbientCube    - float4[24]	feeded with the color data
	//     AABBMin/AABBMax - BoundingBox
	// Return Value:
	//     none
	virtual	void	CalcHQAmbientCube(f32* pAmbientCube,const Vec3& rAABBMin,const Vec3& rAABBMax	)	=0;
};


// water level unknown
#define WATER_LEVEL_UNKNOWN -1000000
#define BOTTOM_LEVEL_UNKNOWN -1000000


// float m_SortId		: offseted by +WATER_LEVEL_SORTID_OFFSET if the camera object line is crossing the water surface
// : otherwise offseted by -WATER_LEVEL_SORTID_OFFSET
#define WATER_LEVEL_SORTID_OFFSET			10000000


// Summary:
//     indirect lighting quadtree definition
namespace NQT
{
	//forward declaration
	template <class TLeafContent, uint32 TMaxCellElems, class TPosType, class TIndexType, bool TUseRadius>
	class CQuadTree;
}

#define FILEVERSION_TERRAIN_SHLIGHTING_FILE 5

enum EVoxelBrushShape
{
	evbsSphere = 1,
	evbsBox,
};

enum EVoxelEditTarget
{
	evetVoxelObjects = 1,
};

enum EVoxelEditOperation
{
	eveoCreate = 1,
	eveoSubstract,
	eveoMaterial,
	eveoBlur,
	eveoCopyTerrain,
};

#define COMPILED_HEIGHT_MAP_FILE_NAME "terrain\\heightmap.dat"
#define COMPILED_VISAREA_MAP_FILE_NAME "terrain\\visareas.dat"
#define COMPILED_SHLIGHTING_FILE_NAME "terrain\\shlighting.dat"

//////////////////////////////////////////////////////////////////////////
#pragma pack(push,1)

struct SIndirectLightingHeaderVersion2
{
	uint16 version;
	uint16 objectSampleCount;
	uint32 nChunkSize;

	SIndirectLightingHeaderVersion2() : version(2), objectSampleCount(0), nChunkSize(0)
	{}
	AUTO_STRUCT_INFO
};

struct SIndirectLightingHeader
{
	uint16 version;
	uint16 pad;
	uint32 objectSampleCount;
	uint32 nChunkSize;

	SIndirectLightingHeader() : version(FILEVERSION_TERRAIN_SHLIGHTING_FILE), objectSampleCount(0), nChunkSize(0)
	{}
	AUTO_STRUCT_INFO
};

struct STerrainInfo
{
	int nHeightMapSize_InUnits;
	int nUnitSize_InMeters;
	int nSectorSize_InMeters;
	int nSectorsTableSize_InSectors;
	float fHeightmapZRatio;
	float fOceanWaterLevel;

	AUTO_STRUCT_INFO
};

#define TERRAIN_CHUNK_VERSION 22
#define VISAREAMANAGER_CHUNK_VERSION 4

struct STerrainChunkHeader
{
	int nChunkVersion;
	int nChunkSize;
	STerrainInfo TerrainInfo;

	AUTO_STRUCT_INFO
};

struct SVisAreaManChunkHeader
{
	int nChunkVersion;
	int nChunkSize;
	int nVisAreasNum;
	int nPortalsNum;
	int nOcclAreasNum;

	AUTO_STRUCT_INFO
};

struct SOcTreeNodeChunk
{
		int		nChunkVersion;
		AABB	nodeBox;
		int		nObjectsBlockSize;
		uchar ucChildsMask;

		AUTO_STRUCT_INFO
};

#pragma pack(pop)

//! Interface to indirect lighting engine 
// Summary:
//     Interface to indirect lighting engine 
struct IIndirectLighting
{
	struct SVoxelInfo
	{
		IRenderNode *pRenderNode;
		IRenderMesh *pRenderMesh;
		Matrix34 mat34;
		SVoxelInfo() : pRenderNode(NULL), pRenderMesh(NULL)
		{
			mat34.SetIdentity();
		}
	};

	// Description:
	//		 retrieves the required information about terrain voxels
	//		 pass NULL to retrieve just the count
	virtual void GetTerrainVoxelInfo(SVoxelInfo *ppVoxels, uint32& rCount) = 0;

	// Description:
	//		 retrieves existing terrain accessibility data
	virtual void RetrieveTerrainAcc(uint8 *pDst, const uint32 cWidth, const uint32 cHeight) = 0;
};

//     Interface to the 3d Engine
struct ITerrain
{
	//! load data into terrain engine from memory block
	virtual bool SetCompiledData(byte * pData, int nDataSize, std::vector<struct CStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable) = 0;

	//! save data from terrain engine into memory block
	virtual bool GetCompiledData(byte * pData, int nDataSize, std::vector<struct CStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable) = 0;

	//! return terrain data memory block size
	virtual int GetCompiledDataSize() = 0;

	//! calculate sectors bounds and error levels
	virtual bool Compile() = 0;

	//! creates and place a new vegetation object on the terrain.
	virtual IRenderNode* AddVegetationInstance( int nStaticGroupID,const Vec3 &vPos,const float fScale,uchar ucBright,uchar angle ) = 0;

	//! set ocean level
	virtual void SetOceanWaterLevel( float fOceanWaterLevel ) = 0;

	//! return whole terrain lightmap texture id
	virtual int GetTerrainLightmapTexId( Vec4 & vTexGenInfo ) = 0;

	//! Updates part of hight map (in terrain units, by default update only elevation)
	virtual void SetTerrainElevation(int x1, int y1, int nSizeX, int nSizeY, float * pTerrainBlock, unsigned char * pSurfaceData, uint32 * pResolMap, int nResolMapSizeX, int nResolMapSizeY) = 0;

  //! Return current amount of terrain textures requests for streaming, if more than 0 = there is streaming in progress
  virtual int GetNotReadyTextureNodesCount() = 0;
};

struct IVisAreaCallback
{
	virtual void OnVisAreaDeleted( IVisArea* pVisArea ) = 0;
};

struct IVisAreaManager
{
	//! load data into VisAreaManager engine from memory block
//	virtual bool SetCompiledData(byte * pData, int nDataSize, std::vector<struct CStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable) = 0;

	//! save data from VisAreaManager engine into memory block
	virtual bool GetCompiledData(byte * pData, int nDataSize, std::vector<struct CStatObj*> ** ppStatObjTable, std::vector<IMaterial*> ** ppMatTable) = 0;

	//! return VisAreaManager data memory block size
	virtual int GetCompiledDataSize() = 0;

	//! the function give back the accumulated number of visareas and portals
	virtual int GetNumberOfVisArea() const = 0;

	//! give back the visarea interface based on the id (0..GetNumberOfVisArea()) it can be a visarea or a portal
	virtual IVisArea* GetVisAreaById( int nID ) const = 0;

	virtual void AddListener( IVisAreaCallback *pListener ) = 0;
	virtual void RemoveListener( IVisAreaCallback *pListener ) = 0;
};

struct IMemoryBlock : public _i_reference_target_t
{
	virtual void * GetData() = 0;
	virtual int GetSize() = 0;
};

//! Interface to the Time Of Day functionality
struct ITimeOfDay
{
	enum EVariableType
	{
		TYPE_FLOAT,
		TYPE_COLOR
	};
	struct SVariableInfo
	{
		const char *name;  // Variable name.
		int nParamId;
		EVariableType type;
		float fValue[3];    // Value of the variable (3 needed for color type)
		ISplineInterpolator* pInterpolator; // Splines that control variable value
	};
	struct SAdvancedInfo
	{
		float fStartTime;
		float fEndTime;
		float fAnimSpeed;
	};
	//////////////////////////////////////////////////////////////////////////
	// Access to variables that control time of the day apperarence.
	//////////////////////////////////////////////////////////////////////////
	virtual int GetVariableCount() = 0;
	virtual bool GetVariableInfo( int nIndex,SVariableInfo &varInfo ) = 0;
	virtual void SetVariableValue( int nIndex,float fValue[3] ) = 0;

	// Time of day is specified in hours.
	virtual void SetTime( float fHour,bool bForceUpdate=false ) = 0;
	virtual float GetTime() = 0;

	// update the current tod
	virtual void Tick() = 0;

	virtual void SetPaused(bool paused) = 0;

	virtual void SetAdvancedInfo( const SAdvancedInfo &advInfo ) = 0;
	virtual void GetAdvancedInfo( SAdvancedInfo &advInfo ) = 0;

	// Update engine paremeters after variable values have been changed.
	virtual void Update( bool bInterpolate=true,bool bForceUpdate=false ) = 0;

	virtual void BeginEditMode() = 0;
	virtual void EndEditMode() = 0;

	virtual void Serialize( XmlNodeRef &node,bool bLoading ) = 0;
	virtual void Serialize( TSerialize ser ) = 0;

	virtual void SetTimer( ITimer * pTimer ) = 0;

	// MP serialization
	static const int NETSER_FORCESET = BIT(0);
	static const int NETSER_COMPENSATELAG = BIT(1);
	static const int NETSER_STATICPROPS = BIT(2);
	virtual void NetSerialize( TSerialize ser, float lag, uint32 flags ) = 0;
};


struct IFoliage : ISkinnable
{
	enum EFoliageFlags { FLAG_FROZEN=1 };
	virtual int Serialize(TSerialize ser) = 0;
	virtual void SetFlags(int flags) = 0;
	virtual int GetFlags() = 0;
	virtual IRenderNode* GetIRenderNode() = 0;
};


struct SSkyLightRenderParams
{
	SSkyLightRenderParams()
	: m_pSkyDomeMesh( 0 )
	, m_skyDomeTextureWidth( 0 )
	, m_skyDomeTextureHeight( 0 )
	, m_pSkyDomeTextureDataMie( 0 )
	, m_pSkyDomeTextureDataRayleigh( 0 )
	, m_skyDomeTexturePitch( 0 )
	, m_skyDomeTextureTimeStamp( -1 )
	, m_partialMieInScatteringConst( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_partialRayleighInScatteringConst( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_sunDirection( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_phaseFunctionConsts( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_hazeColor( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_hazeColorMieNoPremul( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_hazeColorRayleighNoPremul( 0.0f, 0.0f, 0.0f, 0.0f )
	, m_skyColorTop( 0.0f, 0.0f, 0.0f )
	, m_skyColorNorth( 0.0f, 0.0f, 0.0f )
	, m_skyColorEast( 0.0f, 0.0f, 0.0f )
	, m_skyColorSouth( 0.0f, 0.0f, 0.0f )
	, m_skyColorWest( 0.0f, 0.0f, 0.0f )
	{
	}

	// sky dome mesh
	IRenderMesh* m_pSkyDomeMesh;

	// sky dome texture data
	uint32 m_skyDomeTextureWidth;
	uint32 m_skyDomeTextureHeight;
	const void* m_pSkyDomeTextureDataMie;
	const void* m_pSkyDomeTextureDataRayleigh;
	size_t m_skyDomeTexturePitch;
	int m_skyDomeTextureTimeStamp;

	// sky dome shader constants
	Vec4 m_partialMieInScatteringConst;
	Vec4 m_partialRayleighInScatteringConst;
	Vec4 m_sunDirection;
	Vec4 m_phaseFunctionConsts;
	Vec4 m_hazeColor;
	Vec4 m_hazeColorMieNoPremul;
	Vec4 m_hazeColorRayleighNoPremul;

	// sky hemisphere colors
	Vec3 m_skyColorTop;
	Vec3 m_skyColorNorth;
	Vec3 m_skyColorEast;
	Vec3 m_skyColorSouth;
	Vec3 m_skyColorWest;
};

struct sRAEColdData
{
	Vec4												m_RAEPortalInfos[96];											// it store all datas needed to solve the problem between the portals & indirect lighting
//	byte												m_OcclLights[MAX_LIGHTS_NUM];
};

struct SVisAreaInfo
{
  float fHeight;
  Vec3 vAmbientColor;
  bool bAffectedByOutLights;
  bool bIgnoreSkyColor;
  bool bSkyOnly;
  float fViewDistRatio;
  bool bDoubleSide;
  bool bUseDeepness;
  bool bUseInIndoors;
  bool bMergeBrushes;
  bool bOceanIsVisible;
};

struct SDebugFPSInfo
{
	float fAverageFPS;
	float fMinFPS;
	float fMaxFPS;
};


// Summary:
//     Interface to the 3d Engine
struct I3DEngine : public IProcess
{
	//! Initialize 3dengine (call once, after creations)

	// Summary:
	//     Initialize the 3D Engine
	// See Also:
	//     ShutDown
	// Note:
	//     Only call once, after creating the instance.
	virtual bool Init() = 0;

	// Summary:
	//     Set the path used to load levels
	// See Also:
	//     LoadLevel
	// Arguments:
	//     szFolderName - Should contains the folder to be used
	virtual void SetLevelPath( const char * szFolderName ) = 0;

	// Description:
	//     Will load a level from the folder specified with SetLevelPath. If a 
	//     level is already loaded, the resources will be deleted before.
	// See Also:
	//     SetLevelPath
	// Arguments:
	//     szFolderName - Name of the subfolder to load
	//     szMissionName - Name of the mission
	// Return Value:
	//     A boolean which indicate the result of the function; true is 
	//     succeed, or false if failled.
	// Summary:
	//     Load a level
	virtual bool LoadLevel(const char * szFolderName, const char * szMissionName) = 0;
	virtual bool InitLevelForEditor(const char * szFolderName, const char * szMissionName) = 0;

	// Description:
	//     Will unload a current level
	// See Also:
	//
	// Arguments:
	//
	// Return Value:
	//
	// Summary:
	//     Unload a current level

	virtual void UnloadLevel( bool bForceClearParticlesAssets=false ) = 0;
	// Description:
	//    Must be called after the game completely finishes loading the level.
	//    3D engine uses it to precache some resources needed for rendering.
	// See Also:
	//    LoadLevel
	virtual void PostLoadLevel() = 0;

	// Summary:
	//     Handle any work needed at start of new frame.
	// Note:
	//     Should be called for every frame.
  virtual void OnFrameStart() = 0;

	// Summary:
	//     Update the 3D Engine 
	// Note:
	//     Should be called for every frame.
	virtual void Update() = 0;

	// Summary:
	//     Set the camera
	// See Also:
	//     Draw
	// Note:
	//     Must be called before Draw.
	// Arguments:
	//     cam - ...
	//     bToTheScreen - ...
//	virtual void SetCamera(const CCamera &cam, bool bToTheScreen=true) = 0;

	// Summary:
	//     Draw the world
	// See Also:
	//     SetCamera
	// Arguments:
	//   szDebugName - name that can be visualized for debugging purpose, must not be 0
	virtual void RenderWorld(const int nRenderFlags, const CCamera &cam, const char *szDebugName, const int dwDrawFlags = -1, const int nFilterFlags=-1) = 0;

	// Summary:
	//     Draw the world for rendering into a texture
	// Arguments:
	//     DrawFlags - Define what to draw, use any flags defined in EDrawLowDetailFlags
//	virtual void DrawLowDetail(const int & DrawFlags) = 0;

	// Summary:
	//     Shutdown the 3D Engine
	// Arguemnts:
	virtual void ShutDown() = 0;

	// Summary:
	//     Delete the 3D Engine instance
	virtual void Release() = 0;

	/*! Load cgf file and create non animated object.
      Returns pointer to already loaded object with same name if found.
      Reference counting used */

	// Summary:
	//     Loads a static object from a CGF file
	// See Also:
	//     IStatObj
	// Arguments:
	//     szFileName - CGF Filename - should not be 0 or ""
	//     szGeomName - Optional name of geometry inside CGF.
	//     ppSubObject - [Out]Optional Out parameter,Pointer to the

	// Return Value:
	//     A pointer to an object derived from IStatObj.
	virtual IStatObj* LoadStatObj( const char *szFileName,const char *szGeomName=NULL,/*[Out]*/IStatObj::SSubObject **ppSubObject=NULL ) = 0;

	// Summary:
	//     Gets the amount of loaded objects
	// Return Value:
	//     An integer representing the amount of loaded objects.
	virtual int GetLoadedObjectCount() { return 0; }

	// Summary:
	//     Fill pObjectsArray with pointers to loaded static objectsm 
	//     if pObjectsArray is NULL only fills nCount parameter with amount of loaded objects.
	virtual void GetLoadedStatObjArray( IStatObj** pObjectsArray,int &nCount ) = 0;

	// Summary:
	//     Registers an entity to be rendered
	// Arguments:
	//     pEntity - The entity to render
	virtual void RegisterEntity( IRenderNode * pEntity )=0;

  // Summary:
  //     Selects an entity for debugging
  // Arguments:
  //     pEntity - The entity to render
  virtual void SelectEntity( IRenderNode * pEntity )=0;

	// Summary:
	//     Notices the 3D Engine to stop rendering a specified entity
	// Arguments:
	//     pEntity - The entity to stop render
	virtual bool UnRegisterEntity( IRenderNode * pEntity )=0;

	/*! Return whether a world pos is under water. */
	virtual bool IsUnderWater( const Vec3& vPos) const = 0;
  
  /*! Return whether ocean volume is visible or not */
  virtual void SetOceanRenderFlags( uint8 nFlags ) = 0;
  virtual uint8 GetOceanRenderFlags() const = 0;
  virtual uint32 GetOceanVisiblePixelsCount() const = 0;

	/*! Get water level in specified point (taking into account global water level and water volumes)
	Function returns WATER_LEVEL_UNKNOWN if in specified position water was not found */

	// Summary:
	//     Gets the closest walkable bottom z straight beneath the given reference position.
	// Note:
	//     This function will take into account both the global terrain elevation and local voxel (or other solid walkable object).
	// Arguments:
	//     referencePos - Position from where to start searchning downwards.
	//     maxRelevantDepth - Max depth caller is interested in relative to referencePos (for ray casting performance reasons).
	//     objtypes - expects physics entity flags.  Use this to specify what object types make a valid bottom for you.
	// Return Value:
	//     A float value which indicate the global world z of the bottom level beneath the referencePos.
	//     If the referencePos is below terrain but not inside any voxel area BOTTOM_LEVEL_UNKNOWN is returned.
	virtual float GetBottomLevel(const Vec3 & referencePos, float maxRelevantDepth, int objtypes) = 0;
	// A set of overloads for enabling users to use different sets of input params.  Basically, only
	// referencePos is mandatory.  The overloads as such don't need to be virtual but this seems to be
	// a purely virtual interface.
	virtual float GetBottomLevel(const Vec3 & referencePos, float maxRelevantDepth = 10.0f) = 0;
	virtual float GetBottomLevel(const Vec3 & referencePos, int objflags) = 0;

	/*! Get water level in specified point (taking into account global water level and water volumes)
			Function returns WATER_LEVEL_UNKNOWN if in specified position water was not found */

	// Summary:
	//     Gets the water level for a specified position
	// Note:
	//     This function will take into account both the global water level and any water volume present.
	// Arguments:
	//     pvPos - Desired position to inspect the water level
	//     pvFlowDir - Pointer to return the flow direction (optional)
	// Return Value:
	//     A float value which indicate the water level. In case no water was 
	//     found at the specified location, the value WATER_LEVEL_UNKNOWN will 
	//     be returned.
  virtual float GetWaterLevel(const Vec3 * pvPos = NULL, Vec3 * pvFlowDir = NULL) = 0;

  // Summary:
  //     Gets the ocean water level for a specified position
  // Note:
  //     This function only takes into account ocean water
  // Arguments:
  //     pCurrPos - Position to check water level
  // Return Value:
  //     A float value which indicate the water level

  virtual float GetOceanWaterLevel( const Vec3 &pCurrPos ) const = 0;

  // Summary:
  //     Gets caustics parameters
  // Return Value:
  //     A Vec4 value which constains:
  //     x = unused, y = distance attenuation, z = caustics multiplier, w = caustics darkening multiplier  

  virtual Vec4 GetCausticsParams() const = 0;  

  // Summary:
  //     Gets ocean animation parameters
  // Return Value:
  //     2 Vec4 which constain:
  //     0: x = ocean wind direction, y = wind speed, z = waves speed, w = waves amount  
  //     1: x = waves size, y = free, z = free, w = free  

  virtual void GetOceanAnimationParams(Vec4 &pParams0, Vec4 &pParams1 ) const = 0;  

	/*! Get water level in position of specified object taking into account global water level 
      and water volumes. For indoor objects global water level is ignored.
			Function returns WATER_LEVEL_UNKNOWN if in specified position water was not found */

	// Summary:
	//     Removes all particles and decals from the world
	virtual void ResetParticlesAndDecals( ) = 0;

	// Summary:
	//     Creates a new particle emitter
	// Arguments:
	//		 bRegister - True if engine should render emitter itself. False if client will.
	//		 mLoc - World location of emitter.
	//		 pEffect - Effect to use, or
	//		 pParams - Specific particle params to use. 1 and only 1 of these 2 should be non-null.
	// Return Value:
	//     A pointer to an object derived from IParticleEmitter
	virtual IParticleEmitter* CreateParticleEmitter( bool bIndependent, Matrix34 const& mLoc, const ParticleParams& Params ) = 0;

	// Summary:
	//     Deletes a specified particle emitter
	// Arguments:
	//     pPartEmitter - Specify the emitter to delete
	virtual void DeleteParticleEmitter(IParticleEmitter * pPartEmitter) = 0;

	//////////////////////////////////////////////////////////////////////////
	// ParticleEffects
	//////////////////////////////////////////////////////////////////////////

	// Summary:
	//     Create a new particle effect object
	// Return Value:
	//     A pointer to a object derived from IParticleEffect.
	virtual IParticleEffect* CreateParticleEffect() = 0;

	// Summary:
	//     Deletes a specified particle effect
	// Arguments:
	//     pEffect - A pointer to the particle effect object to delete
	virtual void DeleteParticleEffect( IParticleEffect* pEffect ) = 0;

	// Summary:
	//     Searches by name one the particle effect
	// Arguments:
	//     sEffectName - The name of the particle effect to search
	//		 sCaller - Optional name of calling entity, for diagnosis
	// Return Value:
	//     A pointer to a particle effect object matching the specified name. In 
	//     case no effect has been found, the value NULL will be returned.
	virtual IParticleEffect* FindParticleEffect( const char *sEffectName, const char* sCallerType = "", const char* sCallerName = "", bool bLoad = true ) = 0;
	
	//////////////////////////////////////////////////////////////////////////

	// Summary:
	//     Creates new decals on the walls, static objects, terrain and entities
	// Arguments:
	//     Decal - Structure describing the decal effect to be applied
	virtual void CreateDecal( const CryEngineDecalInfo & Decal )=0;

	// Summary:
	//     Removes decals in a specified range
	// Arguments:
	//     vBoxMin - Specify the range in which the decals will be removed
	//     vBoxMax - Specify the range in which the decals will be removed
	//     pEntity - if not NULL will only delete decals attached to this entity
	virtual void DeleteDecalsInRange( AABB * pAreaBox, IRenderNode * pEntity ) = 0;

//DOC-IGNORE-BEGIN
  /*! Call back for renderer.
      Renders far trees/object as sprites. Will be removed from here.*/
	virtual void DrawFarTrees() = 0; // used by renderer

  virtual void GenerateFarTrees() = 0; // used by renderer

//DOC-IGNORE-END

	/*! Load cgf and caf files and creates animated object.
		Returns pointer to already loaded object with same name if found.
		Reference counting used */

	// Summary:
	//     Set the current outdoor ambient color 
	virtual void SetSkyColor(Vec3 vColor)=0;

	// Summary:
	//     Set the current sun color 
	virtual void SetSunColor(Vec3 vColor)=0;

	// Summary:
	//     Set the current sky brightening multiplier
	virtual void SetSkyBrightness(float fMul) = 0;

	// Summary:
	//     Get the current sun/sky color relation
	virtual float GetSunRel() = 0;

	// Summary:
	//     Set to a new sky box
	// Arguments:
	//     szShaderName - Name of the shader used for the sky box
//	virtual void SetSkyBox(const char * szShaderName) = 0;

	// Summary:
	//     Set the view distance
	// Arguments:
	//     fMaxViewDistance - Maximum view distance
//	virtual void SetMaxViewDistance(float fMaxViewDistance)=0;

	// Summary:
	//     Gets the view distance
	// Return Value:
	//     A float value representing the maximum view distance.
	virtual float GetMaxViewDistance()=0;
  
	//! Set/Get fog params

	// Summary:
	//     Set the fog color
	virtual void SetFogColor(const Vec3& vFogColor)=0;

	// Summary:
	//     Gets the fog color
	virtual Vec3 GetFogColor( )=0;
	
	// Summary:
	//   Gets volumetric fog settings
	virtual void GetVolumetricFogSettings( float& globalDensity, float& atmosphereHeight, float& artistTweakDensityOffset, float& globalDensityMultiplierLDR ) = 0;
	
	// Summary:
	//   Sets volumetric fog settings
	virtual void SetVolumetricFogSettings( float globalDensity, float atmosphereHeight, float artistTweakDensityOffset ) = 0;

	// Summary:
	//   Gets volumetric fog modifiers
	virtual void GetVolumetricFogModifiers(float& globalDensityModifier, float& atmosphereHeightModifier) = 0;

	// Summary:
	//   Sets volumetric fog modifiers
	virtual void SetVolumetricFogModifiers(float globalDensityModifier, float atmosphereHeightModifier, bool reset = false) = 0;

	// Summary:
	//   Gets various sky light parameters
	virtual void GetSkyLightParameters( Vec3& sunIntensity, float& Km, float& Kr, float& g, Vec3& rgbWaveLengths ) = 0;

	// Summary:
	//   Sets various sky light parameters
	virtual void SetSkyLightParameters( const Vec3& sunIntensity, float Km, float Kr, float g, 
		const Vec3& rgbWaveLengths, bool forceImmediateUpdate = false ) = 0;
	
	virtual void GetCloudShadingMultiplier( float& sunLightMultiplier, float& skyLightMultiplier ) = 0;
	virtual void SetCloudShadingMultiplier( float sunLightMultiplier, float skyLightMultiplier ) = 0;

	virtual IAutoCubeMapRenderNode* GetClosestAutoCubeMap(const Vec3& p) = 0;

	// Returns
	//   in the range 0.01 .. 100
	virtual float GetHDRDynamicMultiplier() const = 0;
	// Arguments:
	//   value - becomes clamped in range 0.01 .. 100
	virtual void SetHDRDynamicMultiplier( const float value ) = 0;

	virtual void TraceFogVolumes( const Vec3& worldPos, ColorF& fogVolumeContrib ) = 0;
	
	// Summary:
	//     Gets the interpolated terrain elevation for a specified location
	// Note:
	//     All x,y values are valid
	// Arguments:
	//     x - X coordinate of the location
	//     y - Y coordinate of the location
	// Return Value:
	//     A float which indicate the elevation level.
	virtual float GetTerrainElevation(float x, float y, bool bIncludeOutdoorVoxles = false) = 0;

	// Summary:
	//     Gets the terrain elevation for a specified location
	// Note:
	//     Only values between 0 and WORLD_SIZE.
	// Arguments:
	//     x - X coordinate of the location
	//     y - Y coordinate of the location
	// Return Value:
	//     A float which indicate the elevation level.
	virtual float GetTerrainZ(int x, int y) = 0;

  // Summary:
  //     Gets the terrain surface normal for a specified location
  // Arguments:
  //     vPos.x - X coordinate of the location
  //     vPos.y - Y coordinate of the location
  //     vPos.z - ignored
  // Return Value:
  //     A terrain surface normal.
  virtual Vec3 GetTerrainSurfaceNormal(Vec3 vPos) = 0;

	// Summary:
	//     Gets the unit size of the terrain
	// Note:
	//     The value should currently be 2.
	// Return Value:
	//     A int value representing the terrain unit size in meters.
	virtual int   GetHeightMapUnitSize() = 0;

	//! Returns size of terrain in meters ( currently is 2048 )

	// Summary:
	//     Gets the size of the terrain
	// Note:
	//     The value should be 2048 by default.
	// Return Value:
	//     An int representing the terrain size in meters.
	virtual int   GetTerrainSize()=0;


	//! Returns size of terrain sector in meters ( currently is 64 )

	// Summary:
	//     Gets the size of the terrain sectors
	// Note:
	//     The value should be 64 by default.
	// Return Value:
	//     An int representing the size of a sector in meters.
	virtual int   GetTerrainSectorSize()=0;

//DOC-IGNORE-BEGIN

	// Internal functions, mostly used by the editor, which won't be documented for now

	//! Places object at specified position (for editor)
//	virtual bool AddStaticObject(int nObjectID, const Vec3 & vPos, const float fScale, unsigned char ucBright=255) = 0;
	//! Removes static object from specified position (for editor)
//	virtual bool RemoveStaticObject(int nObjectID, const Vec3 & vPos) = 0;
	//! On-demand physicalization of a static object
//	virtual bool PhysicalizeStaticObject(void *pForeignData,int iForeignData,int iForeignFlags) = 0;
	//! Removes all static objects on the map (for editor)
	virtual void RemoveAllStaticObjects() = 0;
	//! Allows to set terrain surface type id for specified point in the map (for editor)
	virtual void SetTerrainSurfaceType(int x, int y, int nType)=0; // from 0 to 6 - sur type ( 7 = hole )
  
	//! Returns true if game modified terrain hight map since last update by editor
	virtual bool IsTerrainHightMapModifiedByGame() = 0;
	//! Updates hight map max height (in meters)
	virtual void SetHeightMapMaxHeight(float fMaxHeight) = 0;
  
	//! set terrain sector texture id, and disable streaming on this sector
	virtual void SetTerrainSectorTexture( const int nTexSectorX, const int nTexSectorY, unsigned int textureId ) = 0;


	//! returns size of smallest terrain texture node (last leaf) in meters
	virtual int GetTerrainTextureNodeSizeMeters() = 0;

	// returns size of terrain texture node
	// Arguments:
	//   nLayer - 0=diffuse texture, 1=occlusionmap
	// Return:
	//   in pixels
	virtual int GetTerrainTextureNodeSizePixels(int nLayer) = 0;

	//! Set group parameters
	virtual bool SetStatInstGroup(int nGroupId, const IStatInstGroup & siGroup) = 0;

	//! Get group parameters
	virtual bool GetStatInstGroup(int nGroupId,       IStatInstGroup & siGroup) = 0;

	//! Set burbed out flag
	virtual void SetTerrainBurnedOut(int x, int y, bool bBurnedOut) = 0;
	//! Get burbed out flag
	virtual bool IsTerrainBurnedOut(int x, int y) = 0;

//DOC-IGNORE-END

	// Summary:
	//   Notifies of an explosion, and maybe creates an hole in the terrain
	// Description:
	//   This function should usually make sure that no static objects are near before making the hole.
	// Arguments:
	//   vPos - Position of the explosion
	//   vHitDir - Direction of the explosion
	//   fRadius - Radius of the explosion
	//   texOrMatName - name of texture or material
	//	 nameIsMaterial - specifies whether "texOrMatName" is a texture or material name
	//   bDeformTerrain - Allow to deform the terrain
	virtual void OnExplosion(Vec3 vPos, float fRadius, bool bDeformTerrain = true) = 0;

//DOC-IGNORE-BEGIN
    // Not used anymore
	//! Makes 3d waves on the water surface
	virtual void AddWaterSplash (Vec3 vPos, enum eSplashType eST, float fForce, int Id=-1) = 0;
//DOC-IGNORE-END

	//! Force to draw quad on entire screen with specified shader (night vision) ( use szShaderName="" to disable drawing )

	//! Set physics material enumerator

	// Summary:
	//   Set the physics material enumerator
	// Arguments:
	//   pPhysMaterialEnumerator - The physics material enumarator to set
	virtual void SetPhysMaterialEnumerator(IPhysMaterialEnumerator * pPhysMaterialEnumerator) = 0;

	// Summary:
	//   Gets the physics material enumerator
	// Return Value:
	//   A pointer to an IPhysMaterialEnumerator derived object.
	virtual IPhysMaterialEnumerator * GetPhysMaterialEnumerator() = 0;

//DOC-IGNORE-BEGIN
//Internal functions
	
	//! Allows to enable fog in editor
	virtual void SetupDistanceFog() = 0;

	//! Load environment settings for specified mission
	virtual void LoadMissionDataFromXMLNode(const char * szMissionName) = 0;

	virtual void LoadEnvironmentSettingsFromXML(XmlNodeRef pInputNode) = 0;

	//! Load detail texture and detail object settings from XML doc (load from current LevelData.xml if pDoc is 0)
	virtual void	LoadTerrainSurfacesFromXML(XmlNodeRef pDoc, bool bUpdateTerrain) = 0;

//DOC-IGNORE-END

	//! Make move/bend vegetations in specified area (not implemented yet)

	// Description:
	//   Physics applied to the area will apply to vegetations and allow it to move/blend.
	// Arguments:
	//   vPos - Center position to apply physics
	//   fRadius - Radius which specify the size of the area to apply physics
	//   fAmountOfForce - The amount of force, should be at least of 1.0f
	// Summary:
	//   Applies physics in a specified area
	virtual void ApplyForceToEnvironment(Vec3 vPos, float fRadius, float fAmountOfForce) = 0;

//DOC-IGNORE-BEGIN
	// Set direction to the sun
//	virtual void SetSunDir( const Vec3& vNewSunDir ) = 0;

	// Return non-normalized direction to the sun
	virtual Vec3 GetSunDir() = 0;

	// Return normalized direction to the sun
  virtual Vec3 GetSunDirNormalized() = 0;

  // Return realtime (updated every frame with real sun position) normalized direction to the scene
  virtual Vec3 GetRealtimeSunDirNormalized() = 0;

// Internal function used by the 3d engine and editor
	//! Returns light mask for this point (valid only during rendering stage)
//	virtual unsigned int GetLightMaskFromPosition(const Vec3 & vPos, float fRadius=1.f) = 0;

// Internal function used by the 3d engine
	//! Returns lighting level for this point
	virtual Vec3 GetAmbientColorFromPosition(const Vec3 & vPos, float fRadius=1.f) = 0;

//Internal function used by 3d engine and renderer
	//! get distance to the sector containig ocean water
	virtual float GetDistanceToSectorWithWater() = 0;
//DOC-IGNORE-END

	//! get environment ambient color specified in editor

	// Summary:
	//   Gets the environment ambient color
	// Note:
	//   Should have been specified in the editor.
	// Return Value:
	//   An rgb value contained in a Vec3 object.
	virtual Vec3 GetSkyColor() = 0;

	//! get environment sun color specified in editor

	// Summary:
	//   Gets the sun color
	// Note:
	//   Should have been specified in the editor.
	// Return Value:
	//   An rgb value contained in a Vec3 object.
	virtual Vec3 GetSunColor() = 0;

	// Summary:
	//   retrieves the current sky brightening multiplier
	// Note:
	// Return Value:
	//   scalar value
	virtual float GetSkyBrightness() = 0;

  // Summary:
  //   retrieves the current SSAO multiplier
  // Note:
  // Return Value:
  //   scalar value
  virtual float GetSSAOAmount() = 0;

	// Returns:
	//   0..1, 0=no shadow, 1=full shadow
	virtual float GetSunShadowIntensity() const= 0;

	//! check object visibility taking into account portals and terrain occlusion test
	//  virtual bool IsBoxVisibleOnTheScreen(const Vec3 & vBoxMin, const Vec3 & vBoxMax, OcclusionTestClient * pOcclusionTestClient = NULL)=0;
	//! check object visibility taking into account portals and terrain occlusion test
	//  virtual bool IsSphereVisibleOnTheScreen(const Vec3 & vPos, const float fRadius, OcclusionTestClient * pOcclusionTestClient = NULL)=0;

	//! Clears all rendering resources, should be called before LoadLevel() and before loading of any textures from script

	// Summary:
	//   Clears all rendering resources
	// Note:
	//   Should always be called before LoadLevel, and also before loading textures from a script.
	// Arguments:
	virtual void ClearRenderResources()=0;

//mat: todo

	//! Free entity render info
	virtual void FreeRenderNodeState(IRenderNode * pEntity) = 0;

	//! Return pointer to full file name of file in current level folder

	// Summary:
	//   Add the level's path to a specified filename
	// Arguments:
	//   szFileName - The filename for which we need to add the path
	// Return Value:
	//   Full path for the filename; including the level path and the filename appended after.
	virtual const char * GetLevelFilePath(const char * szFileName) = 0;

	// Summary:
	//   Displays statistic on the 3d Engine
	// Arguments:
	//   fTextPosX - X position for the text
	//   fTextPosY - Y position for the text
	//   fTextStepY - Amount of pixels to distance each line
	//   bEnhanced - false=normal, true=more interesting information
	virtual void DisplayInfo(float & fTextPosX, float & fTextPosY, float & fTextStepY, const bool bEnhanced ) = 0;

	virtual void DrawText(float x, float y, const char * format, ...) PRINTF_PARAMS(4, 5) =0;
	virtual void DrawTextRA(float x, float y, const char * format, ...) PRINTF_PARAMS(4, 5) =0;

	//! Enable/Disable portal at specified position

	// Summary:
	//   Enable or disable portal at a specified position
	// Arguments:
	//   vPos - Position to place the portal
	//   bActivate - Set to true in order to enable the portal, or to false to disable
	//   pEntity - A pointer to the entity holding the portal
	virtual void ActivatePortal(const Vec3 &vPos, bool bActivate, const char * szEntityName) = 0;

//DOC-IGNORE-BEGIN
	//! Count memory usage
	virtual void GetMemoryUsage(class ICrySizer * pSizer)=0;
//DOC-IGNORE-END

	//! Create visarea

	// Summary:
	//   Creates a new VisArea
	// Return Value:
	//   A pointer to a newly created VisArea object
	virtual IVisArea * CreateVisArea() = 0;

	//! Delete visarea

	// Summary:
	//   Deletes a VisArea
	// Arguments:
	//   pVisArea - A pointer to the VisArea to delete
	virtual void DeleteVisArea(IVisArea * pVisArea) = 0;

	//! Update visarea

//mat: todo

	// Summary:
	//   Updates the VisArea
	// Arguments:
	//   pArea -
	//   pPoints - 
	//   nCount -
	//   szName -
	//   fHeight -
	//   vAmbientColor -
	//   bAffectedByOutLights -
	//   bSkyOnly -
	//   vDynAmbientColor -
	//   fViewDistRatio -
	//   bDoubleSide -
	//   bUseDeepness -
	//   bUseInIndoors -
	virtual void UpdateVisArea(IVisArea * pArea, const Vec3 * pPoints, int nCount, const char * szName, 
    const SVisAreaInfo & info, bool bReregisterObjects) = 0;

	// Summary:
	//   Determines if two VisAreas are connected
	// Description:
	//   Used to determine if a sound is potentially hearable between two VisAreas.
	// Arguments:
	//   pArea1 - A pointer to a VisArea
	//   pArea2 - A pointer to a VisArea
	//   nMaxRecursion - Maximum number of recursions to be done
	//   bSkipDisabledPortals - Indicate if disabled portals should be skipped
	// Return Value:
	//   A boolean value set to true if the two VisAreas are connected, else false will be returned.
	virtual bool IsVisAreasConnected(IVisArea * pArea1, IVisArea * pArea2, int nMaxRecursion = 1, bool bSkipDisabledPortals = true) = 0;

//mat: todo

	// Summary:
	//   Create instance of IRenderNode object with specified type.
	virtual IRenderNode* CreateRenderNode( EERType type ) = 0;

	//! Delete RenderNode object
	virtual void DeleteRenderNode(IRenderNode * pRenderNode) = 0;

	//! set global wind vector.
	virtual void SetWind( const Vec3 & vWind ) = 0;

  // ! get wind direction and force, averaged within a box.
  virtual Vec3 GetWind( const AABB & box, bool bIndoors ) const =0;

	//! return indoor visibility area containing this position

	// Description:
	//   Gets the VisArea which is present at a specified point.
	// Arguments:
	//	 bIgnorePortsl: If true, skip portal VisAreas in check.
	//	 pDist: If not 0: If point inside VisArea: set to the distance inside the VisArea, else distance outside nearest VisArea.
	// Return Value:
	//	 VisArea containing point, if any. 0 otherwise.
	virtual	IVisArea * GetVisAreaFromPos(const Vec3 &vPos) = 0;	

	//! collide, clip particle against vis areas.

	// Description:
	//   Tests for intersection against Vis Areas.
	// Arguments:
	//   box: Volume to test for intersection.
	//   pNodeCache (out, optional): Set to a cached pointer, for quicker calls to ClipToVisAreas.
	// Return Value:
	//	 Whether box intersects any vis areas.
	virtual	bool IntersectsVisAreas(const AABB& box, void** pNodeCache = 0) = 0;	

	// Description:
	//   Clips geometry against the boundaries of VisAreas.
	// Arguments:
	//   pInside: Vis Area to clip inside of. If 0, clip outside all Vis Areas.
	// Return Value:
	//	 Whether it was clipped
	virtual	bool ClipToVisAreas(IVisArea* pInside, Sphere& sphere, Vec3 const& vNormal, void* pNodeCache = 0) = 0;	

	//! enable/disable ocean rendering

	// Summary:
	//   Enable or disable ocean rendering.
	// Arguments:
	//   bOcean - Will enable or disable the rendering of ocean
	virtual void EnableOceanRendering(bool bOcean) = 0;

	// Description:
	//    Retrieve pointer to the material manager interface.
	virtual IMaterialManager* GetMaterialManager() = 0;
	
//DOC-IGNORE-BEGIN
//Internal for the the lightmaps
	//! \brief Create an instance of a lightmap serialization manager
	virtual struct ILMSerializationManager * CreateLMSerializationManager() = 0;
//DOC-IGNORE-END

	//! create new static lsource, returns source id or -1 if it fails

	// Summary:
	//   Creates a new light source
	// Return Value:
	//   Pointer to newly created light.
	virtual struct ILightSource * CreateLightSource() = 0;

	//! delete static lsource (return false if not found)

	// Summary:
	//   Deletes a light
	// Arguments:
	//   Pointer to the light
	virtual void DeleteLightSource(ILightSource * pLightSource) = 0;

	//! gives access to static lsources list (for lmap generator)

	// Summary:
	//   Gives access to the list holding all static light sources
	// Return Value:
	//   An array holding all the CDLight pointers.
	virtual const PodArray<CDLight*> * GetStaticLightSources() = 0;
	virtual const PodArray<ILightSource*> * GetLightEntities() = 0;


  //! Reload heightmap and reset decals and particles, in future will restore deleted vegetations

	// Summary:
	//   Reload the heightmap
	// Description:
	//   Reloading the heightmap will resets all decals and particles.
	// Returns:
	//   success
  virtual bool RestoreTerrainFromDisk() = 0;

//DOC-IGNORE-BEGIN
  // tmp
  virtual const char * GetFilePath(const char * szFileName) { return GetLevelFilePath(szFileName); }
//DOC-IGNORE-END

  //! Post processing effects interfaces    

  virtual void SetPostEffectParam(const char *pParam, float fValue) const = 0;
  virtual void SetPostEffectParamVec4(const char *pParam, const Vec4 &pValue) const = 0;
  virtual void SetPostEffectParamString(const char *pParam, const char *pszArg) const = 0;

  virtual void GetPostEffectParam(const char *pParam, float &fValue) const = 0;  
  virtual void GetPostEffectParamVec4(const char *pParam, Vec4 &pValue) const = 0;  
  virtual void GetPostEffectParamString(const char *pParam, const char *pszArg) const = 0;

  virtual void ResetPostEffects() const = 0;
    
  //! physicalize area if not physicalized yet
  virtual void CheckPhysicalized(const Vec3 & vBoxMin, const Vec3 & vBoxMax) = 0;

	//! in debug mode check memory heap and makes assert, do nothing in release
	virtual void CheckMemoryHeap() = 0;

	/*!
	Set material parameter
	@param szMatName materal name
	@param nTexSlot text slot id, see EFTT_DIFFUSE for example
	@param nSubMatId submaterial id, -1 means use root material
	@param szParamName can be one of:
		
		m_eTGType
		m_eRotType
		m_eUMoveType
		m_eVMoveType
		m_bTexGenProjected

		m_Tiling[0]
		m_Tiling[1]
    m_Tiling[2]
		m_Offs[0]
		m_Offs[1]
		m_Offs[2]

		m_Rot[0]
		m_Rot[1]
		m_Rot[2]
		m_RotOscRate[0]
		m_RotOscRate[1]
		m_RotOscRate[2]
		m_RotOscAmplitude[0]
		m_RotOscAmplitude[1]
		m_RotOscAmplitude[2]
		m_RotOscPhase[0]
		m_RotOscPhase[1]
		m_RotOscPhase[2]

		m_UOscRate
		m_VOscRate
		m_UOscAmplitude
		m_VOscAmplitude
		m_UOscPhase
		m_VOscPhase

	@param fValue new value
*/
	virtual bool SetMaterialFloat( char * szMatName, int nSubMatId, int nTexSlot, char * szParamName, float fValue ) = 0;

	//! close terrain texture file handle and allows to replace/update it
	virtual void CloseTerrainTextureFile() = 0;
	
	//! remove all decals attached to specified entity
	virtual void DeleteEntityDecals(IRenderNode * pEntity) = 0;

	//! prepare data for rendering
	virtual void LoadLightmaps() = 0;

	//! finish objects geometery generation/loading
	virtual void CompleteObjectsGeometry() = 0;

	//! disable CGFs unloading
	virtual void LockCGFResources() = 0;

	//! enable CGFs unloading (this is default state), this function will also release all not used CGF's
	virtual void UnlockCGFResources() = 0;

	//! give access to materials library
	virtual class CMatMan * GetMatMan() = 0;

	//! create static object containing empty IndexedMesh
	virtual IStatObj * CreateStatObj() = 0;

	//! Create the instance of the indexed mesh.
	virtual IIndexedMesh* CreateIndexedMesh() = 0;

	//! paint voxel shape
	virtual void Voxel_Paint(Vec3 vPos, float fRadius, int nSurfaceTypeId, Vec3 vBaseColor, EVoxelEditOperation eOperation, EVoxelBrushShape eShape, EVoxelEditTarget eTarget) = 0;

	//! get list of voxel objects that will be affected by paint operation, IMemoryBlock will contain array of IVoxelObject pointers
	virtual IMemoryBlock * Voxel_GetObjects(Vec3 vPos, float fRadius, int nSurfaceTypeId, EVoxelEditOperation eOperation, EVoxelBrushShape eShape, EVoxelEditTarget eTarget) = 0;

	//! setup voxel flags
	virtual void Voxel_SetFlags(bool bPhysics, bool bSimplify, bool bShadows, bool bMaterials) = 0;

	//! updates rendering mesh in teh stat obj associated with pPhysGeom (creates or clones the object if necessary)
	virtual IStatObj * UpdateDeformableStatObj(IGeometry *pPhysGeom, bop_meshupdate *pLastUpdate=0, IFoliage *pSrcFoliage=0) = 0;

	//! deforms (smears) pObj near the hit point along dir
	virtual IStatObj * SmearStatObj(IStatObj *pObj, const Vec3 &pthit,const Vec3 &dir, float r,float depth) = 0;

	//! save/load state of engine objects
	virtual void SerializeState( TSerialize ser ) = 0;

	//! cleanup after save/load	
	virtual void PostSerialize( bool bReading ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// CGF Loader.
	//////////////////////////////////////////////////////////////////////////
	// Description:
	//    Releases a CGF Content data allocated by LoadChunkFileContent.
	virtual CContentCGF* LoadChunkFileContent( const char *filename,bool bNoWarningMode=false ) = 0;
	// Description:
	//    Releases a CGF Content data allocated by LoadChunkFileContent.
	virtual void ReleaseChunkFileContent( CContentCGF *pCGF ) = 0;

	// Description:
	//    Create ChunkFile.
	virtual IChunkFile * CreateChunkFile( bool bReadOnly=false ) = 0;

	//! return interface to terrain engine
	virtual ITerrain * GetITerrain() = 0;

	//! create terrain engine
	virtual ITerrain * CreateTerrain(const STerrainInfo & TerrainInfo) = 0;

	//! delete terrain
	virtual void DeleteTerrain() = 0;

	//! return interface to visarea manager
	virtual IVisAreaManager * GetIVisAreaManager() = 0;

	//! callback function used by renderer for imposters creation
//	virtual void RenderImposterContent(class CREImposter * pImposter, const CCamera & cam) = 0;

	//! return array of voxel render meshes, nCount will get number of elements writed
	virtual void GetVoxelRenderNodes(struct IRenderNode**pRenderNodes, int & nCount) = 0;
   
  //! Return amount of light affecting a point in space inside a specific range (0 means no light affecting,
  // 1 is completely affected by light). Use accurate parameter for a more expensive but with higher accuracy computation
  virtual float GetLightAmountInRange(const Vec3 &pPos, float fRange, bool bAccurate = 0) = 0;

	//! Places camera into every visarea or every manually set pre-cache points and render the scenes
	virtual void PrecacheLevel(bool bPrecacheAllVisAreas, Vec3 * pPrecachePoints, int nPrecachePointsNum) = 0;

  //! Propose 3dengine to load on next frame all shaders and textures synchronously
  virtual void ProposeContentPrecache() = 0;

  //! Return TOD interface
	virtual ITimeOfDay* GetTimeOfDay() = 0;

	// Description:
	//    Set global 3d engine parameter.
	virtual void SetGlobalParameter( E3DEngineParameter param,const Vec3 &v ) = 0;
	void SetGlobalParameter( E3DEngineParameter param,float val ) { SetGlobalParameter( param,Vec3(val,0,0) ); };

	// Description:
	//    Retrieve global 3d engine parameter.
	virtual void GetGlobalParameter( E3DEngineParameter param,Vec3 &v ) = 0;
	float GetGlobalParameter( E3DEngineParameter param ) { Vec3 v(0,0,0); GetGlobalParameter(param,v); return v.x; };

	// Description
	//		Saves pStatObj to a stream (full mesh for generated ones, path/geom otherwise)
	virtual int SaveStatObj(IStatObj *pStatObj, TSerialize ser) = 0;
	// Description
	//		Loads statobj from a stream
	virtual IStatObj *LoadStatObj(TSerialize ser) = 0;

	//! Return true if input line segment intersect clouds sprites.
	virtual bool CheckIntersectClouds(const Vec3 & p1, const Vec3 & p2) = 0;

	//! Remove references to RenderMesh
	virtual void OnRenderMeshDeleted(IRenderMesh * pRenderMesh) = 0;

	// used to visualize frustum updates
	// Arguments:
	//   szName - must not be 0
	virtual void DebugDraw_PushFrustrum( const char *szName, const CCamera &rCam, const ColorB col, const float fQuadDist=-1.0f ) = 0;
	// used to visualize frustum updates
	// Arguments:
	//   szName - must not be 0
	virtual void DebugDraw_PushFrustrum( const char *szName, const CRenderCamera &rCam, const ColorB col, const float fQuadDist=-1.0f ) = 0;

	// used by editor during AO computations
	virtual bool RayObjectsIntersection( Vec3 vStart, Vec3 vEnd, Vec3 & vHitPoint, EERType eERType ) = 0;

	// free lod transition state
	virtual void FreeRNTmpData(struct CRNTmpData ** ppInfo) = 0;

  // true if e_ambient_occlusion is ON and AO data is valid
  virtual bool IsAmbientOcclusionEnabled() = 0;

	// call function 2 times (first to get the size then to fill in the data)
	// Arguments:
	//   pObjects - 0 if only the count is required
	// Returns
	//   returned count
	virtual uint32 GetObjectsByType( EERType objType, IRenderNode **pObjects=0 )=0;

  virtual struct CLoadingTimeContainer * StartLoadingSectionProfiling(struct CLoadingTimeProfiler * pProfiler, const char * szFuncName) = 0;
  virtual void EndLoadingSectionProfiling(struct CLoadingTimeProfiler * pProfiler) = 0;

  // Print loading time stats into log
  virtual void OutputLoadingTimeStats() = 0;

	virtual void FillDebugFPSInfo(SDebugFPSInfo&) = 0;

	// uses raytracing against the object's rendermesh to refine a physics hit 
	// dir does not have to be normalized
	virtual bool RefineRayHit(ray_hit *phit, const Vec3 &dir) = 0;
};


//==============================================================================================

#pragma pack(push,1)

//! Types of binary files used by 3dengine
enum EFileTypes
{
	eTerrainTextureFile=100,
};

#define FILEVERSION_TERRAIN_TEXTURE_FILE 8

// Common header for binary files used by 3dengine
struct SCommonFileHeader
{
	void Set(ushort t, ushort v) { strcpy(signature,"CRY"); type = t; version = v; }
	bool Check(ushort t, ushort v) { return strcmp(signature,"CRY")==0 && t==type && v==version; }

	char				signature[4];						// File signature, should be "CRY "
	ushort			type;										// File type
	ushort			version;								// File version

	AUTO_STRUCT_INFO
};

// "locally higher texture resolution" following structure can be removed (as well in autotype)
// Sub header for terrain texture file
struct STerrainTextureFileHeader_old
{
	ushort			nSectorSizeMeters;			//
	ushort			nLodsNum;								//
	ushort			nLayerCount;						// STerrainTextureLayerFileHeader count following (also defines how may layers are interleaved) 1/2
	ushort			nReserved;

  AUTO_STRUCT_INFO
};

#define TTFHF_AO_DATA_IS_VALID 1

// Sub header for terrain texture file
struct STerrainTextureFileHeader
{
	ushort			nLayerCount;						// STerrainTextureLayerFileHeader count following (also defines how may layers are interleaved) 1/2
	ushort			dwFlags;
	float				m_fSunShadowIntensity;	// 0=no shadow..1=full shadow

  AUTO_STRUCT_INFO
};

// layer header for terrain texture file (for each layer)
struct STerrainTextureLayerFileHeader
{
	ushort			nSectorSizePixels;	//
	ushort			nReserved;					// ensure padding and for later usage
	ETEX_Format eTexFormat;					// typically eTF_DXT1, eTF_A4R4G4B4 or eTF_R5G6B5
	uint32			nSectorSizeBytes;		// redundant information for more convenient loading code

  AUTO_STRUCT_INFO
};

#pragma pack(pop)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

struct CLoadingTimeProfiler
{
  CLoadingTimeProfiler (ISystem * pSystem, const char * szFuncName) : m_pSystem (pSystem)
  {
    m_pSystem = pSystem;
    if(I3DEngine * pEng = m_pSystem->GetI3DEngine())
      m_pTimeContainer = pEng->StartLoadingSectionProfiling(this, szFuncName);
  }

  ~CLoadingTimeProfiler ()
  {
    if(I3DEngine * pEng = m_pSystem->GetI3DEngine())
      pEng->EndLoadingSectionProfiling(this);
  }

  struct CLoadingTimeContainer * m_pTimeContainer;
  double m_fConstructorTime;
  double m_fConstructorMemUsage;
  ISystem* m_pSystem;
};

#define LOADING_TIME_PROFILE_SECTION(pSystem) CLoadingTimeProfiler __section_loading_time_auto_profiler(pSystem, __FUNCTION__);

// experimental way to track interface version 
// this value will be compared with value passed from system module
const char g3deInterfaceVersion[32] = __TIMESTAMP__;

// CreateCry3DEngine function type definition
typedef I3DEngine * (*PFNCREATECRY3DENGINE)(ISystem	* pSystem,const char * szInterfaceVersion);

//! Creates 3dengine instance

// Description:
//     Create an instance of the 3D Engine. It should usually be called by 
//     ISystem::Init3DEngine.
// See Also:
//     I3DEngine, I3DEngine::Release
// Arguments:
//     ISystem            - Pointer to the current ISystem instance
//     szInterfaceVersion - String version of with the build date
// Summary:
//     Create an instance of the 3D Engine
CRY3DENGINEENGINE_API I3DEngine * CreateCry3DEngine(ISystem	* pSystem,const char * szInterfaceVersion=g3deInterfaceVersion);

#ifdef __cplusplus
}
#endif

#endif //CRY3DENGINEINTERFACE_H

