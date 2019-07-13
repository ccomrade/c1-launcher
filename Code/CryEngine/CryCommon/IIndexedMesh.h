////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IIndexedMesh.h
//  Version:     v1.00
//  Created:     17/9/2004 by Vladimir.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IIndexedMesh_h__
#define __IIndexedMesh_h__
#pragma once

#include "Cry_Color.h"
#include "StlUtils.h"

// Description:
//    2D Texture coordinates used by CMesh.
struct SMeshTexCoord
{
	float s,t;
	bool operator==( const SMeshTexCoord &other ) { return s == other.s && t == other.t; }
	AUTO_STRUCT_INFO
};

// Description:
//    RGBA Color description structure used by CMesh.
struct SMeshColor
{
	uint8 r,g,b,a;
	AUTO_STRUCT_INFO
};


// Description:
//    Defines a single triangle face in the CMesh topology.
struct SMeshFace
{
	unsigned short v[3]; // indices to vertex, normals and optionally tangent basis arrays
	unsigned short t[3]; // indices to texcoords array
	unsigned char nSubset; // index to mesh subsets array.
	unsigned char dwFlags;
	AUTO_STRUCT_INFO
};

// Description:
//    Mesh tangents (tangent space normals).
struct SMeshTangents
{
	Vec4sf Tangent;
	Vec4sf Binormal;
	AUTO_STRUCT_INFO
};

// Description:
//    for skinning every vertex has 4 bones and 4 weights.
struct SMeshBoneMapping
{
	ColorB boneIDs;		//boneIDs per render-batch
	ColorB weights;   //weights for every bone (four in16)
	AUTO_STRUCT_INFO
};

// Description:
//    for skinning every vertex has 4 bones and 4 weights.
struct SMeshShapeDeformation
{
	Vec3 thin;
	Vec3 fat;
	ColorB index;		//boneIDs per render-batch
	AUTO_STRUCT_INFO
};

// Description:
//    Mesh sh coefficients (compressed as 8 coefficients with 1 byte each)
//		correspond to decompression matrix
struct SMeshSHCoeffs
{
	unsigned char coeffs[8];
	AUTO_STRUCT_INFO
};

// Description:
//    Mesh sh decompression data (one per material(subset))
//		correspond to SMeshSHCoeffs
struct SSHDecompressionMat
{
	float offset0[4];
	float offset1[4];
	float scale0[4];
	float scale1[4];
	AUTO_STRUCT_INFO
};

struct SSHInfo
{
	SSHDecompressionMat *pDecompressions;	// one decompression matrix per material(subset)
	uint32 nDecompressionCount;
	SMeshSHCoeffs *pSHCoeffs;
	SSHInfo() : pDecompressions(NULL), nDecompressionCount(0), pSHCoeffs(NULL){}
	~SSHInfo()
	{
		free(pSHCoeffs);	//don't like this free and new mixery, but all other buffers behave that way
		pSHCoeffs = NULL;
		delete [] pDecompressions;
	}
};


// Subset of mesh is a continous range of vertices and indices that share same material.
struct SMeshSubset
{
	Vec3 vCenter;
	float fRadius;

	int nFirstIndexId;
	int nNumIndices;

	int nFirstVertId;
	int nNumVerts;

	int nMatID; // Material Sub-object id.
	int nMatFlags; // Special Material flags.
	int nPhysicalizeType; // Type of physicalization for this subset.

	PodArray<uint16> m_arrGlobalBonesPerSubset; //array with global-boneIDs used for this subset

	SMeshSubset() : vCenter(0,0,0),fRadius(0),nFirstIndexId(0),nNumIndices(0),nFirstVertId(0),nNumVerts(0),nMatID(0),nMatFlags(0),nPhysicalizeType(0) {}

