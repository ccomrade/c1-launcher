//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File:CryHeaders.h
//  Definition of cgf, caf file chunks
//
//	History:
//	-:Created by Vladimir Kajalin
//
//////////////////////////////////////////////////////////////////////

#ifndef __CS_HEADERS_H__
#define __CS_HEADERS_H__

#include "Cry_Math.h"

#ifndef MAX_SUB_MATERIALS
#define MAX_SUB_MATERIALS 32
#endif

enum ChunkTypes
{
	ChunkType_ANY			= 0,
	ChunkType_Mesh			= 0xCCCC0000,
	ChunkType_Helper,
	ChunkType_VertAnim,
	ChunkType_BoneAnim,
	ChunkType_GeomNameList,	//Obsolute
	ChunkType_BoneNameList,
	ChunkType_MtlList,		//obsolute
	ChunkType_MRM,			//obsolute
	ChunkType_SceneProps,
	ChunkType_Light,
	ChunkType_PatchMesh,	//not implemented
	ChunkType_Node,
	ChunkType_Mtl,
	ChunkType_Controller,
	ChunkType_Timing,
	ChunkType_BoneMesh,
	ChunkType_BoneLightBinding, // describes the lights binded to bones
	ChunkType_MeshMorphTarget,  // describes a morph target of a mesh chunk
	ChunkType_BoneInitialPos,		// describes the initial position (4x3 matrix) of each bone; just an array of 4x3 matrices
	ChunkType_SourceInfo, // describes the source from which the cgf was exported: source max file, machine and user
	ChunkType_MtlName, // material name
	ChunkType_ExportFlags, // Special export flags.
	ChunkType_DataStream, // Stream data.
	ChunkType_MeshSubsets, // Array of mesh subsets.
	ChunkType_MeshPhysicsData, // Physicalized mesh data.

	//these are the new compiled chunks for characters
	ChunkType_CompiledBones	= 0xACDC0000,
	ChunkType_CompiledPhysicalBones,
	ChunkType_CompiledMorphTargets,
	ChunkType_CompiledPhysicalProxies,
	ChunkType_CompiledIntFaces,
	ChunkType_CompiledIntSkinVertices,
	ChunkType_CompiledExt2IntMap,


	ChunkType_BreakablePhysics	= 0xAAFC0000,
	ChunkType_FaceMap,
	ChunkType_SpeedInfo, // speed & distance information
	ChunkType_FootPlantInfo, //footplant information
	ChunkType_BonesBoxes	
};

enum ECgfStreamType
{
	CGF_STREAM_POSITIONS,
	CGF_STREAM_NORMALS,
	CGF_STREAM_TEXCOORDS,
	CGF_STREAM_COLORS,
	CGF_STREAM_COLORS2,
	CGF_STREAM_INDICES,
	CGF_STREAM_TANGENTS,
	CGF_STREAM_SHCOEFFS,
	CGF_STREAM_SHAPEDEFORMATION,
	CGF_STREAM_BONEMAPPING,
	CGF_STREAM_FACEMAP,
  CGF_STREAM_VERT_MATS
};

//////////////////////////////////////////////////////////////////////////
enum EPhysicsGeomType
{
	PHYS_GEOM_TYPE_NONE       = -1,
	PHYS_GEOM_TYPE_DEFAULT    = 0,
	PHYS_GEOM_TYPE_NO_COLLIDE = 1,
	PHYS_GEOM_TYPE_OBSTRUCT   = 2,

	PHYS_GEOM_TYPE_DEFAULT_PROXY  = 0x100, // Default physicalization, but only proxy (NoDraw geometry).
};

struct CryVertex
{
	Vec3 p;			//position
	Vec3 n;			//normal vector

	AUTO_STRUCT_INFO
};

struct CryFace
{
	int v0,v1,v2;			//vertex indices
	int MatID;				//mat ID
	int SmGroup;			//smoothing group


	int& operator [] (int i) {return (&v0)[i];}
	int operator [] (int i)const {return (&v0)[i];}
	bool isDegenerate ()const {return v0 == v1 || v1 == v2 || v2 == v0;}
	bool isCollapsed()const {return v0 == v1 && v1 == v2;}

	AUTO_STRUCT_INFO
};

// structure used for sorting the cryFace arrays by material id
struct CryFaceOrderByMatID
{
	bool operator () (const CryFace& faceLeft, const CryFace& faceRight)const
	{
		return faceLeft.MatID < faceRight.MatID;
	}
};

struct CryKey
{
	int			time;		//in ticks
	Vec3	abspos;		//absolute position;

	Vec3	relpos;		//relative position;
	CryQuat		relquat;	//relative quaternion

	AUTO_STRUCT_INFO
};

struct CryUV
{
	float u,v;				//texture coordinates
	AUTO_STRUCT_INFO
};

struct CryTexFace
{
	int t0,t1,t2;			//texture vertex indices

	CryTexFace(){}
	CryTexFace (int _t0,int _t1, int _t2):t0(_t0),t1(_t1),t2(_t2) {}
	int& operator[] (int i) {return (&t0)[i];}
	int operator[] (int i) const {return (&t0)[i];}

	AUTO_STRUCT_INFO
};

// Material & Texture Primitives _____________________________________________________________________________________________________
struct TextureMap
{
	char	name[32];

	//tiling and mirror
	bool	utile;
	bool	umirror;
	bool	vtile;
	bool	vmirror;
	
	//texture position values
	float	uoff_val;
	float	uscl_val;
	float	urot_val;
	float	voff_val;
	float	vscl_val;
	float	vrot_val;
	float	wrot_val;

	//texture position controller chunk id's (if not animated they are -1)
	int	uoff_ctrlID;
	int	uscl_ctrlID;
	int	urot_ctrlID;
	int	voff_ctrlID;
	int	vscl_ctrlID;
	int	vrot_ctrlID;
	int	wrot_ctrlID;
};
/*
//////////////////////////////////////////////////////////////////////////
struct TextureMap
{
	char	path[128];	// Name and path of file.

	unsigned char	type;		// Mapping type.
	unsigned char flags;	// Texture map flags.
	unsigned char	ammount;// Ammopunt of blending (0-100).
	unsigned char	reserved[32];	// Some reserved variables.

	//tiling and mirror
	bool	utile;
	bool	umirror;
	bool	vtile;
	bool	vmirror;

	int		nthFrame;	// Update reflection every Nth frame.
	int		refSize;	// Reflection size.
	float	refBlur;	// Reflection blur.
	
	//texture position values
	float	uoff_val;
	float	uscl_val;
	float	urot_val;
	float	voff_val;
	float	vscl_val;
	float	vrot_val;
	float	wrot_val;

	//texture position controller chunk id's (if not animated they are -1)
	int	uoff_ctrlID;
	int	uscl_ctrlID;
	int	urot_ctrlID;
	int	voff_ctrlID;
	int	vscl_ctrlID;
	int	vrot_ctrlID;
	int	wrot_ctrlID;
};
*/

//////////////////////////////////////////////////////////////////////////
enum TextureMappingType
{
	TEXMAP_NORMAL = 0,
	TEXMAP_ENVIRONMENT,
	TEXMAP_SCREENENVIRONMENT,
	TEXMAP_CUBIC,
	TEXMAP_AUTOCUBIC
};

enum TextureMappingFlags
{
	TEXMAP_NOMIPMAP = 1,
};


struct TextureMap2
{
	char	name[32];	// Name of file.

	unsigned char	type;	  // Mapping type.
	unsigned char	flags;	// Mapping type.
  unsigned char Amount;
	unsigned char Reserved[1];

