// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef IRenderNodeSTATE_H
#define IRenderNodeSTATE_H

// !!! don't change the type !!!
//typedef unsigned short EntityId;					//! unique identifier for each entity instance
#include "IEntity.h"												// EntityId

struct IMaterial;
struct IVisArea;

#define MAX_RECURSION_LEVELS 2

enum EERType
{
	eERType_Unknown,
	eERType_Brush,
	eERType_Vegetation,
	eERType_VoxelMesh,
	eERType_Light,
	eERType_Cloud,
	eERType_VoxelObject,
	eERType_FogVolume,
	eERType_RoadObject_NOT_USED,
	eERType_Decal,
	eERType_ParticleEmitter,
	eERType_WaterVolume,
	eERType_WaterWave,
	eERType_RoadObject_NEW,
	eERType_DistanceCloud,
	eERType_VolumeObject,
	eERType_AutoCubeMap,
	eERType_Rope,
	eERType_Last,
};

struct OcclusionTestClient
{
	OcclusionTestClient() {	nLastOccludedMainFrameID = nLastVisibleMainFrameID = 0; vLastVisPoint.Set(0,0,0); }
	int nLastVisibleMainFrameID, nLastOccludedMainFrameID;
  Vec3 vLastVisPoint;
};

//! RenderNode flags
#define ERF_GOOD_OCCLUDER								0x1
#define ERF_PROCEDURAL									0x2
#define ERF_USE_TERRAIN_COLOR           0x4
#define ERF_CASTSHADOWMAPS							0x8
#define ERF_RENDER_ALWAYS				        0x10
#define ERF_CASTSHADOWINTORAMMAP				0x20
#define ERF_HIDABLE											0x40
#define ERF_HIDABLE_SECONDARY						0x80
#define ERF_HIDDEN											0x100
#define ERF_SELECTED										0x200
#define ERF_USERAMMAPS									0x400
#define ERF_OUTDOORONLY									0x800
#define ERF_UPDATE_IF_PV								0x1000
#define ERF_EXCLUDE_FROM_TRIANGULATION	0x2000
#define ERF_REGISTER_BY_BBOX            0x4000
#define ERF_PICKABLE				            0x8000
#define ERF_FROOZEN											0x10000
#define ERF_MERGE_RESULT  							0x20000
#define ERF_NO_DECALNODE_DECALS 				0x40000
#define ERF_REGISTER_BY_POSITION				0x80000
#define ERF_SUBSURFSCATTER              0x100000
#define ERF_RECVWIND                    0x200000
#define ERF_COLLISION_PROXY             0x400000    // Collision proxy is a special object that is only visible in editor
                                                    // and used for physical collisions with player and vehicles
#define ERF_AFFECTED_BY_VOXELS          0x800000
#define ERF_SPEC_BIT0                   0x1000000   // Bit0 of min config spec.
#define ERF_SPEC_BIT1                   0x2000000   // Bit1 of min config spec.
#define ERF_SPEC_BIT2                   0x4000000   // Bit2 of min config spec.
#define ERF_SPEC_BITS_MASK              (ERF_SPEC_BIT0|ERF_SPEC_BIT1|ERF_SPEC_BIT2)  // Bit mask of the min spec bits.
#define ERF_SPEC_BITS_SHIFT             (24)          // Bit oOffset of the ERF_SPEC_BIT0.

// Should be the same as FOB_ flags
#define ERF_NOTRANS_ROTATE  					  0x10000000
#define ERF_NOTRANS_SCALE 						  0x20000000
#define ERF_NOTRANS_TRANSLATE					  0x40000000
#define ERF_NOTRANS_MASK (ERF_NOTRANS_ROTATE | ERF_NOTRANS_SCALE | ERF_NOTRANS_TRANSLATE)


struct IShadowCaster
{
	virtual bool HasOcclusionmap(int nLod, IRenderNode *pLightOwner ) { return false;}
	virtual bool Render(const SRendParams &RendParams) = 0;
	virtual const AABB GetBBoxVirtual() = 0;
  virtual struct ICharacterInstance* GetEntityCharacter( unsigned int nSlot, Matrix34 * pMatrix = NULL, bool bReturnOnlyVisible = false ) = 0;
};