  int GetMemoryUsage() const { return m_arrGlobalBonesPerSubset.GetDataSize(); }
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    General purpose mesh class.
//////////////////////////////////////////////////////////////////////////
class CMesh
{
public:
	enum EStream
	{
		POSITIONS,
		NORMALS,
		FACES,
		TEXCOORDS,
		COLORS_0,
		COLORS_1,
		INDICES,
		TANGENTS,
		SHCOEFFS,
		SHAPEDEFORMATION,
		BONEMAPPING,
		FACENORMALS,
    VERT_MATS,
		LAST_STREAM,
	};

	SMeshFace* m_pFaces;		//faces as they are comming from max

	// geometry data
	uint16* m_pIndices;  //indices used for the final render-mesh
	Vec3* m_pPositions;  //splitted positions for the final render-mesh
	Vec3* m_pNorms;
	Vec3* m_pFaceNorms;
	SMeshTangents* m_pTangents;
	SMeshTexCoord* m_pTexCoord;
	SMeshColor* m_pColor0;
	SMeshColor* m_pColor1;
	SSHInfo *m_pSHInfo; //sh stuff
  int *m_pVertMats;

	SMeshShapeDeformation* m_pShapeDeformation;  //additional stream for thin/fat morphing
	SMeshBoneMapping* m_pBoneMapping;  //bone-mapping for the final render-mesh (relative to subsets)

	int m_numFaces;
	int m_numVertices;
	int m_nCoorCount;					//number of texture coordinates in m_pTexCoord array
	int m_nIndexCount;



	// For internal use.
	int m_streamSize[LAST_STREAM];

	// Bounding box.
	AABB m_bbox;

	// Array of mesh subsets.
	DynArray<SMeshSubset> m_subsets;

  static PodArray<CMesh*> * GetCounter()
  {
    static PodArray<CMesh*> arrAll;
    return &arrAll;
  }

  size_t GetMemoryUsage()
  {
    int nSize = sizeof(*this);

    nSize += sizeofVector(m_subsets);

    DynArray<SMeshSubset>::const_iterator it, end=m_subsets.end();

    for(it=m_subsets.begin();it!=end;++it)
    {
      const SMeshSubset &ref = *it;
      nSize += ref.GetMemoryUsage();
    }

    for (int stream = 0; stream < LAST_STREAM; stream++)
    {
      void *pStream;
      int nElementSize = 0;
      GetStreamInfo( stream,pStream,nElementSize );
      nSize += m_streamSize[stream]*nElementSize;
    }

    return nSize;
  }

	//////////////////////////////////////////////////////////////////////////
	CMesh()
	{
		m_pFaces = NULL;
		m_pIndices = NULL;
		m_pPositions = NULL;
		m_pNorms = NULL;
		m_pFaceNorms = NULL;
		m_pTexCoord = NULL;
		m_pTangents = NULL;
		m_pColor0 = NULL;
		m_pColor1 = NULL;
		m_pSHInfo = NULL;
    m_pVertMats = NULL;
		
		m_pShapeDeformation = NULL;
		m_pBoneMapping = NULL; 

		m_numFaces = 0;
		m_numVertices = 0;
		m_nCoorCount = 0;
		m_nIndexCount = 0;

		memset( m_streamSize,0,sizeof(m_streamSize) );

    GetCounter()->Add(this);
  }

	~CMesh()
	{
    GetCounter()->Delete(this);

		free(m_pFaces);
		free(m_pIndices);
		free(m_pPositions);
    free(m_pVertMats);
		free(m_pNorms);
		free(m_pFaceNorms);
		free(m_pTangents);
		free(m_pTexCoord);
		free(m_pColor0);
		free(m_pColor1);

		free(m_pShapeDeformation); 
		//free(m_pBoneMapping); 
		delete[] m_pBoneMapping;

		delete m_pSHInfo;
	}

	int GetFacesCount() { return m_numFaces; }
	int GetVertexCount() { return m_numVertices; }
	int GetTexCoordsCount() { return m_nCoorCount; }
	int GetSubSetCount() { return m_subsets.size(); }
	int GetIndexCount() { return m_nIndexCount; }