	//tiling and mirror
	bool	utile;
	bool	umirror;
	bool	vtile;
	bool	vmirror;

	int		nthFrame;	// Update reflection every Nth frame.
	int		refSize;	// Reflection size.
	float	refBlur;	// Reflection blur.
	
	//texture position values
	float	uoff_val;
	float	uscl_val;
	float	urot_val;
	float	voff_val;
	float	vscl_val;
	float	vrot_val;
	float	wrot_val;

	//texture position controller chunk id's (if not animated they are -1)
	int	uoff_ctrlID;
	int	uscl_ctrlID;
	int	urot_ctrlID;
	int	voff_ctrlID;
	int	vscl_ctrlID;
	int	vrot_ctrlID;
	int	wrot_ctrlID;

	AUTO_STRUCT_INFO
};

struct TextureMap3
{
	char	name[128];	// Name of file.

	unsigned char	type;	  // Mapping type.
	unsigned char	flags;	// Mapping type.
  unsigned char Amount;
	unsigned char Reserved[32];

	//tiling and mirror
	bool	utile;
	bool	umirror;
	bool	vtile;
	bool	vmirror;

	int		nthFrame;	// Update reflection every Nth frame.
	int		refSize;	// Reflection size.
	float	refBlur;	// Reflection blur.
	
	//texture position values
	float	uoff_val;
	float	uscl_val;
	float	urot_val;
	float	voff_val;
	float	vscl_val;
	float	vrot_val;
	float	wrot_val;

	//texture position controller chunk id's (if not animated they are -1)
	int	uoff_ctrlID;
	int	uscl_ctrlID;
	int	urot_ctrlID;
	int	voff_ctrlID;
	int	vscl_ctrlID;
	int	vrot_ctrlID;
	int	wrot_ctrlID;

  TextureMap3& operator = (const TextureMap2& tm)
  {
    strcpy(name, tm.name);
    type = tm.type;
    umirror = tm.umirror;
    vtile = tm.vtile;
    vmirror = tm.vmirror;
    
    nthFrame = tm.nthFrame;
    refSize = tm.refSize;
    refBlur = tm.refBlur;
    
    uoff_val = tm.uoff_val;
    uscl_val = tm.uscl_val;
    urot_val = tm.urot_val;
    voff_val = tm.voff_val;
    vscl_val = tm.vscl_val;
    vrot_val = tm.vrot_val;
    wrot_val = tm.wrot_val;
    
    uoff_ctrlID = tm.uoff_ctrlID;
    uscl_ctrlID = tm.uscl_ctrlID;
    urot_ctrlID = tm.urot_ctrlID;
    voff_ctrlID = tm.voff_ctrlID;
    vscl_ctrlID = tm.vscl_ctrlID;
    vrot_ctrlID = tm.vrot_ctrlID;
    wrot_ctrlID = tm.wrot_ctrlID;
    
    return *this;
  }  

	AUTO_STRUCT_INFO
};

struct CryLink
{
	int			BoneID;
	Vec3		offset;
	float		Blending;

	AUTO_STRUCT_INFO
};

// structure used to sort the crylink array by the blending factor, descending
struct CryLinkOrderByBlending
{
	bool operator () (const CryLink& left, const CryLink& right)
	{
		return left.Blending > right.Blending;
	}
};

struct CryIRGB
{
	unsigned char r,g,b;
	AUTO_STRUCT_INFO
};

struct CryFRGB
{
	float r,g,b;
	AUTO_STRUCT_INFO
};

struct NAME_ENTITY
{
	char	name[64];
};

struct SCENEPROP_ENTITY
{
	char	name[32];
	char	value[64];
};
  /*
struct BONE_ENTITY
{
	int		BoneID;
	int		ParentID;
	int		nChildren;
	int		nKeys;
	char	prop[32];	// do not forget to update strncpy len
};
*/

struct phys_geometry;
struct BONE_PHYSICS
{
	phys_geometry *pPhysGeom; // id of a separate mesh for this bone // MUST not be in File Structures!!!
	// additional joint parameters
	int		flags;
	float min[3],max[3];
	float spring_angle[3];
	float spring_tension[3];
	float damping[3];
	float framemtx[3][3];

	AUTO_STRUCT_INFO
};

// the compatible between 32- and 64-bits structure 
struct BONE_PHYSICS_COMP
{
	int nPhysGeom; // id of a separate mesh for this bone
	// additional joint parameters
	int		flags;
	float min[3],max[3];
	float spring_angle[3];
	float spring_tension[3];
	float damping[3];
	float framemtx[3][3];

	AUTO_STRUCT_INFO
};

struct CryBoneDescData
{
	unsigned int m_nControllerID; // unic id of bone (generated from bone name in the max)

	// [Sergiy] physics info for different lods
	// lod 0 is the physics of alive body, lod 1 is the physics of a dead body
	BONE_PHYSICS m_PhysInfo[2]; 
	float m_fMass;

	Matrix34 m_DefaultW2B; //intitalpose matrix World2Bone 
	Matrix34 m_DefaultB2W; //intitalpose matrix Bone2World 

	char m_arrBoneName[256]; 	

	int	m_nLimbId; // set by model state class

	// this bone parent is this[m_nOffsetParent], 0 if the bone is root. Normally this is <= 0
	int m_nOffsetParent;

	// The whole hierarchy of bones is kept in one big array that belongs to the ModelState
	// Each bone that has children has its own range of bone objects in that array,
	// and this points to the beginning of that range and defines the number of bones.
	unsigned m_numChildren;
	// the beginning of the subarray of children is at this[m_nOffsetChildren]
	// this is 0 if there are no children
	int m_nOffsetChildren;

	AUTO_STRUCT_INFO
}; 

struct CryBoneDescData_Comp
{
	unsigned int m_nControllerID; // unic id of bone (generated from bone name in the max)

	// [Sergiy] physics info for different lods
	// lod 0 is the physics of alive body, lod 1 is the physics of a dead body
	BONE_PHYSICS_COMP m_PhysInfo[2]; 
	float m_fMass;

	Matrix34 m_DefaultW2B; //intitalpose matrix World2Bone 
	Matrix34 m_DefaultB2W; //intitalpose matrix Bone2World 

	char m_arrBoneName[256]; 	

	int	m_nLimbId; // set by model state class

	// this bone parent is this[m_nOffsetParent], 0 if the bone is root. Normally this is <= 0
	int m_nOffsetParent;

	// The whole hierarchy of bones is kept in one big array that belongs to the ModelState
	// Each bone that has children has its own range of bone objects in that array,
	// and this points to the beginning of that range and defines the number of bones.
	unsigned m_numChildren;
	// the beginning of the subarray of children is at this[m_nOffsetChildren]
	// this is 0 if there are no children
	int m_nOffsetChildren;

	AUTO_STRUCT_INFO
}; 

#define __copy3(MEMBER) left.MEMBER[0] = right.MEMBER[0]; left.MEMBER[1] = right.MEMBER[1]; left.MEMBER[2] = right.MEMBER[2];
inline void CopyPhysInfo (BONE_PHYSICS& left, const BONE_PHYSICS_COMP& right)
{
	left.pPhysGeom = (phys_geometry*)(INT_PTR)right.nPhysGeom;
	left.flags     = right.flags;
	__copy3(min);
	__copy3(max);
	__copy3(spring_angle);
	__copy3(spring_tension);
	__copy3(damping);
	__copy3(framemtx[0]);
	__copy3(framemtx[1]);
	__copy3(framemtx[2]);
}
inline void CopyPhysInfo (BONE_PHYSICS_COMP& left, const BONE_PHYSICS& right)
{
	left.nPhysGeom = (int)(INT_PTR)right.pPhysGeom;
	left.flags     = right.flags;
	__copy3(min);
	__copy3(max);
	__copy3(spring_angle);
	__copy3(spring_tension);
	__copy3(damping);
	__copy3(framemtx[0]);
	__copy3(framemtx[1]);
	__copy3(framemtx[2]);
}

