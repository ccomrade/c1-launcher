////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   CGFContent.h
//  Version:     v1.00
//  Created:     7/11/2004 by Timur.
//  Compilers:   Visual Studio.NET
//  Description: Describe contents on CGF file.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CGFContent_h__
#define __CGFContent_h__
#pragma once

#include "IIndexedMesh.h"
#include "CryHeaders.h"
#include <CryVertexBinding.h>
#include <Cry_Color.h>
#include <CryArray.h>
 
struct CMaterialCGF;

#define MAX_BONES_IN_BATCH (50)

//////////////////////////////////////////////////////////////////////////
// This structure represents CGF node.
//////////////////////////////////////////////////////////////////////////
struct CNodeCGF : public _reference_target_t
{
	enum ENodeType
	{
		NODE_MESH,
		NODE_LIGHT,
		NODE_HELPER,
	};

	ENodeType type;
	string    name;
	string    properties;
	Matrix34  localTM; // Local space transformation matrix.
	Matrix34  worldTM; // World space transformation matrix.
	Vec3      pos;		 // pos component of matrix
	CryQuat   rot;	   // rotation component of matrix
	Vec3      scl;		 // scale component of matrix
	CNodeCGF* pParent; // Pointer to parent node.
	CNodeCGF* pSharedMesh; // Not NULL if this node is sharing mesh and physics from referenced Node.
	CMesh*     pMesh;  // Pointer to mesh loaded for this node. (Only when type == NODE_MESH)

	HelperTypes helperType; // Only relevant if type==NODE_HELPER
	Vec3        helperSize; // Only relevant if type==NODE_HELPER

	CMaterialCGF *pMaterial; // Material node.

	// Physical data of the node with mesh.
	int nPhysicalizeFlags;
	std::vector<char> physicalGeomData[4]; // (Only when type == NODE_MESH)

	//////////////////////////////////////////////////////////////////////////
	// Used internally.
	int nChunkId; // Chunk id as loaded from CGF.
	int nParentChunkId; // Chunk id of parent Node.
	int nObjectChunkId; // Chunk id of the corresponding mesh.
	int	pos_cont_id;	// position controller chunk id
	int	rot_cont_id;	// rotation controller chunk id
	int scl_cont_id;	// scale controller chunk id  
	//////////////////////////////////////////////////////////////////////////

	// True if have not identity world matrix.
	bool bIdentityMatrix;
	// True when this not is invisible physics proxy.
	bool bPhysicsProxy;
	// True when mapFaceToFace0 is used (usually for nodes with deformation morphs)
	bool bHasFaceMap;
	std::vector<uint16> mapFaceToFace0;	// for each face, stores original (i.e. before compacting and stripification) face index

	//////////////////////////////////////////////////////////////////////////
	// Constructor.
	//////////////////////////////////////////////////////////////////////////
	CNodeCGF()
	{
		type = NODE_MESH;
		localTM.SetIdentity();
		worldTM.SetIdentity();
		pos.Set(0,0,0);
		rot.SetIdentity();
		scl.Set(1.0f,1.0f,1.0f);
		pParent = 0;
		pSharedMesh = 0;
		pMesh = 0;
		pMaterial = 0;
		helperType = HP_POINT;
		helperSize.Set(0,0,0);
		nPhysicalizeFlags = 0;
		nChunkId = 0;
		nParentChunkId = 0;
		nObjectChunkId = 0;
		pos_cont_id = rot_cont_id = scl_cont_id = 0;
		bIdentityMatrix = true;
		bPhysicsProxy = false;
		bHasFaceMap = false;
	}
	~CNodeCGF()
	{
		if (!pSharedMesh)
			delete pMesh;
	}
};


//////////////////////////////////////////////////////////////////////////
// structures for skinning 
//////////////////////////////////////////////////////////////////////////

struct TFace
{
	uint16 i0,i1,i2;
	TFace() {}
	TFace(uint16 v0, uint16 v1, uint16 v2) { i0=v0; i1=v1; i2=v2; }
	TFace(const CryFace& face)	{	i0=face[0];	i1=face[1];	i2=face[2];	}
	void operator = (const TFace& f) {	i0=f.i0; i1=f.i1;	i2=f.i2;	}
};

struct PhysicalProxy 
{
	uint32 ChunkID;
	std::vector<Vec3> m_arrPoints;
	std::vector<uint16> m_arrIndices;
	std::vector<char> m_arrMaterials;
};

struct MorphTargets
{
	uint32 MeshID;
	string m_strName;
	std::vector<SMeshMorphTargetVertex> m_arrIntMorph; 
	std::vector<SMeshMorphTargetVertex> m_arrExtMorph; 
};

typedef MorphTargets * MorphTargetsPtr;

struct IntSkinVertex
{
	Vec3 wpos0;			//vertex-position of model.1 
	Vec3 wpos1;			//vertex-position of model.2 
	Vec3 wpos2;			//vertex-position of model.3 
	uint16 boneIDs[4];
	f32 weights[4];
	ColorB color;   //index for blend-array

