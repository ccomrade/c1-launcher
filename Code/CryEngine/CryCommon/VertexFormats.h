//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File:VertexFormats.h - 
//
//	History:
//	-Feb 23,2001:Created by Marco Corbetta
//
//////////////////////////////////////////////////////////////////////

#ifndef VERTEXFORMATS_H
#define VERTEXFORMATS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <CryArray.h>

// If you change this you also have to change Offs Tables in RenderMesh.cpp
enum eVertexFormat
{
  // General formats
	VERTEX_FORMAT_P3F=1,                // shadow volumes (12 bytes)
  VERTEX_FORMAT_P3F_COL4UB=2,         // usually terrain (16 bytes)
  VERTEX_FORMAT_P3F_TEX2F=3,          // everything else (20 bytes)
	VERTEX_FORMAT_P3F_COL4UB_TEX2F=4,   // usually plants (24 bytes)
  VERTEX_FORMAT_P3F_N4B_COL4UB=5,     // (20 bytes) terrain
  VERTEX_FORMAT_P4S_TEX2F=6,          // (16 bytes)
  VERTEX_FORMAT_P4S_COL4UB_TEX2F=7,   // (20 bytes)

  // Special formats
  VERTEX_FORMAT_P3F_COL4UB_RES4UB_TEX2F_PS4F=8,  // Particles
  VERTEX_FORMAT_TRP3F_COL4UB_TEX2F=9, // fonts (28 bytes)
  VERTEX_FORMAT_TRP3F_TEX2F_TEX3F=10,  // misc
  VERTEX_FORMAT_P3F_TEX3F=11,          // misc
	VERTEX_FORMAT_P3F_TEX2F_TEX3F=12,   // misc

  // Additional streams
  VERTEX_FORMAT_TEX2F=13,              // light maps TC (8 bytes)
	VERTEX_FORMAT_WEIGHTS4UB_INDICES4UB_P3F=14, // skinned weights/indices stream
  VERTEX_FORMAT_COL4UB_COL4UB=15,      // SH coefficients
	VERTEX_FORMAT_2xP3F_INDEX4UB=16,		 // shape deformation stream
  VERTEX_FORMAT_NUMS=17,               // number of vertex formats
};

//#define TANG_FLOATS 1
#ifdef TANG_FLOATS
#define int16f float
#else
#define int16f short
#endif

typedef Vec4_tpl<int16f> Vec4sf;		//used for tangents only

// bNeedNormals=1 - float normals; bNeedNormals=2 - byte normals
_inline int VertFormatForComponents(bool bNeedCol, bool bHasTC, bool bHasPS)
{
  int RequestedVertFormat;

  if (bHasPS)
    RequestedVertFormat = VERTEX_FORMAT_P3F_COL4UB_RES4UB_TEX2F_PS4F;
  else
  if (!bNeedCol && !bHasTC)
    RequestedVertFormat = VERTEX_FORMAT_P3F;
  else
  if (bNeedCol && !bHasTC)
    RequestedVertFormat = VERTEX_FORMAT_P3F_COL4UB;
  else
  if (!bNeedCol && bHasTC)
    RequestedVertFormat = VERTEX_FORMAT_P3F_TEX2F;
  else
  if (bNeedCol && bHasTC)
    RequestedVertFormat = VERTEX_FORMAT_P3F_COL4UB_TEX2F;
  else
  {
    assert(0);
  }

  return RequestedVertFormat;
}

struct UCol
{
	union
	{
		uint8  bcolor[4];
		uint32 dcolor;
	};
	AUTO_STRUCT_INFO
};

// Map 0..1 => 0..255
inline uint8 UnitUFloatToUInt8( float f )
{
	assert(f >= 0.f && f <= 1.f);
	return uint8(pos_round(f * 255.f));
}

// Map -1..1 => 0..254 (so 0 => whole number, extract in shader: f * 2 * 255/254 - 1)
inline uint8 UnitSFloatToUInt8( float f )
{
	assert(f >= -1.f && f <= 1.f);
	return uint8(pos_round(f * 127.f + 127.f));
}

struct struct_VERTEX_FORMAT_P3F // 12 bytes
{
  Vec3 xyz;
};

struct struct_VERTEX_FORMAT_P3F_COL4UB  // 16 bytes
{
  Vec3 xyz;
  UCol color;
};

struct struct_VERTEX_FORMAT_TEX2F // 8 bytes
{
  Vec2 st;
};