#undef __copy3

struct BONE_ENTITY
{
	int		BoneID;
	int		ParentID;
	int		nChildren;

	// Id of controller (CRC32 From name of bone).
	unsigned int	ControllerID;

	char	prop[32];	// do not forget to update strncpy len // [Sergiy] why not use sizeof??? :)
	BONE_PHYSICS_COMP phys;

	//BONE_ENTITY() {};

	AUTO_STRUCT_INFO
};

  /*
struct MAT_ENTITY
{
	char	name[64];	//material name
	bool	IsStdMat;	//if false the below values are meaningless

	CryIRGB	col_d;		//diffuse color
	CryIRGB	col_s;		//specular color
	CryIRGB	col_a;		//ambiant color
	
	char	map_d[32];	//diffuse map file name
	char	map_o[32];	//opacity map file name
	char	map_b[32];	//bump map file name

	float	Dyn_Bounce;
	float	Dyn_StaticFriction;
	float	Dyn_SlidingFriction;
};
*/

struct MAT_ENTITY_DATA
{
	enum MTL_CHUNK_FLAGS
	{
		MTLFLAG_WIRE =		0x001,
		MTLFLAG_2SIDED =	0x002,
		MTLFLAG_FACEMAP = 0x004,
		MTLFLAG_FACETED = 0x008,
		MTLFLAG_ADDITIVE= 0x010,
		MTLFLAG_SUBTRACTIVE= 0x020,
		MTLFLAG_CRYSHADER= 0x040,
		MTLFLAG_PHYSICALIZE= 0x080,
		MTLFLAG_ADDITIVEDECAL = 0x0100,
		MTLFLAG_USEGLOSSINESS = 0x0200,
	};

	char	name[64];	  //material/shader name
	bool	IsStdMat;	  //if false the below values are meaningless
  int  m_New;

	CryIRGB	col_d;		//diffuse color
	CryIRGB	col_s;		//specular color
	CryIRGB	col_a;		//ambient color

	float specLevel; // Specular level.
  float	specShininess;
	float	selfIllum;	// self illumination.
	float opacity;		// Opacity 0-1.
	float alpharef;		// Alpha-test 0-1.
  float m_Temp[8]; // For future modifications

	int flags;
	
	TextureMap3 map_a;	//Ambient map file name
	TextureMap3 map_d;	//Diffuse map file name
	TextureMap3 map_o;	//Opacity map file name
	TextureMap3 map_b;	//Bump map file name
  TextureMap3 map_s;	//Specular Texture settings
  TextureMap3 map_g;	//Gloss Texture settings
  TextureMap3 map_detail;	//Detail Texture settings
  TextureMap3 map_e;	//Environment Texture settings
	TextureMap3 map_subsurf; //Subsurface scattering Texture settings
  TextureMap3	map_displ;//Displacement Texture settings
  TextureMap3	map_custom;//Custom texture settings
  TextureMap3	map_custom_secondary;//Custom texture settings

	float	Dyn_Bounce;
	float	Dyn_StaticFriction;
	float	Dyn_SlidingFriction;

	int iGameID;

	int   nChildren;
};

struct MAT_ENTITY : public MAT_ENTITY_DATA
{
	int m_iMaterialChildren;
};

struct KEY_HEADER
{
	int		KeyTime;	//in ticks
	AUTO_STRUCT_INFO
};

//========================================
//File Headers
//========================================
#define FILE_SIGNATURE	"CryTek"

struct FILE_HEADER
{
	char	Signature[7];
	int		FileType;
	int		Version;
	int		ChunkTableOffset;

	AUTO_STRUCT_INFO
};

enum FileTypes
{
	FileType_Geom			= 0xFFFF0000,
	FileType_Anim,
};
/*
#define GeomFileVersion		0x0623
#define AnimFileVersion		0x0290
  */
#define GeomFileVersion		0x0744
#define AnimFileVersion		0x0744

//========================================
//Chunk Table Entity
//========================================


struct CHUNK_HEADER_0623
{
	enum {VERSION = 0x0623};
	ChunkTypes	ChunkType;
	int			ChunkVersion;
	int			FileOffset;

	AUTO_STRUCT_INFO
};

struct CHUNK_HEADER_0744
{
	enum {VERSION = 0x0744};
	ChunkTypes	ChunkType;
	int			ChunkVersion;
	int			FileOffset;
	int			ChunkID;		//new in version 0x0744

	CHUNK_HEADER_0744()
	{
		FileOffset = -1;
	}

	AUTO_STRUCT_INFO
};

typedef CHUNK_HEADER_0744 CHUNK_HEADER;

struct RANGE_ENTITY
{
	char name[32];
	int start;
	int end;
	AUTO_STRUCT_INFO
};

//========================================
//Timing Chunk Header
//========================================
struct TIMING_CHUNK_DESC_0918
{
	enum {VERSION = 0x0918};
	CHUNK_HEADER	chdr;

	float			SecsPerTick;		// seconds/ticks
	int				TicksPerFrame;		// ticks/Frame

	RANGE_ENTITY	global_range;		// covers all of the time ranges
	int				nSubRanges;

	AUTO_STRUCT_INFO
};

//========================================
//SpeedInfo Header
//========================================
struct SPEED_CHUNK_DESC
{
	enum {VERSION = 0x0920};

	CHUNK_HEADER chdr;

	float			Speed;
	float			Distance;
	float			Slope;
	int       Looped;

	AUTO_STRUCT_INFO
};

struct SPEED_CHUNK_DESC_1
{
	enum {VERSION = 0x0921};

	CHUNK_HEADER chdr;

	float			Speed;
	float			Distance;
	float			Slope;
	int       Looped;
	f32				MoveDir[3];

	AUTO_STRUCT_INFO
};

struct SPEED_CHUNK_DESC_2
{
	enum {VERSION = 0x0922};

	CHUNK_HEADER chdr;

	float			Speed;
	float			Distance;
	float			Slope;
	int       Looped;
	f32				MoveDir[3];
	QuatT			StartPosition;

	AUTO_STRUCT_INFO
};

//========================================
//SpeedInfo Header
//========================================
struct FOOTPLANT_INFO
{
	enum {VERSION = 0x0920};

	CHUNK_HEADER chdr;

//	float			Speed;
//	float			Distance;
	int nPoses;

	f32 m_LHeelStart,m_LHeelEnd;
	f32 m_LToe0Start,m_LToe0End;
	f32 m_RHeelStart,m_RHeelEnd;
	f32 m_RToe0Start,m_RToe0End;

	AUTO_STRUCT_INFO
};


typedef TIMING_CHUNK_DESC_0918 TIMING_CHUNK_DESC;
#define TIMING_CHUNK_DESC_VERSION TIMING_CHUNK_DESC::VERSION

//========================================
//Material Chunk Header
//========================================
enum MtlTypes { MTL_UNKNOWN, MTL_STANDARD, MTL_MULTI, MTL_2SIDED };

struct MTL_CHUNK_DESC_0744
{
	enum {VERSION = 0x0744};
	CHUNK_HEADER	chdr;
	char			name[64];	//material name 
	MtlTypes		MtlType;	//one of the MtlTypes enum values