	AUTO_STRUCT_INFO
};

struct ExtSkinVertex
{
	Vec3 wpos0;				//vertex-position of model-1 
	Vec3 wpos1;				//vertex-position of model-2 
	Vec3 wpos2;				//vertex-position of model-3 
	f32	 u,v;		
	Vec4sf binormal;  //stored as four int16 
	Vec4sf tangent;   //stored as four int16 
	ColorB color;			//index for blend-array
	ColorB boneIDs;		//boneIDs per render-batch
	ColorB weights;   //weights for every bone (four in16)

	AUTO_STRUCT_INFO
};





//////////////////////////////////////////////////////////////////////////
// TCB Controller implementation.
//////////////////////////////////////////////////////////////////////////


// retrieves the position and orientation (in the logarithmic space, i.e. instead of quaternion, its logarithm is returned)
// may be optimal for motion interpolation
struct PQLog
{
	Vec3 vPos;
	Vec3 vRotLog; // logarithm of the rotation
	void blendPQ (const PQLog& pqFrom, const PQLog& pqTo, f32 fBlend);
};

struct CControllerType
{
	uint16 type,index;
	CControllerType() {	type=index=~0; }
};

struct TCBFlags
{
	uint8 f0,f1;
	TCBFlags() {	f0=f1=0; }
};

struct CStoredSkinningInfo 
{
	int32 m_nTicksPerFrame;
	f32 m_secsPerTick;
	int32 m_nStart;
	int32 m_nEnd;
	f32 m_Speed;
	f32 m_Distance;
	f32 m_Slope;
	int m_Looped;
	f32 m_LHeelStart,m_LHeelEnd;
	f32 m_LToe0Start,m_LToe0End;
	f32 m_RHeelStart,m_RHeelEnd;
	f32 m_RToe0Start,m_RToe0End;
	Vec3 m_MoveDirection; // raw storage


	CStoredSkinningInfo() :  m_Speed(-1.0f), m_Distance(-1.0f), m_Looped(0), m_LHeelStart(-10000.0f),m_LHeelEnd(-10000.0f),
		m_LToe0Start(-10000.0f),m_LToe0End(-10000.0f),m_RHeelStart(-10000.0f),m_RHeelEnd(-10000.0f),m_RToe0Start(-10000.0f),m_RToe0End(-10000.0f), m_Slope(-1.0f)
	{

	}
  AUTO_STRUCT_INFO
};

// structure for recreating controllers
struct CControllerInfo
{
	uint32 m_nControllerID;
	uint32 m_nPosKeyTimeTrack;
	uint32 m_nPosTrack;
	uint32 m_nRotKeyTimeTrack;
	uint32 m_nRotTrack;

	CControllerInfo() : m_nControllerID(-1), m_nPosKeyTimeTrack(-1), m_nPosTrack(-1), m_nRotKeyTimeTrack(-1), m_nRotTrack(-1) {}

  AUTO_STRUCT_INFO
};

struct MeshCollisionInfo
{
	AABB m_aABB;
	OBB m_OBB;
	Vec3 m_Pos;
	DynArray<short int> m_arrIndexes;
	int m_iBoneId;

	MeshCollisionInfo()
	{
		// This didn't help much.
		// The BBs are reset to opposite infinites, 
		// but never clamped/grown by any member points.
		m_aABB.min.zero();
		m_aABB.max.zero();
		m_OBB.m33.SetIdentity();
		m_OBB.h.zero();
		m_OBB.c.zero();
		m_Pos.zero();
	}
};


struct CSkinningInfo : public _reference_target_t
{
	DynArray<CryBoneDescData> m_arrBonesDesc;		//animation-bones 
	DynArray<PhysicalProxy> m_arrPhyBoneMeshes; //collision proxi
	DynArray<MorphTargetsPtr> m_arrMorphTargets;
	DynArray<TFace> m_arrIntFaces;
	DynArray<IntSkinVertex> m_arrIntVertices; 
	DynArray<uint16> m_arrExt2IntMap;
	DynArray<BONE_ENTITY> m_arrBoneEntities;			//physical-bones
	DynArray<MeshCollisionInfo> m_arrCollisions;

	uint32 m_numChunks;
	bool m_bRotatedMorphTargets;
	bool m_bProperBBoxes;

	CSkinningInfo() : m_bRotatedMorphTargets(false), m_bProperBBoxes(false) {}

	~CSkinningInfo()
	{
			for (DynArray<MorphTargetsPtr>::iterator it = m_arrMorphTargets.begin(), end = m_arrMorphTargets.end(); it != end; ++it)
				delete *it;
	}

};

