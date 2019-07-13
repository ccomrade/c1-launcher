/*=============================================================================
  IShader.h : Shaders common interface.
  Copyright (c) 2001-2002 Crytek Studios. All Rights Reserved.

  Revision history:
    * Created by Honich Andrey

=============================================================================*/

#ifndef _ISHADER_H_
#define _ISHADER_H_

#include "smartptr.h"
#include "IMaterial.h"

#include "Cry_XOptimise.h"
#ifdef LINUX
# include <platform.h>
#endif

struct IShader;
struct IVisArea;
class CShader;
struct STexAnim;
struct SShaderPass;
struct CRenderMesh;
struct SShaderItem;
class ITexture;
struct SParam;
class CMaterial;


//================================================================
// Geom. Culling type
enum ECull
{
  eCULL_Back = 0,
  eCULL_Front,
  eCULL_None
};

//=========================================================================
// Array Pointers for Shaders

enum ESrcPointer
{
  eSrcPointer_Unknown,
  eSrcPointer_Vert,
  eSrcPointer_Color,
  eSrcPointer_Tex,
  eSrcPointer_TexLM,
  eSrcPointer_Normal,
  eSrcPointer_Binormal,
  eSrcPointer_Tangent,
  eSrcPointer_Max,
};

//////////////////////////////////////////////////////////////////////
// CRenderObject::m_ObjFlags: Flags used by shader pipeline
#define FOB_TRANS_ROTATE    (1<<0)      // 1
#define FOB_TRANS_SCALE     (1<<1)      // 2
#define FOB_TRANS_TRANSLATE (1<<2)      // 4
#define FOB_TRANS_MASK (FOB_TRANS_ROTATE | FOB_TRANS_SCALE | FOB_TRANS_TRANSLATE)

#define FOB_RENDER_INTO_SHADOWMAP (1<<3)	// 8
#define FOB_OWNER_GEOMETRY  (1<<4)				// 0x10
#define FOB_NO_FOG					(1<<5)				// 0x20
#define FOB_NO_Z_PASS       (1<<6)       // 0x40
#define FOB_DETAILPASS      (1<<7)       // 0x80

#define FOB_FOGPASS         (1<<8)       // 0x100
#define FOB_SELECTED        (1<<9)       // 0x200
#define FOB_CAMERA_SPACE		(1<<10)      // 0x400
#define FOB_ONLY_Z_PASS			(1<<11)      // 0x800

#define FOB_BLEND_WITH_TERRAIN_COLOR (1<<12)	// 0x1000
#define FOB_REMOVED         (1<<13)						// 0x2000
#define FOB_SOFT_PARTICLE   (1<<14)						// 0x4000
#define FOB_OCEAN_PARTICLE				(1<<15)						// 0x8000

#define FOB_CUSTOM_CAMERA   (1<<16)      // 0x10000
#define FOB_AMBIENT_OCCLUSION (1<<17)    // 0x20000
#define FOB_BENDED          (1<<18)      // 0x40000
#define FOB_MTLLAYERS_OBJSPACE  (1<<19)      // 0x80000

#define FOB_INSHADOW        (1<<20)      // 0x100000
#define FOB_DISSOLVE       (1<<21)       // 0x200000
#define FOB_RAE_GEOMTERM		(1<<22)      // 0x400000
#define FOB_NEAREST         (1<<23)      // 0x800000

#define FOB_CHARACTER       (1<<24)      // 0x1000000
#define FOB_MTLLAYERS_BLEND (1<<25)      // 0x2000000
#define FOB_PARTICLE_MASK		(FOB_SOFT_PARTICLE | FOB_NO_FOG | FOB_OCEAN_PARTICLE)

#define FOB_HIGHPRECISION   (1<<26)      // 0x4000000

#define FOB_DECAL           (1<<27)      // 0x8000000
#define FOB_DECAL_TEXGEN_2D (1<<28)      // 0x10000000
#define FOB_DECAL_TEXGEN_3D (1<<29)      // 0x20000000
#define FOB_VEGETATION      (1<<30)      // 0x40000000
#define FOB_DECAL_MASK		  (FOB_DECAL | FOB_DECAL_TEXGEN_2D | FOB_DECAL_TEXGEN_3D)

// WARNING: FOB_MASK_AFFECTS_MERGING must start from 0x10000 (important for instancing)
#define FOB_MASK_AFFECTS_MERGING  (FOB_VEGETATION | FOB_CHARACTER | FOB_BENDED | FOB_RAE_GEOMTERM | FOB_INSHADOW | FOB_MTLLAYERS_OBJSPACE | FOB_HIGHPRECISION | FOB_AMBIENT_OCCLUSION | FOB_CUSTOM_CAMERA | FOB_MTLLAYERS_BLEND | FOB_DISSOLVE | FOB_NEAREST)

struct SWaveForm;
struct SWaveForm2;

typedef TAllocator16<Matrix44> MatrixAllocator16;
typedef std::vector<Matrix44, MatrixAllocator16> MatrixArray16;


#define FRF_REFRACTIVE 1
#define FRF_GLOW       2
#define FRF_HEAT       4

//=========================================================================

enum EParamType
{
  eType_UNKNOWN,
  eType_BYTE,
  eType_BOOL,
  eType_SHORT,
  eType_INT,
  eType_FLOAT,
  eType_STRING,
  eType_FCOLOR,
  eType_VECTOR,
  eType_TEXTURE_HANDLE,
  eType_CAMERA
};

union UParamVal
{
  byte m_Byte;
  bool m_Bool;
  short m_Short;
  int m_Int;
  float m_Float;
  char *m_String;
  float m_Color[4];
  float m_Vector[3];
  CCamera *m_pCamera;
};

struct SShaderParam
{
  // in order to facilitate the memory allocation tracking, we're using here this class;
  // if you don't like it, please write a substitute for all string within the project and use them everywhere
  char m_Name[32];
  EParamType m_Type;
  UParamVal m_Value;
  string m_Script;

  _inline void Construct()
  {
    m_Value.m_Int = 0;
    m_Type = eType_UNKNOWN;
    m_Name[0] = 0;
  }
  _inline SShaderParam()
  {
    Construct();
  }
  size_t Size()
  {
    size_t nSize = sizeof(*this);
    nSize += sizeof(m_Name);
    if (m_Type == eType_STRING)
      nSize += strlen (m_Value.m_String) + 1;

    return nSize;
  }
  _inline void Destroy()
  {
    if (m_Type == eType_STRING)
      delete [] m_Value.m_String;
  }
  _inline ~SShaderParam()
  {
    Destroy();
  }
  _inline SShaderParam (const SShaderParam& src)
  {
    memcpy(m_Name, src.m_Name, sizeof(m_Name));
    m_Script = src.m_Script;
    m_Type = src.m_Type;
    m_Value = src.m_Value;
  }
  _inline SShaderParam& operator = (const SShaderParam& src)
  {
    this->~SShaderParam();
    new(this) SShaderParam(src);
    return *this;
  }

  static bool SetParam(const char* name, DynArray<SShaderParam> *Params, UParamVal& pr)
  {
    uint i;
    for (i=0; i<(uint)Params->size(); i++)
    { 
      SShaderParam *sp = &(*Params)[i]; 
      if (!sp)
        continue;
      if (!stricmp(sp->m_Name, name))
      {
        switch (sp->m_Type)
        {          
          case eType_FLOAT:
            sp->m_Value.m_Float = pr.m_Float;
            break;
          case eType_SHORT:
            sp->m_Value.m_Short = pr.m_Short;
            break;
          case eType_INT:
          case eType_TEXTURE_HANDLE:
            sp->m_Value.m_Int = pr.m_Int;
            break;

          case eType_VECTOR:
            sp->m_Value.m_Vector[0] = pr.m_Vector[0];
            sp->m_Value.m_Vector[1] = pr.m_Vector[1];
            sp->m_Value.m_Vector[2] = pr.m_Vector[2];
            break;

          case eType_FCOLOR:
            sp->m_Value.m_Color[0] = pr.m_Color[0];
            sp->m_Value.m_Color[1] = pr.m_Color[1];
            sp->m_Value.m_Color[2] = pr.m_Color[2];
            sp->m_Value.m_Color[3] = pr.m_Color[3];
            break;

          case eType_STRING:
            {
              char *str = pr.m_String;
              size_t len = strlen(str)+1;
              sp->m_Value.m_String = new char [len];
              strcpy(sp->m_Value.m_String, str);
            }
            break;
        }
        break;
      }
    }
    if (i == Params->size())
      return false;
    return true;
  }
  static bool GetValue(const char* szName, DynArray<SShaderParam> *Params, float *v, int nID);
};


//////////////////////////////////////////////////////////////////////////
// Description:
//    IShaderPublicParams can be used to hold a collection of the shader public params.
//    Manipulate this collection, and use them during rendering by submit to the SRendParams.
//////////////////////////////////////////////////////////////////////////
struct IShaderPublicParams
{
  virtual void AddRef() = 0;
  virtual void Release() = 0;

  // Description:
  //    Change number of parameters in collection.
  virtual void SetParamCount( int nParam ) = 0;
  
  // Description:
  //    Retrieve number of parameters in collection.
  virtual int  GetParamCount() const = 0;

  // Description:
  //    Retrieve shader public parameter at specified index of the collection.
  virtual SShaderParam& GetParam( int nIndex ) = 0;
  virtual const SShaderParam& GetParam( int nIndex ) const = 0;

  // Description:
  //    Set a shader parameter (and if doens't exists, add it to the parameters list)
  virtual void SetParam(const char *pszName, UParamVal &pParam, EParamType nType = eType_FLOAT) = 0;

  // Description:
  //    Assign shader public parameter at specified index of the collection.
  virtual void SetParam( int nIndex,const SShaderParam &param ) = 0;

  // Description:
  //    Assign existing shader params list
  virtual void SetShaderParams( const DynArray<SShaderParam> &pParams) = 0;

  // Description:
  //    Adds a new shader public parameter at the end of the collection.
  virtual void AddParam( const SShaderParam &param ) = 0;

  // Description:
  //    Assign collection of shader public params to the specified render params structure.
  //virtual void AssignToRenderParams( struct SRendParams &rParams ) = 0;

  // Description:
  //    Get shader params
  virtual DynArray<SShaderParam> *GetShaderParams() = 0;
  virtual const DynArray<SShaderParam> *GetShaderParams() const = 0;
};

struct SInstanceInfo
{
  Matrix34 m_Matrix;
  ColorF m_AmbColor;
};