	union
	{
		struct
		{
			CryIRGB		col_d;		//diffuse color
			CryIRGB		col_s;		//specular color
			CryIRGB		col_a;		//ambiant color
			
			TextureMap	tex_d;		//Diffuse Texture settings
			TextureMap	tex_o;		//opacity Texture settings
			TextureMap	tex_b;		//Bump Texture settings

			float		Dyn_Bounce;
			float		Dyn_StaticFriction;
			float		Dyn_SlidingFriction;
		};

		struct
		{
			int		nChildren;
		};
	};
};

//////////////////////////////////////////////////////////////////////////
struct MTL_CHUNK_DESC_0745
{
	enum {VERSION=0x0745};
	CHUNK_HEADER	chdr;
	char			name[64];	  //material/shader name 
	MtlTypes		MtlType;	//one of the MtlTypes enum values

	union
	{
		struct
		{
			CryIRGB		col_d;		//diffuse color
			CryIRGB		col_s;		//specular color
			CryIRGB		col_a;		//ambient color
			float     specLevel; // Specular level.
      float     specShininess;
			float			selfIllum;	// self illumination.
			float			opacity;		// Opacity 0-1.
			
			TextureMap2	tex_a;		//Ambient Texture settings
			TextureMap2	tex_d;		//Diffuse Texture settings
			TextureMap2	tex_s;		//Specular Texture settings
			TextureMap2	tex_o;		//Opacity Texture settings
			TextureMap2	tex_b;		//Bump Texture settings
      TextureMap2	tex_g;		//Gloss Texture settings
      TextureMap2	tex_c;		//Cubemap Texture settings
      TextureMap2	tex_rl;		//Reflection Texture settings
			TextureMap2	tex_subsurf;		//Subsurface scattering Texture settings
      TextureMap2	tex_det;  //Detail Texture settings

			int			flags;
			
			float		Dyn_Bounce;
			float		Dyn_StaticFriction;
			float		Dyn_SlidingFriction;
		};

		struct
		{
			int		nChildren;
		};
	};

	AUTO_STRUCT_INFO
};

struct MTL_CHUNK_DESC_0746
{
	enum {VERSION = 0x0746};
	CHUNK_HEADER	chdr;
	char			name[64];	  //material/shader name 
  char      Reserved[60];
  float			alphaTest;
	MtlTypes		MtlType;	//one of the MtlTypes enum values

	union
	{
		struct
		{
			CryIRGB		col_d;		//diffuse color
			CryIRGB		col_s;		//specular color
			CryIRGB		col_a;		//ambient color
			float     specLevel; // Specular level.
      float     specShininess;
			float			selfIllum;	// self illumination.
			float			opacity;		// Opacity 0-1.
			
			TextureMap3	tex_a;		//Ambient Texture settings
			TextureMap3	tex_d;		//Diffuse Texture settings
			TextureMap3	tex_s;		//Specular Texture settings
			TextureMap3	tex_o;		//Opacity Texture settings
			TextureMap3	tex_b;		//Bump Texture settings
      TextureMap3	tex_g;		//Gloss Texture settings
      TextureMap3	tex_fl;		//Filter texture (Detail Texture settings)
      TextureMap3	tex_rl;		//Reflection Texture settings
			TextureMap3	tex_subsurf;		//Subsurface scattering Texture settings
      TextureMap3	tex_det;  //Detail Texture settings

			int			flags;
			
			float		Dyn_Bounce;
			float		Dyn_StaticFriction;
			float		Dyn_SlidingFriction;
		};

		struct
		{
			int		nChildren;
		};
	};

	AUTO_STRUCT_INFO
};

// The default MTL_CHUNK_DESC is different in the engine and in the plugin (exporter)
typedef MTL_CHUNK_DESC_0744 MTL_CHUNK_DESC;
typedef MTL_CHUNK_DESC_0746 MTL_CHUNK_DESC_EXPORTER;


struct MTL_NAME_CHUNK_DESC_0800
{
	enum {VERSION = 0x0800};
	enum EFlags
	{
		FLAG_MULTI_MATERIAL  = 0x0001, // Have sub materials info.
		FLAG_SUB_MATERIAL		 = 0x0002, // This is sub material.
		FLAG_SH_COEFFS			 = 0x0004, // This material should get spherical harmonics coefficients computed.
		FLAG_SH_2SIDED			 = 0x0008, // This material will be used as 2 sided in the sh precomputation
		FLAG_SH_AMBIENT			 = 0x0010, // This material will get an ambient sh term(to not shadow it entirely)
	};

	CHUNK_HEADER	chdr;
	int nFlags; // see EFlags.
	int nFlags2;
	char name[128];	//material/shader name 
	int nPhysicalizeType;
	int nSubMaterials;
	int nSubMatChunkId[MAX_SUB_MATERIALS];
	int nAdvancedDataChunkId;
	float sh_opacity;
	int reserve[32];

	AUTO_STRUCT_INFO
};

typedef MTL_NAME_CHUNK_DESC_0800 MTL_NAME_CHUNK_DESC;
typedef MTL_NAME_CHUNK_DESC_0800 MTL_NAME_CHUNK_DESC_EXPORTER;


//========================================
//Light Chunk Header
//========================================
enum LightTypes { LT_OMNI, LT_SPOT, LT_DIRECT, LT_AMBIENT };

struct LIGHT_CHUNK_DESC_0351
{
	enum {VERSION = 0x0351};
	int			ChunkType;		//must be ChunkType_Light
	int			ChunkVersion;	

	char		name[64];		// light name
    int			type;			// one of the LightTypes values
    bool 		on;				// light is on
	Vec3	pos;			// world position
	float	    tm[4][4];				// transformation matrix
	CryIRGB		color;			// RGB color
	float 		intens;			// multiplier value

	float 		hotsize;		// for spot and direct lights hotspot value
	float 		fallsize;		// for spot and direct lights falloff value
	bool		useNearAtten;	// near atten. on
	float		nearAttenStart;	// near attenuation ranges
	float		nearAttenEnd;
	bool   		useAtten;		// far atteniation
	float 		attenStart;		// far attenuation ranges
	float 		attenEnd;
	bool   		shadow;			// shadow is on

	AUTO_STRUCT_INFO
};

struct LIGHT_CHUNK_DESC_0744
{
	enum {VERSION = 0x0744};
	CHUNK_HEADER	chdr;

	LightTypes	type;			// one of the LightTypes values
    bool 		on;				// light is on
	CryIRGB		color;			// RGB color
	float 		intens;			// multiplier value

	float 		hotsize;		// for spot and direct lights hotspot value
	float 		fallsize;		// for spot and direct lights falloff value
	bool		useNearAtten;	// near atten. on
	float		nearAttenStart;	// near attenuation ranges
	float		nearAttenEnd;
	bool   		useAtten;		// far atteniation
	float 		attenStart;		// far attenuation ranges
	float 		attenEnd;
	bool   		shadow;			// shadow is on

	Vec3		vDirection;			//spot light direction
	char		szLightImage[256];	//spot light texture

	AUTO_STRUCT_INFO
};

typedef LIGHT_CHUNK_DESC_0744 LIGHT_CHUNK_DESC;
#define LIGHT_CHUNK_DESC_VERSION LIGHT_CHUNK_DESC::VERSION