struct IOctreeNode 
{
	struct CVisArea * m_pVisArea;
	struct CTerrainNode * m_pTerrainNode;
  bool m_bCompiled; // if true - this node needs to be recompiled for example update nodes max view distance
};

struct SLodTransitionState
{
  float fStartTime;
  IStatObj * pStatObj;
  uchar ucLods[2];
};

struct SLightInfo
{
  bool operator == (const SLightInfo & other) const 
  { return other.vPos.IsEquivalent(vPos, 0.1f) && fabs(other.fRadius - fRadius)< 0.1f; }
  Vec3 vPos;
  float fRadius;
  bool bAffecting;
};

struct IRenderNode : public IShadowCaster
{
	enum EInternalFlags
	{
		DECAL_OWNER     = BIT(0), // Owns some decals.
		PARTICLES_OWNER = BIT(1), // Owns particles.
		UPDATE_DECALS   = BIT(2), // The node changed geometry - decals must be updated
		PER_OBJECT_SHADOW_PASS_NEEDED = BIT(3),  // special shadow processing needed
		WAS_INVISIBLE   = BIT(4), // was invisible last frame 
		WAS_IN_VISAREA  = BIT(5), // was inside vis-ares last frame
		WAS_FARAWAY			= BIT(6),// was considered 'far away' for the purposes of physics deactivation
		HAS_OCCLUSION_PROXY = BIT(7) // This node has occlusion proxy
	};

	IRenderNode()
	{
		m_dwRndFlags = 0;
		m_ucViewDistRatio = 100;
		m_ucLodRatio = 100;
		m_pOcNode = 0;
		m_fWSMaxViewDist = 0;
		m_nInternalFlags = 0;
    m_nMaterialLayers = 0;    
    m_pRNTmpData = NULL;
    m_pPrev = m_pNext = NULL;
  }

	// debug info about object
	virtual const char* GetName() const = 0;
	virtual const char* GetEntityClassName() const = 0;
	virtual string GetDebugString( char type = 0 ) const { return ""; }
	virtual float GetImportance() const	{ return 1.f; }

	// Description:
	//    Releases IRenderNode.
	virtual void ReleaseNode() { delete this; }

	// Description:
	//    Set render node transformation matrix.
	virtual void SetMatrix( const Matrix34& mat ) {}

	// Description:
	//    Get local bounds of the render node.
  virtual void GetLocalBounds( AABB &bbox ) { AABB WSBBox(GetBBox()); bbox = AABB(WSBBox.min - GetPos(true), WSBBox.max - GetPos(true)); }

	virtual Vec3 GetPos(bool bWorldOnly = true) const = 0;
	virtual const AABB GetBBox() const = 0;
	virtual void SetBBox( const AABB& WSBBox ) = 0;

	// render node geometry
	virtual bool Render(const struct SRendParams & EntDrawParams) = 0;

	// hide/disable node in renderer
	virtual void Hide( bool bHide )
	{
		SetRndFlags( ERF_HIDDEN, bHide );
	}

	// gives access to object components
	virtual struct IStatObj * GetEntityStatObj( unsigned int nPartId, unsigned int nSubPartId = 0, Matrix34 * pMatrix = NULL, bool bReturnOnlyVisible = false) { return 0; }
	virtual void SetEntityStatObj( unsigned int nSlot, IStatObj * pStatObj, const Matrix34 * pMatrix = NULL ) {};
	virtual struct ICharacterInstance* GetEntityCharacter( unsigned int nSlot, Matrix34 * pMatrix = NULL, bool bReturnOnlyVisible = false ) { return 0; }

	// rendering flags
	ILINE void SetRndFlags(unsigned int dwFlags) { m_dwRndFlags = dwFlags; }
	ILINE void SetRndFlags(unsigned int dwFlags, bool bEnable) { if(bEnable) SetRndFlags(m_dwRndFlags|dwFlags); else SetRndFlags(m_dwRndFlags&(~dwFlags)); }
	ILINE unsigned int GetRndFlags() const { return m_dwRndFlags; }

	// object draw frames (set if was drawn)
	ILINE void SetDrawFrame( int nFrameID, int nRecursionLevel ) 
  { 
    assert(m_pRNTmpData);
    int * pDrawFrames = (int *)m_pRNTmpData;
    pDrawFrames[nRecursionLevel] = nFrameID; 
  }