struct SRenderObjData
{
  struct IRenderMesh *				m_pLMTCBufferO;
  uint32											m_nRAEPortalMask;												// mask of the lights with portal
  DynArray16<SInstanceInfo>  *m_pInstancingInfo;
  DynArray<Vec4>              m_Constants;
  SRenderObjData()
  {
    m_pLMTCBufferO = NULL;
    m_nRAEPortalMask = 0;
    m_pInstancingInfo = NULL;
  }
};


//////////////////////////////////////////////////////////////////////
// Objects using in shader pipeline

// interface for the skinnable objects (renderer calls its functions to get teh skinning data)
struct ISkinnable 
{
  virtual void AddRef() = 0;
  virtual void Release() = 0;

  //! Renderer calls this function to allow update the video vertex buffers right before the rendering
  virtual uint32 GetSkeletonPose(int nLod, const Matrix34& RenderMat34, QuatTS*& pBoneQuatsL, QuatTS*& pBoneQuatsS, QuatTS*& pMBBoneQuatsL, QuatTS*& pMBBoneQuatsS, Vec4 shapeDeformationData[], uint32 &DoWeNeedMorphtargets, uint8*& pRemapTable ) = 0;
};

//same as in the 3dEngine
#define MAX_LIGHTS_NUM 32

struct ShadowMapFrustum;

// Size of CRenderObject currently is 256 bytes. If you add new members better to ask me before or just
// make sure CRenderObject is cache line aligned (64 bytes)
class CRenderObject
{
public:
  CRenderObject()
  {
    //m_ShaderParams = NULL;
    m_fRenderQuality = 1.0f;
    m_nObjDataId = -1;
    //m_pMaterialLayerManager = 0;
    m_pWeights = NULL;
    m_fSort = 0;
    //m_nRAEColdDataIdx = ~0;

    m_nMotionBlurAmount = 0;
    m_nMaterialLayers = 0;
    m_nVisionParams = 0;
    m_pCurrMaterial = 0;

    m_vBending.zero();
    m_fBendScale = 0.0f;
  }
  ~CRenderObject();

  int16 m_Id;
  int16 m_VisId;
  int16 m_nObjDataId;
  uint16 m_FogVolumeContribIdx;

  uint m_ObjFlags;

  SInstanceInfo m_II;
  Matrix34 m_prevMatrix;

  uint32                      m_nMaterialLayers;          // which mtl layers active and how much to blend them
  uint8												m_nLod;	           
  uint8                       m_nMotionBlurAmount;        // per object motion blur scale
  uint8												m_AlphaRef;								//

  uint                        m_DynLMMask;

  // Different useful vars (ObjVal component in shaders)
  // [0] - used for blending trees sun-rabbits on distance (0-1)
  float                       m_fTempVars[6]; 

  CRendElement *							m_pRE;										//
  IRenderMesh *               m_pWeights;								//
  void *											m_CustomData;							//  
  uint												m_RState;									//

  float												m_fSort;									// Custom sort value

  uint32                      m_nMDV; 	            		// Vertex modificator flags

  short                       m_nRAEId;                 // This 2 parameters must be paired (see EF_TryToMerge function)
  short												m_nTextureID;             // Custom texture id

  short												m_nTextureID1;            // Custom texture id
  short                       m_nWaveID;    
  short                       m_nFrameLight;
  short                       m_nLightStyle;

  short                       m_nScissorX1, m_nScissorY1, m_nScissorX2, m_nScissorY2;
  
  float												m_fDistance;								// distance to the object
  float												m_fRenderQuality;						// 1.0f - full quality, 0.0f - lowest quality, used by CStatObj
  
  PodArray<ShadowMapFrustum*> *	m_pShadowCasters;						// list of shadow casters 
  void *											m_pID;											// will define instance id, e.g. pointer to CStatObj, CTerrainNode, COcean

  float                       m_fAlpha;                   // object alpha

  Vec2												m_vBending;									// 2D bending direction and magnitude.

  IMaterial             *     m_pCurrMaterial;            // current material (is this available somewhere ??)

  struct ISkinnable *					m_pCharInstance;

  uint32											m_HMAData;
  IVisArea*										m_pVisArea;									//	VisArea that contains this object, used for RAM-ambientcube query 

  union
  {
    ITexture  *m_pLightImage;
    CDLight *m_pLight;
    CCamera *m_pCustomCamera;    //
    float m_fBendScale;
    uint32 m_nVisionParams; // Vision modes stuff
 };

//=========================================================================================================

  _inline Vec3 GetTranslation() const
  {
    return m_II.m_Matrix.GetTranslation();
  }
  _inline float GetScaleX() const
  {
    return cry_sqrtf(m_II.m_Matrix(0,0)*m_II.m_Matrix(0,0) + m_II.m_Matrix(0,1)*m_II.m_Matrix(0,1) + m_II.m_Matrix(0,2)*m_II.m_Matrix(0,2));
  }
  _inline float GetScaleZ() const
  {
    return cry_sqrtf(m_II.m_Matrix(2,0)*m_II.m_Matrix(2,0) + m_II.m_Matrix(2,1)*m_II.m_Matrix(2,1) + m_II.m_Matrix(2,2)*m_II.m_Matrix(2,2));
  }
  _inline bool IsMergable()
  {
    if (m_fAlpha != 1.0f)
      return false;
    if (m_pShadowCasters)
      return false;
    if (m_pCharInstance)
      return false;
    return true;
  }

  static TArray<SWaveForm2> m_Waves;
  static DynArray<SRenderObjData> m_ObjData;

  void Init();

  void CloneObject(CRenderObject *srcObj)
  {
    int Id = m_Id;
    int VisId = m_VisId;
    memcpy(this, srcObj, sizeof(*srcObj));
    m_Id = Id;
    m_VisId = VisId;
  }

  ILINE Matrix34 &GetMatrix()
  {
    return m_II.m_Matrix;
  }

  ILINE IRenderMesh *GetWeights() { return m_pWeights; }

  virtual void AddWaves(SWaveForm2 **wf);
  virtual SRenderObjData *GetObjData(bool bCreate=false);
  virtual void RemovePermanent();
  //virtual void SetShaderFloat(const char *Name, float Val);

  ILINE DynArray16<SInstanceInfo> *GetInstanceInfo()
  {
    return m_nObjDataId>=0 ? m_ObjData[m_nObjDataId].m_pInstancingInfo : NULL;
  }
  // Sergiy: it's enough to allocate 16 bytes more, even on 64-bit machine
  // - and we need to store only the offset, not the actual pointer
  void* operator new( size_t Size )
  {
    byte *ptr = (byte *)malloc(Size+16+4);
    memset(ptr, 0, Size+16+4);
    byte *bPtrRes = (byte *)((INT_PTR)(ptr+4+16) & ~0xf);
    ((byte**)bPtrRes)[-1] = ptr;

    return bPtrRes;
  }
  void* operator new(size_t Size, const std::nothrow_t &nothrow)
  {
    return ::operator new(Size);
  }
  void* operator new[](size_t Size)
  {
    byte *ptr = (byte *)malloc(Size+16+2*sizeof(INT_PTR));
    memset(ptr, 0, Size+16+2*sizeof(INT_PTR));
    byte *bPtrRes = (byte *)((INT_PTR)(ptr+16+2*sizeof(INT_PTR)) & ~0xf);
    ((byte**)bPtrRes)[-2] = ptr;

    return bPtrRes-sizeof(INT_PTR);
  }
  void* operator new[](size_t Size, const std::nothrow_t &nothrow)
  {
    return ::operator new[](Size);
  }
  void operator delete( void *Ptr )
  {
    byte *bActualPtr = ((byte **)Ptr)[-1];
    assert (bActualPtr <= (byte*)Ptr && (byte*)Ptr-bActualPtr < 20);
    free ((void *)bActualPtr);
  }

  void operator delete[]( void *Ptr )
  {
    byte *bActualPtr = ((byte **)Ptr)[-1];
    free ((void *)bActualPtr);
  }
};


//=================================================================================

class CInputLightMaterial
{
public:
  CInputLightMaterial() :
    m_Diffuse(0,0,0,0),
    m_Specular(0,0,0,0),
    m_Emission(0,0,0,0),
    m_SpecShininess(0)
  {
  }

  ColorF m_Diffuse;
  ColorF m_Specular;
  ColorF m_Emission;
  float m_SpecShininess;

  _inline friend bool operator == (const CInputLightMaterial &m1, const CInputLightMaterial &m2)
  {
    if (m1.m_Diffuse == m2.m_Diffuse && m1.m_Specular == m2.m_Specular && m1.m_Emission == m2.m_Emission && m1.m_SpecShininess == m2.m_SpecShininess)
      return true;
    return false;
  }

  int Size()
  {
    int nSize = sizeof(CInputLightMaterial);
    return nSize;
  }
};

enum ETEX_Type
{
  eTT_1D = 0,
  eTT_2D,
  eTT_3D,
  eTT_Cube,
  eTT_AutoCube,
  eTT_Auto2D,
  eTT_User,
};

// Texture formats
enum ETEX_Format
{
  eTF_Unknown = 0,
  eTF_R8G8B8,
  eTF_A8R8G8B8,
  eTF_X8R8G8B8,
  eTF_A8,
  eTF_A8L8,
  eTF_L8,
  eTF_A4R4G4B4,
  eTF_R5G6B5,
  eTF_R5G5B5,
  eTF_V8U8,
  eTF_CxV8U8,
  eTF_X8L8V8U8,
  eTF_L8V8U8,
  eTF_L6V5U5,
  eTF_V16U16,
  eTF_A16B16G16R16,
  eTF_A16B16G16R16F,
  eTF_A32B32G32R32F,
  eTF_G16R16F,
  eTF_R16F,
  eTF_R32F,
  eTF_DXT1,
  eTF_DXT3,
  eTF_DXT5,
  eTF_3DC,

  eTF_G16R16,

  eTF_NULL,

  //hardware depth buffers
  eTF_DF16,
  eTF_DF24,
  eTF_D16,
  eTF_D24S8,

  eTF_D32F,

  eTF_DEPTH16,
  eTF_DEPTH24,
};