//========================================
//Mesh Chunk Header
//========================================
struct MESH_CHUNK_DESC_0623
{
	enum {VERSION = 0x0623};
	int		ChunkType;		//must be ChunkType_Mesh
	int		ChunkVersion;
	int		GeomID;
	bool	HasBoneInfo;
	bool	HasVertexCol;
	bool	InWorldSpace;
	int		nVerts;
	int		nTVerts;		// # of texture vertices
	int		nFaces;
	int		MatID;
	int		PropStrLen;		//lenght of the property string

	AUTO_STRUCT_INFO
};

struct MESH_CHUNK_DESC_0744
{
	enum {VERSION = 0x0744};
	CHUNK_HEADER	chdr;

	enum EFlags1
	{
		FLAG1_BONE_INFO = 0x01,
	};
	enum EFlags2
	{
		FLAG2_HAS_VERTEX_COLOR = 0x01,
		FLAG2_HAS_VERTEX_ALPHA = 0x02,
	};
	//bool	HasBoneInfo;
	//bool	HasVertexCol;
	unsigned char flags1;
	unsigned char flags2;
	int		nVerts;
	int		nTVerts;		// # of texture vertices
	int		nFaces;
	int		VertAnimID;		// id of the related vertAnim chunk if present. otherwise it is -1

	AUTO_STRUCT_INFO
};

// Compiled Mesh chunk.
struct MESH_CHUNK_DESC_0800
{
	enum {VERSION = 0x0800};
	CHUNK_HEADER	chdr;
	enum EFlags
	{
	};
	int nFlags;  // @see EFlags
	int nFlags2;

	// Just for info.
	int nVerts;       // Number of vertices.
	int nIndices;     // Number of indices.
	int nSubsets;     // Number of mesh subsets.

	int nSubsetsChunkId; // Chunk id of subsets. (Must be ChunkType_MeshSubsets)
	int nVertAnimID;		// id of the related vertAnim chunk if present. otherwise it is -1

	// ChunkIDs of data streams (Must be ChunkType_DataStream).
	int nStreamChunkID[16]; // Index is one of ECgfStreamType values.

	// Chunk IDs of physical mesh data. (Must be ChunkType_MeshPhysicsData)
	int nPhysicsDataChunkId[4];
	
	// Bounding box of the mesh.
	Vec3 bboxMin;
	Vec3 bboxMax;
	
	int reserved[32];

	AUTO_STRUCT_INFO
};

typedef MESH_CHUNK_DESC_0744 MESH_CHUNK_DESC;
#define MESH_CHUNK_DESC_VERSION MESH_CHUNK_DESC::VERSION

//////////////////////////////////////////////////////////////////////////
// Stream chunk contains data about mesh data stream (position,normals,etc...)
//////////////////////////////////////////////////////////////////////////
struct STREAM_DATA_CHUNK_DESC_0800
{
	enum {VERSION = 0x0800};
	CHUNK_HEADER	chdr;

	enum EFlags { }; // Not implemented.

	int nFlags;
	int nStreamType;  // Stream type one of ECgfStreamType.
	int nCount;       // Number of elements.
	int nElementSize; // Element Size.
	int reserved[2];

	// Data starts here at the end of the chunk..
	//char streamData[nCount*nElementSize];

	AUTO_STRUCT_INFO
};

//////////////////////////////////////////////////////////////////////////
// Contain array of mesh subsets.
// Each subset holds an info about material id, indices ranges etc...
//////////////////////////////////////////////////////////////////////////
struct MESH_SUBSETS_CHUNK_DESC_0800
{
	AUTO_STRUCT_INFO
	enum {VERSION = 0x0800};
	CHUNK_HEADER	chdr;

	enum EFlags
	{
		SH_HAS_DECOMPR_MAT = 0x0001,
		BONEINDICES        = 0x0002
	};

	int nFlags;
	int nCount;       // Number of elements.
	int reserved[2];

	struct MeshSubset
	{
		int nFirstIndexId;
		int nNumIndices;
		int nFirstVertId;
		int nNumVerts;
		int nMatID; // Material sub-object Id.
		float fRadius;
		Vec3 vCenter;

		AUTO_STRUCT_INFO
	};

	struct MeshBoneIDs
	{
		uint32 numBoneIDs;
		uint16 arrBoneIDs[0x80];
		AUTO_STRUCT_INFO
	};

	// Data starts here at the end of the chunk.
	//Subset streamData[nCount];

	// if nFlags & SH_HAS_DECOMPR_MAT
	/*	struct SSHDecompressionMat
	{
		float offset0[4];
		float offset1[4];
		float scale0[4];
		float scale1[4];
	}SSHDecompressionMat;*/

	MESH_SUBSETS_CHUNK_DESC_0800() : nFlags(0), nCount(0)
	{}
};

//////////////////////////////////////////////////////////////////////////
// Contain array of mesh subsets.
// Each subset holds an info about material id, indices ranges etc...
//////////////////////////////////////////////////////////////////////////
struct MESH_PHYSICS_DATA_CHUNK_DESC_0800
{
	enum {VERSION = 0x0800};
	CHUNK_HEADER	chdr;

	int nDataSize;  // Size of physical data at the end of the chunk.
	int nFlags;
	int nTetrahedraDataSize;
	int nTetrahedraChunkId; // Chunk of physics Tetrahedra data.
	int reserved[2];
	
	// Data starts here at the end of the chunk.
	//char physicsData[nDataSize];
	//char tetrahedraData[nTetrahedraDataSize];

	AUTO_STRUCT_INFO
};

//========================================
//SceneProps Chunk Header
//========================================
struct SCENEPROPS_CHUNK_DESC_0201
{
	enum {VERSION = 0x0201};
	int		ChunkType;		//must be ChunkType_SceneProps
	int		ChunkVersion;
	int		nProps;
	AUTO_STRUCT_INFO
};

struct SCENEPROPS_CHUNK_DESC_0744
{
	enum {VERSION = 0x0744};

	CHUNK_HEADER	chdr;
	int		nProps;
	AUTO_STRUCT_INFO
};

typedef SCENEPROPS_CHUNK_DESC_0744 SCENEPROPS_CHUNK_DESC;
#define SCENEPROPS_CHUNK_DESC_VERSION SCENEPROPS_CHUNK_DESC::VERSION

//////////////////////////////////////////////////////////////////////////
// Custom Attribute structure.
//////////////////////////////////////////////////////////////////////////
enum CryCustomAttribFlags
{
	CATYPE_INT,
	CATYPE_FLOAT,
	CATYPE_BOOL,
	CATYPE_STRING
};

struct CryCustomAttribute
{
	// One of CryCustomAttribFlags.
	int type;
	char name[32];
	char value[64];
	AUTO_STRUCT_INFO
};

//////////////////////////////////////////////////////////////////////////
// Custom Attributes chunk description.
//////////////////////////////////////////////////////////////////////////
struct CUSTOM_ATTRIBS_CHUNK_DESC_0744
{
	enum {VERSION = 0x0744};

	CHUNK_HEADER	chdr;
	int numAttribs;

	// Here goes array of custom attributes.
	// CryCustomAttribute attributes[];

	AUTO_STRUCT_INFO
};

typedef SCENEPROPS_CHUNK_DESC_0744 SCENEPROPS_CHUNK_DESC;
#define SCENEPROPS_CHUNK_DESC_VERSION SCENEPROPS_CHUNK_DESC::VERSION


//========================================
//MRM Chunk Header
//========================================
struct MRM_CHUNK_DESC
{
	int		ChunkType;		//must be ChunkType_MRM
	int		ChunkVersion;
	int		GeomID;
	AUTO_STRUCT_INFO
};