	ILINE int GetDrawFrame( int nRecursionLevel = 0 ) const
  { 
    if(!m_pRNTmpData)
      return 0;

    int * pDrawFrames = (int *)m_pRNTmpData;
    return pDrawFrames[nRecursionLevel]; 
  }

	//## Lightmaps (here dot3lightmaps only)

	// Summary: 
	//   Assigns a texture set reference for dot3 lightmapping. The object will Release() it at the end of its lifetime
	virtual void SetLightmap(struct RenderLMData * pLMData, float *pTexCoords, uint32 iNumTexCoords, int nLod=0,const int32 SubObjIdx=0) {};

	// Summary: 
	//   Assigns a texture set reference for dot3 lightmapping. The object will Release() it at the end of its lifetime, special call from lightmap serializer/compiler to set occlusion map values
	virtual void SetLightmap(RenderLMData *pLMData, float *pTexCoords, uint32 iNumTexCoords, const unsigned char cucOcclIDCount, /*const std::vector<std::pair<EntityId, EntityId> >& aIDs, */const int8 nFirstOcclusionChannel,const int32 SubObjIdx){};

	//  Returns:
	//    true if there are occlusion map texture coodinates and a occlusion map texture assignment
	virtual bool HasOcclusionmap(int nLod, IRenderNode *pLightOwner ) { return false; };

	//  Returns:
	//    true if there are lightmap texture coodinates and a lightmap texture assignment
	virtual bool HasLightmap(int nLod) { return false; };
	//  Returns:
	//    Lightmap texture set for this object, or NULL if there's none assigned. Don't release obtained copy, it's not a reference
	//  See Also: 
	//    SetLightmap
	//	virtual RenderLMData * GetLightmap(int nLod) { return 0; };
	virtual int GetLightmap(int nLod) { return 0; };
	// Summary:
	//   Returns vertex buffer holding instance specific texture coordinate set for dot3 lightmaps
	//	virtual struct IRenderMesh * GetLightmapTexCoord(int nLod) { return 0; };
	virtual int GetLightmapTexCoord(int nLod) { return 0; };
	virtual SLMData* GetLightmapData(int nLod,int SubObject=-1)	{	return 0;}

	// Summary:
	//   Returns IRenderMesh of the object
	virtual struct IRenderMesh * GetRenderMesh(int nLod) { return 0; };

  // Returns:
	//   Current VisArea or null if in outdoors or entity was not registered in 3dengine
	IVisArea * GetEntityVisArea() const { return m_pOcNode ? (IVisArea*)(m_pOcNode->m_pVisArea) : NULL; }

	// Returns:
	//   Current VisArea or null if in outdoors or entity was not registered in 3dengine
  struct CTerrainNode * GetEntityTerrainNode() const { return (m_pOcNode && !m_pOcNode->m_pVisArea) ? m_pOcNode->m_pTerrainNode : NULL; }

  /* Allows to adjust default max view distance settings, 
    if fMaxViewDistRatio is 100 - default max view distance is used */
  virtual void SetViewDistRatio(int nViewDistRatio) 
  { 
    nViewDistRatio = SATURATEB(nViewDistRatio);
    if(m_ucViewDistRatio != nViewDistRatio)
    {
      m_ucViewDistRatio = nViewDistRatio; 
      if(m_pOcNode) 
        m_pOcNode->m_bCompiled = false; 
    }
  }

	/*! Makes object visible at any distance */
	ILINE void SetViewDistUnlimited() { SetViewDistRatio(255); }

	// Summary:
	//   Retrieves the view distance settings
	ILINE int GetViewDistRatio() const { return (m_ucViewDistRatio==255) ? 1000l : m_ucViewDistRatio; }

  //! return max view distance ratio
  ILINE float GetViewDistRatioNormilized() const { return 0.01f*GetViewDistRatio(); }

  /*! Allows to adjust default lod distance settings, 
  if fLodRatio is 100 - default lod distance is used */
  virtual void SetLodRatio(int nLodRatio) { m_ucLodRatio = min(255,max(0,nLodRatio)); }
	
	//! return lod distance ratio
	ILINE int GetLodRatio() const { return m_ucLodRatio; }