	void SetFacesCount(int nNewCount)
	{
		if (m_numFaces!=nNewCount || m_numFaces==0)
		{
			ReallocStream( FACES,nNewCount );
			m_numFaces = nNewCount;
		}
	}

	void SetVertexCount(int nNewCount)
	{
		if (m_numVertices!=nNewCount || m_numVertices==0)
		{
			ReallocStream( POSITIONS,nNewCount );
			ReallocStream( NORMALS,nNewCount );

      if(m_pColor0)
        ReallocStream( COLORS_0,nNewCount );

      if(m_pColor1)
        ReallocStream( COLORS_1,nNewCount );

      if(m_pVertMats)
        ReallocStream( VERT_MATS,nNewCount );

      m_numVertices = nNewCount;
		}
	}

	void SetTexCoordsCount(int nNewCount)
	{
		if (m_nCoorCount != nNewCount || m_nCoorCount == 0)
		{
			ReallocStream( TEXCOORDS,nNewCount );
			m_nCoorCount = nNewCount;
		}
	}

	void SetTexCoordsAndTangentsCount(int nNewCount)
	{
		if (m_nCoorCount != nNewCount || m_nCoorCount == 0)
		{
			ReallocStream( TEXCOORDS,nNewCount );
			ReallocStream( TANGENTS,nNewCount );
			m_nCoorCount = nNewCount;
		}
	}

	void SetIndexCount(int nNewCount)
	{
		if (m_nIndexCount != nNewCount || m_nIndexCount == 0)
		{
			ReallocStream( INDICES,nNewCount );
			m_nIndexCount = nNewCount;
		}
	}

	// Set stream size.
	void GetStreamInfo( int stream,void* &pStream,int &nElementSize )
	{
		pStream = 0;
		nElementSize = 0;
		assert( stream >= 0 && stream < LAST_STREAM );
		switch (stream)
		{
		case POSITIONS:
			pStream = m_pPositions;
			nElementSize = sizeof(Vec3);
			break;
    case VERT_MATS:
      pStream = m_pVertMats;
      nElementSize = sizeof(int);
      break;
		case NORMALS:
			pStream = m_pNorms;
			nElementSize = sizeof(Vec3);
			break;
		case FACENORMALS:
			pStream = m_pFaceNorms;
			nElementSize = sizeof(Vec3);
			break;
		case FACES:
			pStream = m_pFaces;
			nElementSize = sizeof(SMeshFace);
			break;
		case TEXCOORDS:
			pStream = m_pTexCoord;
			nElementSize = sizeof(SMeshTexCoord);
			break;
		case COLORS_0:
			pStream = m_pColor0;
			nElementSize = sizeof(SMeshColor);
			break;
		case COLORS_1:
			pStream = m_pColor1;
			nElementSize = sizeof(SMeshColor);
			break;
		case INDICES:
			pStream = m_pIndices;
			nElementSize = sizeof(uint16);
			break;
		case TANGENTS:
			pStream = m_pTangents;
			nElementSize = sizeof(SMeshTangents);
			break;
		case SHCOEFFS:
			pStream = m_pSHInfo?m_pSHInfo->pSHCoeffs : NULL;
			nElementSize = sizeof(SMeshSHCoeffs);
			break;
		case SHAPEDEFORMATION:
			pStream = m_pShapeDeformation;
			nElementSize = sizeof(SMeshShapeDeformation);
			break;
		case BONEMAPPING:
			pStream = m_pBoneMapping;
			nElementSize = sizeof(SMeshBoneMapping);
			break;
		default:
			assert(0); // unknown stream.
		}
	}