#define MRM_CHUNK_DESC_VERSION 0x0201

//========================================
//Dummy Chunk Header
//========================================
struct DUMMY_CHUNK_DESC
{
	int		ChunkType;		//must be ChunkType_Dummy
	int		ChunkVersion;
	char	name[64];

	AUTO_STRUCT_INFO
};
#define DUMMY_CHUNK_DESC_VERSION 0x0201

//========================================
//Vertex Anim Chunk Header
//========================================
struct VERTANIM_CHUNK_DESC_0201
{
	int		ChunkType;		// must be ChunkType_VertAnim
	int		ChunkVersion;
	bool	InWorldSpace;
	int		GeomID;			// node ID
	int		nKeys;			// # of keys
	int		nVerts;			// # of vertices this object has
	int		nFaces;			// # of faces this object has (for double check purpose)		
	float	SecsPerTick;	// seconds/ticks
	int		TicksPerFrame;	// ticks/Frame

	AUTO_STRUCT_INFO
};

struct VERTANIM_CHUNK_DESC_0744
{
	CHUNK_HEADER	chdr;

	int		GeomID;			// ID of the related mesh chunk
	int		nKeys;			// # of keys
	int		nVerts;			// # of vertices this object has
	int		nFaces;			// # of faces this object has (for double check purpose)		

	AUTO_STRUCT_INFO
};

typedef VERTANIM_CHUNK_DESC_0744 VERTANIM_CHUNK_DESC;
#define VERTANIM_CHUNK_DESC_VERSION 0x0744

//========================================
//Bone Anim Chunk Header
//========================================
/*
struct BONEANIM_CHUNK_DESC
{
	int		ChunkType;		// must be ChunkType_BoneAnim
	int		ChunkVersion;
	float	SecsPerTick;	// seconds/ticks
	int		TicksPerFrame;	// ticks/Frame
	int		startframe;		// in frames
	int		endframe;		// in frames
};*/


struct BONEANIM_CHUNK_DESC_0290
{
	enum {VERSION = 0x0290};

	CHUNK_HEADER	chdr;
	int		nBones;

	AUTO_STRUCT_INFO
};

typedef BONEANIM_CHUNK_DESC_0290 BONEANIM_CHUNK_DESC;

#define BONEANIM_CHUNK_DESC_VERSION BONEANIM_CHUNK_DESC::VERSION

//========================================
//GeomList Chunk Header #OBSOLETE#
//========================================
struct GEOMNAMELIST_CHUNK_DESC_0201
{
	enum {VERSION=0x0201};
	int		ChunkType;		//must be ChunkType_GeomNameList
	int		ChunkVersion;
	int		nEntities;

	AUTO_STRUCT_INFO
};
typedef GEOMNAMELIST_CHUNK_DESC_0201 GEOMNAMELIST_CHUNK_DESC;

#define GEOMNAMELIST_CHUNK_DESC_VERSION GEOMNAMELIST_CHUNK_DESC::VERSION;

//========================================
//Bonelist Chunk Header 
//========================================
struct BONENAMELIST_CHUNK_DESC_0201
{
	enum {VERSION=0x0201};
	int		ChunkType;		//must be ChunkType_BoneNameList
	int		ChunkVersion;
	int		nEntities;

	AUTO_STRUCT_INFO
};

struct BONENAMELIST_CHUNK_DESC_0744
{
	enum {VERSION=0x0744};
	CHUNK_HEADER chdr;
	int			 nEntities;

	AUTO_STRUCT_INFO
};

// this structure describes the bone names
// it's followed by numEntities packed \0-terminated strings, the list terminated by double-\0
struct BONENAMELIST_CHUNK_DESC_0745
{
	enum {VERSION = 0x0745};
	int numEntities;
	AUTO_STRUCT_INFO
};


struct COMPILED_BONE_CHUNK_DESC_0800
{
	enum {VERSION=0x0800};
	CHUNK_HEADER chdr;
	char reserved[32];
	AUTO_STRUCT_INFO
};

struct COMPILED_PHYSICALBONE_CHUNK_DESC_0800
{
	enum {VERSION=0x0800};
	CHUNK_HEADER chdr;
	char reserved[32];
	AUTO_STRUCT_INFO
};

struct COMPILED_PHYSICALPROXY_CHUNK_DESC_0800
{
	enum {VERSION=0x0800};
	CHUNK_HEADER chdr;
	uint32 numPhysicalProxies;
	AUTO_STRUCT_INFO
};

struct COMPILED_MORPHTARGETS_CHUNK_DESC_0800
{
	enum {VERSION=0x0800, VERSION1=0x801};
	CHUNK_HEADER chdr;
	uint32 numMorphTargets;
	AUTO_STRUCT_INFO
};


struct COMPILED_INTFACES_CHUNK_DESC_0800
{
	enum {VERSION=0x0800};
	CHUNK_HEADER chdr;
	AUTO_STRUCT_INFO
};

struct COMPILED_INTSKINVERTICES_CHUNK_DESC_0800
{
	enum {VERSION=0x0800};
	CHUNK_HEADER chdr;
	char reserved[32];
	AUTO_STRUCT_INFO
};

struct COMPILED_EXT2INTMAP_CHUNK_DESC_0800
{
	enum {VERSION=0x0800};
	CHUNK_HEADER chdr;
	AUTO_STRUCT_INFO
};

struct COMPILED_BONEBOXES_CHUNK_DESC_0800
{
	enum {VERSION=0x0800, VERSION1=0x801};
	CHUNK_HEADER chdr;
	AUTO_STRUCT_INFO
};

//========================================
//MtlList Chunk Header  ##OBSOLETE##
//========================================
struct MTLLIST_CHUNK_DESC_0201
{
	enum {VERSION = 0x0201};
	int		ChunkType;		//must be ChunkType_MtlList
	int		ChunkVersion;
	int		nEntities;
	AUTO_STRUCT_INFO
};
typedef MTLLIST_CHUNK_DESC_0201 MTLLIST_CHUNK_DESC;
#define MTLLIST_CHUNK_DESC_VERSION MTLLIST_CHUNK_DESC::VERSION


// Keyframe and Timing Primitives __________________________________________________________________________________________________________________
struct BaseKey
{
	int time;
	AUTO_STRUCT_INFO
};

struct BaseTCB
{
	float t,c,b;
	float ein, eout;
	AUTO_STRUCT_INFO
};

struct BaseKey1 : BaseKey { float		val; AUTO_STRUCT_INFO };
struct BaseKey3 : BaseKey { Vec3	val; AUTO_STRUCT_INFO };
struct BaseKeyQ : BaseKey { CryQuat	val; AUTO_STRUCT_INFO };

struct CryLin1Key : BaseKey1 { AUTO_STRUCT_INFO };
struct CryLin3Key : BaseKey3 { AUTO_STRUCT_INFO };
struct CryLinQKey : BaseKeyQ { AUTO_STRUCT_INFO };
struct CryTCB1Key : BaseKey1 , BaseTCB { AUTO_STRUCT_INFO };
struct CryTCB3Key : BaseKey3 , BaseTCB { AUTO_STRUCT_INFO };
struct CryTCBQKey : BaseKeyQ , BaseTCB { AUTO_STRUCT_INFO };
struct CryBez1Key : BaseKey1 { float		intan, outtan; AUTO_STRUCT_INFO };
struct CryBez3Key : BaseKey3 { Vec3	intan, outtan; AUTO_STRUCT_INFO };
struct CryBezQKey : BaseKeyQ { AUTO_STRUCT_INFO };
struct CryBoneKey : BaseKey
{
	Vec3	abspos;
	Vec3	relpos;
	CryQuat	relquat;

