
//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//  File:Renderer.h - API Indipendent
//
//  History:
//  -Jan 31,2001:Originally created by Marco Corbetta
//  -: Taken over by Andrey Khonich
//
//////////////////////////////////////////////////////////////////////

#ifndef _IRENDERER_H
#define _IRENDERER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum eSplashType
{
  EST_Water,
};

// Global typedefs.
//////////////////////////////////////////////////////////////////////
#ifndef BOOL
typedef int                 BOOL;
#endif
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef int                 INT;
typedef unsigned int        UINT;

#ifndef uchar
typedef unsigned char   uchar;
typedef unsigned int    uint;
typedef unsigned short  ushort;
#endif

// callback used for DXTCompress
typedef void (*MIPDXTcallback)( const void *buffer, size_t count, void * userData );

//forward declarations.
//////////////////////////////////////////////////////////////////////
typedef void* WIN_HWND;
typedef void* WIN_HINSTANCE;
typedef void* WIN_HDC;
typedef void* WIN_HGLRC;

class   CVertexBuffer;
class   CREMesh;
//class   CImage;
struct  CStatObj;
class   CVegetation;
struct  ShadowMapFrustum;
struct  IStatObj;
class   CObjManager;
struct  ShadowMapFrustum;
struct  SPrimitiveGroup;
struct  ICharacterInstance;
class   CRendElement;
class   CRenderObject;
class   CTexMan;
//class   ColorF;
class   CShadowVolEdge;
class   CCamera;
class   CDLight;
struct  ILog;
struct  IConsole;
struct  ITimer;
struct  ISystem;
class   ICrySizer;
struct IRenderAuxGeom;
struct SREPointSpriteCreateParams;
struct SPointSpriteVertex;
struct RenderLMData;
struct SShaderParam;
struct SSkyLightRenderParams;
struct sRAEColdData;
struct SParticleRenderInfo;
struct IVideoPlayer;
struct IParticleVertexCreator;

//////////////////////////////////////////////////////////////////////
typedef unsigned char bvec4[4];
typedef float vec4_t[4];
typedef unsigned char byte;
typedef float vec2_t[2];

//DOC-IGNORE-BEGIN
#include "Cry_Color.h"
#include "Tarray.h"

#include <IFont.h>
//DOC-IGNORE-END

enum EScreenAspectRatio
{
  eAspect_Unknown,
  eAspect_4_3,
  eAspect_16_9,
  eAspect_16_10,
};

class CRenderCamera
{
public:

  // X: Right
  // Y: Up
  // Z: Back (-Z = Forward)
  Vec3 X, Y, Z;            // Normalized camera coordinate-axis vectors
  Vec3 Orig;               // Location of origin of camera system in world coords
  float wL,wR,wB,wT;       // Window extents defined as a rect on NearPlane
  float Near,Far;          // Distances to near and far planes (in Viewing dir)

  CRenderCamera();
  CRenderCamera(const CRenderCamera &Cam);
  void Copy(const CRenderCamera &Cam);

  void LookAt(const Vec3& Eye, const Vec3& ViewRefPt, const Vec3& ViewUp);
  void Perspective(float Yfov, float Aspect, float Ndist, float Fdist);
  void Frustum(float l, float r, float b, float t, float Ndist, float Fdist);

//  void TightlyFitToSphere(const Vec3& Eye, const Vec3& ViewUp, const Vec3& Cntr, float RadX, float RadY);

  void GetLookAtParams(Vec3 *Eye, Vec3 *ViewRefPt, Vec3 *ViewUp) const;
  void GetPerspectiveParams(float *Yfov, float *Xfov, float *Aspect, float *Ndist, float *Fdist) const;
  void GetFrustumParams(float *l, float *r, float *b, float *t, float *Ndist, float *Fdist) const;
  const Vec3& wCOP() const; // WORLD COORDINATE CENTER-OF-PROJECTION (EYE)
  Vec3 ViewDir() const;     // VIEWING DIRECTION
  Vec3 ViewDirOffAxis() const;

  float* GetXform_Screen2Obj(float* M, int WW, int WH) const;
  float* GetXform_Obj2Screen(float* M, int WW, int WH) const;

  float* GetModelviewMatrix(float* M) const;
  float* GetProjectionMatrix(float* M) const;
  float* GetViewportMatrix(float* M, int WW, int WH) const;

  void SetModelviewMatrix(const float* M);

  float* GetInvModelviewMatrix(float* M) const;
  float* GetInvProjectionMatrix(float* M) const;
  float* GetInvViewportMatrix(float* M, int WW, int WH) const;

  Vec3 WorldToCam(const Vec3 &wP) const;
  float WorldToCamZ(const Vec3 &wP) const;
  Vec3 CamToWorld(const Vec3 &cP) const;

  void LoadIdentityXform();
  void Xform(const float M[16]);

  void Translate(const Vec3& trans);
  void Rotate(const float M[9]);

  void GetPixelRay(float sx, float sy, int ww, int wh, Vec3 *Start, Vec3 *Dir) const;

  void CalcVerts(Vec3 *V) const;    // CALCS EIGHT CORNERS OF VIEW-FRUSTUM