  //! return lod distance ratio
  ILINE float GetLodRatioNormalized() const { return 0.01f*m_ucLodRatio; }

  //! set material layers mask
  ILINE void SetMaterialLayers(uint8 nMtlLayers) { m_nMaterialLayers = nMtlLayers; }

	virtual void SetMinSpec( int nMinSpec )
	{
		m_dwRndFlags &= ~ERF_SPEC_BITS_MASK;
		m_dwRndFlags |= (nMinSpec << ERF_SPEC_BITS_SHIFT) & ERF_SPEC_BITS_MASK;
	};
	ILINE int  GetMinSpec() const { return (m_dwRndFlags&ERF_SPEC_BITS_MASK) >> ERF_SPEC_BITS_SHIFT; };

  //! get material layers mask
  virtual uint8 GetMaterialLayers() const { return m_nMaterialLayers; }

	// get/set physical entity
	virtual struct IPhysicalEntity* GetPhysics() const = 0;
	virtual void SetPhysics( IPhysicalEntity* pPhys ) = 0;

	//! Physicalize if it isn't already
	virtual void CheckPhysicalized() {};

	// physicalize node
	virtual void Physicalize(bool bInstant=false) {} 
	// physicalize stat obj's foliage
	virtual bool PhysicalizeFoliage(bool bPhysicalize=true, int iSource=0) { return false; } 
	// get physical entity (rope) for a given branch (if foliage is physicalized)
	virtual IPhysicalEntity* GetBranchPhys(int idx) { return 0; }
	// returns physicalized foliage, if it *is* physicalized
	virtual struct IFoliage* GetFoliage() { return 0; }

  virtual ~IRenderNode() 
  {     
    // Make sure I3DEngine::FreeRenderNodeState(this) is called in destructor of derived class
    assert(!m_pRNTmpData); 
  };

	//! Set override material for this instance.
	virtual void SetMaterial( IMaterial *pMat ) = 0;
	//! Query override material of this instance.
	virtual IMaterial* GetMaterial(Vec3 * pHitPos = NULL) = 0;

  virtual int GetEditorObjectId() { return 0; }
  virtual void SetEditorObjectId(int nEditorObjectId) {}
  virtual void SetStatObjGroupId(int nVegetationanceGroupId) { }
	virtual void SetMergeGroupId(int nId) { }
  virtual float GetMaxViewDist() { return 0; }

//  virtual void Serialize(bool bSave, struct ICryPak * pPak, FILE * f) {}
  virtual EERType GetRenderNodeType() { return eERType_Unknown; }
  virtual void Dephysicalize( bool bKeepIfReferenced=false ) {}
  virtual void Dematerialize( ) {}
  virtual void GetMemoryUsage(ICrySizer * pSizer) { }

	virtual void Precache() {};
  
	virtual const AABB GetBBoxVirtual() { return GetBBox(); }

	virtual float GetLodForDistance(float fDistance) { return 0; }

  virtual void SetWindBending( Vec2 &pWindBending ) {}
  virtual Vec2 GetWindBending() { return Vec2(0.0f,0.0f); }

  virtual void OnRenderNodeBecomeVisible() {}

  virtual	bool IsMovableByGame() const { return false; }

	//////////////////////////////////////////////////////////////////////////
	// Variables
	//////////////////////////////////////////////////////////////////////////
public:

	// max view distance
	float m_fWSMaxViewDist;
	
	// render flags
	int m_dwRndFlags; 

	// current objects tree cell
	IOctreeNode * m_pOcNode;

	// max view distance settings
	unsigned char m_ucViewDistRatio;

	// LOD settings
	unsigned char m_ucLodRatio;

	// Flags for render node internal usage, one or more bits from EInternalFlags.
	unsigned char m_nInternalFlags;

	// material layers bitmask -> which material layers are active
	unsigned char m_nMaterialLayers;

  // pointer to temporary data allocated only for currently visible objects
  struct CRNTmpData * m_pRNTmpData;

  // every sector has linked list of IRenderNode objects
  IRenderNode * m_pNext, * m_pPrev;
};

struct SVegetationSpriteInfo
{
  struct SSectorTextureSet * pTerrainTexInfoForSprite;
  class CVegetation * pVegetation;
  uint16 dwAngle;
  uint16 ucSpriteAlphaTestRef;
};