	AUTO_STRUCT_INFO
};

struct CryKeyPQLog
{
	int nTime;
	Vec3 vPos;
	Vec3 vRotLog; // logarithm of the rotation

	// resets to initial position/rotation/time
	void reset ()
	{
		nTime = 0;
		vPos.x = vPos.y = vPos.z = 0;
		vRotLog.x = vRotLog.y = vRotLog.z = 0;
	}

	AUTO_STRUCT_INFO
};


// This structure only for intrmediate results!
struct CryKeyPQ
{
	int nTime;
	Vec3 vPos;
	CryQuat vRot; // logarithm of the rotation

	// resets to initial position/rotation/time
	void reset ()
	{
		nTime = 0;
		vPos.x = vPos.y = vPos.z = 0;
		vRot.v.x = vRot.v.y = vRot.v.z = vRot.w;
	}

};


//========================================
//Controller Chunk Header
//========================================
enum CtrlTypes 
{ 
	CTRL_NONE, 
	CTRL_CRYBONE, 
	CTRL_LINEER1,	CTRL_LINEER3,	CTRL_LINEERQ, 
	CTRL_BEZIER1,	CTRL_BEZIER3,	CTRL_BEZIERQ, 
	CTRL_TCB1,		CTRL_TCB3,		CTRL_TCBQ,
	CTRL_BSPLINE_2O, // 2-byte fixed values, open
	CTRL_BSPLINE_1O, // 1-byte fixed values, open
	CTRL_BSPLINE_2C, // 2-byte fixed values, closed
	CTRL_BSPLINE_1C,  // 1-byte fixed values, closed
	CTRL_CONST       // constant position&rotation
};

enum CtrlFlags
{
	CTRL_ORT_CYCLE = 0x01,
	CTRL_ORT_LOOP = 0x02
};

// descriptor-header of the controller chunk, precedes the controller data
struct CONTROLLER_CHUNK_DESC_0826
{
	enum {VERSION=0x0826};

	CHUNK_HEADER	chdr;

	CtrlTypes		type;		//one ot the CtrlTypes values
	int				nKeys;		// # of keys this controller has; toatl # of knots (positional and orientational) in the case of B-Spline

	//unsigned short nSubCtrl;	// # of sub controllers; not used now/reserved
	unsigned int nFlags;		// Flags of controller.
	//int				nSubCtrl;	// # of sub controllers; not used now/reserved

  unsigned nControllerId; // unique generated in exporter id based on crc32 of bone name

	AUTO_STRUCT_INFO
};

// intermediate version between the CONTROLLER_BSPLINE_DATA_0826 and CONTROLLER_CHUNK_DESC_0826
// followed by CryKeyPQLog array
struct CONTROLLER_CHUNK_DESC_0827
{
	enum {VERSION = 0x0827};
	unsigned numKeys;
	unsigned nControllerId;

	AUTO_STRUCT_INFO
};


// The same as CONTROLLER_CHUNK_DESC_0827 but contain CHUNK_HEADER
// followed by CryKeyPQLog array
struct CONTROLLER_CHUNK_DESC_0828
{
	enum {VERSION = 0x0828};
	CHUNK_HEADER	chdr;

	unsigned numKeys;
	unsigned nControllerId;

	AUTO_STRUCT_INFO
};

struct BASE_CONTROLLER_CHUNK
{

};
// The same as CONTROLLER_CHUNK_DESC_0827 but contain CHUNK_HEADER
// followed by CryKeyPQLog array
struct CONTROLLER_CHUNK_DESC_0829 : public BASE_CONTROLLER_CHUNK
{
	enum {VERSION = 0x0829};

	enum { eKeyTimeRotation = 0, eKeyTimePosition = 1, eKeyTimeScale = 2 };
	CHUNK_HEADER	chdr;

	/*
	unsigned nControllerId;

	// Bitset with information of rotation info
	uint32 RotationFormat;
	uint32 numRotationKeys;
	// if we have rotation information we ALWAYS have a keytimes info
	uint32 PositionFormat;
	uint32 numPositionKeys;
	uint32 PositionKeysInfo;

	uint32 ScaleFormat;
	uint32 numScaleKeys;
	uint32 ScaleKeysInfo; 

	*/

	unsigned int nControllerId;

	uint16 numRotationKeys;
	uint16 numPositionKeys;
	uint8 RotationFormat;
	uint8 RotationTimeFormat;
	uint8 PositionFormat;
	uint8 PositionKeysInfo;
	uint8 PositionTimeFormat;

	AUTO_STRUCT_INFO
};

struct CONTROLLER_CHUNK_DESC_0830 : public BASE_CONTROLLER_CHUNK
{
	enum {VERSION = 0x0830};

	enum { eKeyTimeRotation = 0, eKeyTimePosition = 1, eKeyTimeScale = 2 };

//	enum {eRPWavelet = 0, eRWaveletPRaw = 1, ePRawPWavelet = 2, eRPRaw = 3 };
	CHUNK_HEADER	chdr;


	unsigned int nControllerId;

	uint16 numRotationKeys;
	uint16 numPositionKeys;
	uint8 RotationFormat;
	uint8 RotationTimeFormat;
	uint8 PositionFormat;
	uint8 PositionKeysInfo;
	uint8 PositionTimeFormat;
	uint8 ChunkType;

	AUTO_STRUCT_INFO
};

// Removed PQLog stuff, as intermediate result now we using floats
// followed by CryKeyPQLog array
struct CONTROLLER_CHUNK_DESC_0831
{
	enum {VERSION = 0x0831};
	unsigned numKeys;
	unsigned nControllerId;
};


struct CONTROLLER_CHUNK_DESC_0900
{
	enum {VERSION = 0x0900};

	CHUNK_HEADER	chdr;

	uint32 numKeyPos;
	uint32 numKeyRot;
	uint32 numKeyTime;
	uint32 numAnims;

	AUTO_STRUCT_INFO
};

struct CONTROLLER_CHUNK_DESC_0901
{
	enum {VERSION = 0x0901};

	CHUNK_HEADER	chdr;

	uint32 numTimesteps;

	AUTO_STRUCT_INFO
};

struct CONTROLLER_CHUNK_DESC_0902
{
	enum {VERSION = 0x0902};

	CHUNK_HEADER	chdr;

	uint32 numStartDirs;

	AUTO_STRUCT_INFO
};

struct CONTROLLER_CHUNK_DESC_0903
{
	enum {VERSION = 0x0903};

	CHUNK_HEADER	chdr;

	uint32 numKeyPos;
	uint32 numKeyRot;
	uint32 numKeyTime;
	uint32 numAnims;

	AUTO_STRUCT_INFO
};

struct CONTROLLER_CHUNK_DESC_0904
{
	enum {VERSION = 0x0904};

	CHUNK_HEADER	chdr;

	uint32 numKeyPos;
	uint32 numKeyRot;
	uint32 numKeyTime;
	uint32 numAnims;

	AUTO_STRUCT_INFO
};

//struct CONTROLLER_CHUNK_DESC_0830
//{
//	enum {VERSION = 0x0830};
//
//	enum { eKeyTimeRotation = 0, eKeyTimePosition = 1, eKeyTimeScale = 2 };
//	CHUNK_HEADER	chdr;
//
//	unsigned int nControllerId;
//
//	// Bitset with information of rotation info
//
//	uint16 numRotationKeys;
//	uint16 numPositionKeys;
//	uint8 RotationFormat;
//	uint8 RotationTimeFormat;
//	uint8 PositionFormat;
//	uint8 PositionKeysInfo;
//
//
//	AUTO_STRUCT_INFO
//};