#define FT_NOMIPS           0x1
#define FT_TEX_NORMAL_MAP   0x2
#define FT_TEX_SKY          0x4
#define FT_USAGE_DEPTHSTENCIL 0x8
#define FT_TEX_LM           0x10
#define FT_FILESINGLE			  0x20				// supress loading of attached alpha
#define FT_TEX_FONT         0x40
#define FT_HAS_ATTACHED_ALPHA 0x80
#define FT_DONTSYNCMULTIGPU 0x100				// through NVAPI we tell driver not to sync
#define FT_FORCE_CUBEMAP    0x200
#define FT_USAGE_FSAA       0x400
#define FT_FORCE_MIPS       0x800
#define FT_USAGE_RENDERTARGET 0x1000
#define FT_USAGE_DYNAMIC    0x2000
#define FT_DONT_RESIZE      0x4000
#define FT_DONT_ANISO       0x8000
#define FT_DONT_RELEASE     0x10000
#define FT_DONT_GENNAME     0x20000
#define FT_DONT_STREAM      0x40000
#define FT_USAGE_PREDICATED_TILING 0x80000
#define FT_FAILED           0x100000
#define FT_FROMIMAGE        0x200000
#define FT_STATE_CLAMP      0x400000
#define FT_USAGE_ATLAS      0x800000
#define FT_ALPHA            0x1000000
#define FT_REPLICATE_TO_ALL_SIDES 0x2000000
#define FT_FILTER_POINT     0x10000000
#define FT_FILTER_LINEAR    0x20000000
#define FT_FILTER_BILINEAR  0x30000000
#define FT_FILTER_TRILINEAR 0x40000000
#define FT_FILTER_ANISO2    0x50000000
#define FT_FILTER_ANISO4    0x60000000
#define FT_FILTER_ANISO8    0x70000000
#define FT_FILTER_ANISO16   0x80000000
#define FT_FILTER_MASK      0xf0000000

//#define FT_AFFECT_INSTANCE  (FT_ALLOW_3DC | FT_NOMIPS | FT_TEX_NORMAL_MAP | FT_FORCE_DXT | FT_FORCE_CUBEMAP | FT_REPLICATE_TO_ALL_SIDES | FT_FILTER_MASK | FT_ALPHA)
#define FT_AFFECT_INSTANCE  (FT_NOMIPS | FT_TEX_NORMAL_MAP | FT_USAGE_FSAA | FT_FORCE_CUBEMAP | FT_REPLICATE_TO_ALL_SIDES | FT_FILTER_MASK | FT_ALPHA)


//////////////////////////////////////////////////////////////////////
// Texture object interface
class CTexture;
class ITexture
{
public:
  virtual int AddRef()=0;
  virtual int Release()=0;
  virtual int ReleaseForce()=0;

  virtual const char *GetName()=0;
  virtual int GetWidth() = 0;
  virtual int GetHeight() = 0;
  virtual int GetSourceWidth() = 0;
  virtual int GetSourceHeight() = 0;
  virtual int GetTextureID() = 0;
  virtual uint GetFlags() = 0;
  virtual int GetNumMips() = 0;
  virtual int GetDeviceDataSize() = 0;
  virtual ETEX_Type GetTextureType() = 0;
  virtual bool IsTextureLoaded() = 0;
  virtual void PrecacheAsynchronously(float fDist, int nFlags) = 0;
  virtual void Preload (int nFlags)=0;
  virtual byte *GetData32(int nSide=0, int nLevel=0)=0;
  virtual byte *LockData(int& nPitch, int nSide=0, int nLevel=0)=0;
  virtual void UnlockData(int nSide=0, int nLevel=0)=0;
  virtual bool SaveTGA(const char *szName, bool bMips=false)=0;
  virtual bool SaveJPG(const char *szName, bool bMips=false)=0;
  virtual bool SetFilter(int nFilter)=0;   // FILTER_ flags
  virtual void SetClamp(bool bEnable) = 0; // Texture addressing set
  virtual ColorF GetAvgColor() = 0;

  // Used for debugging/profiling.
  virtual const char* GetFormatName() = 0;
  virtual const char* GetTypeName() = 0;
  virtual bool IsShared() const = 0;
};

//=========================================================================================

struct IDynTextureSource
{
  virtual void AddRef() = 0;
  virtual void Release() = 0;

  virtual bool Update(float distToCamera) = 0;
  virtual bool Apply(int nTUnit, int nTS = -1) = 0;	
  virtual void GetTexGenInfo(float& offsX, float& offsY, float& scaleX, float& scaleY) const = 0;

  enum EDynTextureSource
  {
    DTS_I_FLASHPLAYER,
    DTS_I_VIDEOPLAYER
  };	
  virtual void GetDynTextureSource(void*& pIDynTextureSource, EDynTextureSource& dynTextureSource) = 0;
};

//=========================================================================================

class IDynTexture
{
public:
  virtual void Release() = 0;
  virtual void GetSubImageRect(uint32 & nX, uint32 & nY, uint32 & nWidth, uint32 & nHeight) = 0;
  virtual void GetImageRect(uint32 & nX, uint32 & nY, uint32 & nWidth, uint32 & nHeight) = 0;
  virtual int GetTextureID() = 0;
  virtual void Lock() = 0;
  virtual void UnLock() = 0;
  virtual int GetWidth() = 0;
  virtual int GetHeight() = 0;
  virtual bool IsValid() = 0;
  virtual bool Update(int nNewWidth, int nNewHeight)=0;
  virtual void Apply(int nTUnit, int nTS=-1)=0;
  virtual bool SetRT(int nRT, bool bPush, struct SD3DSurface *pDepthSurf)=0;
  virtual bool SetRectStates()=0;
  virtual bool RestoreRT(int nRT, bool bPop)=0;
  virtual ITexture *GetTexture()=0;
  virtual void SetUpdateMask()=0;
  virtual void ResetUpdateMask()=0;
  virtual bool IsSecondFrame()=0;
};


//==============================================================================
// Vertex modificators definitions 

#define MDV_BENDING            0x100
#define MDV_DET_BENDING        0x200
#define MDV_DET_BENDING_GRASS  0x400
#define MDV_WIND               0x800
#define MDV_TERRAIN_ADAPT      0x1000
#define MDV_DEPTH_OFFSET       0x2000

//==============================================================================
// Deformations / Morphing types
enum EDeformType
{
  eDT_Unknown = 0,
  eDT_SinWave = 1,
  eDT_VerticalWave = 2,
  eDT_Bulge = 3,
  eDT_Squeeze = 4,
  eDT_Perlin2D = 5,
  eDT_Perlin3D = 6,
  eDT_FromCenter = 7,
  eDT_Bending = 8,  
  eDT_ProcFlare = 9,
  eDT_AutoSprite = 10,
  eDT_Beam = 11,
  eDT_FixedOffset = 12,
};


// Wave form evaluators
enum EWaveForm
{
  eWF_None,
  eWF_Sin,
  eWF_HalfSin,
  eWF_InvHalfSin,
  eWF_Square,
  eWF_Triangle,
  eWF_SawTooth,
  eWF_InvSawTooth,
  eWF_Hill,
  eWF_InvHill,
};

#define WFF_CLAMP 1
#define WFF_LERP  2

// Wave form definition
struct SWaveForm
{
  EWaveForm m_eWFType;
  byte m_Flags;

  float m_Level;
  float m_Level1;
  float m_Amp;
  float m_Amp1;
  float m_Phase;
  float m_Phase1;
  float m_Freq;
  float m_Freq1;

  SWaveForm(EWaveForm eWFType, float fLevel, float fAmp, float fPhase, float fFreq)
  {
    m_eWFType = eWFType;
    m_Level = m_Level1 = fLevel;
    m_Amp = m_Amp1 = fAmp;
    m_Phase = m_Phase1 = fPhase;
    m_Freq = m_Freq1 = fFreq;
  }

  int Size()
  {
    int nSize = sizeof(SWaveForm);
    return nSize;
  }
  SWaveForm()
  {
    memset(this, 0, sizeof(SWaveForm));
  }
  bool operator == (SWaveForm wf)
  {
    if (m_eWFType == wf.m_eWFType && m_Level == wf.m_Level && m_Amp == wf.m_Amp && m_Phase == wf.m_Phase && m_Freq == wf.m_Freq && m_Level1 == wf.m_Level1 && m_Amp1 == wf.m_Amp1 && m_Phase1 == wf.m_Phase1 && m_Freq1 == wf.m_Freq1 && m_Flags == wf.m_Flags)
      return true;
    return false;
  }
  _inline friend bool operator == (const SWaveForm &m1, const SWaveForm &m2)
  {
    if (m1.m_eWFType == m2.m_eWFType && m1.m_Level == m2.m_Level && m1.m_Amp == m2.m_Amp && m1.m_Phase == m2.m_Phase && m1.m_Freq == m2.m_Freq && m1.m_Level1 == m2.m_Level1 && m1.m_Amp1 == m2.m_Amp1 && m1.m_Phase1 == m2.m_Phase1 && m1.m_Freq1 == m2.m_Freq1 && m1.m_Flags == m2.m_Flags)
      return true;
    return false;
  }

  SWaveForm& operator += (const SWaveForm& wf )
  {
    m_Level  += wf.m_Level;
    m_Level1 += wf.m_Level1;
    m_Amp  += wf.m_Amp;
    m_Amp1 += wf.m_Amp1;
    m_Phase  += wf.m_Phase;
    m_Phase1 += wf.m_Phase1;
    m_Freq  += wf.m_Freq;
    m_Freq1 += wf.m_Freq1;
    return *this;
  }
};

struct SWaveForm2
{
  EWaveForm m_eWFType;

  float m_Level;
  float m_Amp;
  float m_Phase;
  float m_Freq;

  SWaveForm2()
  {
    memset(this, 0, sizeof(SWaveForm2));
  }
  bool operator == (SWaveForm2 wf)
  {
    if (m_eWFType == wf.m_eWFType && m_Level == wf.m_Level && m_Amp == wf.m_Amp && m_Phase == wf.m_Phase && m_Freq == wf.m_Freq)
      return true;
    return false;
  }
  _inline friend bool operator == (const SWaveForm2 &m1, const SWaveForm2 &m2)
  {
    if (m1.m_eWFType == m2.m_eWFType && m1.m_Level == m2.m_Level && m1.m_Amp == m2.m_Amp && m1.m_Phase == m2.m_Phase && m1.m_Freq == m2.m_Freq)
      return true;
    return false;
  }

  SWaveForm2& operator += (const SWaveForm2& wf )
  {
    m_Level  += wf.m_Level;
    m_Amp  += wf.m_Amp;
    m_Phase  += wf.m_Phase;
    m_Freq  += wf.m_Freq;
    return *this;
  }
};

struct SDeformInfo
{
  EDeformType m_eType;
  SWaveForm2 m_WaveX;
  SWaveForm2 m_WaveY;
  float m_fDividerX;
  float m_fDividerY;
  Vec3 m_vNoiseScale;

  SDeformInfo()
  {
    m_eType = eDT_Unknown;
    m_fDividerX = 0.01f;
    m_fDividerY = 0.01f;
    m_vNoiseScale = Vec3(1,1,1);
  }
  _inline friend bool operator == (const SDeformInfo &m1, const SDeformInfo &m2)
  {
    if (m1.m_eType == m2.m_eType &&
        m1.m_WaveX == m2.m_WaveX &&
        m1.m_WaveY == m2.m_WaveY &&
        m1.m_vNoiseScale == m2.m_vNoiseScale &&
        m1.m_fDividerX != m2.m_fDividerX &&
        m1.m_fDividerY != m2.m_fDividerY)
      return true;
    return false;
  }
  int Size()
  {
    return sizeof(SDeformInfo);
  }
};

