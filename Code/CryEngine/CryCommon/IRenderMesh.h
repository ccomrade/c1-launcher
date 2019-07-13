// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _RenderMesh_H_
#define _RenderMesh_H_

class CMesh;
struct IIndexedMesh;
struct SMRendTexVert;
struct UCol;
struct GeomInfo;

struct ExtSkinVertex;
struct TFace;
struct SMeshSubset;
struct GeomQuery;

//! structure for tangent basises storing
struct TangData
{
  Vec3 tangent;
  Vec3 binormal;      
  Vec3 tnormal;     
};

#define FSM_MORPH_TARGETS 1
#define FSM_NO_TANGENTS   2
#define FSM_VOXELS        4
#define FSM_CREATE_DEVICE_MESH 8


// Invalidate video buffer flags
#define FMINV_STREAM      1
#define FMINV_STREAM_MASK ((1<<VSF_NUM)-1)
#define FMINV_INDICES     0x100
#define FMINV_ALL        -1

#define FSL_READ         1
#define FSL_WRITE        2

struct IRenderMesh
{
	enum EMemoryUsageArgument
	{
		MEM_USAGE_COMBINED,
		MEM_USAGE_ONLY_SYSTEM,
		MEM_USAGE_ONLY_VIDEO,
	};

	//////////////////////////////////////////////////////////////////////////
	// Reference Counting.
	virtual void AddRef() = 0;
	virtual int Release() = 0;
	//////////////////////////////////////////////////////////////////////////

	// Returns type name given to the render mesh on creation time.
	virtual const char* GetTypeName() = 0;
	// Returns the name of the source given to the render mesh on creation time.
	virtual const char* GetSourceName() = 0;

	// Create render buffers from render mesh.
	virtual void SetMesh( CMesh &mesh,int nSecColorsSetOffset=0, uint32 flags=0, Vec3 *pBSStreamTemp=0) = 0;	
	// Creates an indexed mesh from this render mesh (accepts an optional pointer to an IIndexedMesh object that should be used)
	virtual IIndexedMesh* GetIndexedMesh(IIndexedMesh *pIdxMesh=0) = 0;

	virtual void SetVertCount( int nCount ) = 0;
	virtual int GetVertCount() = 0;

	virtual unsigned short *GetIndices(int * pIndicesCount) = 0;

	virtual byte *GetStridedPosPtr(int& Stride, int Id=0, bool bWrite=false) = 0;
	virtual byte *GetStridedUVPtr(int & Stride, int Id=0, bool bWrite=false) = 0;
	virtual byte *GetStridedColorPtr(int & Stride, int Id=0, bool bWrite=false) = 0;

	virtual byte *GetStridedHWSkinPtr(int& Stride, int Id=0, bool bWrite=false) = 0;
	virtual byte *GetStridedShapePtr(int& Stride, int Id=0, bool bWrite=false) = 0;


	virtual byte *GetStridedBinormalPtr(int& Stride, int Id=0, bool bWrite=false) = 0;
	virtual byte *GetStridedTangentPtr(int& Stride, int Id=0, bool bWrite=false) = 0;

  virtual void UnlockStream(int nStream) = 0;

	virtual IRenderMesh *GetVertexContainer() = 0;
	virtual PodArray<CRenderChunk> *	GetChunks() = 0;
  virtual PodArray<CRenderChunk> *	GetChunksSkinned() = 0;
  virtual void CreateChunksSkinned() = 0;
	
	virtual void SetSysIndicesCount( uint32 nIndices ) = 0;
	virtual uint32 GetSysIndicesCount() = 0;

	virtual CVertexBuffer * GetSysVertBuffer() = 0;
	virtual void AddRenderElements(CRenderObject * pObj=0, int nSortId=EFSLIST_GENERAL, int nAW=1, IMaterial * pIMatInfo=NULL, int nTechniqueID=0) = 0;
	virtual void SetRECustomData(float * pfCustomData, float fFogScale=0, float fAlpha=1) = 0;
	
	virtual void SetChunk(  int nIndex,CRenderChunk &chunk,bool bForceInitChunk=false ) = 0;
	virtual void SetChunk( IMaterial *pNewMat,int nFirstVertId,int nVertCount,int nFirstIndexId,int nIndexCount,int nMatID = 0,bool bForceInitChunk=false ) = 0;