struct struct_VERTEX_FORMAT_P3F_TEX2F
{
  Vec3 xyz;
  Vec2 st;
};

struct struct_VERTEX_FORMAT_P4S_TEX2F
{
  Vec4sf xyz;
  Vec2 st;
};

struct struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F
{
  Vec3 xyz;
  UCol color;
  Vec2 st;
  AUTO_STRUCT_INFO
  bool operator == (struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F & other);
};

struct struct_VERTEX_FORMAT_P4S_COL4UB_TEX2F
{
  Vec4sf xyz;
  UCol color;
  Vec2 st;
};

struct struct_VERTEX_FORMAT_P3F_N4B_COL4UB
{
  Vec3 xyz;
  UCol normal;
  UCol color;
};

//=======================================================================

struct struct_VERTEX_FORMAT_WEIGHTS4UB_INDICES4UB_P3F
{
  UCol weights;
  UCol indices;
	Vec3 boneSpace;
};

struct struct_VERTEX_FORMAT_2xP3F_INDEX4UB
{	
	Vec3 thin;	
	Vec3 fat;	
	UCol index;
};

// SH coefs
struct struct_VERTEX_FORMAT_COL4UB_COL4UB
{
  UCol coef0;
  UCol coef1;
};

struct struct_VERTEX_FORMAT_P3F_COL4UB_RES4UB_TEX2F_PS4F
{
  Vec3 xyz;
  UCol color;
	UCol reserved;		// w = backlight, xy = displacement, z = polygon break
  float st[2];
	Vec2 xaxis;
	Vec2 yaxis;

	inline void SetDisplacement( float dx, float dy )
	{
		reserved.bcolor[2] = UnitSFloatToUInt8(dx);
		reserved.bcolor[1] = UnitSFloatToUInt8(dy);
	}
	inline void SetBacklighting( float f)
	{
		reserved.bcolor[0] = UnitUFloatToUInt8(f);
	}
	inline void SetPolygonBreak(bool b)
	{
		reserved.bcolor[3] = b;
	}
	inline uint8 GetPolygonBreak() const
	{
		return reserved.bcolor[3];
	}
};

struct struct_VERTEX_FORMAT_TRP3F_COL4UB_TEX2F
{
  Vec4 pos;
  UCol color;
  Vec2 st;
};

struct struct_VERTEX_FORMAT_TRP3F_TEX2F_TEX3F
{
  Vec4 p;
  Vec2 st0;
  Vec3 st1;
};

struct struct_VERTEX_FORMAT_P3F_TEX3F
{
  Vec3 p;
  Vec3 st;
};

struct struct_VERTEX_FORMAT_P3F_TEX2F_TEX3F
{
	Vec3 p;
	Vec2 st0;
	Vec3 st1;
};

//=============================================================

_inline int16f tPackF2B(const float f)
{
#ifdef TANG_FLOATS
  return f;
#else
  return (int16f)(f * 32767.0f);
#endif
}
_inline float tPackB2F(const int16f i)
{
#ifdef TANG_FLOATS
  return i;
#else
  return (float)((float)i / 32767.0f);
#endif
}

_inline Vec4sf tPackF2Bv(const Vec4& v)
{
  Vec4sf vs;
  vs.x = tPackF2B(v.x);
  vs.y = tPackF2B(v.y);
  vs.z = tPackF2B(v.z);
  vs.w = tPackF2B(v.w);

  return vs;
}
_inline Vec4sf tPackF2Bv(const Vec3& v)
{
  Vec4sf vs;
  vs.x = tPackF2B(v.x);
  vs.y = tPackF2B(v.y);
  vs.z = tPackF2B(v.z);
  vs.w = tPackF2B(1.0f);

  return vs;
}
_inline Vec4 tPackB2F(const Vec4sf v)
{
  Vec4 vs;
  vs.x = tPackB2F(v.x);
  vs.y = tPackB2F(v.y);
  vs.z = tPackB2F(v.z);
  vs.w = tPackB2F(v.w);

  return vs;
}
_inline void tPackB2F(const Vec4sf& v, Vec4& vDst)
{
  vDst.x = tPackB2F(v.x);
  vDst.y = tPackB2F(v.y);
  vDst.z = tPackB2F(v.z);
  vDst.w = 1.0f;
}
_inline void tPackB2FScale(const Vec4sf& v, Vec4& vDst, const Vec3& vScale)
{
  vDst.x = (float)v.x * vScale.x;
  vDst.y = (float)v.y * vScale.y;
  vDst.z = (float)v.z * vScale.z;
  vDst.w = 1.0f;
}
_inline void tPackB2FScale(const Vec4sf& v, Vec3& vDst, const Vec3& vScale)
{
  vDst.x = (float)v.x * vScale.x;
  vDst.y = (float)v.y * vScale.y;
  vDst.z = (float)v.z * vScale.z;
}