//==============================================================================

// Color operations
enum EColorOp
{
  eCO_NOSET = 0,
  eCO_DISABLE = 1,
  eCO_REPLACE = 2,
  eCO_DECAL = 3,
  eCO_ARG2 = 4,
  eCO_MODULATE = 5,
  eCO_MODULATE2X = 6,
  eCO_MODULATE4X = 7,
  eCO_BLENDDIFFUSEALPHA = 8,
  eCO_BLENDTEXTUREALPHA = 9,
  eCO_DETAIL = 10,
  eCO_ADD = 11,
  eCO_ADDSIGNED = 12,
  eCO_ADDSIGNED2X = 13,
  eCO_MULTIPLYADD = 14,
  eCO_BUMPENVMAP = 15,
  eCO_BLEND = 16,
  eCO_MODULATEALPHA_ADDCOLOR = 17,
  eCO_MODULATECOLOR_ADDALPHA = 18,
  eCO_MODULATEINVALPHA_ADDCOLOR = 19,
  eCO_MODULATEINVCOLOR_ADDALPHA = 20,
  eCO_DOTPRODUCT3 = 21,
  eCO_LERP = 22,
  eCO_SUBTRACT = 23,
};

enum EColorArg
{
  eCA_Specular,
  eCA_Texture,
  eCA_Diffuse,
  eCA_Previous,
  eCA_Constant,
};

#define DEF_TEXARG0 (eCA_Texture|(eCA_Diffuse<<3))
#define DEF_TEXARG1 (eCA_Texture|(eCA_Previous<<3))

// Animating Texture sequence definition
struct STexAnim
{
  TArray<CTexture *> m_TexPics;
  int m_Rand;
  int m_NumAnimTexs;
  bool m_bLoop;
  float m_Time;

  int Size()
  {
    int nSize = sizeof(STexAnim);
    nSize += m_TexPics.GetMemoryUsage();
    return nSize;
  }

  STexAnim()
  {
    m_Rand = 0;
    m_NumAnimTexs = 0;
    m_bLoop = false;
    m_Time = 0.0f;
  }

  ~STexAnim()
  {     
    for (uint i=0; i<m_TexPics.Num(); i++)
    {
      ITexture *pTex = (ITexture *) m_TexPics[i];
      SAFE_RELEASE(pTex);
    }
    m_TexPics.Free();
  }

  STexAnim& operator = (const STexAnim& sl)
  {
    // make sure not same object
    if(this == &sl)   
    {
      return *this;
    }

    for (uint i=0; i<m_TexPics.Num(); i++)
    {
      ITexture *pTex = (ITexture *)m_TexPics[i];
      SAFE_RELEASE(pTex);
    }
    m_TexPics.Free();

    for (uint i=0; i<sl.m_TexPics.Num(); i++)
    {
      ITexture *pTex = (ITexture *)sl.m_TexPics[i];
      if(pTex)
      {
        pTex->AddRef();
      }

      m_TexPics.AddElem(sl.m_TexPics[i]);
    }

    m_Rand = sl.m_Rand;
    m_NumAnimTexs = sl.m_NumAnimTexs;
    m_bLoop = sl.m_bLoop;
    m_Time = sl.m_Time;

    return *this;
  }

};

enum ETexModRotateType
{
  ETMR_NoChange,
  ETMR_Fixed,
  ETMR_Constant,
  ETMR_Oscillated,
};

enum ETexModMoveType
{
  ETMM_NoChange,
  ETMM_Fixed,
  ETMM_Constant,
  ETMM_Jitter,
  ETMM_Pan,
  ETMM_Stretch,
  ETMM_StretchRepeat,
};

enum ETexGenType
{
  ETG_Stream,
  ETG_World,
  ETG_Camera,
  ETG_WorldEnvMap,
  ETG_CameraEnvMap,
  ETG_NormalMap,
  ETG_SphereMap,
};