	// Set stream size.
	void ReallocStream( int stream,int nNewCount )
	{
		assert( stream >= 0 && stream < LAST_STREAM );
		if (stream < 0 || stream >= LAST_STREAM)
			return;
		if (m_streamSize[stream] == nNewCount)
			return; // Stream already have required size.
		void* pStream = 0;
		int nElementSize = 0;
		GetStreamInfo( stream,pStream,nElementSize );
		pStream = ReAllocElements(pStream,nNewCount,nElementSize);
		m_streamSize[stream] = nNewCount;
		switch (stream)
		{
		case POSITIONS:
			m_pPositions = (Vec3*)pStream;
			m_numVertices = nNewCount;
			break;
    case VERT_MATS:
      m_pVertMats = (int*)pStream;
      m_numVertices = nNewCount;
      break;
		case NORMALS:
			m_pNorms = (Vec3*)pStream;
			m_numVertices = nNewCount;
			break;
		case FACENORMALS:
			m_pFaceNorms = (Vec3*)pStream;
			break;
		case FACES:
			m_pFaces = (SMeshFace*)pStream;
			m_numFaces = nNewCount;
			break;
		case TEXCOORDS:
			m_pTexCoord = (SMeshTexCoord*)pStream;
			m_nCoorCount = nNewCount;
			break;
		case COLORS_0:
			m_pColor0 = (SMeshColor*)pStream;
			break;
		case COLORS_1:
			m_pColor1 = (SMeshColor*)pStream;
			break;
		case INDICES:
			m_pIndices = (uint16*)pStream;
			m_nIndexCount = nNewCount;
			break;
		case TANGENTS:
			m_pTangents = (SMeshTangents*)pStream;
			break;
		case SHCOEFFS:
			if(!m_pSHInfo)
				m_pSHInfo = new SSHInfo;
			m_pSHInfo->pSHCoeffs = (SMeshSHCoeffs*)pStream;
			break;
		case SHAPEDEFORMATION:
			m_pShapeDeformation = (SMeshShapeDeformation*)pStream;
			break;
		case BONEMAPPING:
			m_pBoneMapping = (SMeshBoneMapping*)pStream;
			break;
		default:
			assert(0); // unknown stream.
		}
		m_streamSize[stream] = nNewCount;
	}