_inline void tPackB2F(const Vec4sf& v, Vec3& vDst)
{
  vDst.x = tPackB2F(v.x);
  vDst.y = tPackB2F(v.y);
  vDst.z = tPackB2F(v.z);
}

struct SPipTangents
{
  Vec4sf Tangent;
  Vec4sf Binormal;
};

struct SAuxVertex
{
	Vec3 xyz;
	UCol color;
};

_inline void *CreateVertexBuffer(int nFormat, int nVerts)
{
  switch(nFormat)
  {
    case VERTEX_FORMAT_P3F:
      return new struct_VERTEX_FORMAT_P3F[nVerts];

    case VERTEX_FORMAT_P3F_COL4UB_TEX2F:
      return new struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F[nVerts];

    case VERTEX_FORMAT_P3F_N4B_COL4UB:
      return new struct_VERTEX_FORMAT_P3F_N4B_COL4UB[nVerts];

    case VERTEX_FORMAT_P3F_TEX2F:
      return new struct_VERTEX_FORMAT_P3F_TEX2F[nVerts];

    case VERTEX_FORMAT_P4S_COL4UB_TEX2F:
      return new struct_VERTEX_FORMAT_P4S_COL4UB_TEX2F[nVerts];
    case VERTEX_FORMAT_P4S_TEX2F:
      return new struct_VERTEX_FORMAT_P4S_TEX2F[nVerts];

    case VERTEX_FORMAT_P3F_COL4UB:
      return new struct_VERTEX_FORMAT_P3F_COL4UB[nVerts];

    case VERTEX_FORMAT_TRP3F_COL4UB_TEX2F:
      return new struct_VERTEX_FORMAT_TRP3F_COL4UB_TEX2F[nVerts];

    case VERTEX_FORMAT_TRP3F_TEX2F_TEX3F:
      return new struct_VERTEX_FORMAT_TRP3F_TEX2F_TEX3F[nVerts];

    case VERTEX_FORMAT_P3F_TEX3F:
      return new struct_VERTEX_FORMAT_P3F_TEX3F[nVerts];

		case VERTEX_FORMAT_P3F_TEX2F_TEX3F:
			return new struct_VERTEX_FORMAT_P3F_TEX2F_TEX3F[nVerts];

    case VERTEX_FORMAT_TEX2F:
      return new struct_VERTEX_FORMAT_TEX2F[nVerts];

		case VERTEX_FORMAT_WEIGHTS4UB_INDICES4UB_P3F:
			return new struct_VERTEX_FORMAT_WEIGHTS4UB_INDICES4UB_P3F[nVerts];

		case VERTEX_FORMAT_2xP3F_INDEX4UB:
			return new struct_VERTEX_FORMAT_2xP3F_INDEX4UB[nVerts];

    case VERTEX_FORMAT_COL4UB_COL4UB:
      return new struct_VERTEX_FORMAT_COL4UB_COL4UB[nVerts];

    case VERTEX_FORMAT_P3F_COL4UB_RES4UB_TEX2F_PS4F:
      assert(!"returning non-array which will subsequently get deleted with delete[].");
      // this should return an array, but I (Danny) don't know if it should be [1] or [nVerts]
      // currently it results in invalid code
      return new struct_VERTEX_FORMAT_P3F_COL4UB_RES4UB_TEX2F_PS4F[nVerts];

    default:
      assert(0);
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Sizes
extern const int m_VertexSize[];

// we don't care about truncation of the struct member offset, because
// it's a very small integer (even fits into a signed byte)
#pragma warning(push)
#pragma warning(disable:4311)

//============================================================================
// Custom vertex streams definitions
// NOTE: If you add new stream ID also include vertex declarations creating in 
//       CD3D9Renderer::EF_InitD3DVertexDeclarations (D3DRendPipeline.cpp)

// Stream IDs
enum EStreamIDs
{
  VSF_GENERAL,									// General vertex buffer
  VSF_TANGENTS,									// Tangents buffer
  VSF_LMTC,											// Lightmap TC
  VSF_HWSKIN_INFO,							// HW skinning buffer
  VSF_SH_INFO,									// SH buffer
	VSF_HWSKIN_SHAPEDEFORM_INFO,  // HW skinning (shape deformation) buffer
	VSF_HWSKIN_MORPHTARGET_INFO,  // HW skinning (morph targets) buffer
																// <- Insert new stream IDs here
  VSF_NUM,											// Number of vertex streams
  
  VSF_MORPHBUDDY = 8,           // Morphing (from m_pMorphBuddy)
  VSF_MORPHBUDDY_WEIGHTS = 15,  // Morphing weights
};

// Stream Masks (Used during updating)
enum EStreamMasks
{
  VSM_GENERAL    = 1 << VSF_GENERAL,
  VSM_TANGENTS   = 1 << VSF_TANGENTS,
  VSM_LMTC       = 1 << VSF_LMTC,
  VSM_HWSKIN     = 1 << VSF_HWSKIN_INFO,
  VSM_SH         = 1 << VSF_SH_INFO,
	VSM_HWSKIN_SHAPEDEFORM       = 1 << VSF_HWSKIN_SHAPEDEFORM_INFO,
	VSM_HWSKIN_MORPHTARGET       = 1 << VSF_HWSKIN_MORPHTARGET_INFO,

  VSM_MORPHBUDDY = 1 << VSF_MORPHBUDDY,

  VSM_MASK     = ((1 << VSF_NUM)-1),
};

extern const int m_StreamSize[];

union UHWBuf
{
  void *m_pPtr;
  uint32 m_nID;
};

struct alloc_info_struct { int ptr; int bytes_num; bool busy; const char *szSource; };
struct SVertPool
{
  int m_nBufSize;
  void *m_pVB;
  PodArray<alloc_info_struct> m_alloc_info;
};

struct SVertexStream
{
  void *m_VData;      // pointer to buffer data
  UHWBuf m_VertBuf;   // HW buffer descriptor
  bool m_bLocked;     // Used in Direct3D only
  bool m_bDynamic;
  int m_nBufOffset;
  SVertPool *m_pPool;
  SVertexStream()
  {
    Reset();
    m_bDynamic = false;
    m_nBufOffset = 0;
    m_pPool = NULL;
  }

  void Reset()
  {
    m_VData = NULL;
    m_VertBuf.m_pPtr = NULL;
    m_bLocked = false;
  }
  void *GetStream(int *nOffs)
  {
    if (nOffs)
      *nOffs = m_nBufOffset;
    if (m_pPool && !m_bDynamic)
      return m_pPool->m_pVB;
    else
      return m_VertBuf.m_pPtr;
  }
};

//////////////////////////////////////////////////////////////////////
// General VertexBuffer created by CreateVertexBuffer() function
class CVertexBuffer
{
public:
  CVertexBuffer()
  {
    for (int i=0; i<VSF_NUM; i++)
    {
      m_VS[i].Reset();
    }
    m_nVertexFormat = 0;
  }

  CVertexBuffer(void* pData, int nVertexFormat, int nVertCount=0)
  {
    for (int i=0; i<VSF_NUM; i++)
    {
      m_VS[i].m_VData = NULL;
      m_VS[i].m_VertBuf.m_pPtr = NULL;
      m_VS[i].m_bLocked = false;
    }
    m_VS[VSF_GENERAL].m_VData = pData;
    m_nVertexFormat = nVertexFormat;
  }
  void *GetStream(int nStream, int *nOffs)
  {
    return m_VS[nStream].GetStream(nOffs);
  }

  SVertexStream m_VS[VSF_NUM]; // 4 vertex streams and one index stream

  int m_nVertexFormat;

  int Size(int Flags, int nVerts);
};

struct SBufInfoTable
{
  int OffsTC;
  int OffsColor;
};

extern SBufInfoTable gBufInfoTable[];

#pragma warning(pop)

struct SVertBufComps
{
	bool m_bHasTC;
	bool m_bHasColors;
  bool m_bShortPos;
};

_inline void GetVertBufComps(SVertBufComps *Comps, int Format)
{
  memset(Comps, 0, sizeof(SVertBufComps));
  if (gBufInfoTable[Format].OffsTC)
    Comps->m_bHasTC = true;
  if (gBufInfoTable[Format].OffsColor)
    Comps->m_bHasColors = true;
  if (Format == VERTEX_FORMAT_P4S_TEX2F || Format == VERTEX_FORMAT_P4S_COL4UB_TEX2F)
    Comps->m_bShortPos = true;
}



#endif