//////////////////////////////////////////////////////////////////////////
// This structure represents Material inside CGF.
//////////////////////////////////////////////////////////////////////////
struct CMaterialCGF : public _reference_target_t
{
	string name; // Material name;
	int nFlags;  // Material flags.
	int nPhysicalizeType;
	bool bOldMaterial;
	float shOpacity;
	MAT_ENTITY *pMatEntity; // Old material entity.

	// Array of sub materials.
	std::vector<CMaterialCGF*> subMaterials;

	//////////////////////////////////////////////////////////////////////////
	// Used internally.
	int nChunkId;
	//////////////////////////////////////////////////////////////////////////

	CMaterialCGF() : pMatEntity(0),nFlags(0),nChunkId(0),bOldMaterial(false),nPhysicalizeType(PHYS_GEOM_TYPE_DEFAULT),shOpacity(1.f) {}
	~CMaterialCGF()
	{
		delete pMatEntity;
	}
};

//////////////////////////////////////////////////////////////////////////
// Info about physicalization of the CGF.
//////////////////////////////////////////////////////////////////////////
struct CPhysicalizeInfoCGF
{
	bool bWeldVertices;
	float fWeldTolerance; // Min Distance between vertices when they collapse to single vertex if bWeldVertices enabled.

	// breakable physics
	int nGranularity;
	int nMode;

	Vec3 * pRetVtx;
	int nRetVtx;
	int * pRetTets;
	int nRetTets;

	CPhysicalizeInfoCGF() : bWeldVertices(true),fWeldTolerance(0.01f),nMode(-1), nGranularity(-1), pRetVtx(0),
		nRetVtx(0), pRetTets(0), nRetTets(0){}

	~CPhysicalizeInfoCGF()
	{
		if(pRetVtx)
		{
			delete []pRetVtx;
			pRetVtx = 0;
		}
		if(pRetTets)
		{
			delete []pRetTets;
			pRetTets = 0;
		}
	}
};

//////////////////////////////////////////////////////////////////////////
struct CExportInfoCGF
{
	bool bMergeAllNodes;
	bool bCompiledCGF;
	bool bHavePhysicsProxy;

	unsigned int rc_version[4]; // Resource compiler version.
	char rc_version_string[16]; // Version as a string.
};

//////////////////////////////////////////////////////////////////////////
// This class contain all info loaded from the CGF file.
//////////////////////////////////////////////////////////////////////////
class CContentCGF
{
public:
	//////////////////////////////////////////////////////////////////////////
	CContentCGF( const char *filename )
	{
		m_filename = filename;
		m_pMergedMesh = 0;
		memset( &m_exportInfo,0,sizeof(m_exportInfo) );
		m_exportInfo.bMergeAllNodes = true;
		m_pCommonMaterial = 0;
	}

	//////////////////////////////////////////////////////////////////////////
	~CContentCGF()
	{
		// Free nodes.
		m_nodes.clear();
		delete m_pMergedMesh;
	}

	//////////////////////////////////////////////////////////////////////////
	const char* GetFilename() const { return m_filename.c_str(); }

	//////////////////////////////////////////////////////////////////////////
	// Access to CGF nodes.
	void AddNode( CNodeCGF *pNode ) { m_nodes.push_back(pNode); }
	int GetNodeCount() { return m_nodes.size(); }
	CNodeCGF* GetNode( int i ) { return m_nodes[i]; }
	void ClearNodes() { m_nodes.clear(); }
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Access to CGF materials.
	void AddMaterial( CMaterialCGF *pNode ) { m_materials.push_back(pNode); }
	int GetMaterialCount() { return m_materials.size(); }
	CMaterialCGF* GetMaterial( int i ) { return m_materials[i]; }
	void ClearMaterials() { m_materials.clear(); }
	//////////////////////////////////////////////////////////////////////////

	CMaterialCGF* GetCommonMaterial() const { return m_pCommonMaterial; }
	void SetCommonMaterial( CMaterialCGF *pMtl ) { m_pCommonMaterial = pMtl; }

	// Return Merged mesh.
	CMesh* GetMergedMesh() { return m_pMergedMesh; };
	CMesh* AllocMergedMesh()
	{
		m_pMergedMesh = new CMesh;
		return m_pMergedMesh;
	}

	CPhysicalizeInfoCGF* GetPhysiclizeInfo() { return &m_physicsInfo; };
	CExportInfoCGF* GetExportInfo() { return &m_exportInfo; };
	CSkinningInfo* GetSkinningInfo() { return &m_SkinningInfo; };

private:
	string m_filename;
	CSkinningInfo m_SkinningInfo;
	DynArray<_smart_ptr<CNodeCGF> > m_nodes;
	DynArray<_smart_ptr<CMaterialCGF> > m_materials;

	// Return Merged mesh.
	CMesh *m_pMergedMesh;
	_smart_ptr<CMaterialCGF> m_pCommonMaterial;

	CPhysicalizeInfoCGF m_physicsInfo;
	CExportInfoCGF m_exportInfo;
};


#endif //__CGFContent_h__