struct ILightSource : public IRenderNode
{
	virtual void SetLightProperties(const CDLight & light) = 0;
	virtual CDLight &GetLightProperties() = 0;
	virtual const Matrix34& GetMatrix() = 0;
	virtual struct ShadowMapFrustum * GetShadowFrustum(int nId = 0) = 0;
	virtual bool IsLightAreasVisible() = 0;
  virtual void SetCastingException(IRenderNode * pNotCaster) = 0;
};

//////////////////////////////////////////////////////////////////////////
// ICloudRenderNode is an interface to the Cloud Render Node object.
//////////////////////////////////////////////////////////////////////////
struct SCloudMovementProperties
{
	bool m_autoMove;
	Vec3 m_speed;
	Vec3 m_spaceLoopBox;
	float m_fadeDistance;
};

struct ICloudRenderNode : public IRenderNode
{
	// Description:
	//    Loads a cloud from a cloud description XML file.
	virtual bool LoadCloud( const char *sCloudFilename ) = 0;
	virtual bool LoadCloudFromXml( XmlNodeRef cloudNode ) = 0;
	virtual void SetMovementProperties(const SCloudMovementProperties& properties) = 0;
};

//////////////////////////////////////////////////////////////////////////
// IRoadRenderNode is an interface to the Road Render Node object.
//////////////////////////////////////////////////////////////////////////
struct IRoadRenderNode : public IRenderNode
{
	virtual void SetVertices(const Vec3 *pVerts, int nVertsNum, float fTexCoordBegin, float fTexCoordEnd, float fTexCoordBeginGlobal, float fTexCoordEndGlobal) = 0;
	virtual void SetSortPriority(uint8 sortPrio) = 0;
};

const int IVOXELOBJECT_FLAG_LINK_TO_TERRAIN = 1;
const int IVOXELOBJECT_FLAG_GENERATE_LODS = 2;
const int IVOXELOBJECT_FLAG_COMPUTE_AO = 4;
const int IVOXELOBJECT_FLAG_SNAP_TO_TERRAIN = 8;
const int IVOXELOBJECT_FLAG_SMART_BASE_COLOR = 16;
const int IVOXELOBJECT_FLAG_COMPILED = 32;
const int IVOXELOBJECT_FLAG_EXIST = 64;

//////////////////////////////////////////////////////////////////////////
// IVoxelObject is an interface to the Voxel Object Render Node object.
//////////////////////////////////////////////////////////////////////////
struct IVoxelObject : public IRenderNode
{
	virtual struct IMemoryBlock * GetCompiledData() = 0;
	virtual void SetCompiledData(void * pData, int nSize, uchar ucChildId = 0) = 0;
  virtual void SetObjectName( const char * pName ) = 0;
  virtual void SetMatrix( const Matrix34& mat ) = 0;
	virtual bool ResetTransformation() = 0;
	virtual void InterpolateVoxelData() = 0;
	virtual void SetFlags(int nFlags) = 0;
	virtual void Regenerate() = 0;
	virtual void CopyHM() = 0;
  virtual bool IsEmpty() = 0;
};

//////////////////////////////////////////////////////////////////////////
// IFogVolumeRenderNode is an interface to the Fog Volume Render Node object.
//////////////////////////////////////////////////////////////////////////
struct SFogVolumeProperties
{	
	// common parameters
	// center position & rotation values are taken from the entity matrix
	int			m_volumeType;
	Vec3		m_size; 
	ColorF	m_color;
	bool		m_useGlobalFogColor;
	float		m_globalDensity;
	float   m_softEdges;
	float		m_fHDRDynamic;				// 0 to get the same results in LDR, <0 to get darker, >0 to get brighter
	
	// height based fog specifics
	float m_heightFallOffDirLong;
	float m_heightFallOffDirLati;
	float m_heightFallOffShift;
	float m_heightFallOffScale;
};

struct IFogVolumeRenderNode : public IRenderNode
{
	virtual void SetFogVolumeProperties( const SFogVolumeProperties& properties ) = 0;
	virtual const Matrix34& GetMatrix() const = 0;

	virtual float GetDistanceToCameraSquared() const = 0;
	virtual void FadeGlobalDensity(float fadeTime, float newGlobalDensity) = 0;
};