	virtual void * GetCustomData() = 0;
	virtual CVertexBuffer * GetVideoVertBuffer() = 0;
	virtual void UpdateSysIndices(const ushort *pNewInds, int nInds) = 0;
	virtual void UpdateSysVertices(void * pNewVertices, int nNewVerticesCount, int nVertFormat, int nStream) = 0;
  virtual void UpdateVidVertices(void * pNewVertices, int nNewVerticesCount, int nStream, bool bReleaseSys)=0;
  virtual bool CreateSysVertices(int nVerts=0, int VertFormat=-1, int nStream=VSF_GENERAL)=0;

	virtual void SetMaterial( IMaterial * pNewMat, int nCustomTID = 0 ) = 0;
	virtual IMaterial* GetMaterial() = 0;
	virtual void SetBBox(const Vec3 & vBoxMin, const Vec3 & vBoxMax) = 0;
	virtual void GetBBox(Vec3 & vBoxMin, Vec3 & vBoxMax) = 0;
  virtual void UpdateBBoxFromMesh() = 0;
	virtual void InvalidateVideoBuffer(int flags=-1) = 0;
	virtual void AddRE(CRenderObject * pObj, IShader * pEf, int nList=EFSLIST_GENERAL, int nAW=1) = 0;
	virtual uint GetUpdateFrame() = 0;
	virtual void SetVertexContainer(IRenderMesh *pBuf) = 0;
	virtual void SetUpdateFrame(uint nUpdateFrame) = 0;
	virtual bool IsMaterialsWasCreatedInRenderer() = 0;
	virtual int	GetPrimetiveType() = 0;
	virtual int GetVertexFormat() = 0;
//	virtual bool CreateFromVertexBufferSource( struct VertexBufferSource* pSource ) = 0;
	virtual void Render(const struct SRendParams & rParams, CRenderObject * pObj, IMaterial *pMaterial, bool bSkinned=false) = 0;
	virtual void RenderDebugLightPass(const Matrix34 & mat, const int nLightMask, const float fAlpha, const int nMaxLightNumber) = 0;
	virtual uint * GetPhysVertexMap() = 0;
	virtual void SetPhysVertexMap(uint * pVtxMap) = 0;
	virtual bool IsEmpty() = 0;
	virtual void CopyTo(IRenderMesh *pDst, bool bUseSysBuf, int nAppendVtx=0) = 0;
	virtual void SetSkinningDataVegetation(struct SMeshBoneMapping *pBoneMapping, Vec3 *pBSStreamTemp=0) = 0;
	virtual void SetSkinningDataCharacter(CMesh& mesh, struct SMeshBoneMapping *pBoneMapping, Vec3 *pBSStreamTemp=0) = 0;
	virtual void CreateVidVertices(int nVerts, int VertFormat, int nStream=VSF_GENERAL) = 0;
	virtual void SetMorphBuddy(IRenderMesh *pMorph) = 0;
	virtual IRenderMesh *GetMorphBuddy() = 0;
	virtual IRenderMesh *GenerateMorphWeights() = 0; 
	virtual float ComputeExtent(GeomQuery& geo, EGeomForm eForm)=0;
	virtual void GetRandomPos(RandomPos& ran, GeomQuery& geo, EGeomForm eForm)=0;
	
	// Debug draw this render mesh.
	virtual void DebugDraw( const Matrix34 &mat,int nFlags,uint32 nVisibleChunksMask=-1,ColorB *pColor=0 ) = 0;

	// Returns mesh memory usage and add it to the CrySizer (if not NULL).
	// Arguments:
	//     pSizer - Sizer interface, can be NULL if caller only want to calculate size
	//     nType - see EMemoryUsageArgument
	virtual int GetMemoryUsage( ICrySizer *pSizer,EMemoryUsageArgument nType ) = 0;

	// Get allocated only in video memory or only in system memory.
	virtual int GetAllocatedBytes( bool bVideoMem ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Description:
	//    Return estimated number of device memory used by the render mesh textures.
	// Arguments:
	//    pMaterial - Material used to render this RenderMesh, if NULL internal RenderMesh material will be used for estimation.
	// Return:
	//    Size of texture memory in bytes.
	virtual int GetTextureMemoryUsage( IMaterial *pMaterial,ICrySizer *pSizer=NULL ) = 0;
	virtual float GetAverageTrisNumPerChunk(IMaterial * pMat) = 0;
};

#endif // _RenderMesh_H_