  void CalcTileVerts(Vec3 *V, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const;    // CALCS EIGHT CORNERS FOR TILE OF VIEW-FRUSTUM 

  void CalcRegionVerts(Vec3 *V, const Vec2& vMin, const Vec2& vMax) const;

  void CalcTiledRegionVerts(Vec3 *V, Vec2& vMin, Vec2& vMax, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const;

};

inline float* Frustum16fv(float* M, float l, float r, float b, float t, float n, float f)
{
  M[0]=(2*n)/(r-l); M[4]=0;           M[8]=(r+l)/(r-l);   M[12]=0;
  M[1]=0;           M[5]=(2*n)/(t-b); M[9]=(t+b)/(t-b);   M[13]=0;
  M[2]=0;           M[6]=0;           M[10]=-(f+n)/(f-n); M[14]=(-2*f*n)/(f-n);
  M[3]=0;           M[7]=0;           M[11]=-1;           M[15]=0;
  return M;
}

inline float* Viewing16fv(float* M, const Vec3 X, const Vec3 Y, const Vec3 Z, const Vec3 O)
{
  M[0]=X.x;  M[4]=X.y;  M[8]=X.z;  M[12]=-X|O;
  M[1]=Y.x;  M[5]=Y.y;  M[9]=Y.z;  M[13]=-Y|O;
  M[2]=Z.x;  M[6]=Z.y;  M[10]=Z.z; M[14]=-Z|O;
  M[3]=0;    M[7]=0;    M[11]=0;   M[15]=1;
  return M;
}


inline CRenderCamera::CRenderCamera()
{
  X.Set(1,0,0); Y.Set(0,1,0); Z.Set(0,0,1);
  Orig.Set(0,0,0);
  Near=1.4142f; Far=10; wL=-1; wR=1; wT=1; wB=-1;
}

inline CRenderCamera::CRenderCamera(const CRenderCamera &Cam)
{
  Copy(Cam);
}

inline void CRenderCamera::Copy(const CRenderCamera &Cam)
{
  X=Cam.X;  Y=Cam.Y;  Z=Cam.Z;  Orig=Cam.Orig;
  Near=Cam.Near;  Far=Cam.Far;
  wL=Cam.wL;  wR=Cam.wR;  wT=Cam.wT;  wB=Cam.wB;
}

inline void CRenderCamera::LookAt(const Vec3& Eye, const Vec3& ViewRefPt, const Vec3& ViewUp)
{
  Z = Eye-ViewRefPt;  Z.Normalize(); 
  X = ViewUp % Z;     X.Normalize();
  Y = Z % X;          Y.Normalize();
  Orig = Eye;
}

inline void CRenderCamera::Perspective(float Yfov, float Aspect, float Ndist, float Fdist)
{
  Near = Ndist;  Far=Fdist;
  wT = tanf(Yfov*0.5f)*Near;  wB=-wT;
  wR = wT*Aspect; wL=-wR;
}

inline void CRenderCamera::Frustum(float l, float r, float b, float t, float Ndist, float Fdist)
{
  Near=Ndist;  Far=Fdist;
  wR=r;  wL=l;  wB=b;  wT=t;
}


inline void CRenderCamera::GetLookAtParams(Vec3 *Eye, Vec3 *ViewRefPt, Vec3 *ViewUp) const
{
  *Eye = Orig;
  *ViewRefPt = Orig - Z;
  *ViewUp = Y;
}

inline void CRenderCamera::GetPerspectiveParams(float *Yfov, float *Xfov, float *Aspect, float *Ndist, float *Fdist) const
{
  *Yfov = atanf(wT/Near) * 57.29578f * 2.0f;
  *Xfov = atanf(wR/Near) * 57.29578f * 2.0f;
  *Aspect = wT/wR;
  *Ndist = Near;
  *Fdist = Far;
}

inline void CRenderCamera::GetFrustumParams(float *l, float *r, float *b, float *t, float *Ndist, float *Fdist) const
{
  *l = wL;
  *r = wR;
  *b = wB;
  *t = wT;
  *Ndist = Near;
  *Fdist = Far;
}

inline const Vec3& CRenderCamera::wCOP() const
{
  return( Orig );
}

inline Vec3 CRenderCamera::ViewDir() const
{
  return(-Z);
}

inline Vec3 CRenderCamera::ViewDirOffAxis() const
{
  float x=(wL+wR)*0.5f, y=(wT+wB)*0.5f;  // MIDPOINT ON VIEWPLANE WINDOW
  Vec3 ViewDir = X*x + Y*y - Z*Near;
  ViewDir.Normalize();
  return ViewDir;
}

inline Vec3 CRenderCamera::WorldToCam(const Vec3& wP) const
{
  Vec3 sP(wP-Orig);
  Vec3 cP(X|sP,Y|sP,Z|sP);
  return cP;
}

inline float CRenderCamera::WorldToCamZ(const Vec3& wP) const
{
  Vec3 sP(wP-Orig);
  float zdist = Z|sP;
  return zdist;
}

inline Vec3 CRenderCamera::CamToWorld(const Vec3& cP) const
{
  Vec3 wP(X*cP.x + Y*cP.y + Z*cP.z + Orig);
  return wP;
}

inline void CRenderCamera::LoadIdentityXform()
{
  X.Set(1,0,0);
  Y.Set(0,1,0);
  Z.Set(0,0,1);
  Orig.Set(0,0,0);
}

inline void CRenderCamera::Xform(const float M[16])
{
  X.Set( X.x*M[0] + X.y*M[4] + X.z*M[8],
    X.x*M[1] + X.y*M[5] + X.z*M[9],
    X.x*M[2] + X.y*M[6] + X.z*M[10] );
  Y.Set( Y.x*M[0] + Y.y*M[4] + Y.z*M[8],
    Y.x*M[1] + Y.y*M[5] + Y.z*M[9],
    Y.x*M[2] + Y.y*M[6] + Y.z*M[10] );
  Z.Set( Z.x*M[0] + Z.y*M[4] + Z.z*M[8],
    Z.x*M[1] + Z.y*M[5] + Z.z*M[9],
    Z.x*M[2] + Z.y*M[6] + Z.z*M[10] );
  Orig.Set( Orig.x*M[0] + Orig.y*M[4] + Orig.z*M[8] + M[12],
    Orig.x*M[1] + Orig.y*M[5] + Orig.z*M[9] + M[13],
    Orig.x*M[2] + Orig.y*M[6] + Orig.z*M[10] + M[14] );

  float Scale = X.GetLength();
  X /= Scale;
  Y /= Scale;
  Z /= Scale;

  wL*=Scale;
  wR*=Scale;
  wB*=Scale;
  wT*=Scale;
  Near*=Scale;
  Far*=Scale;
};

inline void CRenderCamera::Translate(const Vec3& trans)
{
  Orig += trans;
}

inline void CRenderCamera::Rotate(const float M[9])
{
  X.Set( X.x*M[0] + X.y*M[3] + X.z*M[6],
    X.x*M[1] + X.y*M[4] + X.z*M[7],
    X.x*M[2] + X.y*M[5] + X.z*M[8] );
  Y.Set( Y.x*M[0] + Y.y*M[3] + Y.z*M[6],
    Y.x*M[1] + Y.y*M[4] + Y.z*M[7],
    Y.x*M[2] + Y.y*M[5] + Y.z*M[8] );
  Z.Set( Z.x*M[0] + Z.y*M[3] + Z.z*M[6],
    Z.x*M[1] + Z.y*M[4] + Z.z*M[7],
    Z.x*M[2] + Z.y*M[5] + Z.z*M[8] );
}

inline float* CRenderCamera::GetModelviewMatrix(float* M) const
{
  Viewing16fv(M,X,Y,Z,Orig);
  return M;
}

inline float* CRenderCamera::GetProjectionMatrix(float* M) const
{
  Frustum16fv(M,wL,wR,wB,wT,Near,Far);
  return(M);  
}

inline void CRenderCamera::GetPixelRay(float sx, float sy, int ww, int wh, Vec3 *Start, Vec3 *Dir) const
{
  Vec3 wTL = Orig + (X*wL) + (Y*wT) - (Z*Near);  // FIND LOWER-LEFT
  Vec3 dX = (X*(wR-wL))/(float)ww;               // WORLD WIDTH OF PIXEL
  Vec3 dY = (Y*(wT-wB))/(float)wh;               // WORLD HEIGHT OF PIXEL
  wTL += (dX*sx - dY*sy);                         // INCR TO WORLD PIXEL
  wTL += (dX*0.5 - dY*0.5);                       // INCR TO PIXEL CNTR
  *Start = Orig;
  *Dir = wTL-Orig;
}

inline void CRenderCamera::CalcVerts(Vec3 *V)  const
{
  float NearZ = -Near;
  V[0].Set(wR,wT,NearZ);
  V[1].Set(wL,wT,NearZ);
  V[2].Set(wL,wB,NearZ);
  V[3].Set(wR,wB,NearZ);

  float FarZ=-Far, FN=Far/Near;
  float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;
  V[4].Set(fwR,fwT,FarZ);
  V[5].Set(fwL,fwT,FarZ);
  V[6].Set(fwL,fwB,FarZ);
  V[7].Set(fwR,fwB,FarZ);

  for (int i=0; i<8; i++)
    V[i] = CamToWorld(V[i]); 
}

inline void CRenderCamera::CalcTileVerts(Vec3 *V, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY)  const
{
	float NearZ = -Near;

	float TileWidth = abs(wR - wL)/nGridSizeX ;
	float TileHeight = abs(wT - wB)/nGridSizeY;
  float TileL = wL + TileWidth * nPosX;
	float TileR = wL + TileWidth * (nPosX + 1);
	float TileB = wB + TileHeight * nPosY;
	float TileT = wB + TileHeight * (nPosY + 1);

	V[0].Set(TileR,TileT,NearZ);
	V[1].Set(TileL,TileT,NearZ);
	V[2].Set(TileL,TileB,NearZ);
	V[3].Set(TileR,TileB,NearZ);

	float FarZ=-Far, FN=Far/Near;
	float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

	float TileFarWidth = abs(fwR - fwL)/nGridSizeX ;
	float TileFarHeight = abs(fwT - fwB)/nGridSizeY;
	float TileFarL = fwL + TileFarWidth * nPosX;
	float TileFarR = fwL + TileFarWidth * (nPosX + 1);
	float TileFarB = fwB + TileFarHeight * nPosY;
	float TileFarT = fwB + TileFarHeight * (nPosY + 1);

	V[4].Set(TileFarR,TileFarT,FarZ);
	V[5].Set(TileFarL,TileFarT,FarZ);
	V[6].Set(TileFarL,TileFarB,FarZ);
	V[7].Set(TileFarR,TileFarB,FarZ);

	for (int i=0; i<8; i++)
		V[i] = CamToWorld(V[i]); 
}

inline void CRenderCamera::CalcTiledRegionVerts(Vec3 *V, Vec2& vMin, Vec2& vMax, f32 nPosX, f32 nPosY, f32 nGridSizeX, f32 nGridSizeY) const
{
  float NearZ = -Near;

  Vec2 vTileMin, vTileMax;

  vMin.x = max(vMin.x, nPosX/nGridSizeX);
  vMax.x = min(vMax.x, (nPosX + 1)/nGridSizeX);

  vMin.y = max(vMin.y, nPosY/nGridSizeY);
  vMax.y = min(vMax.y, (nPosY + 1)/nGridSizeY);

  vTileMin.x = abs(wR - wL) * vMin.x;
  vTileMin.y = abs(wT - wB) * vMin.y;
  vTileMax.x = abs(wR - wL) * vMax.x;
  vTileMax.y = abs(wT - wB) * vMax.y;

  float TileWidth = abs(wR - wL)/nGridSizeX ;
  float TileHeight = abs(wT - wB)/nGridSizeY;

  float TileL = wL + vTileMin.x;
  float TileR = wL + vTileMax.x;
  float TileB = wB + vTileMin.y;
  float TileT = wB + vTileMax.y;

  V[0].Set(TileR,TileT,NearZ);
  V[1].Set(TileL,TileT,NearZ);
  V[2].Set(TileL,TileB,NearZ);
  V[3].Set(TileR,TileB,NearZ);

  float FarZ=-Far, FN=Far/Near;
  float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

  Vec2 vTileFarMin, vTileFarMax;

  vTileFarMin.x = abs(fwR - fwL) * vMin.x;
  vTileFarMin.y = abs(fwT - fwB) * vMin.y;
  vTileFarMax.x = abs(fwR - fwL) * vMax.x;
  vTileFarMax.y = abs(fwT - fwB) * vMax.y;

  float TileFarWidth = abs(fwR - fwL)/nGridSizeX ;
  float TileFarHeight = abs(fwT - fwB)/nGridSizeY;

  float TileFarL = fwL + vTileFarMin.x;
  float TileFarR = fwL + vTileFarMax.x;
  float TileFarB = fwB + vTileFarMin.y;
  float TileFarT = fwB + vTileFarMax.y;

  V[4].Set(TileFarR,TileFarT,FarZ);
  V[5].Set(TileFarL,TileFarT,FarZ);
  V[6].Set(TileFarL,TileFarB,FarZ);
  V[7].Set(TileFarR,TileFarB,FarZ);

  for (int i=0; i<8; i++)
    V[i] = CamToWorld(V[i]); 

  //recalculate light quad region for this particular quad 
  /*vMin.x = vMin.x - (vMin.x / nGridSizeX);
  vMax.x = vMax.x - (vMin.x / nGridSizeX);

  vMin.y = vMin.y - (vMin.y / nGridSizeY);
  vMax.y = vMax.y - (vMin.y / nGridSizeY);*/
}


inline void CRenderCamera::CalcRegionVerts(Vec3 *V, const Vec2& vMin, const Vec2& vMax) const
{
  float NearZ = -Near;

  Vec2 vTileMin, vTileMax;

  vTileMin.x = abs(wR - wL) * vMin.x;
  vTileMin.y = abs(wT - wB) * vMin.y;
  vTileMax.x = abs(wR - wL) * vMax.x;
  vTileMax.y = abs(wT - wB) * vMax.y;

  float TileL = wL + vTileMin.x;
  float TileR = wL + vTileMax.x;
  float TileB = wB + vTileMin.y;
  float TileT = wB + vTileMax.y;

  V[0].Set(TileR,TileT,NearZ);
  V[1].Set(TileL,TileT,NearZ);
  V[2].Set(TileL,TileB,NearZ);
  V[3].Set(TileR,TileB,NearZ);

  float FarZ=-Far, FN=Far/Near;
  float fwL=wL*FN, fwR=wR*FN, fwB=wB*FN, fwT=wT*FN;

  Vec2 vTileFarMin, vTileFarMax;

  vTileFarMin.x = abs(fwR - fwL) * vMin.x;
  vTileFarMin.y = abs(fwT - fwB) * vMin.y;
  vTileFarMax.x = abs(fwR - fwL) * vMax.x;
  vTileFarMax.y = abs(fwT - fwB) * vMax.y;

  float TileFarL = fwL + vTileFarMin.x;
  float TileFarR = fwL + vTileFarMax.x;
  float TileFarB = fwB + vTileFarMin.y;
  float TileFarT = fwB + vTileFarMax.y;

  V[4].Set(TileFarR,TileFarT,FarZ);
  V[5].Set(TileFarL,TileFarT,FarZ);
  V[6].Set(TileFarL,TileFarB,FarZ);
  V[7].Set(TileFarR,TileFarB,FarZ);

  for (int i=0; i<8; i++)
    V[i] = CamToWorld(V[i]); 
}


class SBoundingVolume
{
public:
  SBoundingVolume() : m_vCenter(0, 0, 0), m_fRadius(0) {}
  ~SBoundingVolume() {}

  void SetCenter(const Vec3 &center)  { m_vCenter = center; }
  void SetRadius(float radius)        { m_fRadius = radius; }
  const Vec3& GetCenter() const       { return m_vCenter;   }
  float GetRadius() const             { return m_fRadius;   }

protected:
  Vec3    m_vCenter;
  float   m_fRadius;
};

class SMinMaxBox : public SBoundingVolume
{
public:
  SMinMaxBox()
  {
    Clear();
  }
  //! Destructor
  virtual ~SMinMaxBox() {}

  void  AddPoint(const Vec3 &pt)
  {
    if(pt.x > m_max.x)
      m_max.x = pt.x;
    if(pt.x < m_min.x)
      m_min.x = pt.x;

    if(pt.y > m_max.y)
      m_max.y = pt.y;
    if(pt.y < m_min.y)
      m_min.y = pt.y;

    if(pt.z > m_max.z )
      m_max.z = pt.z;
    if(pt.z < m_min.z )
      m_min.z = pt.z;

    // update the center and radius
    UpdateSphere();
  }
  void  AddPoint(float x , float y , float z)
  {
    AddPoint(Vec3(x, y, z));
  }

  void  Union(const SMinMaxBox& box)  { AddPoint(box.GetMin()); AddPoint(box.GetMax()); }

  const Vec3 &GetMin() const     { return m_min; }
  const Vec3 &GetMax() const     { return m_max; }

  void  SetMin(const Vec3 &min)  { m_min = min; UpdateSphere(); }
  void  SetMax(const Vec3 &max)  { m_max = max; UpdateSphere(); }

  float GetWidthInX() const       { return m_max.x - m_min.x;}
  float GetWidthInY() const       { return m_max.y - m_min.y;}
  float GetWidthInZ() const       { return m_max.z - m_min.z;}

  bool  PointInBBox(const Vec3 &pt) const;

  bool  ViewFrustumCull(const CRenderCamera &cam, const Matrix44 &mat);

  void  Transform(const Matrix34& mat)
  {
    Vec3 verts[8];
    CalcVerts(verts);
    Clear();
    for (int i=0; i<8; i++)
    {
      AddPoint(mat.TransformPoint(verts[i]));
    }
  }

  // Reset the bounding box
  void  Clear()
  {
    m_min = Vec3(999999.0f, 999999.0f, 999999.0f);
    m_max = Vec3(-999999.0f, -999999.0f, -999999.0f);
  }

protected:
  void UpdateSphere()
  {
    m_vCenter =  m_min;
    m_vCenter += m_max;
    m_vCenter *= 0.5f;

    Vec3 rad  =  m_max;
    rad      -= m_vCenter;
    m_fRadius =  rad.len();
  }
  void CalcVerts(Vec3 pVerts[8]) const
  {
    pVerts[0].Set(m_max.x, m_max.y, m_max.z); pVerts[4].Set(m_max.x, m_max.y, m_min.z);
    pVerts[1].Set(m_min.x, m_max.y, m_max.z); pVerts[5].Set(m_min.x, m_max.y, m_min.z);
    pVerts[2].Set(m_min.x, m_min.y, m_max.z); pVerts[6].Set(m_min.x, m_min.y, m_min.z);
    pVerts[3].Set(m_max.x, m_min.y, m_max.z); pVerts[7].Set(m_max.x, m_min.y, m_min.z);
  }

private:
  Vec3 m_min;   // Original object space BV
  Vec3 m_max;
};



//////////////////////////////////////////////////////////////////////
#define R_CULL_DISABLE  0
#define R_CULL_NONE     0
#define R_CULL_FRONT    1
#define R_CULL_BACK     2

//////////////////////////////////////////////////////////////////////
#define R_TEXGEN_LINEAR 1

//////////////////////////////////////////////////////////////////////
#define R_FOGMODE_LINEAR  1
#define R_FOGMODE_EXP2    2

//////////////////////////////////////////////////////////////////////
#define R_DEFAULT_LODBIAS 0

//////////////////////////////////////////////////////////////////////
#define R_PRIMV_TRIANGLES				0
#define R_PRIMV_TRIANGLE_STRIP	1
#define R_PRIMV_QUADS						2
#define R_PRIMV_TRIANGLE_FAN		3
#define R_PRIMV_MULTI_STRIPS		4
#define R_PRIMV_HWSKIN_GROUPS		6
#define R_PRIMV_LINES						7
#define R_PRIMV_LINESTRIP				8

//////////////////////////////////////////////////////////////////////
#define FILTER_NONE      -1
#define FILTER_POINT      0
#define FILTER_LINEAR     1
#define FILTER_BILINEAR   2
#define FILTER_TRILINEAR  3
#define FILTER_ANISO2X    4
#define FILTER_ANISO4X    5
#define FILTER_ANISO8X    6
#define FILTER_ANISO16X   7

//////////////////////////////////////////////////////////////////////
#define TADDR_WRAP        0
#define TADDR_CLAMP       1
#define TADDR_MIRROR      2
#define TADDR_BORDER      3

//////////////////////////////////////////////////////////////////////
#define R_SOLID_MODE    1
#define R_WIREFRAME_MODE  2

#define R_GL_RENDERER   0
#define R_DX9_RENDERER  2
#define R_DX10_RENDERER 3
#define R_NULL_RENDERER 4
#define R_CUBAGL_RENDERER 5

//////////////////////////////////////////////////////////////////////
// Render features

#define RFT_MULTITEXTURE 1
#define RFT_OCCLUSIONQUERY 4
#define RFT_PALTEXTURE   8      // Support paletted textures
#define RFT_HWGAMMA      0x10
#define RFT_ALLOWRECTTEX  0x20  // Allow non-power-of-two textures
#define RFT_COMPRESSTEXTURE  0x40
#define RFT_ALLOWANISOTROPIC 0x100  // Allows anisotropic texture filtering
#define RFT_SUPPORTZBIAS     0x200
#define RFT_HW_ENVBUMPPROJECTED 0x400 // Allows projected environment maps with EMBM
#define RFT_ALLOWSECONDCOLOR 0x800
#define RFT_DETAILTEXTURE    0x1000
#define RFT_TEXGEN_REFLECTION 0x2000
#define RFT_TEXGEN_EMBOSS     0x4000
#define RFT_OCCLUSIONTEST     0x8000 // Support hardware occlusion test

#define RFT_HW_GF2        0x10000 // GF2 class hardware (ATI Radeon 7500 as well :) )
#define RFT_HW_GF3        0x20000 // NVidia GF3 class hardware (ATI Radeon 8500 as well :) )
#define RFT_HW_ATI				0x30000 // unclassified ATI hardware
#define RFT_HW_NVIDIA     0x40000 // unclassified NVidia hardware
#define RFT_HW_GFFX       0x50000 // Geforce FX class hardware
#define RFT_HW_NV4X       0x60000 // NV4X class hardware
#define RFT_HW_MASK       0x70000 // Graphics chip mask

#define RFT_HW_HDR        0x80000 // Hardware supports high dynamic range rendering

#define RFT_HW_PS20       0x100000  // Pixel shaders 2.0
#define RFT_HW_PS2X       0x200000  // Pixel shaders 2.X
#define RFT_HW_PS30       0x400000  // Pixel shaders 3.0
#define RFT_HW_PS40       0x800000  // Pixel shaders 4.0

#define RFT_ZLOCKABLE     0x4000000  // depth buffer can be locked for read
#define RFT_DIRECTACCESSTOVIDEOMEMORY   0x10000000
#define RFT_RGBA          0x20000000 // RGBA order (otherwise BGRA)
#define RFT_DEPTHMAPS     0x40000000 // depth maps are supported
#define RFT_HW_VERTEXTEXTURES 0x80000000 // vertex texture fecthing supported

//====================================================================
// PrecacheResources flags

#define FPR_NEEDLIGHT     1
#define FPR_2D            2
#define FPR_IMMEDIATELLY  4


//====================================================================
// Draw shaders flags (EF_EndEf3d)

#define SHDF_ALLOWHDR               (1<<0)
#define SHDF_SORT                   (1<<1)
#define SHDF_ZPASS                  (1<<2)
#define SHDF_ZPASS_ONLY             (1<<3)
#define SHDF_DO_NOT_CLEAR_Z_BUFFER  (1<<4)
#define SHDF_ALLOWPOSTPROCESS       (1<<5)
#define SHDF_DO_NOT_RENDER_TARGET   (1<<6)
#define SHDF_CLEAR_SHADOW_MASK      (1<<7)
#define SHDF_ALLOW_AO               (1<<8)
#define SHDF_ALLOW_WATER            (1<<9)
#define SHDF_NOASYNC                (1<<10)

//////////////////////////////////////////////////////////////////////

// Render State flags
#define GS_BLSRC_MASK              0xf
#define GS_BLSRC_ZERO              0x1
#define GS_BLSRC_ONE               0x2
#define GS_BLSRC_DSTCOL            0x3
#define GS_BLSRC_ONEMINUSDSTCOL    0x4
#define GS_BLSRC_SRCALPHA          0x5
#define GS_BLSRC_ONEMINUSSRCALPHA  0x6
#define GS_BLSRC_DSTALPHA          0x7
#define GS_BLSRC_ONEMINUSDSTALPHA  0x8
#define GS_BLSRC_ALPHASATURATE     0x9

#define GS_BLDST_MASK              0xf0
#define GS_BLDST_ZERO              0x10
#define GS_BLDST_ONE               0x20
#define GS_BLDST_SRCCOL            0x30
#define GS_BLDST_ONEMINUSSRCCOL    0x40
#define GS_BLDST_SRCALPHA          0x50
#define GS_BLDST_ONEMINUSSRCALPHA  0x60
#define GS_BLDST_DSTALPHA          0x70
#define GS_BLDST_ONEMINUSDSTALPHA  0x80

#define GS_BLEND_MASK              0xff

#define GS_DEPTHWRITE              0x00000100

#define GS_COLMASK_RT1             0x00000200
#define GS_COLMASK_RT2             0x00000400
#define GS_COLMASK_RT3             0x00000800

#define GS_NOCOLMASK_R             0x00001000
#define GS_NOCOLMASK_G             0x00002000
#define GS_NOCOLMASK_B             0x00004000
#define GS_NOCOLMASK_A             0x00008000
#define GS_COLMASK_RGB             (GS_NOCOLMASK_A)
#define GS_COLMASK_A               (GS_NOCOLMASK_R | GS_NOCOLMASK_G | GS_NOCOLMASK_B)
#define GS_COLMASK_NONE            (GS_NOCOLMASK_R | GS_NOCOLMASK_G | GS_NOCOLMASK_B | GS_NOCOLMASK_A)
#define GS_COLMASK_MASK            GS_COLMASK_NONE
#define GS_COLMASK_SHIFT           12

#define GS_WIREFRAME               0x00010000
#define GS_NODEPTHTEST             0x00020000

#define GS_DEPTHFUNC_LEQUAL        0x00000000
#define GS_DEPTHFUNC_EQUAL         0x00100000
#define GS_DEPTHFUNC_GREAT         0x00200000
#define GS_DEPTHFUNC_LESS          0x00300000
#define GS_DEPTHFUNC_GEQUAL        0x00400000
#define GS_DEPTHFUNC_NOTEQUAL      0x00500000
#define GS_DEPTHFUNC_MASK          0x00700000

#define GS_STENCIL                 0x00800000

#define GS_ALPHATEST_MASK          0xf0000000
#define GS_ALPHATEST_GREATER       0x10000000
#define GS_ALPHATEST_LESS          0x20000000
#define GS_ALPHATEST_GEQUAL        0x40000000
#define GS_ALPHATEST_LEQUAL        0x80000000

#define FORMAT_8_BIT   8
#define FORMAT_24_BIT 24
#define FORMAT_32_BIT 32

// Read FrameBuffer type
enum ERB_Type
{
  eRB_BackBuffer,
  eRB_FrontBuffer,
  eRB_ShadowBuffer
};

//////////////////////////////////////////////////////////////////////
// Import and Export interfaces passed to the renderer
struct SCryRenderInterface
{
  ILog     *ipLog;
  IConsole *ipConsole;
  ITimer   *ipTimer;
  ISystem  *ipSystem;
};

//////////////////////////////////////////////////////////////////////
struct tLmInfo
{
  float           fS[3],fT[3];
  unsigned short  nTextureIdLM;     // general color light map
  unsigned short  nTextureIdLM_LD;  // lights direction texture for DOT3 LM
};

#define VBF_DYNAMIC 1

struct SDispFormat
{
  int m_Width;
  int m_Height;
  int m_BPP;
};

struct SAAFormat
{
  char szDescr[64];
  int nSamples;
  int nQuality;
  int nAPIType;
};

enum ERendStats
{
  eRS_VidBuffer,
  eRS_ShaderPipeline,
  eRS_CurTexturesInfo,
};

// terrain sector texturing info
struct SSectorTextureSet
{
  SSectorTextureSet(unsigned short nT0, unsigned short nT1)
  {
    nTex0 = nT0;
    nTex1 = nT1;
    fTerrainMaxZ=fTerrainMinZ=fTexOffsetX=fTexOffsetY=0; 
    fTexScale=1.f;
  }

  unsigned short nTex0, nTex1;
  float fTexOffsetX,fTexOffsetY,fTexScale,fTerrainMinZ,fTerrainMaxZ;
  Vec3 nodeBox[2];
  Vec3 stencilBox[2];
};

//////////////////////////////////////////////////////////////////////
/*struct IndexedVertexBuffer
{
  PodArray<unsigned short> indices;
  CVertexBuffer * pVertexBuffer;
  int strip_step;
  Vec3 vBoxMin,vBoxMax;
};*/

//////////////////////////////////////////////////////////////////////
//DOC-IGNORE-BEGIN
#include "IShader.h"
//DOC-IGNORE-END

// Flags passed in function FreeResources
#define FRR_SHADERS   1
#define FRR_SHADERTEXTURES 2
#define FRR_TEXTURES  4
#define FRR_SYSTEM    8
#define FRR_RESTORE   0x10
#define FRR_REINITHW  0x20
#define FRR_ALL      -1

// Refresh render resources flags
// Flags passed in function RefreshResources
#define FRO_SHADERS  1
#define FRO_SHADERTEXTURES  2
#define FRO_TEXTURES 4
#define FRO_GEOMETRY 8
#define FRO_FORCERELOAD 0x10

//=============================================================================
// Shaders render target stuff

#define FRT_CLEAR_COLOR   1
#define FRT_CLEAR_DEPTH   2
#define FRT_CLEAR_STENCIL 4
#define FRT_CLEAR (FRT_CLEAR_COLOR | FRT_CLEAR_DEPTH | FRT_CLEAR_STENCIL)
#define FRT_CLEAR_FOGCOLOR 8
#define FRT_CLEAR_IMMEDIATE 16

#define FRT_CAMERA_REFLECTED_WATERPLANE 0x10
#define FRT_CAMERA_REFLECTED_PLANE      0x20
#define FRT_CAMERA_CURRENT              0x40

#define FRT_USE_FRONTCLIPPLANE          0x100
#define FRT_USE_BACKCLIPPLANE           0x200

#define FRT_GENERATE_MIPS               0x800

#define FRT_RENDTYPE_CUROBJECT          0x1000
#define FRT_RENDTYPE_CURSCENE           0x2000
#define FRT_RENDTYPE_RECURSIVECURSCENE  0x4000
#define FRT_RENDTYPE_COPYSCENE          0x8000





#ifdef PS2
typedef struct __HDC* HDC;
typedef struct __HGLRC* HGLRC;
#endif


// Draw3dBBox PrimType params
#define DPRIM_WHIRE_BOX     0
#define DPRIM_LINE          1
#define DPRIM_SOLID_BOX     2
#define DPRIM_WHIRE_SPHERE  3
#define DPRIM_SOLID_SPHERE  4

enum EBufferType
{
  eBT_Static = 0,
  eBT_Dynamic,
};

//! Flags used in DrawText function.
//! @see SDrawTextInfo
enum EDrawTextFlags
{
  //! Text must be fixed pixel size.
  eDrawText_FixedSize = 0x01,
  eDrawText_Center = 0x02,
  eDrawText_Right = 0x04,
};

//////////////////////////////////////////////////////////////////////////
//! This structure used in DrawText method of renderer.
//! It provide all necesarry information of how to render text on screen.
//! @see IRenderer::Draw2dText
struct SDrawTextInfo
{
  //! One of EDrawTextFlags flags.
  //! @see EDrawTextFlags
  int     flags;
  //! Text color, (r,g,b,a) all members must be specified.
  float   color[4];
  float xscale;
  float yscale;

  SDrawTextInfo()
  {
    flags = 0;
    color[0] = color[1] = color[2] = color[3] = 1;
    xscale=1.0f;
		yscale=1.0f;
  }
};

//SLI/CROSSFIRE GPU maximum count
#define MAX_GPU_NUM 4

#define MAX_FRAME_ID_STEP_PER_FRAME 8
#define MAX_VALID_TEXTURE_ID 65535
const int MAX_GSM_LODS_NUM = 8;

const f32 DRAW_NEAREST_MIN = 0.03f;
const f32 DRAW_NEAREST_MAX = 40.0f;

//===================================================================

//////////////////////////////////////////////////////////////////////////
#ifndef EXCLUDE_SCALEFORM_SDK
struct SSF_GlobalDrawParams
{
	enum EFillType
	{
		None,

		SolidColor,
		Texture,
		Glyph,

		GColor,			
		G1Texture,			
		G1TextureColor,	
		G2Texture,			
		G2TextureColor,	
		G3Texture
	};
	EFillType fillType;

	enum EVertexFmt
	{
		Vertex_None,

		Vertex_XY16i,
		Vertex_XY16iC32,
		Vertex_XY16iCF32,
		Vertex_Glyph,
	};
	EVertexFmt vertexFmt;
	const void* pVertexPtr;
	uint32 numVertices;

	enum EIndexFmt
	{
		Index_None,

		Index_16
	};
	EIndexFmt indexFmt;
	const void* pIndexPtr;
	uint32 numIndices;

	const Matrix34* pTransMat;

	enum ETexState
	{
		TS_Clamp				= 0x01,

		TS_FilterLin		= 0x02,
		TS_FilterTriLin	= 0x04
	};

	struct STextureInfo
	{
		int texID;
		uint32 texState;
		Matrix34 texGenMat;
	};
	STextureInfo texture[2];

	ColorF colTransform1st;
	ColorF colTransform2nd;

	bool isMultiplyDarkBlendMode;

	uint32 blendModeStates;
	uint32 renderMaskedStates;

	enum EAlphaBlendOp
	{
		Add,
		Substract,
		RevSubstract,
		Min,
		Max
	};
	EAlphaBlendOp blendOp;

	//////////////////////////////////////////////////////////////////////////
	SSF_GlobalDrawParams()
	{
		Reset();
	}

	//////////////////////////////////////////////////////////////////////////
	void Reset()
	{
		fillType = None;

		vertexFmt = Vertex_None;
		pVertexPtr = 0;
		numVertices = 0;

		indexFmt = Index_None;
		pIndexPtr = 0;
		numIndices = 0;

		pTransMat = 0;

		texture[0].texID = -1;
		texture[0].texState = 0;
		texture[0].texGenMat.SetIdentity();
		texture[1].texID = -1;
		texture[1].texGenMat.SetIdentity();
		texture[1].texState = 0;

		colTransform1st = ColorF(0, 0, 0, 0);
		colTransform2nd = ColorF(0, 0, 0, 0);

		isMultiplyDarkBlendMode = false;

		blendModeStates = 0;
		renderMaskedStates = 0;

		blendOp = Add;
	}
};
#endif // #ifndef EXCLUDE_SCALEFORM_SDK

//////////////////////////////////////////////////////////////////////
struct IRendererEventListener
{
	virtual void OnPostCreateDevice	() = 0;
	virtual void OnPostResetDevice	() = 0;
};

//////////////////////////////////////////////////////////////////////
enum ERenderType
{
	eRT_Undefined,
	eRT_Null,
	eRT_DX9,
	eRT_DX10,
	eRT_Xbox360,
	eRT_PS3	
};

//////////////////////////////////////////////////////////////////////
struct SCustomRenderInitArgs
{
	bool appStartedFromMediaCenter;
};

//////////////////////////////////////////////////////////////////////
struct IRenderer//: public IRendererCallbackServer
{
	virtual void AddListener		(IRendererEventListener *pRendererEventListener) = 0;
	virtual void RemoveListener	(IRendererEventListener *pRendererEventListener) = 0;

	virtual ERenderType GetRenderType() const = 0;

  //! Init the renderer, params are self-explanatory
  virtual WIN_HWND Init(int x,int y,int width,int height,unsigned int cbpp, int zbpp, int sbits, bool fullscreen,WIN_HINSTANCE hinst, WIN_HWND Glhwnd=0, bool bReInit=false, const SCustomRenderInitArgs* pCustomArgs=0)=0;

  virtual bool SetCurrentContext(WIN_HWND hWnd)=0;
  virtual bool CreateContext(WIN_HWND hWnd, bool bAllowFSAA=false)=0;
  virtual bool DeleteContext(WIN_HWND hWnd)=0;

  virtual int GetFeatures()=0;
  virtual void GetVideoMemoryUsageStats( size_t& vidMemUsedThisFrame, size_t& vidMemUsedRecently )=0;
  virtual int GetCurrentNumberOfDrawCalls()=0;
	
  //! Shut down the renderer
  virtual void  ShutDown(bool bReInit=false)=0;
  virtual void  ShutDownFast()=0;

  //! Creates array of all supported video formats (except low resolution formats)
	//! Returns number of formats in memory
  virtual int EnumDisplayFormats(SDispFormat *Formats)=0;
	//! Return all supported by video card video AA formats
	virtual int EnumAAFormats(const SDispFormat &rDispFmt, SAAFormat *Formats)=0;

  //! Changes resolution of the window/device (doen't require to reload the level
  virtual bool  ChangeResolution(int nNewWidth, int nNewHeight, int nNewColDepth, int nNewRefreshHZ, bool bFullScreen)=0;

  //! Shut down the renderer
  virtual void  Release()=0;

  //! Free the allocated resources
  virtual void  FreeResources(int nFlags)=0;

  //! Should be called at the beginning of every frame
  virtual void  BeginFrame()=0;

  //! Should be called at the end of every frame
//  virtual void  Update    (void)=0;
// replaced by RenderDebug();EndFrame();

	//! e.g. r_ShowDynTextures
	virtual void RenderDebug()=0;

	//! Should be called at the end of every frame
	virtual void  EndFrame()=0;

  virtual void  GetViewport(int *x, int *y, int *width, int *height)=0;
  virtual void  SetViewport(int x, int y, int width, int height)=0;
	virtual	void	SetRenderTile(f32 nTilesPosX=0.f, f32 nTilesPosY=0.f, f32 nTilesGridSizeX=1.f, f32 nTilesGridSizeY=1.f)=0;
  virtual void  SetScissor(int x=0, int y=0, int width=0, int height=0)=0;

  virtual EScreenAspectRatio GetScreenAspect(int nWidth, int nHeight)=0;

  //! Make this renderer current renderer.
  //! Only relevant for OpenGL ignored of DX, used by Editors.
  virtual void  MakeCurrent() = 0;

  //! Draw a triangle strip
  virtual void  DrawTriStrip(CVertexBuffer *src, int vert_num=4)=0;

	virtual int  GetDynVBSize( int vertexType = VERTEX_FORMAT_P3F_COL4UB_TEX2F ) = 0;
  virtual void *GetDynVBPtr(int nVerts, int &nOffs, int Pool) = 0;
  virtual void DrawDynVB(int nOffs, int Pool, int nVerts) = 0;
  virtual void DrawDynVB(struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F *pBuf, ushort *pInds, int nVerts, int nInds, int nPrimType) = 0;

  //! Create a vertex buffer
  virtual CVertexBuffer *CreateBuffer(int  vertexcount,int vertexformat, const char *szSource, bool bDynamic=false)=0;
	virtual void CreateBuffer(int size, int vertexformat, CVertexBuffer *buf, int Type, const char *szSource, bool bDynamic=false)=0;

  //! Release a vertex buffer
  virtual void  ReleaseBuffer(CVertexBuffer *bufptr, int nVerts)=0;

  //! Draw a vertex buffer
  virtual void  DrawBuffer(CVertexBuffer *src,SVertexStream *indicies,int numindices, int offsindex, int prmode,int vert_start=0,int vert_stop=0, CRenderChunk *pChunk=NULL)=0;

  //! Update a vertex buffer
  virtual void  UpdateBuffer(CVertexBuffer *dest,const void *src,int vertexcount, bool bUnLock, int nOffs=0, int Type=0)=0;

  virtual void  UnlockBuffer(CVertexBuffer *dest, int Type, int nVerts)=0;

  virtual void  CreateIndexBuffer(SVertexStream *dest,const void *src,int indexcount, int oldIndexCount)=0;
  //! Update indicies
  virtual void  UpdateIndexBuffer(SVertexStream *dest,const void *src, int indexcount, int oldIndexCount, bool bUnLock=true, bool bDynamic=false)=0;
  virtual void  ReleaseIndexBuffer(SVertexStream *dest, int nIndices)=0;

  //! Check for an error in the current frame
  virtual void  CheckError(const char *comment)=0;

  //! Draw a bbox specified by mins/maxs (debug puprposes)
  virtual void  Draw3dBBox(const Vec3 &mins,const Vec3 &maxs, int nPrimType=DPRIM_WHIRE_BOX)=0;

  //! Draw a primitive specified by min/max vertex (for debug purposes)
  //! because of legacy code, the default implementation calls Draw3dBBox.
  //! in the newly changed renderer implementations, this will be the principal function and Draw3dBBox will eventually only draw 3dbboxes
  virtual void  Draw3dPrim(const Vec3 &mins,const Vec3 &maxs, int nPrimType=DPRIM_WHIRE_BOX, const float* fRGBA = NULL)
  {
    // default implementaiton ignores color
    Draw3dBBox(mins, maxs,nPrimType);
  }

  //! Set the renderer camera
  virtual void  SetCamera(const CCamera &cam)=0;

  //! Get the renderer camera
  virtual const CCamera& GetCamera()=0;

  //! Set delta gamma
  virtual bool  SetGammaDelta(const float fGamma)=0;

  //! Restore gamma (reset gamma setting if not in fullscreen mode)
  virtual void  RestoreGamma(void)=0;

  //! Change display size
  virtual bool  ChangeDisplay(unsigned int width,unsigned int height,unsigned int cbpp)=0;

  //! Chenge viewport size
  virtual void  ChangeViewport(unsigned int x,unsigned int y,unsigned int width,unsigned int height)=0;

  //! Save source data to a Tga file (NOTE: Should not be here)
  virtual bool  SaveTga(unsigned char *sourcedata,int sourceformat,int w,int h,const char *filename,bool flip)=0;

  //! Set the current binded texture
  virtual void  SetTexture(int tnum)=0; 

  //! Set the white texture
  virtual void  SetWhiteTexture()=0;

  //! Write a message on the screen
  virtual void  WriteXY( int x,int y, float xscale,float yscale,float r,float g,float b,float a,const char *message, ...) PRINTF_PARAMS(10, 11) =0;
  //! Write a message on the screen with additional flags.
  //! for flags @see
  virtual void  Draw2dText( float posX,float posY,const char *szText,SDrawTextInfo &info )=0;

  //! Draw a 2d image on the screen (Hud etc.)
  virtual void  Draw2dImage (float xpos,float ypos,float w,float h,int texture_id,float s0=0,float t0=0,float s1=1,float t1=1,float angle=0,float r=1,float g=1,float b=1,float a=1,float z=1)=0;

  //! Draw a image using the current matrix
  virtual void DrawImage(float xpos,float ypos,float w,float h,int texture_id,float s0,float t0,float s1,float t1,float r,float g,float b,float a)=0;

	// Draw a image using the current matrix, more flexibale than DrawImage
	// order for s and t: 0=left_top, 1=right_top, 2=right_bottom, 3=left_bottom
  virtual void DrawImageWithUV(float xpos,float ypos,float z,float width,float height,int texture_id,float s[4],float t[4],float r=1,float g=1,float b=1,float a=1)=0;

  //! Set the polygon mode (wireframe, solid)
  virtual int SetPolygonMode(int mode)=0;

  //! Get screen width
  virtual int   GetWidth() = 0;

  //! Get screen height
  virtual int   GetHeight() = 0;

  //! Memory status information
  virtual void GetMemoryUsage(ICrySizer* Sizer)=0;

  //! Get a screenshot and save to a file
  virtual bool ScreenShot(const char *filename=NULL, int width=0)=0;

  //! Get current bpp
  virtual int GetColorBpp()=0;

  //! Get current z-buffer depth
  virtual int GetDepthBpp()=0;

  //! Get current stencil bits
  virtual int GetStencilBpp()=0;

  //! Project to screen
  virtual void ProjectToScreen( float ptx, float pty, float ptz,
                                float *sx, float *sy, float *sz )=0;

  //! Unproject to screen
  virtual int UnProject(float sx, float sy, float sz,
                float *px, float *py, float *pz,
                const float modelMatrix[16],
                const float projMatrix[16],
                const int    viewport[4])=0;

  //! Unproject from screen
  virtual int UnProjectFromScreen( float  sx, float  sy, float  sz,
                           float *px, float *py, float *pz)=0;

  //! for editor
  virtual void  GetModelViewMatrix(float *mat)=0;

  //! for editor
  virtual void  GetProjectionMatrix(float *mat)=0;

  //! for editor
  virtual Vec3 GetUnProject(const Vec3 &WindowCoords,const CCamera &cam)=0;

  virtual bool WriteDDS(byte *dat, int wdt, int hgt, int Size, const char *name, ETEX_Format eF, int NumMips)=0;
  virtual bool WriteTGA(byte *dat, int wdt, int hgt, const char *name, int src_bits_per_pixel, int dest_bits_per_pixel )=0;
  virtual bool WriteJPG(byte *dat, int wdt, int hgt, char *name, int src_bits_per_pixel )=0;

  /////////////////////////////////////////////////////////////////////////////////
  //Replacement functions for Font

  virtual bool FontUploadTexture(class CFBitmap*, ETEX_Format eTF=eTF_A8R8G8B8)=0;
  virtual int  FontCreateTexture(int Width, int Height, byte *pData, ETEX_Format eTF=eTF_A8R8G8B8, bool genMips=false)=0;
  virtual bool FontUpdateTexture(int nTexId, int X, int Y, int USize, int VSize, byte *pData)=0;
  virtual void FontReleaseTexture(class CFBitmap *pBmp)=0;
  virtual void FontSetTexture(class CFBitmap*, int nFilterMode)=0;
  virtual void FontSetTexture(int nTexId, int nFilterMode)=0;
  virtual void FontSetRenderingState(unsigned int nVirtualScreenWidth, unsigned int nVirtualScreenHeight)=0;
  virtual void FontSetBlending(int src, int dst)=0;
  virtual void FontRestoreRenderingState()=0;

  /////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////
  // external interface for shaders
  /////////////////////////////////////////////////////////////////////////////////

  virtual bool EF_PrecacheResource(IShader *pSH, float fDist, float fTimeToReady, int Flags)=0;
  virtual bool EF_PrecacheResource(ITexture *pTP, float fDist, float fTimeToReady, int Flags)=0;
  virtual bool EF_PrecacheResource(IRenderMesh *pPB, float fDist, float fTimeToReady, int Flags)=0;
  virtual bool EF_PrecacheResource(CDLight *pLS, float fDist, float fTimeToReady, int Flags)=0;


	virtual CRenderObject* EF_AddParticlesToScene(const SShaderItem& si, CRenderObject* pRO, IParticleVertexCreator* pPVC, SParticleRenderInfo const& RenInfo, int nAW, bool& canUseGS) = 0;

#ifndef EXCLUDE_GPU_PARTICLE_PHYSICS
	//  member functions to add support for GPU particle systems to renderer.  Function body in CREParticleGPU.cpp
	virtual void  EF_AddGPUParticlesToScene( int32 nGPUParticleIdx, AABB const& bb, const SShaderItem& shaderItem, CRenderObject* pRO, bool nAW, bool canUseGS ) = 0;
#endif 

  // Add 2D polygon to the list
  virtual void EF_AddPolyToScene2D(int Ef, int numPts, const struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F *verts)=0;
  virtual void EF_AddPolyToScene2D(const SShaderItem& si, int nTempl, int numPts, const struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F *verts)=0;
  
	virtual CRenderObject* EF_AddPolygonToScene(const SShaderItem& si, int numPts, const struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F *verts, const SPipTangents *tangs, CRenderObject *obj, uint16 *inds, int ninds, int nAW, bool bMerge = true)=0;
	virtual CRenderObject* EF_AddPolygonToScene(const SShaderItem& si, CRenderObject* obj, int numPts, int ninds, struct_VERTEX_FORMAT_P3F_COL4UB_TEX2F*& verts, SPipTangents*& tangs, uint16*& inds, int nAW, bool bMerge = true)=0;

  /////////////////////////////////////////////////////////////////////////////////
  // Shaders/Shaders management /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  virtual string       *EF_GetShaderNames(int& nNumShaders)=0;
  // Load shader for name (name)
  virtual IShader      *EF_LoadShader (const char *name, int flags=0, uint nMaskGen=0)=0;

  virtual const SShaderProfile &GetShaderProfile(EShaderType eST) const= 0;
  virtual void          SetShaderQuality(EShaderType eST, EShaderQuality eSQ) = 0;

  // Get renderer quality
  virtual ERenderQuality EF_GetRenderQuality() const = 0;
  // Get shader type quality
  virtual EShaderQuality EF_GetShaderQuality(EShaderType eST) = 0;

  // Load shader item for name (name)
  virtual SShaderItem   EF_LoadShaderItem (const char *szName, bool bShare, int flags=0, SInputShaderResources *Res=NULL, uint nMaskGen=0)=0;
  // reload file
  virtual bool          EF_ReloadFile (const char *szFileName)=0;
  // Reinit all shader files (build hash tables)
  virtual void          EF_ReloadShaderFiles (int nCategory)=0;
  // Reload all texturer files
  virtual void          EF_ReloadTextures ()=0;
  // Get texture object by ID
  virtual ITexture     *EF_GetTextureByID(int Id)=0;
  // Loading of the texture for name(nameTex)
  virtual ITexture     *EF_LoadTexture(const char* nameTex, uint flags, byte eTT, float fAmount1=-1.0f, float fAmount2=-1.0f)=0;
  // Load lightmap for name (name)
  virtual int           EF_LoadLightmap (const char *name)=0;
  virtual bool          EF_ScanEnvironmentCM (const char *name, int size, Vec3& Pos)=0;

  // Create splash
  virtual void          EF_AddSplash (Vec3 Pos, eSplashType eST, float fForce, int Id=-1)=0;

  // Hide shader template (exclude from list)
  virtual bool          EF_HideTemplate(const char *name)=0;
  // UnHide shader template (include in list)
  virtual bool          EF_UnhideTemplate(const char *name)=0;
  // UnHide all shader templates (include in list)
  virtual bool          EF_UnhideAllTemplates()=0;

  // Create new RE (RenderElement) of type (edt)
  virtual CRendElement *EF_CreateRE (EDataType edt)=0;

  // Begin using shaders (return first index for allow recursions)
  virtual void EF_StartEf ()=0;

  // Get CRenderObject for RE transformation
  virtual CRenderObject *EF_GetObject (bool bTemp=false, int num=-1)=0;

  // Add shader to the list
  virtual void EF_AddEf (CRendElement *pRE, SShaderItem& pSH, CRenderObject *pObj, int nList=EFSLIST_GENERAL, int nAW=1)=0;

  // Draw all shaded REs in the list
  virtual void EF_EndEf3D (int nFlags)=0;

  // Dynamic lights
	void EF_ClearLightsList() {}; // For FC Compatability.
  virtual bool EF_IsFakeDLight (CDLight *Source)=0;
  virtual void EF_ADDDlight(CDLight *Source)=0;
  virtual bool EF_UpdateDLight(CDLight *pDL)=0;
  virtual void EF_ADDFillLight(const SFillLight & rLight){}

  /////////////////////////////////////////////////////////////////////////////////
  // 2d interface for the shaders
  /////////////////////////////////////////////////////////////////////////////////
  virtual void EF_EndEf2D(bool bSort)=0;

  // Return different common shader parameters (used in ShaderBrowser) CryIndEd.exe
	// Arguments:
	//   Query - e.g. EFQ_GetShaderCombinations
	virtual void *EF_Query(int Query, INT_PTR Param=0)=0;

  // for stats
  virtual int  GetPolyCount()=0;
  virtual void GetPolyCount(int &nPolygons,int &nShadowVolPolys)=0;

  // 3d engine set this color to fog color
  virtual void SetClearColor(const Vec3 & vColor)=0;

  // create/delete RenderMesh object
  virtual IRenderMesh * CreateRenderMesh(bool bDynamic, const char *szType,const char *szSourceName)=0;

  virtual struct IRenderMesh * CreateRenderMeshInitialized(
    void * pVertBuffer, int nVertCount, int nVertFormat,
    ushort* pIndices, int nIndices,
    int nPrimetiveType, const char *szType,const char *szSourceName, EBufferType eBufType = eBT_Static,
    int nMatInfoCount=1, int nClientTextureBindID=0,
    bool (*PrepareBufferCallback)(IRenderMesh *, bool)=NULL,
    void *CustomData=NULL,
    bool bOnlyVideoBuffer=false, bool bPrecache=true, SPipTangents * pTangents=NULL)=0;

  virtual void DeleteRenderMesh(struct IRenderMesh * pLBuffer)=0;
  virtual int GetFrameID(bool bIncludeRecursiveCalls=true)=0;

  virtual void MakeMatrix(const Vec3 & pos, const Vec3 & angles,const Vec3 & scale, Matrix34* mat)=0;

//////////////////////////////////////////////////////////////////////
  /*! Draw an image on the screen as a label text
      @param vPos:  3d position
      @param fSize: size of the image
      @nTextureId:  Texture Id dell'immagine
  */
  virtual void DrawLabelImage(const Vec3 &vPos,float fSize,int nTextureId)=0;

  virtual void DrawLabel(Vec3 pos, float font_size, const char * label_text, ...) PRINTF_PARAMS(4, 5)=0;
  virtual void DrawLabelEx(Vec3 pos, float font_size, float * pfColor, bool bFixedSize, bool bCenter, const char * label_text, ...) PRINTF_PARAMS(7, 8)=0;
  virtual void Draw2dLabel( float x,float y, float font_size, float * pfColor, bool bCenter, const char * label_text, ...) PRINTF_PARAMS(7, 8)=0;

//////////////////////////////////////////////////////////////////////

  virtual float ScaleCoordX(float value)=0;
  virtual float ScaleCoordY(float value)=0;

  virtual void SetState(int State, int AlphaRef=-1)=0;
  virtual void SetCullMode  (int mode=R_CULL_BACK)=0;

  virtual bool EnableFog  (bool enable)=0;
  virtual void SetFog   (float density,float fogstart,float fogend,const float *color,int fogmode)=0;
  virtual void SetFogColor(float * color)=0;

  virtual void SetColorOp(byte eCo, byte eAo, byte eCa, byte eAa)=0;

  virtual void SetTerrainAONodes(PodArray<SSectorTextureSet> * terrainAONodes)=0;

  //////////////////////////////////////////////////////////////////////
  //! interface for auxiliary geometry (for debugging, editor purposes, etc.)
  virtual IRenderAuxGeom* GetIRenderAuxGeom() = 0;
  //////////////////////////////////////////////////////////////////////

  //! NOTE: the following functions will be removed.
  virtual void EnableVSync(bool enable)=0;
  virtual void PushMatrix()=0;
  virtual void PopMatrix()=0;

  virtual void EnableTMU(bool enable)=0;
  virtual void SelectTMU(int tnum)=0;

  virtual unsigned int DownLoadToVideoMemory(unsigned char *data,int w, int h, ETEX_Format eTFSrc, ETEX_Format eTFDst, int nummipmap, bool repeat=true, int filter=FILTER_BILINEAR, int Id=0, char *szCacheName=NULL, int flags=0)=0;
  virtual void UpdateTextureInVideoMemory(uint tnum, unsigned char *newdata,int posx,int posy,int w,int h,ETEX_Format eTFSrc=eTF_R8G8B8)=0; 
	// without header
	// Arguments:
	//   vLumWeight - 0,0,0 if default should be used
  virtual bool DXTCompress( byte *raw_data,int nWidth,int nHeight,ETEX_Format eTF, bool bUseHW, bool bGenMips, int nSrcBytesPerPix, const Vec3 vLumWeight, MIPDXTcallback callback )=0;
  virtual bool DXTDecompress(byte *srcData, const size_t srcFileSize, byte *dstData, int nWidth,int nHeight,int nMips,ETEX_Format eSrcTF, bool bUseHW, int nDstBytesPerPix)=0;
  virtual void RemoveTexture(unsigned int TextureId)=0;

  virtual void TextToScreen(float x, float y, const char * format, ...) PRINTF_PARAMS(4, 5)=0;
  virtual void TextToScreenColor(int x, int y, float r, float g, float b, float a, const char * format, ...) PRINTF_PARAMS(8, 9)=0;
  virtual void ResetToDefault()=0;
  virtual void SetMaterialColor(float r, float g, float b, float a)=0;

  virtual void DrawLine(const Vec3 & vPos1, const Vec3 & vPos2)=0;
  virtual void Graph(byte *g, int x, int y, int wdt, int hgt, int nC, int type, char *text, ColorF& color, float fScale)=0;
  virtual void FlushTextMessages()=0;
  virtual void DrawObjSprites(PodArray<struct SVegetationSpriteInfo> *pList, float fMaxViewDist, CObjManager *pObjMan, float fZoomFactor)=0;
  virtual void GenerateObjSprites(PodArray<struct SVegetationSpriteInfo> *pList, float fMaxViewDist, CObjManager *pObjMan, float fZoomFactor)=0;
  virtual void DrawQuad(const Vec3 &right, const Vec3 &up, const Vec3 &origin,int nFlipMode=0)=0;
  virtual void DrawQuad(float dy,float dx, float dz, float x, float y, float z)=0;
  virtual void ClearBuffer(uint nFlags, ColorF *vColor, float depth = 1.0f)=0;
  virtual void ReadFrameBuffer(unsigned char * pRGB, int nImageX, int nSizeX, int nSizeY, ERB_Type eRBType, bool bRGBA, int nScaledX=-1, int nScaledY=-1)=0;
	virtual void ReadFrameBufferFast(unsigned int* pDstARGBA8, int dstWidth, int dstHeight)=0;
  virtual char* GetVertexProfile(bool bSupportedProfile)=0;
  virtual char* GetPixelProfile(bool bSupportedProfile)=0;
	// Arguments:
	//		fBrightnessMultiplier - >0, object is rendered with 1/fBrightnessMultiplier brightness so it can later be used with tex2d()*fBrightnessMultiplier (needed for HDR sprites in 8bit)
	//		rParams used to pass SH data
	virtual uint RenderOccludersIntoBuffer(const CCamera & viewCam, int nTexSize, PodArray<struct IRenderNode*> & lstOccluders, float * pBuffer)=0;
  virtual void Set2DMode(bool enable, int ortox, int ortoy,float znear=-1e10f,float zfar=1e10f)=0;
  virtual int ScreenToTexture()=0;
  virtual void EnableSwapBuffers(bool bEnable) = 0;
  virtual WIN_HWND GetHWND() = 0;
	virtual WIN_HWND GetCurrentContextHWND() = 0;

  virtual void OnEntityDeleted(struct IRenderNode * pRenderNode)=0;

  virtual int CreateRenderTarget (int nWidth, int nHeight, ETEX_Format eTF)=0;
  virtual bool DestroyRenderTarget (int nHandle)=0;
  virtual bool SetRenderTarget (int nHandle)=0;
  virtual float EF_GetWaterZElevation(float fX, float fY)=0;

  //! Used for pausing timer related stuff (eg: for texture animations, and shader 'time' parameter)
  virtual void PauseTimer(bool bPause)=0;

	// Description:
	//    Creates an Interface to the public params container.
	// Return:
	//    Created IShaderPublicParams interface.
	virtual IShaderPublicParams* CreateShaderPublicParams() = 0;

#ifndef EXCLUDE_SCALEFORM_SDK
	enum ESFMaskOp
	{
		BeginSubmitMask_Clear,
		BeginSubmitMask_Inc,
		BeginSubmitMask_Dec,
		EndSubmitMask,
		DisableMask
	};
	virtual void SF_ConfigMask(ESFMaskOp maskOp, unsigned int stencilRef) = 0;
	virtual void SF_DrawIndexedTriList(int baseVertexIndex, int minVertexIndex, int numVertices, int startIndex, int triangleCount, const SSF_GlobalDrawParams& params) = 0;
	virtual void SF_DrawLineStrip(int baseVertexIndex, int lineCount, const SSF_GlobalDrawParams& params) = 0;
	virtual void SF_DrawGlyphClear(const SSF_GlobalDrawParams& params) = 0;
	virtual void SF_Flush() = 0;
	virtual int SF_CreateTexture(int width, int height, int numMips, unsigned char* pData, ETEX_Format eTF) = 0;
	struct SUpdateRect
	{
		int dstX, dstY;
		int srcX, srcY;
		int width, height;
		
		void Set(int dx, int dy, int sx, int sy, int w, int h)
		{
			dstX = dx; dstY = dy;
			srcX = sx; srcY = sy;
			width = w; height = h;
		}
	};
	virtual bool SF_UpdateTexture(int texId, int mipLevel, int numRects, const SUpdateRect* pRects, unsigned char* pData, size_t pitch, ETEX_Format eTF) = 0;
	virtual void SF_GetMeshMaxSize(int& numVertices, int& numIndices) const = 0;
#endif // #ifndef EXCLUDE_SCALEFORM_SDK

	//////////////////////////////////////////////////////////////////////////
	// Creates an instance of the IVideoPlayer interface.
	virtual IVideoPlayer* CreateVideoPlayerInstance() const = 0;

	virtual void SetCloudShadowTextureId( int id, const Vec3 & vSpeed  ) = 0;
	virtual void SetSkyLightRenderParams( const SSkyLightRenderParams* pSkyLightRenderParams ) = 0;
	virtual uint16 PushFogVolumeContribution( const ColorF& fogVolumeContrib ) = 0;

	virtual int GetMaxTextureSize()=0;

	virtual const char * GetTextureFormatName(ETEX_Format eTF) = 0;
	virtual int GetTextureFormatDataSize(int nWidth, int nHeight, int nDepth, int nMips, ETEX_Format eTF) = 0;

	virtual void SetDefaultMaterials(IMaterial * pDefMat, IMaterial * pTerrainDefMat) = 0;

  virtual bool IsMultiGPUModeActive() const = 0;
};


// Query types for CryInd editor (used in EF_Query() function)
#define EFQ_DeleteMemoryArrayPtr		1
#define EFQ_DeleteMemoryPtr					2
#define EFQ_GetShaderCombinations		3
#define EFQ_SetShaderCombinations		4
#define EFQ_CloseShaderCombinations 5

#define EFQ_ShaderGraphBlocks				6

#define EFQ_RecurseLevel						19
#define EFQ_Pointer2FrameID					20
#define EFQ_DeviceLost							23
#define EFQ_D3DDevice								25
#define EFQ_glReadPixels						26
#define EFQ_LightSource							27

#define EFQ_Alloc_APITextures				28
#define EFQ_Alloc_APIMesh						29
// Memory allocated by meshes in system memory
#define EFQ_Alloc_Mesh_SysMem				30
#define EFQ_Mesh_Count							31

#define EFQ_HDRModeEnabled					32

// Query will return all textures in the renderer,
// First pass 0 as parameter to get number of textures, 
// then pass pointer to the ITexture array big enough to hold that many texture pointers.
#define EFQ_GetAllTextures					33

// Query will return all IRenderMesh objects in the renderer,
// First pass 0 as parameter to get number of meshes, 
// then pass pointer to the IRenderMesh array big enough to hold that many render mesh pointers.
#define EFQ_GetAllMeshes						34
// multigpu (crossfire/sli) is enabled
#define EFQ_MultiGPUEnabled					35
#define EFQ_DrawNearFov							36
#define EFQ_TextureStreamingEnabled	37
#define EFQ_FSAAEnabled							38

//////////////////////////////////////////////////////////////////////

#define STRIPTYPE_NONE           0
#define STRIPTYPE_ONLYLISTS      1
#define STRIPTYPE_SINGLESTRIP    2
#define STRIPTYPE_MULTIPLESTRIPS 3
#define STRIPTYPE_DEFAULT        4

/////////////////////////////////////////////////////////////////////

//DOC-IGNORE-BEGIN
#include "VertexFormats.h"
#include "IRenderMesh.h"
//DOC-IGNORE-END

// this structure used to pass render parameters to Render() functions of IStatObj and ICharInstance
struct SRendParams
{
  SRendParams()
  {
    memset(this, 0, sizeof(SRendParams));
    fAlpha = 1.f;
		fRenderQuality = 1.f;
    nRenderList = EFSLIST_GENERAL;
    nAfterWater = 1;
  }

  SRendParams (const SRendParams& rThat)
  {    
    memcpy (this, &rThat, sizeof(SRendParams));
  }

  //! object transformations
  Matrix34    *pMatrix;
  //! 
  struct SInstancingInfo * pInstInfo;
  //! object previous transformations - motion blur specific
  Matrix34    *pPrevMatrix;
  //! custom sorting offset
  float       fCustomSortOffset;
	//! shader technique to use
  int nTechniqueID;
	//! light mask to specify which light to use on the object
  unsigned int nDLightMask;
  //! amount of bending animations for vegetations
  Vec2       vBending;
  //! list of shadow map casters
  PodArray<ShadowMapFrustum*> * pShadowMapCasters;
  //! object alpha
  float     fAlpha;
  //! force a sort value for render elements
  int       nRenderList;
  //! Ambient color for the object
  ColorF AmbientColor;
  //! distance from camera
  float     fDistance;
  //! CRenderObject flags
  int       dwFObjFlags;
  //! light-map information
	struct SLMData* m_pLMData;
//#ifdef _DEBUG
//	int m_LMDataCount;
//#endif
//  struct RenderLMData * pLightMapInfo;
//  struct IRenderMesh * pLMTCBuffer;									// Object instance specific tex LM texture coords;
	IVisArea*		m_pVisArea;														//	VisArea that contains this object, used for RAM-ambientcube query 
  //! Override material.
  IMaterial *pMaterial;
  //! custom shader params
  //TArray <SShaderParam> * pShaderParams;
  //! CRenderObject custom data
  void * pCCObjCustomData;
  //! Object Id for objects identification in renderer
  struct IRenderNode * pRenderNode;
	//! Quality of shaders rendering
	float fRenderQuality;
	//! skeleton implementation for bendable foliage
	ISkinnable *pFoliage;
	//! weights stream for deform morphs
	IRenderMesh *pWeights;
	//! defines per object AlphaRef value if used by shader
	uint8 nAlphaRef;
	//! motion blur object scale
	uint8   nMotionBlurAmount;
	// material layers bitmask -> which material layers are active
	uint8 nMaterialLayers;
  uint32 nMaterialLayersBlend;
	//! custom offset for sorting by distance
	byte  nAfterWater;
	//! TerrainTexInfo for grass
	struct SSectorTextureSet * pTerrainTexInfo;
	//! Heightmap adaption data
//	Vec4*	m_pHMAGradients;
	uint32 m_HMAData;
	//! storage for LOD transition states
	struct CRNTmpData ** ppRNTmpData;
	//! LOD transition states slot id
	uint8 nLodTransSlotId;
  // Vision modes stuff
  uint32 nVisionParams;
};

#endif //_IRENDERER_H