//////////////////////////////////////////////////////////////////////////
// IDecalRenderNode is an interface to the Decal Render Node object.
//////////////////////////////////////////////////////////////////////////
struct SDecalProperties
{
	enum EProjectionType
	{
		ePlanar,
		eProjectOnStaticObjects,
		eProjectOnTerrain,
		eProjectOnTerrainAndStaticObjects
	};

	EProjectionType m_projectionType;
	uint8 m_sortPrio;
	Vec3 m_pos;
	Vec3 m_normal;
	Matrix33 m_explicitRightUpFront;
	float m_radius;
	const char* m_pMaterialName;
};

struct IDecalRenderNode : public IRenderNode
{
	virtual void SetDecalProperties( const SDecalProperties& properties ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// IWaterVolumeRenderNode is an interface to the Water Volume Render Node object.
//////////////////////////////////////////////////////////////////////////
struct IWaterVolumeRenderNode : public IRenderNode
{
	enum EWaterVolumeType
	{
		eWVT_Unknown,
		eWVT_Ocean,
		eWVT_Area,
		eWVT_River
	};

	virtual void SetFogDensity( float fogDensity ) = 0;
	virtual float GetFogDensity() const = 0;
	virtual void SetFogColor( const Vec3& fogColor ) = 0;

	virtual void CreateOcean( uint64 volumeID, /* TBD */ bool keepSerializationParams = false ) = 0;
	virtual void CreateArea( uint64 volumeID, const Vec3* pVertices, unsigned int numVertices, const Vec2& surfUVScale, const Plane& fogPlane, bool keepSerializationParams = false ) = 0;
	virtual void CreateRiver( uint64 volumeID, const Vec3* pVertices, unsigned int numVertices, float uTexCoordBegin, float uTexCoordEnd, const Vec2& surfUVScale, const Plane& fogPlane, bool keepSerializationParams = false ) = 0;	

	virtual void SetAreaPhysicalArea( const Vec3* pVertices, unsigned int numVertices, float volumeDepth, float streamSpeed, bool keepSerializationParams = false ) = 0;
	virtual void SetRiverPhysicsArea( const Vec3* pVertices, unsigned int numVertices, float volumeDepth, float streamSpeed, bool keepSerializationParams = false ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// SWaterWaveParams is an interface to the Water Wave Render Node object.
//////////////////////////////////////////////////////////////////////////
struct SWaterWaveParams
{
  SWaterWaveParams():m_fSpeed( 5.0f ) , m_fSpeedVar( 2.0f ), m_fLifetime( 8.0f ) , m_fLifetimeVar( 2.0f ), 
    m_fHeight(0.75f), m_fHeightVar(0.4f), m_fPosVar( 5.0f ), m_fCurrLifetime(-1.0f), m_fCurrFrameLifetime(-1.0f), 
    m_fCurrSpeed(0.0f), m_fCurrHeight(1.0f)
  {

  }

  Vec3 m_pPos;
  float m_fSpeed, m_fSpeedVar;
  float m_fLifetime, m_fLifetimeVar;
  float m_fHeight, m_fHeightVar;
  float m_fPosVar;

  float m_fCurrLifetime;    
  float m_fCurrFrameLifetime;    
  float m_fCurrSpeed;
  float m_fCurrHeight;
};

struct IWaterWaveRenderNode : public IRenderNode
{
  virtual void Create( uint64 nID, const Vec3 *pVertices, uint32 nVertexCount, const Vec2 &pUVScale, const Matrix34 &pWorldTM ) = 0;	    
  virtual void SetParams( const SWaterWaveParams &pParams ) = 0;
  virtual const SWaterWaveParams &GetParams() const = 0;
};

//////////////////////////////////////////////////////////////////////////////////////
// IDistanceCloudRenderNode is an interface to the Distance Cloud Render Node object.
//////////////////////////////////////////////////////////////////////////////////////
struct SDistanceCloudProperties
{
	Vec3 m_pos;
	float m_sizeX;
	float m_sizeY;
	float m_rotationZ;
	const char* m_pMaterialName;
};

struct IDistanceCloudRenderNode : public IRenderNode
{
	virtual void SetProperties( const SDistanceCloudProperties& properties ) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////
// IVolumeObjectRenderNode is an interface to the Volume Object Render Node object.
//////////////////////////////////////////////////////////////////////////////////////
struct SVolumeObjectProperties
{
};

struct SVolumeObjectMovementProperties
{
	bool m_autoMove;
	Vec3 m_speed;
	Vec3 m_spaceLoopBox;
	float m_fadeDistance;
};

struct IVolumeObjectRenderNode : public IRenderNode
{
	virtual void LoadVolumeData(const char* filePath) = 0;
	virtual void SetProperties(const SVolumeObjectProperties& properties) = 0;
	virtual void SetMovementProperties(const SVolumeObjectMovementProperties& properties) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Auto cube-map render point.
// Objects with auto cube maps applied will pick nearest auto cube-map point,
// To generate and share their environment cube-maps.
//////////////////////////////////////////////////////////////////////////
struct SAutoCubeMapProperties
{
	// render flags
	uint32 m_flags;

	// obb
	OBB m_obb;

	// reference position for auto cubemap generation
	Vec3 m_refPos;
};

struct IAutoCubeMapRenderNode : public IRenderNode
{
	virtual void SetProperties(const SAutoCubeMapProperties& properties) = 0;
	virtual const SAutoCubeMapProperties& GetProperties() const = 0;

	virtual uint32 GetID() const = 0;

	virtual void SetPrivateData(const void* pData) = 0;
	virtual const void* GetPrivateData() const = 0;
};


//////////////////////////////////////////////////////////////////////////
struct IRopeRenderNode : public IRenderNode
{
	enum ERopeParamFlags
	{
		eRope_BindEndPoints   = 0x0001, // Bind rope at both end points.
		eRope_CheckCollisinos = 0x0002, // Rope will check collisions.
		eRope_Subdivide       = 0x0004, // Rope will be subdivided in physics.
		eRope_Smooth          = 0x0008, // Rope will be smoothed after physics.
		eRope_NoAttachmentCollisions = 0x0010  // Rope will ignore collisions against the objects it's attached to 
	};
	struct SRopeParams
	{
		int nFlags; // ERopeParamFlags

		float fThickness;
		// Radius for the end points anchors that bind rope to objects in world.
		float fAnchorRadius;

		//////////////////////////////////////////////////////////////////////////
		// Rendering/Tesselation.
		//////////////////////////////////////////////////////////////////////////
		int nNumSegments;
		int nNumSides;
		float fTextureTileU;
		float fTextureTileV;

		//////////////////////////////////////////////////////////////////////////
		// Rope Physical/Params.
		//////////////////////////////////////////////////////////////////////////
		int nPhysSegments;
		int nMaxSubVtx;

		// Rope Physical parameters.
		float mass;        // Rope mass, if mass is 0 it will be static.
		float tension;
		float friction;
		float frictionPull;
		
		Vec3 wind;
		float windVariance;
		float airResistance;
		float waterResistance;

		float jointLimit;
		float maxForce;
	};
	struct SEndPointLink
	{
		IPhysicalEntity *pPhysicalEntity;
		Vec3 offset;
		int nPartId;
	};

	virtual void SetName( const char *sNodeName ) = 0;

	virtual void SetParams( const SRopeParams& params ) = 0;
	virtual const SRopeParams& GetParams() const = 0;

	virtual void   SetEntityOwner( uint32 nEntityId ) = 0;
	virtual uint32 GetEntityOwner() const = 0;

	virtual void SetPoints( const Vec3 *pPoints,int nCount ) = 0;
	virtual int GetPointsCount() const = 0;
	virtual const Vec3* GetPoints() const = 0;

	virtual void LinkEndPoints() = 0;
	virtual uint32 GetLinkedEndsMask() = 0;

	virtual void LinkEndEntities(IPhysicalEntity* pStartEntity, IPhysicalEntity* pEndEntity) = 0;
	// Retreve information about linked objects at the end points, links must be a pointer to the 2 SEndPointLink structures.
	virtual void GetEndPointLinks( SEndPointLink *links ) = 0;

	// Callback from physics.
	virtual void OnPhysicsPostStep() = 0;
	virtual void ForceInvalidate() = 0;


	virtual void ResetPoints() = 0;
};

#endif // IRenderNodeSTATE_H