// descriptor of B-Spline controller
struct CONTROLLER_BSPLINE_DATA_0826
{
	enum {VERSION = 0x0826};
	
	// spline degree, normally 3 (cubic)
	// degree 0 is piecewise-constant (CP=K-1), -1 is constant (always 0 knots, 1 control point)
	int nDegree; 
	
	// number of knots; number of Control Points is deduced from
	// the number of knots and the degree: CP = K + D - 1
	int nKnots;
};

typedef CONTROLLER_BSPLINE_DATA_0826 CONTROLLER_BSPLINE_DATA;


#define CRY_RENDERMODE_SHADOW   0
#define CRY_RENDERMODE_TEXTURED 1
#define CRY_RENDERMODE_BUMP     2
#define CRY_RENDERMODE_REFRACT  3

//========================================
//Node Chunk Header
//========================================
struct NODE_CHUNK_DESC_0823
{
	enum {VERSION = 0x0823};
	CHUNK_HEADER	chdr;

	char		name[64];

	int			ObjectID;		// ID of this node's object chunk (if present)
	int			ParentID;		// chunk ID of the parent Node's chunk
	int			nChildren;		// # of children Nodes
	int			MatID;			// Material chunk No

	bool		IsGroupHead;	
	bool		IsGroupMember;

	float	tm[4][4];				// transformation matrix
	Vec3	pos;			// pos component of matrix
	CryQuat		rot;			// rotation component of matrix
	Vec3	scl;			// scale component of matrix

	int			pos_cont_id;	// position controller chunk id
	int			rot_cont_id;	// rotation controller chunk id
	int			scl_cont_id;	// scale controller chunk id

	int			PropStrLen;		// lenght of the property string

	AUTO_STRUCT_INFO
};
typedef NODE_CHUNK_DESC_0823 NODE_CHUNK_DESC;
#define NODE_CHUNK_DESC_VERSION NODE_CHUNK_DESC::VERSION

//========================================
//Helper Chunk Header
//========================================
enum HelperTypes
{
	HP_POINT=0,
	HP_DUMMY=1,
	HP_XREF=2,
	HP_CAMERA=3,
	HP_GEOMETRY=4
};

struct HELPER_CHUNK_DESC_0744
{
	enum {VERSION = 0x0744};
	CHUNK_HEADER	chdr;

  HelperTypes	type;			// one of the HelperTypes values
	Vec3	size;			// size in local x,y,z axises (for dummy only)

	AUTO_STRUCT_INFO
};

typedef HELPER_CHUNK_DESC_0744 HELPER_CHUNK_DESC; 
#define HELPER_CHUNK_DESC_VERSION HELPER_CHUNK_DESC::VERSION

// ChunkType_BoneLightBinding
// describes the lights binded to bones
struct BONELIGHTBINDING_CHUNK_DESC_0001
{
	enum {VERSION = 0x0001};
	unsigned numBindings; // the number of bone-light binding substructures following this structure

	AUTO_STRUCT_INFO
};

// single bone-light binding structure; numBindings such structures follow the BONELIGHTBINDING_CHUNK_DESC structure
struct SBoneLightBind
{
	unsigned nLightChunkId;     // chunk id of the light that's bound to a bone
	unsigned nBoneId;           // parent bone id
	Vec3 vLightOffset;         // light position in the parent bone coordinate system
	Vec3 vRotLightOrientation; // logarithm of quaternion that describes the light orientation relative to the parent bone

	AUTO_STRUCT_INFO
};

// ChunkType_MeshMorphTarget  - morph target of a mesh chunk
// This chunk contains only the information about the vertices that are changed in the mesh
// This chunk is followed by an array of numMorphVertices structures SMeshMorphTargetVertex,
// immediately followed by the name (null-terminated, variable-length string) of the morph target.
// The string is after the array because of future alignment considerations; it may be padded with 0s.
struct MESHMORPHTARGET_CHUNK_DESC_0001
{
	enum {VERSION = 0x0001};
	unsigned nChunkIdMesh;   // the chunk id of the mesh chunk (ChunkType_Mesh) for which this morph target is
	unsigned numMorphVertices;  // number of MORPHED vertices

	AUTO_STRUCT_INFO
};


// an array of these structures follows the MESHMORPHTARGET_CHUNK_DESC_0001
// there are numMorphVertices of them
struct SMeshMorphTargetVertex
{
	unsigned nVertexId; // vertex index in the original (mesh) array of vertices
	Vec3 ptVertex;     // the target point of the morph target

	AUTO_STRUCT_INFO
};

struct SMeshMorphTargetHeader
{
	uint32 MeshID;
	uint32 NameLength;  //size of the name string
	uint32 numIntVertices; //type SMeshMorphTargetVertex
	uint32 numExtVertices; //type SMeshMorphTargetVertex

	AUTO_STRUCT_INFO
};

struct SMeshPhysicalProxyHeader
{
	uint32 ChunkID;
	uint32 numPoints;     //size of the name string
	uint32 numIndices; //type SMeshMorphTargetVertex
	uint32 numMaterials; //type SMeshMorphTargetVertex

	AUTO_STRUCT_INFO
};

//
// ChunkType_BoneInitialPos		- describes the initial position (4x3 matrix) of each bone; just an array of 4x3 matrices
// This structure is followed by 
struct BONEINITIALPOS_CHUNK_DESC_0001
{
	enum {VERSION = 0x0001};
	// the chunk id of the mesh chunk (ChunkType_Mesh) with bone info for which these bone initial positions are applicable.
	// there might be some unused bones here as well. There must be the same number of bones as in the other chunks - they're placed
	// in BoneId order.
	unsigned nChunkIdMesh; 
	// this is the number of bone initial pose matrices here
	unsigned numBones;

	AUTO_STRUCT_INFO
};

// an array of these matrices follows the  BONEINITIALPOS_CHUNK_DESC_0001 header
// there are numBones of them
// TO BE REPLACED WITH Matrix43
struct SBoneInitPosMatrix
{
	float mx[4][3];
	float* operator [] (int i) {return mx[i];}
	const float* operator [] (int i)const {return mx[i];}

	const Vec3& getOrt (int nOrt)const {return *(const Vec3*)(mx[nOrt]);}

	AUTO_STRUCT_INFO
};

//////////////////////////////////////////////////////////////////////////
// Custom Attributes chunk description.
//////////////////////////////////////////////////////////////////////////
struct EXPORT_FLAGS_CHUNK_DESC
{
	enum {VERSION = 0x0001};
	enum EFlags
	{
		MERGE_ALL_NODES = 0x0001,
	};

	CHUNK_HEADER	chdr;
	unsigned int flags; // @see EFlags
	unsigned int rc_version[4]; // Resource compiler version.
	char rc_version_string[16]; // Version as a string.
	unsigned int reserved[32];

	AUTO_STRUCT_INFO
};

struct BREAKABLE_PHYSICS_CHUNK_DESC
{
	enum {VERSION = 0x0001};
	CHUNK_HEADER	chdr;
	unsigned int granularity;
	int nMode;
	int nRetVtx;
	int nRetTets;
	int nReserved[10];

	AUTO_STRUCT_INFO
};

struct FACEMAP_CHUNK_DESC
{
	enum {VERSION = 0x0001};
	CHUNK_HEADER chdr;
	char nodeName[64];
	int StreamID;

	AUTO_STRUCT_INFO
};

#endif