#define CASE_TEXMOD(var_name)\
  if(!stricmp(#var_name,szParamName))\
  {\
    var_name = fValue;\
    return true;\
  }\

#define CASE_TEXMODANGLE(var_name)\
  if(!stricmp(#var_name,szParamName))\
{\
  var_name = Degr2Word(fValue);\
  return true;\
}\

#define CASE_TEXMODBYTE(var_name)\
  if(!stricmp(#var_name,szParamName))\
{\
  var_name = (byte)fValue;\
  return true;\
}\

#define CASE_TEXMODBOOL(var_name)\
  if(!stricmp(#var_name,szParamName))\
{\
  var_name = (fValue==1.f);\
  return true;\
}\

struct SEfTexModificator
{
  bool SetMember(const char * szParamName, float fValue)
  {
    CASE_TEXMODBYTE(m_eTGType);
    CASE_TEXMODBYTE(m_eRotType);
    CASE_TEXMODBYTE(m_eUMoveType);
    CASE_TEXMODBYTE(m_eVMoveType);
    CASE_TEXMODBOOL(m_bTexGenProjected);

    CASE_TEXMOD(m_Tiling[0]);
    CASE_TEXMOD(m_Tiling[1]);
    CASE_TEXMOD(m_Tiling[2]);
    CASE_TEXMOD(m_Offs[0]);
    CASE_TEXMOD(m_Offs[1]);
    CASE_TEXMOD(m_Offs[2]);

    CASE_TEXMODANGLE(m_Rot[0]);
    CASE_TEXMODANGLE(m_Rot[1]);
    CASE_TEXMODANGLE(m_Rot[2]);
    CASE_TEXMODANGLE(m_RotOscRate[0]);
    CASE_TEXMODANGLE(m_RotOscRate[1]);
    CASE_TEXMODANGLE(m_RotOscRate[2]);
    CASE_TEXMODANGLE(m_RotOscAmplitude[0]);
    CASE_TEXMODANGLE(m_RotOscAmplitude[1]);
    CASE_TEXMODANGLE(m_RotOscAmplitude[2]);
    CASE_TEXMODANGLE(m_RotOscPhase[0]);
    CASE_TEXMODANGLE(m_RotOscPhase[1]);
    CASE_TEXMODANGLE(m_RotOscPhase[2]);
    CASE_TEXMOD(m_RotOscCenter[0]);
    CASE_TEXMOD(m_RotOscCenter[1]);
    CASE_TEXMOD(m_RotOscCenter[2]);

    CASE_TEXMOD(m_UOscRate);
    CASE_TEXMOD(m_VOscRate);
    CASE_TEXMOD(m_UOscAmplitude);
    CASE_TEXMOD(m_VOscAmplitude);
    CASE_TEXMOD(m_UOscPhase);
    CASE_TEXMOD(m_VOscPhase);

    return false;
  }

  byte m_eTGType;
  byte m_eRotType;
  byte m_eUMoveType;
  byte m_eVMoveType;
  bool m_bTexGenProjected;

  float m_Tiling[3];
  float m_Offs[3];

  ushort m_Rot[3];
  ushort m_RotOscRate[3];
  ushort m_RotOscAmplitude[3];
  ushort m_RotOscPhase[3];
  float m_RotOscCenter[3];

  float m_UOscRate;
  float m_VOscRate;
  float m_UOscAmplitude;
  float m_VOscAmplitude;
  float m_UOscPhase;
  float m_VOscPhase;

  // This members are used only during updating of the matrices
  int m_nFrameUpdated;
  int m_nLastRecursionLevel;
  int m_UpdateFlags;
  bool m_bDontUpdate;
  Matrix44 m_TexMatrix;
  Matrix44 m_TexGenMatrix;
  float m_LastUTime;
  float m_LastVTime;
  float m_CurrentUJitter;
  float m_CurrentVJitter;

  _inline friend bool operator != (const SEfTexModificator &m1, const SEfTexModificator &m2)
  {
    if (m1.m_eTGType != m2.m_eTGType ||
        m1.m_eRotType != m2.m_eRotType ||
        m1.m_eUMoveType != m2.m_eUMoveType ||
        m1.m_eVMoveType != m2.m_eVMoveType ||
        m1.m_bTexGenProjected != m2.m_bTexGenProjected ||
        m1.m_UOscRate != m2.m_UOscRate ||
        m1.m_VOscRate != m2.m_VOscRate ||
        m1.m_UOscAmplitude != m2.m_UOscAmplitude ||
        m1.m_VOscAmplitude != m2.m_VOscAmplitude ||
        m1.m_UOscPhase != m2.m_UOscPhase ||
        m1.m_VOscPhase != m2.m_VOscPhase)
      return true;
    for (int i=0; i<3; i++)
    {
      if (m1.m_Tiling[i] != m2.m_Tiling[i] ||
          m1.m_Offs[i] != m2.m_Offs[i] ||
          m1.m_Rot[i] != m2.m_Rot[i] ||
          m1.m_RotOscRate[i] != m2.m_RotOscRate[i] ||
          m1.m_RotOscAmplitude[i] != m2.m_RotOscAmplitude[i] ||
          m1.m_RotOscPhase[i] != m2.m_RotOscPhase[i] ||
          m1.m_RotOscCenter[i] != m2.m_RotOscCenter[i])
        return true;
    }
    return false;
  }
};

struct STexState
{
  signed char m_nMinFilter;
  signed char m_nMagFilter;
  signed char m_nMipFilter;
  signed char m_nAddressU;
  signed char m_nAddressV;
  signed char m_nAddressW;
  signed char m_nAnisotropy;
  bool m_bActive;
  DWORD  m_dwBorderColor;
  bool m_bSRGBLookup;
  void *m_pDeviceState;
  bool m_bComparison;

  STexState ()
  {
    m_nMinFilter = 0;
    m_nMagFilter = 0;
    m_nMipFilter = 0;
    m_nAnisotropy = 0;
    m_nAddressU = 0;
    m_nAddressV = 0;
    m_nAddressW = 0;
    m_dwBorderColor = 0;
    m_bSRGBLookup = false;
    m_bActive = false;
    m_bComparison = true;
    m_pDeviceState = NULL;
    m_bComparison = false;
  }
  STexState(int nFilter, bool bClamp)
  {
    m_pDeviceState = NULL;
    int nAddress = bClamp ? TADDR_CLAMP : TADDR_WRAP;
    SetFilterMode(nFilter);
    SetClampMode(nAddress, nAddress, nAddress);
    SetBorderColor(0);
    m_bActive = false;
    m_bComparison = false;
  }
	STexState(int nFilter, int nAddressU, int nAddressV, int nAddressW, unsigned int borderColor)
	{
		m_pDeviceState = NULL;
		SetFilterMode(nFilter);
		SetClampMode(nAddressU, nAddressV, nAddressW);
		SetBorderColor(borderColor);
		m_bActive = false;
		m_bComparison = false;
	}
#ifdef _RENDERER
  ~STexState();
  STexState (const STexState& src);
#else
  ~STexState(){}
  STexState (const STexState& src)
  {
    memcpy(this, &src, sizeof(STexState));
  }
#endif
  STexState& operator = (const STexState& src)
  {
    this->~STexState();
    new(this) STexState(src);
    return *this;
  }
  _inline friend bool operator == (const STexState &m1, const STexState &m2)
  {
    if (*(uint *)&m1.m_nMinFilter == *(uint *)&m2.m_nMinFilter &&
        *(uint *)&m1.m_nAddressV == *(uint *)&m2.m_nAddressV &&
        m1.m_dwBorderColor == m2.m_dwBorderColor &&
        m1.m_bComparison == m2.m_bComparison)
      return true;
    return false;
  }
  void Release()
  {
    delete this;
  }

  bool SetFilterMode(int nFilter);
  bool SetClampMode(int nAddressU, int nAddressV, int nAddressW);
  void SetBorderColor(DWORD dwColor);
  void SetComparisonFilter(bool bEnable);
  void PostCreate();
};


#define FSAMP_NOMIPS 1
#define FSAMP_BUMPPLANTS 1

struct IRenderTarget
{
  virtual void Release()=0;
  virtual void AddRef()=0;
};

struct STexSampler
{
  string m_Name;
  string m_Texture;
  union
  {
    CTexture *m_pTex;
    ITexture *m_pITex;
  };
  union
  {
    struct SHRenderTarget *m_pTarget;
    IRenderTarget *m_pITarget;
  };

  int m_nTexState;
  byte m_eTexType;						// ETEX_Type e.g. eTT_2D or eTT_Cube
  int8 m_nSamplerSlot;
  uint m_nFlags;
  uint m_nTexFlags;
  STexAnim *m_pAnimInfo;
  IDynTextureSource* m_pDynTexSource;
  STexSampler()
  {
    m_pTex = NULL;
    m_eTexType = eTT_2D;
    m_nFlags = 0;
    m_nTexFlags = 0;
    m_pAnimInfo = NULL;
    m_pTarget = NULL;
    m_pDynTexSource = 0;
    m_nSamplerSlot = -1;
  }
  ~STexSampler()
  {
    SAFE_RELEASE(m_pITex);
    // TODO: ref counted deleting of m_pAnimInfo & m_pTarget! - CW
    SAFE_RELEASE(m_pDynTexSource);
    SAFE_RELEASE(m_pITarget);
  }

  int Size()
  {
    int nSize = sizeof(this);
    nSize += m_Name.size();
    nSize += m_Texture.size();
    return nSize;
  }
  uint GetTexFlags() { return m_nTexFlags; }
  void Update();
  STexSampler (const STexSampler& src)
  {
    m_pITex = src.m_pITex;
    if (m_pITex)
      m_pITex->AddRef();

		m_pDynTexSource = src.m_pDynTexSource;
		if (m_pDynTexSource)
			m_pDynTexSource->AddRef();

    m_Name = src.m_Name;
    m_Texture = src.m_Texture;
    m_pITarget = src.m_pITarget;
    if (m_pITarget)
      m_pITarget->AddRef();
    m_eTexType = src.m_eTexType;
    m_nFlags = src.m_nFlags;
    m_nTexFlags = src.m_nTexFlags;
    m_pAnimInfo = src.m_pAnimInfo;
    m_nTexState = src.m_nTexState;
    m_nSamplerSlot = src.m_nSamplerSlot;
  }
  STexSampler& operator = (const STexSampler& src)
  {
    this->~STexSampler();
    new(this) STexSampler(src);
    return *this;
  }
  _inline friend bool operator != (const STexSampler &m1, const STexSampler &m2)
  {
    if (stricmp(m1.m_Name.c_str(), m2.m_Name.c_str()) != 0 ||
      m1.m_eTexType != m2.m_eTexType || 
      m1.m_nTexFlags != m2.m_nTexFlags ||
      m1.m_nFlags != m2.m_nFlags)
      return true;
    return false;
  }
};


struct SEfResTexture
{
  // in order to facilitate the memory allocation tracking, we're using here this class;
  // if you don't like it, please write a substitute for all string within the project and use them everywhere
  string m_Name;
  byte m_TexFlags;
  byte m_Amount;
  bool m_bUTile;
  bool m_bVTile;
  signed char m_Filter;

  SEfTexModificator m_TexModificator;
  STexSampler m_Sampler;

  void Update(int nTSlot);

  _inline friend bool operator != (const SEfResTexture &m1, const SEfResTexture &m2)
  {
    if (stricmp(m1.m_Name.c_str(), m2.m_Name.c_str()) != 0 ||
        m1.m_TexFlags != m2.m_TexFlags || 
        m1.m_Amount != m2.m_Amount ||
        m1.m_bUTile != m2.m_bUTile ||
        m1.m_bVTile != m2.m_bVTile ||
        m1.m_Filter != m2.m_Filter ||
        m1.m_TexModificator != m2.m_TexModificator ||
        m1.m_Sampler != m2.m_Sampler)
      return true;
    return false;
  }

  bool IsNeedTexTransform()
  {
    if (m_TexModificator.m_eRotType != ETMR_NoChange || m_TexModificator.m_eUMoveType != ETMM_NoChange || m_TexModificator.m_eVMoveType != ETMM_NoChange)
      return true;
    return false;
  }
  bool IsNeedTexGen()
  {
    if (m_TexModificator.m_eTGType != ETG_Stream)
      return true;
    return false;
  }
  int Size()
  {
    int nSize = sizeof(SEfResTexture) - sizeof(STexSampler);
    nSize += m_Name.size();
    nSize += m_Sampler.Size();

    return nSize;
  }

  ~SEfResTexture()
  {
  }

  void Reset()
  {
    memset(&m_TexModificator, 0, sizeof(m_TexModificator));
    m_bUTile = true;
    m_bVTile = true;
    m_Amount = 100;
    m_Filter = FILTER_NONE;
    m_TexModificator.m_Tiling[0] = 1.0f;
    m_TexModificator.m_Tiling[1] = 1.0f;
    m_TexModificator.m_Tiling[2] = 1.0f;
    m_TexModificator.m_TexMatrix.SetIdentity();
    m_TexModificator.m_nFrameUpdated = -1;
  }
  SEfResTexture& operator=(const SEfResTexture& src)
  {
    m_Sampler = src.m_Sampler;
    memcpy(&m_TexModificator, &src.m_TexModificator, sizeof(SEfTexModificator));
    m_Amount = src.m_Amount;
    m_TexFlags = src.m_TexFlags;
    m_Name = src.m_Name;
    m_bUTile = src.m_bUTile;
    m_bVTile = src.m_bVTile;
    m_Filter = src.m_Filter;

    return *this;
  }

  SEfResTexture()
  {
    Reset();
  }
};

#define EFTT_DIFFUSE           0
#define EFTT_BUMP              1
#define EFTT_GLOSS             2
#define EFTT_ENV               3
#define EFTT_DETAIL_OVERLAY    4
#define EFTT_BUMP_DIFFUSE      5
#define EFTT_PHONG             6
#define EFTT_BUMP_HEIGHT       7
#define EFTT_SPECULAR          8
#define EFTT_DECAL_OVERLAY     9
#define EFTT_NORMALMAP         10
#define EFTT_SUBSURFACE        11
#define EFTT_CUSTOM            12
#define EFTT_CUSTOM_SECONDARY  13
#define EFTT_OPACITY           14
#define EFTT_LIGHTMAP          15
#define EFTT_LIGHTMAP_HDR      16
#define EFTT_LIGHTMAP_DIR      17
#define EFTT_RAE			         18
#define EFTT_OCCLUSION         19

#define EFTT_MAX               20

struct SBaseShaderResources
{
  uint m_ResFlags;
  float m_AlphaRef;
  uint8 m_PostEffects;
  DynArray<SShaderParam> m_ShaderParams;
  float m_fReflectionUpdateFactor;
  uint32 m_nReflectionMask;								// EDrawLowDetailFlags, defines what should be reflected
  string m_TexturePath;
  const char *m_szMaterialName;

  int Size()
  {
    int nSize = sizeof(SBaseShaderResources) + m_ShaderParams.size()*sizeof(SShaderParam);
    return nSize;
  }
  SBaseShaderResources& operator=(const SBaseShaderResources& src)
  {
    m_szMaterialName = src.m_szMaterialName;
    m_ResFlags = src.m_ResFlags;
    m_AlphaRef = src.m_AlphaRef;
    m_PostEffects = src.m_PostEffects;
    m_ShaderParams = src.m_ShaderParams;
    m_nReflectionMask = src.m_nReflectionMask;
    m_fReflectionUpdateFactor = src.m_fReflectionUpdateFactor;
    return *this;
  }

  SBaseShaderResources()
  {
    m_ResFlags = 0;
    m_AlphaRef = 0;
    m_PostEffects = 0;
    m_szMaterialName = NULL;
    m_nReflectionMask = -1;			// reflect everything
    m_fReflectionUpdateFactor = 0;
  }

  void ReleaseParams()
  {
    m_ShaderParams.clear();
  }
  virtual ~SBaseShaderResources()
  {
    ReleaseParams();
  }
};

struct IRenderShaderResources
{
  virtual void AddRef() = 0;
  virtual void UpdateConstants(IShader *pSH) = 0;
  virtual void CloneConstants(const IRenderShaderResources* pSrc) = 0;
  virtual void ExportModificators(IRenderShaderResources* pTrg, CRenderObject *pObj) = 0;
  virtual void SetInputLM(const CInputLightMaterial& lm) = 0;
  virtual void ToInputLM(CInputLightMaterial& lm) = 0;
  virtual ColorF& GetDiffuseColor() = 0;
  virtual ColorF& GetSpecularColor() = 0;
  virtual ColorF& GetEmissiveColor() = 0;
  virtual float& GetSpecularShininess() = 0;
  virtual int GetResFlags() = 0;
  virtual void SetMtlLayerNoDrawFlags( uint8 nFlags ) = 0;
  virtual uint8 GetMtlLayerNoDrawFlags() const = 0;
  virtual CCamera *GetCamera() = 0;
  virtual void SetCamera(CCamera *pCam) = 0;
  virtual void SetMaterialName(const char *szName) = 0;
  virtual float& GetGlow() = 0;
  virtual float& GetAlphaRef() = 0;
  virtual float& GetOpacity() = 0;
  virtual SEfResTexture *GetTexture(int nSlot) const = 0;
  virtual DynArray<SShaderParam>& GetParameters() = 0;

  virtual void Release() = 0;
  virtual void ConvertToInputResource(struct SInputShaderResources *pDst) = 0;
  virtual IRenderShaderResources *Clone() = 0;
  virtual void SetShaderParams(struct SInputShaderResources *pDst, IShader *pSH) = 0;
};


struct SInputShaderResources : public SBaseShaderResources
{
  CInputLightMaterial m_LMaterial;
  SEfResTexture m_Textures[EFTT_MAX];
  SDeformInfo m_DeformInfo;
  float m_Opacity;
  float m_GlowAmount;

  int Size()
  {
    int nSize = SBaseShaderResources::Size() - sizeof(SEfResTexture) * EFTT_MAX;
    nSize += m_TexturePath.size();
    for (int i=0; i<EFTT_MAX; i++)
    {
      nSize += m_Textures[i].Size();
    }
    return nSize;
  }
  SInputShaderResources& operator=(const SInputShaderResources& src)
  {
    SBaseShaderResources::operator = (src);
    m_TexturePath = src.m_TexturePath;
    m_DeformInfo = src.m_DeformInfo;
    m_Opacity = src.m_Opacity;
    m_GlowAmount = src.m_GlowAmount;
    int i;
    for (i=0; i<EFTT_MAX; i++)
    {
      m_Textures[i] = src.m_Textures[i];
    }
    m_LMaterial = src.m_LMaterial;
    return *this;
  }

  SInputShaderResources()
  {
    for (int i=0; i<EFTT_MAX; i++)
    {
      m_Textures[i].Reset();
    }
    m_Opacity = 0;
    m_GlowAmount = 0.0f;
  }

  SInputShaderResources(struct IRenderShaderResources *pSrc)
  {
    pSrc->ConvertToInputResource(this);
    m_ShaderParams = pSrc->GetParameters();
  }

  virtual ~SInputShaderResources()
  {
  }
  bool IsEmpty(int nTSlot) const
  {
    return m_Textures[nTSlot].m_Name.empty();
  }
};

//===================================================================================
// Shader gen structure (used for automatic shader script generating)

#define SHGF_HIDDEN   1
#define SHGF_PRECACHE 2
#define SHGF_AUTO_PRECACHE 4
#define SHGF_LOWSPEC_AUTO_PRECACHE 8

#define SHGD_LM_DIFFUSE  1
#define SHGD_LM_SPECULAR 2
#define SHGD_TEX_BUMP    4
#define SHGD_TEX_ENVCM   8
#define SHGD_TEX_GLOSS   0x10
#define SHGD_TEX_BUMPDIF 0x20
#define SHGD_TEX_OPACITY 0x40
#define SHGD_TEX_SUBSURFACE 0x80
#define SHGD_HW_BILINEARFP16   0x100
#define SHGD_HW_SEPARATEFP16   0x200
#define SHGD_HW_DYN_BRANCHING  0x400
#define SHGD_HW_STAT_BRANCHING 0x800
#define SHGD_TEX_CUSTOM 0x1000
#define SHGD_TEX_CUSTOM_SECONDARY 0x2000
#define SHGD_TEX_DECAL 0x4000
#define SHGD_HW_DYN_BRANCHING_POSTPROCESS 0x8000
#define SHGD_HW_ALLOW_POM 0x10000
#define SHGD_HW_SM30 0x20000

struct SShaderGenBit
{
  SShaderGenBit()
  {
    m_Mask = 0;
    m_Flags = 0;
    m_nDependencySet = 0;
    m_nDependencyReset = 0;
  }
  string m_ParamName;
  string m_ParamProp;
  string m_ParamDesc;
  int m_NameLength;
  uint64 m_Mask;
  uint m_Flags;
  uint32 m_dwToken;
  std::vector<uint32> m_PrecacheNames;
  std::vector<string> m_DependSets;
  std::vector<string> m_DependResets;
  uint32 m_nDependencySet;
  uint32 m_nDependencyReset;
};

struct SShaderGen
{
  uint32 m_nRefCount;
  TArray<SShaderGenBit *> m_BitMask;
  SShaderGen()
  {
    m_nRefCount = 1;
  }
  ~SShaderGen()
  {
    uint i;
    for (i=0; i<m_BitMask.Num(); i++)
    {
      SShaderGenBit *pBit = m_BitMask[i];
      SAFE_DELETE(pBit);
    }
    m_BitMask.Free();
  }
  void Release()
  {
    m_nRefCount--;
    if (!m_nRefCount)
      delete this;
  }
};

//===================================================================================

enum EShaderType
{
  eST_All=-1,				// to set all with one call

  eST_General=0,
  eST_Metal,
  eST_Glass,
  eST_Vegetation,
  eST_Ice,
  eST_Terrain,
  eST_Shadow,
  eST_Water,
  eST_FX,
  eST_PostProcess,
  eST_HDR,
  eST_Sky,
  eST_Particle,
  eST_Max						// to define array size
};


enum EShaderQuality
{
  eSQ_Low=0,
  eSQ_Medium=1,
  eSQ_High=2,
  eSQ_VeryHigh=3,
  eSQ_Max=4
};

enum ERenderQuality
{
  eRQ_Low=0,
  eRQ_Medium=1,
  eRQ_High=2,
  eRQ_VeryHigh=3,
  eRQ_Max=4
};

// shader profile flags 
#define SPF_LOADNORMALALPHA		0x1

struct SShaderProfile
{
  SShaderProfile() :m_iShaderProfileQuality(eSQ_High), m_nShaderProfileFlags(SPF_LOADNORMALALPHA)
  {
  }

  EShaderQuality GetShaderQuality() const 
  { 
    return (EShaderQuality)CLAMP(m_iShaderProfileQuality,0,eSQ_VeryHigh);
  }

  void SetShaderQuality( const EShaderQuality &rValue ) 
  { 
    m_iShaderProfileQuality = (int)rValue;
  }

  // ----------------------------------------------------------------

  int									m_iShaderProfileQuality;		// EShaderQuality e.g. eSQ_Medium, use Get/Set functions if possible
  uint32							m_nShaderProfileFlags;			// SPF_...
};

//====================================================================================
// Phys. material flags

#define MATF_NOCLIP 1

//====================================================================================
// Registered shader techniques ID's

#define TTYPE_Z          0
#define TTYPE_CAUSTICS   1
#define TTYPE_DETAIL     2
#define TTYPE_SHADOWPASS 3
#define TTYPE_SHADOWGEN  4
#define TTYPE_SHADOWGENGS 5
#define TTYPE_GLOWPASS   6
#define TTYPE_MOTIONBLURPASS 7
#define TTYPE_SCATTERPASS 8
#define TTYPE_CUSTOMRENDERPASS 9
#define TTYPE_RAINPASS 10

#define TTYPE_MAX        11

//====================================================================================

#define EFSLIST_PREPROCESS     1        // pre-process items
#define EFSLIST_GENERAL        2        // opaque ambient_light+shadow passes
#define EFSLIST_TERRAINLAYER   3        // unsorted terrain layers
#define EFSLIST_DECAL          4        // opaque or transparent decals
#define EFSLIST_WATER_VOLUMES  5       // after decals
#define EFSLIST_TRANSP         6       // sorted by distance under-water render items
#define EFSLIST_WATER          7       // water-ocean render items
#define EFSLIST_HDRPOSTPROCESS    8       // hdr post-processing screen effects
#define EFSLIST_AFTER_HDRPOSTPROCESS 9   // after hdr post-processing screen effects
#define EFSLIST_POSTPROCESS    10       // post-processing screen effects
#define EFSLIST_AFTER_POSTPROCESS 11   // after post-processing screen effects
#define EFSLIST_SHADOW_GEN    12  
#define EFSLIST_SHADOW_PASS   13   
#define EFSLIST_REFRACTPASS   14
#define EFSLIST_NUM           15

//================================================================
// Different preprocess flags for shaders that require preprocessing (like recursive render to texture, screen effects, visibility check, ...)
// SShader->m_nPreprocess flags in priority order
#define  SPRID_FIRST          16 
#define  SPRID_CORONA         16 
#define  FSPR_CORONA          (1<<SPRID_CORONA)
#define  SPRID_PANORAMA       17
#define  FSPR_PANORAMA        (1<<SPRID_PANORAMA)
#define  SPRID_PORTAL         18
#define  FSPR_PORTAL          (1<<SPRID_PORTAL)
#define  SPRID_SCANCM         19
#define  FSPR_SCANCM          (1<<SPRID_SCANCM)
#define  SPRID_SCANTEXWATER   20
#define  FSPR_SCANTEXWATER    (1<<SPRID_SCANTEXWATER)
#define  SPRID_SCANTEX        21
#define  FSPR_SCANTEX         (1<<SPRID_SCANTEX)
#define  SPRID_SCANLCM        22
#define  FSPR_SCANLCM         (1<<SPRID_SCANLCM)
#define  SPRID_GENSPRITES     23
#define  FSPR_GENSPRITES      (1<<SPRID_GENSPRITES)
#define  SPRID_CUSTOMTEXTURE  24
#define  FSPR_CUSTOMTEXTURE   (1<<SPRID_CUSTOMTEXTURE)
#define  SPRID_SHADOWMAPGEN   25
#define  FSPR_SHADOWMAPGEN    (1<<SPRID_SHADOWMAPGEN)
#define  SPRID_RAINOVERLAY    26
#define  FSPR_RAINOVERLAY     (1<<SPRID_RAINOVERLAY)
#define  SPRID_REFRACTED      27
#define  FSPR_REFRACTED       (1<<SPRID_REFRACTED)
#define  SPRID_SCREENTEXMAP   28
#define  FSPR_SCREENTEXMAP    (1<<SPRID_SCREENTEXMAP)

#define  FSPR_MAX             (1 << 29)

#define FEF_DONTSETTEXTURES   1					// set: explicit setting of samplers (e.g. tex->Apply(1,nTexStatePoint)), not set: set sampler by sematics (e.g. $ZTarget)
#define FEF_DONTSETSTATES     2

// SShader::m_Flags
// Different useful flags
#define EF_RELOAD        1       // Shader needs tangent vectors array
#define EF_FORCE_RELOAD  2
#define EF_RELOADED      4
#define EF_ENVLIGHTING   8
#define EF_HASCULL       0x10
#define EF_CANTBEINSTANCED 0x20
#define EF_DECAL         0x80
#define EF_MATERIAL      0x100
#define EF_LOCALCONSTANTS 0x200
#define EF_SKY_HDR       0x400
#define EF_LIGHTSTYLE    0x800
#define EF_NOCHUNKMERGING 0x1000
#define EF_SUNFLARES     0x2000
#define EF_NEEDNORMALS   0x4000  // Need normals operations
#define EF_OFFSETBUMP    0x8000
#define EF_NOTFOUND      0x10000
#define EF_DEFAULT       0x20000
#define EF_SKY           0x40000
#define EF_USELIGHTS     0x80000
#define EF_ALLOW3DC      0x100000
#define EF_FOGSHADER     0x200000
#define EF_USEPROJLIGHTS 0x400000
#define EF_SUPPORTSINSTANCING_CONST 0x2000000
#define EF_SUPPORTSINSTANCING_ATTR  0x4000000
#define EF_SUPPORTSINSTANCING (EF_SUPPORTSINSTANCING_CONST | EF_SUPPORTSINSTANCING_ATTR)
#define EF_DIFFUSELIGHT  0x8000000
#define EF_CLIENTEFFECT  0x10000000
#define EF_SYSTEM        0x20000000
#define EF_REFRACTIVE    0x40000000
#define EF_NOPREVIEW     0x80000000

#define EF_PARSE_MASK    (EF_SUPPORTSINSTANCING | EF_SKY | EF_SKY_HDR | EF_HASCULL | EF_USELIGHTS | EF_REFRACTIVE)

// SShader::Flags2
// Additional Different useful flags
#define EF2_PREPR_GENSPRITES 0x1
#define EF2_PREPR_OUTSPACE 0x2
#define EF2_NOCASTSHADOWS  0x4
#define EF2_NODRAW         0x8
#define EF2_HASOPAQUE      0x40
#define EF2_AFTERHDRPOSTPROCESS  0x80
#define EF2_DONTSORTBYDIST 0x100
#define EF2_FORCE_WATERPASS    0x200
#define EF2_FORCE_GENERALPASS   0x400
#define EF2_IGNORERESOURCESTATES  0x1000
#define EF2_USELIGHTMATERIAL  0x2000
#define EF2_FORCE_TRANSPASS       0x4000
#define EF2_DEFAULTVERTEXFORMAT 0x8000
#define EF2_FORCE_ZPASS 0x10000
#define EF2_FORCE_DRAWLAST 0x20000
#define EF2_FORCE_DRAWAFTERWATER 0x40000
#define EF2_DEPTHMAP_SUBSURFSCATTER 0x80000
#define EF2_SUPPORTS_REPLACEBASEPASS 0x100000
#define EF2_SINGLELIGHTPASS 0x200000
#define EF2_SKIPGENERALPASS 0x400000
#define EF2_HAIR            0x800000
#define EF2_DETAILBUMPMAPPING 0x1000000
#define EF2_HASALPHATEST      0x2000000
#define EF2_HASALPHABLEND     0x4000000

struct IShader
{
public:
  virtual int GetID() = 0;
  virtual int AddRef()=0;
  virtual int Release()=0;
  virtual int ReleaseForce()=0;

  virtual const char *GetName()=0;
  virtual const char *GetName() const =0;
  virtual int GetFlags() = 0;
  virtual int GetFlags2() = 0;
  virtual void SetFlags2(int Flags) = 0;
  virtual bool Reload(int nFlags, const char *szShaderName) = 0;
  virtual TArray<CRendElement *> *GetREs (int nTech) = 0;
  virtual DynArray<SShaderParam>& GetPublicParams() = 0;
  virtual int GetTexId () = 0;
  virtual ITexture *GetBaseTexture(int *nPass, int *nTU) = 0;
  virtual unsigned int GetUsedTextureTypes(void) = 0;
  virtual int GetVertexFormat(void) = 0;
  virtual ECull GetCull(void) = 0;
  virtual int Size(int Flags) = 0;
  virtual uint GetGenerationMask() = 0;
  virtual SShaderGen* GetGenerationParams() = 0;
  virtual int GetTechniqueID(int nTechnique, int nRegisteredTechnique) = 0;

  virtual EShaderType GetShaderType() = 0;
  virtual uint32      GetVertexModificator() = 0;
};

struct SShaderItem
{
  IShader *m_pShader;
  IRenderShaderResources *m_pShaderResources;
  int m_nTechnique;
  uint m_nPreprocessFlags;

  SShaderItem()
  {
    m_pShader = NULL;
    m_pShaderResources = NULL;
    m_nTechnique = -1;
    m_nPreprocessFlags = 0;
  }
  SShaderItem(IShader *pSH)
  {
    m_pShader = pSH;
    m_pShaderResources = NULL;
    m_nTechnique = -1;
    m_nPreprocessFlags = 0;
    if (pSH && (pSH->GetFlags2() & EF2_PREPR_GENSPRITES))
      m_nPreprocessFlags |= FSPR_GENSPRITES;
  }
  SShaderItem(IShader *pSH, IRenderShaderResources *pRS)
  {
    m_pShader = pSH;
    m_pShaderResources = pRS;
    m_nTechnique = -1;
    m_nPreprocessFlags = 0;
    if (pSH)
    {
      if (pSH->GetFlags2() & EF2_PREPR_GENSPRITES)
        m_nPreprocessFlags |= FSPR_GENSPRITES;
    }
  }
  SShaderItem(IShader *pSH, IRenderShaderResources *pRS, int nTechnique)
  {
    m_pShader = pSH;
    m_pShaderResources = pRS;
    m_nTechnique = nTechnique;
    m_nPreprocessFlags = 0;
    if (pSH)
    {
      if (pSH->GetFlags2() & EF2_PREPR_GENSPRITES)
        m_nPreprocessFlags |= FSPR_GENSPRITES;
    }
  }
  void PostLoad();
  _inline bool IsZWrite() const
  { // note: if you change this function please check bTransparent variable in CRenderMesh::Render()
    IShader *pSH = m_pShader;
    if (pSH->GetFlags2() & EF2_NODRAW)
      return false;
    if (pSH->GetFlags2() & EF2_FORCE_ZPASS)
      return true;
    if (pSH->GetFlags() & EF_DECAL)
      return false;
    if (m_pShaderResources)
    {
      ColorF& cDif = m_pShaderResources->GetDiffuseColor();
      if (cDif[3] != 1.0f)
        return false;
    }
    return true;
  }
  struct SShaderTechnique *GetTechnique() const;
  bool IsMergable(SShaderItem& PrevSI);
};

//////////////////////////////////////////////////////////////////////
struct CRenderChunk
{
  Vec3 m_vCenter;
  float m_fRadius;
  int nFirstIndexId;
  int nNumIndices;
  int nFirstVertId;
  int nNumVerts;
  CREMesh *pRE;        // Pointer to the mesh.
  int m_nMatFlags;       // Material flags from originally assigned material @see EMaterialFlags.
  int m_nMatID;          // Material Sub-object id.
  ushort m_dwNumSections;
  PodArray<uint16> m_arrChunkBoneIDs;

  //////////////////////////////////////////////////////////////////////////
  CRenderChunk()
  {
    m_dwNumSections=0;
    m_vCenter.Set(0,0,0);
    m_fRadius=0;
    nFirstIndexId=0;
    nNumIndices=0;
    nFirstVertId=0;
    nNumVerts=0;
    pRE=0;
    m_nMatFlags = 0;
    m_nMatID = 0;
  }

  int Size();
};


//////////////////////////////////////////////////////////////////////
// DLights
#define DLF_DETAIL          1
#define DLF_DIRECTIONAL     2
#define DLF_DYNAMIC         4       //dynamic lights
#define DLF_ACTIVE          8       //light is active/disactive
#define DLF_CASTSHADOW_MAPS 0x10    //light casting shadows
#define DLF_POINT           0x20
#define DLF_PROJECT         0x40
#define DLF_HAS_CBUFFER			0x80
#define DLF_POSITIONCHANGED 0x100
#define DLF_NOATTENUATION   0x200
#define DLF_UPDATED         0x400
#define DLF_INWORLDSPACE    0x800
#define DLF_DISABLED            0x1000
#define DLF_STATIC_ADDED    0x2000  //this static light has been already added to the list
#define DLF_HASAMBIENT      0x4000
#define DLF_HEATSOURCE      0x8000
#define DLF_LIGHTSOURCE     0x10000
#define DLF_FAKE            0x20000 //actually it's not LS, just render elements (Flares, beams, ...)
#define DLF_SUN             0x40000 //only sun may use this flag
#define DLF_COPY            0x80000
#define DLF_LOCAL           0x100000
#define DLF_LM              0x200000
#define DLF_THIS_AREA_ONLY  0x400000 // affects only current area/sector
#define DLF_FILL_LIGHT      0x800000 // cheap ambient light
#define DLF_NEGATIVE        0x1000000 // make ambient darker
#define DLF_INDOOR_ONLY     0x2000000 // do not affect heightmap
#define DLF_ONLY_FOR_HIGHSPEC 0x4000000 //!< This light is active as dynamic light only for high spec machines.
#define DLF_SPECULAR_ONLY_FOR_HIGHSPEC  0x8000000 //!< This light have specular component enabled only for high spec machines.
//#define DLF_LMDOT3          0x10000000
//#define DLF_FAKE_RADIOSITY  0x20000000
#define DLF_SPECULAROCCLUSION 0x40000000	//!< Use occlusion map for specular part of the light
#define DLF_DIFFUSEOCCLUSION 0x80000000	//!< Use occlusion map for diffuse part of the light

#define DLF_LIGHTTYPE_MASK (DLF_DIRECTIONAL | DLF_POINT | DLF_PROJECT)

//Area light types
#define DLAT_SPHERE				0x1
#define DLAT_RECTANGLE		0x2
#define DLAT_POINT				0x4

struct IEntity;

// Marco's NOTE: Andrey / Vlad please subividide this class
// by putting the members into functions and use the names
// info only if in debug mode, other strcutres can be
// allocated only if needed in the constructor and destroyed in
// the destructor, and you can even define an operator -> to
// access the data into the various structures inside the class

//////////////////////////////////////////////////////////////////////
class CDLight
{
public:

  //! constructor
  CDLight( void )
  {
    memset(this, 0, sizeof(CDLight));
    m_fLightFrustumAngle = 45.0f;
    m_fProjectorNearPlane = 0.0f;
    m_fRadius = 4.0f;
    m_fDirectFactor = 1.0f;
    m_SpecMult = 1.0f;
    m_Flags = DLF_LIGHTSOURCE;
    m_NumCM = -1;
    m_n3DEngineLightId = -1;
    m_ProjMatrix.SetIdentity();
    m_ObjMatrix.SetIdentity();
    m_sName = "";
    m_fCoronaDistSizeFactor = 1.0f;
    m_fCoronaDistIntensityFactor = 1.0f;
    m_AreaLightType = DLAT_POINT;
    m_nAreaSampleNumber = 1;
    m_bLightmapLinearAttenuation  = true;
    m_vSpotDirection.Set(0,1,0);
    m_fInAngle = gf_PI;
    m_fOutAngle = gf_PI;
    m_fHDRDynamic = 0.0f;
  }

  //! destructor
  ~CDLight()
  {
    SAFE_RELEASE(m_Shader.m_pShader);
    SAFE_RELEASE(m_pLightImage);
  }

	// good for debugging
	bool IsOk() const
	{
		for(int i=0;i<3;++i)
		{			
			if(m_Color[i]<0 || m_Color[i]>100.0f || _isnan(m_Color[i]))
				return false;
			if(m_BaseColor[i]<0 || m_BaseColor[i]>100.0f || _isnan(m_BaseColor[i]))
				return false;
		}
		return true;
	}

  void MakeBaseParams()
  {
    m_BaseOrigin = m_Origin;
    m_BaseColor = m_Color;
    m_SpecMult = m_SpecMult;
    m_RAEBaseAmbientColor = m_RAEAmbientColor;
    m_fBaseRadius = m_fRadius;
    m_fBaseLightFrustumAngle = m_fLightFrustumAngle;
  }
  void SetMatrix(const Matrix34& Matrix)
  {
    //scale the cubemap to adjust the default 45 degree 1/2 angle fustrum to 
    //the desired angle (0 to 90 degrees)
    float scaleFactor = cry_tanf((90.0f-m_fLightFrustumAngle)*gf_PI/180.0f);
    m_ProjMatrix = Matrix33(Matrix) * Matrix33::CreateScale(Vec3(1,scaleFactor,scaleFactor));
    Matrix44 transMat;
    transMat.SetIdentity();
    transMat(3,0) = -Matrix(0,3); transMat(3,1) = -Matrix(1,3); transMat(3,2) = -Matrix(2,3);
    m_ProjMatrix = transMat * m_ProjMatrix;
    m_ObjMatrix = Matrix;
  }

  //! assignment operator
  CDLight& operator=( const CDLight& dl )
  {
    memcpy(this, &dl, sizeof(CDLight));
    if (m_Shader.m_pShader)
      m_Shader.m_pShader->AddRef();
    if (m_pLightImage)
      m_pLightImage->AddRef();
    m_Flags |= DLF_COPY;
    return *this;
  }

	// use this instead of m_Color
	void SetLightColor( const ColorF &cColor )
	{
		m_Color = cColor;
		m_BaseColor = cColor;
	}

	// use this instead of m_Color
	const ColorF &GetFinalColor( const ColorF &cColor ) const
	{
		return m_Color;
	}


  int                             m_Id;
  Vec3                            m_Origin;          //world space position
  Vec3                            m_BaseOrigin;          //world space position
  ColorF                          m_Color;                  // clamped diffuse light color   todo: make private, only accessible through SetLightColor() and GetFinalColor()
  ColorF                          m_BaseColor;                  // clamped diffuse light color
  float                           m_SpecMult;
  float                           m_BaseSpecMult;
  Vec3                            m_vObjectSpacePos;     //Object space position
  float                           m_fRadius;
  float                           m_fBaseRadius;
  float                           m_fDirectFactor;
  float                           m_fStartRadius;
  float                           m_fEndRadius;
  float                           m_fLastTime;
  int                             m_NumCM;
  Vec4                            m_ShadowChanMask;
  float														m_fHDRDynamic;			// 0 to get the same results in HDR, <0 to get darker, >0 to get brighter

  // Scissor parameters (2d extent)
  short             m_sX;
  short             m_sY;
  short             m_sWidth;
  short             m_sHeight;
  // Far/near planes
  float             m_fNear;
  float             m_fFar;

  struct ILightSource * m_pOwner;
  uint32 m_nEntityId;             // Id of the entity who owns this light.
//  int m_nStaticLightId;
  
  //for static spot light sources casting volumetric shadows  
  int m_nReserved; // compensates for the vtbl
  int                             m_CustomTextureId;
  Matrix44                        m_ProjMatrix;
  Matrix34                        m_ObjMatrix;

  CRenderObject *                 m_pObject[4][4];                //!< Object for light coronas and light flares

  //the light image
  ITexture*                       m_pLightImage;
  float                           m_fLightFrustumAngle;
  float                           m_fProjectorNearPlane;
  float                           m_fBaseLightFrustumAngle;
  float                           m_fAnimSpeed;

  SShaderItem                     m_Shader;
  Ang3                            m_ProjAngles;

  uint                            m_Flags;                  //!< flags from above (prefix DLF_)

//  char                            m_sDebugName[16];
  uint                            m_nLightStyle;
  float                           m_fCoronaScale;
  float                           m_fCoronaDistSizeFactor;
  float                           m_fCoronaDistIntensityFactor;


  float                           m_fStartTime;
  float                           m_fLifeTime;              //!< lsource will be removed after this number of seconds

  ShadowMapFrustum						**  m_pShadowMapFrustums; //!<
  int                             m_nShadowMaskId;         //!<
  int                             m_nShadowMaskChan;       //!<

  int                             m_n3DEngineLightId;         //!<
  int                             m_nFrameID;               //!<

  const char*                     m_sName; // Optional name of the lightsource.

  //RAE
  ColorF													m_RAEBaseAmbientColor;		//!< the original color
  ColorF                          m_RAEAmbientColor;        //!< ambient light color * photon energy
/* for approximation not needed
  float														m_fRAEPhotonCurvness;			//!< the curvness of the respond curve.			
  float														m_fRAEPhotonStart;				//!< the start position of the respond curve
  float														m_fRAEShadowCurvness;			//!< the curvness of  the shadow respond curve
  float														m_fRAEDLRadiusDivider;		//!< the direct lights radius divider
*/

  //Prepocessed light infos
  uint														m_AreaLightType;						//!< Area light type
  int															m_nAreaSampleNumber;				//!< How much sample needed for this area light
  bool														m_bLightmapLinearAttenuation; //!< To be compatible with realtime lights - prebaked lights can use linear attenuation
  float														m_fFalloffStart;								//!< The start of the falloff
  float														m_fFalloffEnd;									//!< The end of the falloff
  //spotlight infos
  Vec3														m_vSpotDirection;								//!< The direction of spot light
  f32															m_fInAngle;											//!< Inner circle of the spot light
  f32															m_fOutAngle;										//!< Outer circle of the spot light
};

struct SFillLight
{
  Vec3  m_vOrigin;
  float m_fRadius;
  float m_fIntensity;
  bool  bNegative;
};

//===================================================================================
// Runtime shader flags for HW skinning

enum EHWSkinningRuntimeFlags
{
  eHWS_ShapeDeform = 0x01,
  eHWS_MorphTarget = 0x02,
  eHWS_MotionBlured = 0x04
};


//===================================================================================
// Shader graph support

enum EGrBlockType
{
  eGrBlock_Unknown,
  eGrBlock_VertexInput,
  eGrBlock_VertexOutput,
  eGrBlock_PixelInput,
  eGrBlock_PixelOutput,
  eGrBlock_Texture,
  eGrBlock_Sampler,
  eGrBlock_Function,
  eGrBlock_Constant,
};

enum EGrBlockSamplerType
{
  eGrBlockSampler_Unknown,
  eGrBlockSampler_2D,
  eGrBlockSampler_3D,
  eGrBlockSampler_Cube,
  eGrBlockSampler_Bias2D,
  eGrBlockSampler_BiasCube,
};

enum EGrNodeType
{
  eGrNode_Unknown,
  eGrNode_Input,
  eGrNode_Output,
};

enum EGrNodeFormat
{
  eGrNodeFormat_Unknown,
  eGrNodeFormat_Float,
  eGrNodeFormat_Vector,
  eGrNodeFormat_Matrix,
  eGrNodeFormat_Int,
  eGrNodeFormat_Bool,
  eGrNodeFormat_Texture2D,
  eGrNodeFormat_Texture3D,
  eGrNodeFormat_TextureCUBE,
};

enum EGrNodeIOSemantic
{
  eGrNodeIOSemantic_Unknown,
  eGrNodeIOSemantic_Custom,
  eGrNodeIOSemantic_VPos,
  eGrNodeIOSemantic_Color0,
  eGrNodeIOSemantic_Color1,
  eGrNodeIOSemantic_Color2,
  eGrNodeIOSemantic_Color3,
  eGrNodeIOSemantic_Normal,
  eGrNodeIOSemantic_TexCoord0,
  eGrNodeIOSemantic_TexCoord1,
  eGrNodeIOSemantic_TexCoord2,
  eGrNodeIOSemantic_TexCoord3,
  eGrNodeIOSemantic_TexCoord4,
  eGrNodeIOSemantic_TexCoord5,
  eGrNodeIOSemantic_TexCoord6,
  eGrNodeIOSemantic_TexCoord7,
  eGrNodeIOSemantic_Tangent,
  eGrNodeIOSemantic_Binormal,
};

struct SShaderGraphFunction
{
  string m_Data;
  string m_Name;
  std::vector<string> inParams;
  std::vector<string> outParams;
  std::vector<string> szInTypes;
  std::vector<string> szOutTypes;
};

struct SShaderGraphNode
{
  EGrNodeType m_eType;
  EGrNodeFormat m_eFormat;
  EGrNodeIOSemantic m_eSemantic;
  string m_CustomSemantics;
  string m_Name;
  bool m_bEditable;
  bool m_bWasAdded;
  SShaderGraphFunction *m_pFunction;
  DynArray<SShaderParam> m_Properties;

  SShaderGraphNode()
  {
    m_eType = eGrNode_Unknown;
    m_eFormat = eGrNodeFormat_Unknown;
    m_eSemantic = eGrNodeIOSemantic_Unknown;
    m_bEditable = false;
    m_bWasAdded = false;
    m_pFunction = NULL;
  }
  ~SShaderGraphNode();
};


typedef std::vector<SShaderGraphNode *> FXShaderGraphNodes;
typedef FXShaderGraphNodes::iterator FXShaderGraphNodeItor;

struct SShaderGraphBlock
{
  EGrBlockType m_eType;
  EGrBlockSamplerType m_eSamplerType;
  string m_ClassName;
  FXShaderGraphNodes  m_Nodes;

  ~SShaderGraphBlock();
};

typedef std::vector<SShaderGraphBlock *> FXShaderGraphBlocks;
typedef FXShaderGraphBlocks::iterator FXShaderGraphBlocksItor;

#include "RendElement.h"

#endif// _ISHADER_H_