	// Copy mesh from source mesh.
	void Copy( CMesh &mesh )
	{
		for (int stream = 0; stream < LAST_STREAM; stream++)
		{
			if (mesh.m_streamSize[stream])
			{
				ReallocStream( stream,mesh.m_streamSize[stream] );
				void* pSrcStream = 0;
				void* pTrgStream = 0;
				int nElementSize = 0;
				mesh.GetStreamInfo( stream,pSrcStream,nElementSize );
				GetStreamInfo( stream,pTrgStream,nElementSize );
				if (pSrcStream && pTrgStream)
					memcpy( pTrgStream,pSrcStream,m_streamSize[stream]*nElementSize );
			}
		}
		m_bbox = mesh.m_bbox;
		m_subsets = mesh.m_subsets;
	}
	bool CompareStreams( CMesh &mesh )
	{
		for (int stream = 0; stream < LAST_STREAM; stream++)
		{
			if (m_streamSize[stream] != mesh.m_streamSize[stream])
				return false;

			if (m_streamSize[stream])
			{
				void* pStream1 = 0;
				void* pStream2 = 0;
				int nElementSize = 0;
				GetStreamInfo( stream,pStream1,nElementSize );
				mesh.GetStreamInfo( stream,pStream2,nElementSize );
				if ((pStream1 && !pStream2) || (!pStream1 && pStream2))
					return false;
				if (pStream1 && pStream2)
				{
					if (memcmp( (char*)pStream1,(char*)pStream2,m_streamSize[stream]*nElementSize ) != 0)
						return false;
				}
			}
		}
		return true;
	}
	// Add streams from source mesh to the end of existing streams.
	void Append( CMesh &mesh )
	{
		int nOldVertexCount = m_numVertices;
		int nOldTVerts = m_nCoorCount;
		int nOldFaceCount = m_numFaces;
		int nOldIndexCount = m_nIndexCount;
		for (int stream = 0; stream < LAST_STREAM; stream++)
		{
			if (mesh.m_streamSize[stream])
			{
				int nOldOffset = m_streamSize[stream];
				ReallocStream( stream,m_streamSize[stream]+mesh.m_streamSize[stream] );
				void* pSrcStream = 0;
				void* pTrgStream = 0;
				int nElementSize = 0;
				mesh.GetStreamInfo( stream,pSrcStream,nElementSize );
				GetStreamInfo( stream,pTrgStream,nElementSize );
				if (pSrcStream && pTrgStream)
					memcpy( (char*)pTrgStream+nOldOffset*nElementSize,pSrcStream,mesh.m_streamSize[stream]*nElementSize );
			}
		}
		// Remap faces.
		for (int i=nOldFaceCount; i<m_numFaces; i++)
		{
			int nSrcIndex = i - nOldFaceCount;
			m_pFaces[i].v[0] = nOldVertexCount + mesh.m_pFaces[nSrcIndex].v[0];
			m_pFaces[i].v[1] = nOldVertexCount + mesh.m_pFaces[nSrcIndex].v[1];
			m_pFaces[i].v[2] = nOldVertexCount + mesh.m_pFaces[nSrcIndex].v[2];
			m_pFaces[i].t[0] = nOldTVerts + mesh.m_pFaces[nSrcIndex].t[0];
			m_pFaces[i].t[1] = nOldTVerts + mesh.m_pFaces[nSrcIndex].t[1];
			m_pFaces[i].t[2] = nOldTVerts + mesh.m_pFaces[nSrcIndex].t[2];
		}
		// Remap indices.
		for (int i = nOldIndexCount; i < m_nIndexCount; i++)
		{
			m_pIndices[i] = nOldVertexCount + mesh.m_pIndices[i-nOldIndexCount];
		}
		m_bbox.Add( mesh.m_bbox.min );
		m_bbox.Add( mesh.m_bbox.max );
	}

	void RemoveRangeFromStream( int stream,int nFirst,int nCount )
	{
		int nStreamCount = m_streamSize[stream];
		int nElementSize;
		void *pStream = 0;
		GetStreamInfo(stream,pStream,nElementSize);
		if (nFirst > nStreamCount || nStreamCount == 0 || pStream == 0)
			return;
		if (nFirst+nCount > nStreamCount)
			nCount = nStreamCount-nFirst;
		if (nCount <= 0)
			return;

		if (nFirst+nCount < nStreamCount)
		{
			char *pRangeFirst = (char*)pStream + nFirst*nElementSize;
			char *pRangeEnd = (char*)pStream + nFirst*nElementSize + nCount*nElementSize;
			memmove( pRangeFirst,pRangeEnd,nCount*nElementSize );
		}
		ReallocStream(stream,nStreamCount-nCount);
	}

	void Debug() const
	{
		DynArray<SMeshSubset>::const_iterator it, end=m_subsets.end();

		for(it=m_subsets.begin();it!=end;++it)
		{
			const SMeshSubset &ref = *it;

			assert(m_pIndices);
			for(int32 dwI=ref.nFirstIndexId; dwI<ref.nFirstIndexId+ref.nNumIndices; ++dwI)
			{
				uint16 val = m_pIndices[dwI];

				if(val<ref.nFirstVertId || val>=ref.nFirstVertId+ref.nNumVerts)
				{
					char str[80];

					sprintf(str,"CMesh::Debug ERROR %d. %d [%d;%d[\n",dwI,val,ref.nFirstVertId,ref.nFirstVertId+ref.nNumVerts);
//					OutputDebugString(str);		
					assert(0);
				}
			}
		}
	}


private:
	void *ReAllocElements(void *old_ptr,int new_elem_num, int size_of_element)
	{
		return realloc(old_ptr,new_elem_num*size_of_element);
	}
};

// Description:
//    Editable mesh interface.
//    IndexedMesh can be created directly or loaded from CGF file, before rendering it is converted into IRenderMesh.
//    IStatObj is used to host IIndexedMesh, and corresponding IRenderMesh.
struct IIndexedMesh
{
	/*! Structure used for read-only access to mesh data
	Used by GetMesh() function */
	struct SMeshDesc
	{
		SMeshFace * m_pFaces; // pointer to array of faces
		Vec3 * m_pVerts; // pointer to array of vertices
		Vec3 * m_pNorms; // pointer to array of normals
		struct SMeshColor* m_pColor; // pointer to array of vertex colors
		struct SMeshTexCoord * m_pTexCoord; // pointer to array of texture coordinates
		uint16* m_pIndices; // Index array.
		int m_nFaceCount; // number of elements m_pFaces array
		int m_nVertCount; // number of elements in m_pVerts, m_pNorms and m_pColor arrays
		int m_nCoorCount; // number of elements in m_pTexCoord array
		int m_nIndexCount; // number of elements in m_pTexCoord array
	};

