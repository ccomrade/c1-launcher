// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __RENDELEMENT_H__
#define __RENDELEMENT_H__

//=============================================================

#include "VertexFormats.h"

class CRendElement;
struct CRenderChunk;
struct PrimitiveGroup;
class CShader;
struct SShaderTechnique;
class CParserBin;
struct SParserFrame;

enum EDataType
{
  eDATA_Unknown = 0,
  eDATA_Dummy,
  eDATA_Sky,		
  eDATA_Beam,		
  eDATA_ClientPoly,
  eDATA_ClientPoly2D,
  eDATA_Flare,
  eDATA_Terrain,
  eDATA_SkyZone,
  eDATA_Mesh,
  eDATA_Imposter,
	eDATA_PanoramaCluster,
  eDATA_TerrainSector,
  eDATA_FarTreeSprites,  
  eDATA_ShadowMapGen,
  eDATA_TerrainDetailTextureLayers,
  eDATA_TerrainParticles,
  eDATA_Ocean,  
  eDATA_OcclusionQuery,
  eDATA_TempMesh,
	eDATA_Particle,
  eDATA_PostProcess, 
  eDATA_HDRProcess,  
  eDATA_Cloud,  
	eDATA_HDRSky,  
	eDATA_FogVolume,
	eDATA_WaterVolume,
  eDATA_WaterWave,
  eDATA_WaterOcean,
	eDATA_VolumeObject
};

#include <Cry_Color.h>

//=======================================================

#define FCEF_TRANSFORM 1
#define FCEF_TRACE     2
#define FCEF_NODEL     4

#define FCEF_MODIF_TC   0x10
#define FCEF_MODIF_VERT 0x20
#define FCEF_MODIF_COL  0x40
#define FCEF_MODIF_MASK 0xf0

#define FCEF_NEEDFILLBUF 0x100
#define FCEF_ALLOC_CUST_FLOAT_DATA 0x200
#define FCEF_MERGABLE    0x400
#define FCEF_GEOM_SHADER 0x800

#define FGP_NOCALC 1
#define FGP_SRC    2
#define FGP_REAL   4
#define FGP_WAIT   8

#define FGP_STAGE_SHIFT 0x10

#define MAX_CUSTOM_TEX_BINDS_NUM 2

class CRendElement
{
public:
  EDataType m_Type;
  uint m_Flags;

public:
  void *m_CustomData;
  int m_CustomTexBind[MAX_CUSTOM_TEX_BINDS_NUM];
	
  static CRendElement m_RootGlobal;
  CRendElement *m_NextGlobal;
  CRendElement *m_PrevGlobal;

protected:

  _inline void UnlinkGlobal()
  {
    if (!m_NextGlobal || !m_PrevGlobal)
      return;
    m_NextGlobal->m_PrevGlobal = m_PrevGlobal;
    m_PrevGlobal->m_NextGlobal = m_NextGlobal;
    m_NextGlobal = m_PrevGlobal = NULL;
  }
  _inline void LinkGlobal( CRendElement* Before )
  {
    if (m_NextGlobal || m_PrevGlobal)
      return;
    m_NextGlobal = Before->m_NextGlobal;
    Before->m_NextGlobal->m_PrevGlobal = this;
    Before->m_NextGlobal = this;
    m_PrevGlobal = Before;
  }

public:
  CRendElement()
  {
    m_Type = eDATA_Unknown;
    m_NextGlobal = NULL;
    m_PrevGlobal = NULL;
    m_Flags = 0;
    m_CustomData = NULL;
		for(int i=0; i<MAX_CUSTOM_TEX_BINDS_NUM; i++)
	    m_CustomTexBind[i] = -1;
    if (!m_RootGlobal.m_NextGlobal)
    {
      m_RootGlobal.m_NextGlobal = &m_RootGlobal;
      m_RootGlobal.m_PrevGlobal = &m_RootGlobal;
    }
    if (this != &m_RootGlobal)
      LinkGlobal(&m_RootGlobal);
  }

  virtual ~CRendElement();
  virtual void Release();

  const char *mfTypeString();

  EDataType mfGetType() { return m_Type; }

  void mfSetType(EDataType t) { m_Type = t; }

  uint mfGetFlags(void) { return m_Flags; }
  void mfSetFlags(uint fl) { m_Flags = fl; }
  void mfUpdateFlags(uint fl) { m_Flags |= fl; }
  void mfClearFlags(uint fl) { m_Flags &= ~fl; }

  virtual void mfPrepare();
  virtual bool mfCullByClipPlane(CRenderObject *pObj);
  virtual CRenderChunk *mfGetMatInfo();
  virtual PodArray<CRenderChunk> *mfGetMatInfoList();
  virtual int mfGetMatId();
  virtual void mfReset();
  virtual bool mfIsHWSkinned() { return false; }
  virtual CRendElement *mfCopyConstruct(void);
  virtual void mfCenter(Vec3& centr, CRenderObject*pObj);
  virtual void mfGetBBox(Vec3& vMins, Vec3& vMaxs)
  {
    vMins.Set(0,0,0);
    vMaxs.Set(0,0,0);
  }
  virtual void mfGetPlane(Plane& pl);
  virtual float mfDistanceToCameraSquared(Matrix34& matInst);
  virtual bool mfCompile(CParserBin& Parser, SParserFrame& Frame) { return false; }
  virtual bool mfDraw(CShader *ef, SShaderPass *sfm);
  virtual void *mfGetPointer(ESrcPointer ePT, int *Stride, EParamType Type, ESrcPointer Dst, int Flags);
  virtual bool mfPreDraw(SShaderPass *sl) { return true; }
  virtual float mfMinDistanceToCamera(Matrix34& matInst) {return -1;};
  virtual bool mfCheckUpdate(int nVertFormat, int Flags) {return true;}
  virtual void mfPrecache(const SShaderItem& SH) {}
  virtual int Size() {return 0;}

  static void ShutDown();
};

//==========================================================

#endif  // __RENDELEMENT_H__