	// Release indexed mesh.
	virtual void Release() = 0;

	//! Gives read-only access to mesh data
	virtual void GetMesh(SMeshDesc & MeshDesc) = 0;

	virtual CMesh* GetMesh() = 0;
	virtual void SetMesh( CMesh &mesh ) = 0;

	/*! Reallocates required number of faces,
	Calling this function invalidates SMeshDesc pointers */
	virtual void SetFacesCount(int nNewCount) = 0;

	//! Return number of allocated faces
	virtual int GetFacesCount() = 0;

	/*! Reallocates required number of vertices, normals and colors
	calling this function invalidates SMeshDesc pointers */
	virtual void SetVertexCount(int nNewCount) = 0;

	/*! Reallocates number of colors, 
	calling this function invalidates SMeshDesc pointers */
	virtual void SetColorsCount(int nNewCount) = 0;

	//! Return number of allocated vertices, normals and colors
	virtual int GetVertexCount() = 0;

	/*! Reallocates required number of texture coordinates
	Calling this function invalidates SMeshDesc pointers */
	virtual void SetTexCoordsCount(int nNewCount) = 0;

	/*! Reallocates required number of texture coordinates and tangents
	Calling this function invalidates SMeshDesc pointers */
	virtual void SetTexCoordsAndTangentsCount(int nNewCount) = 0;

	//! Return number of allocated texture coordinates
	virtual int GetTexCoordsCount() = 0;

	// Set number of indices in the mesh.
	virtual void SetIndexCount( int nNewCount ) = 0;
	// Get number of indices in the mesh.
	virtual int GetIndexCount() = 0;

	// allocates m_pBoneMapping in CMesh
	virtual void AllocateBoneMapping() = 0;

	// allocates m_pSHInfo and pDecompressions,pSHCoeffs in it
	virtual void AllocateSHData() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Subset access.
	//////////////////////////////////////////////////////////////////////////
	virtual void SetSubSetCount( int nSubsets ) = 0;
	virtual int GetSubSetCount() = 0;
	virtual SMeshSubset& GetSubSet( int nIndex ) = 0;
	virtual void SetSubsetBoneIds( int idx, const PodArray<uint16> &boneIds ) = 0;

	// Description:
	//    Mark indexed mesh as being updated.
	//    Call it after modifying vertex or face data of the indexed mesh.
	//    On the next rendering IStatObj will convert it to the new IRenderMesh.
	virtual void Invalidate() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Mesh bounding box.
	//////////////////////////////////////////////////////////////////////////
	virtual void SetBBox( const AABB &box ) = 0;
	virtual AABB GetBBox() = 0;
	virtual void CalcBBox() = 0;

	// Description:
	//    Optimizes mesh.
	virtual void Optimize( const char * szComment=NULL, std::vector<uint16> *pVertexRemapping=NULL,std::vector<uint16> *pIndexRemapping=NULL ) = 0;
};

#endif // __IIndexedMesh_h__
