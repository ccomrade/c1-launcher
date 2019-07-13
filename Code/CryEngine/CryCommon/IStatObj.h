//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//	
//	File:IStatObj.h
//  Interface for CStatObj class
//
//	History:
//	-:Created by Vladimir Kajalin
//
//////////////////////////////////////////////////////////////////////

#ifndef _IStatObj_H_
#define _IStatObj_H_

// forward declarations
//////////////////////////////////////////////////////////////////////
struct    IMaterial;
struct		ShadowMapFrustum;
class			CRenderObject;
struct		IShader;
class			CDLight;

//! Interface to non animated object
struct phys_geometry;
struct IChunkFile;

#include "Cry_Math.h"
#include "Cry_Geo.h"
#include "IPhysics.h"

//////////////////////////////////////////////////////////////////////////
// Type of static sub object.
//////////////////////////////////////////////////////////////////////////
enum EStaticSubObjectType
{
	STATIC_SUB_OBJECT_MESH,         // This simple geometry part of the multi-sub object geometry.
	STATIC_SUB_OBJECT_HELPER_MESH,  // Special helper mesh, not rendered usually, used for broken pieces.
	STATIC_SUB_OBJECT_POINT,
	STATIC_SUB_OBJECT_DUMMY,
	STATIC_SUB_OBJECT_XREF,
	STATIC_SUB_OBJECT_CAMERA,
	STATIC_SUB_OBJECT_LIGHT,
};

//////////////////////////////////////////////////////////////////////////
// Flags that can be set on static object.
//////////////////////////////////////////////////////////////////////////
enum EStaticObjectFlags
{
	STATIC_OBJECT_HIDDEN  =    BIT(0), // When set static object will not be displayed.
	STATIC_OBJECT_CLONE		=	   BIT(1), // specifies whether this object was cloned for modification
	STATIC_OBJECT_GENERATED  = BIT(2), // tells that the object was generated procedurally (breakable obj., f.i.)
	STATIC_OBJECT_CANT_BREAK = BIT(3), // StatObj has geometry unsuitable for procedural breaking
	STATIC_OBJECT_DEFORMABLE = BIT(4), // StatObj can be procedurally smeared (using SmearStatObj)
	STATIC_OBJECT_COMPOUND   = BIT(5), // StatObj has subobject meshes

	// Special flags.
	STATIC_OBJECT_SPAWN_ENTITY    = BIT(20), // StatObj spawns entity when broken.
	STATIC_OBJECT_PICKABLE        = BIT(21), // StatObj can be picked by players.
  STATIC_OBJECT_NO_AUTO_HIDEPOINTS = BIT(22), // Do not generate AI auto hide points around object if it's dynamic
};


struct SRayHitInfo
{
  SRayHitInfo() 
  {
    memset(this,0,sizeof(*this));
  }
	//////////////////////////////////////////////////////////////////////////
	// Input parameters.
	Vec3    inReferencePoint;
	Ray     inRay;
	bool    bInFirstHit;
	bool    bUseCache;
  bool    bOnlyZWrite;
	float   fMaxHitDistance; // When not 0, only hits with closer distance will be registered.

	//////////////////////////////////////////////////////////////////////////
	// Output parameters.
	float fDistance; // Distance from reference point.
	Vec3 vHitPos;
	Vec3 vHitNormal;
	int  nHitMatID; // Material Id that was hit.
	int  nHitSurfaceID; // Material Id that was hit.
  struct IRenderMesh * pRenderMesh;
  struct IStatObj * pStatObj;
};

// Summary:
//     Interface to hold static object data
struct IStatObj
{
	//////////////////////////////////////////////////////////////////////////
	// SubObject
	//////////////////////////////////////////////////////////////////////////
	struct SSubObject
	{
		EStaticSubObjectType nType;
		string name;
		string properties;
		int nParent;          // Index of the parent sub object, if there`s hierarchy between them.
		Matrix34 tm;          // Transformation matrix.
		Matrix34 localTM;     // Local transformation matrix, relative to parent.
		IStatObj *pStatObj;   // Static object for sub part of CGF.
		Vec3 helperSize;      // Size of the helper (if helper).
		struct IRenderMesh *pWeights; // render mesh with a single deformation weights stream
		unsigned int bIdentityMatrix : 1; // True if sub object matrix is identity.
		unsigned int bHidden : 1; // True if sub object is hidden.
	};
	//////////////////////////////////////////////////////////////////////////

	// Description:
	//     Increase the reference count of the object.
	// Summary:
	//     Notifies that the object is being used
	virtual void AddRef() = 0;

	// Description:
	//     Decrease the reference count of the object. If the reference count 
	//     reaches zero, the object will be deleted from memory.
	// Summary:
	//     Notifies that the object is no longer needed
	virtual void Release() = 0;

	// Description:
	//     Set static object flags.
	// Arguments:
	//     nFlags - flags to set, a combination of EStaticObjectFlags values.
	virtual void SetFlags( int nFlags ) = 0;
	
	// Description:
	//     Retrieve flags set on the static object.
	virtual int  GetFlags() = 0;
	
	// Description:
	//     Provide access to the faces, vertices, texture coordinates, normals and 
	//     colors of the object used later for CRenderMesh construction.
	// Return Value:
	//     
	// Summary:
	//     Get the object source geometry
	virtual struct IIndexedMesh * GetIndexedMesh(bool bCreateIfNone=false) = 0;

	//! Access to rendering geometry for indoor engine ( optimized vert arrays, lists of shader pointers )
	virtual struct IRenderMesh * GetRenderMesh()=0;
	//! Assign RenderMesh to static object.
	virtual void SetRenderMesh( struct IRenderMesh *pRM ) = 0;

	// Description:
	//     Returns the physical representation of the object.
	// Arguments:
	//     nType - Pass 0 to get the physic geometry or pass 1 to get the obstruct geometry
	// Return Value:
	//     A pointer to a phys_geometry structure. 
	// Summary:
	//     Get the physic representation
	virtual phys_geometry * GetPhysGeom(int nType = 0)=0;

	// Description:
	//     Creates a skinnable object instance for foliage simulation
	// Arguments:
	//     pTrunk - existing physical entity of the trunk (foliage gets attached to it)
	//     mtxWorld - its world matrix
	//     pRes - destination pointer (if the object is deleted after timeouting, it will write 0 there)
	//     lifeTime - idle time after which the object gets deleted
	// Return Value:
	//     the number of physicalized leaves
	// Summary:
	//     physicalizes StatObj's foliage
	virtual int PhysicalizeFoliage(IPhysicalEntity *pTrunk, const Matrix34 &mtxWorld, struct IFoliage *&pRes, float lifeTime=0.0f, int iSource=0) = 0;

	// Description:
	//		 Updates rendermesh's vertices, normals, and tangents with the data provided
	// Summary:
	//		 Updates vertices in the range [iVtx0..iVtx0+nVtx-1], vertices are in their original order
	//		 (as they are physicalized). Clones the object if necessary to make the modifications
	// Return Value:
	//     modified IStatObj (a clone or this one, if it's already a clone)
	virtual IStatObj *UpdateVertices(strided_pointer<Vec3> pVtx, strided_pointer<Vec3> pNormals, int iVtx0,int nVtx, int *pVtxMap=0) = 0;

	// Description:
	//     Copies foliage data to another statobj
	// Arguments:
	//     pObjDst - target stat obj
	//     bMove - true if the data needs to be removed from the original statobj
	// Return Value:
	// Summary:
	//     Copies foliage data to another statobj
	virtual void CopyFoliageData(IStatObj *pObjDst, bool bMove=false, IFoliage *pSrcFoliage=0, int *pVtxMap=0, 
		primitives::box *pMovedBoxes=0,int nMovedBoxes=-1) = 0;

	// Description:
	//     Sets and replaces the physical representation of the object.
	// Arguments:
	//		 pPhysGeom - A pointer to a phys_geometry class. 
	//     nType - Pass 0 to set the physic geometry or pass 1 to set the obstruct geometry
	// Summary:
	//     Set the physic representation
	virtual void SetPhysGeom(phys_geometry *pPhysGeom, int nType = 0)=0;

	// Description:
	//     Returns a tetrahedral lattice, if any (used for breakable objects)
	virtual ITetrLattice * GetTetrLattice() = 0;

  virtual float GetAIVegetationRadius() const = 0;
  virtual void SetAIVegetationRadius(float radius) = 0;

	// Description:
	//     Set default material for the geometry.
	// Arguments:
	//     pMaterial - A valid pointer to the material.
	virtual void SetMaterial( IMaterial *pMaterial ) = 0;

	// Description:
	//     Returns default material of the geometry.
	// Arguments:
	//     nType - Pass 0 to get the physic geometry or pass 1 to get the obstruct geometry
	// Return Value:
	//     A pointer to a phys_geometry class. 
	virtual IMaterial* GetMaterial() = 0;

	// Return Value:
	//     A Vec3 object countaining the bounding box.
	// Summary:
	//     Get the minimal bounding box component
	virtual Vec3 GetBoxMin()=0;

	// Return Value:
	//     A Vec3 object countaining the bounding box.
	// Summary:
	//     Get the minimal bounding box component
	virtual Vec3 GetBoxMax()=0;

	// Arguments:
	//     Minimum bounding box component
	// Summary:
	//     Set the minimum bounding box component
	virtual void	SetBBoxMin(const Vec3 &vBBoxMin)=0;

	// Arguments:
	//     Minimum bounding box component
	// Summary:
	//     Set the minimum bounding box component
	virtual void	SetBBoxMax(const Vec3 &vBBoxMax)=0;

	// Summary:
	//     Get the object radius
	// Return Value:
	//     A float containing the radius
	virtual float GetRadius()=0;

	// Description:
	//     Reloads one or more component of the object. The possible flags 
	//     are FRO_SHADERS, FRO_TEXTURES and FRO_GEOMETRY.
	// Arguments:
	//     nFlags - One or more flag which indicate which element of the object
	//     to reload
	// Summary:
	//     Reloads the object
	virtual void Refresh(int nFlags)=0;

	// Description:
	//     Registers the object elements into the renderer.
	// Arguments:
	//     rParams   - Render parameters
	//     nLogLevel - Level of the LOD
	// Summary:
	//     Renders the object
	virtual void Render(const struct SRendParams &rParams)=0;

	// Summary:
	//     Get the bounding box
	// Arguments:
	//     Mins - Position of the bottom left close corner of the bounding box
	//     Maxs - Position of the top right far corner of the bounding box
	virtual AABB GetAABB()=0;

	// Summary:
	//     Generate a random point in object.
	// Arguments:
	//		 geo - Query results and context
	//     eType - Object geometry to use (physics or render geometry)
	//     eForm - Object aspect to generate on (surface, volume, etc)
	//		 tWorld - World transformation of render object.
	virtual float ComputeExtent(GeomQuery& geo, EGeomForm eForm)=0;
	virtual void GetRandomPos(RandomPos& ran, GeomQuery& geo, EGeomForm eForm)=0;

	// Description:
	//     Returns the LOD object, if present.
	// Arguments:
	//     nLodLevel - Level of the LOD
	//     bReturnNearest - if true will return nearest available LOD to nLodLevel.
	// Return Value:
	//     A static object with the desired LOD. The value NULL will be return if there isn't any LOD object for the level requested.
	// Summary:
	//     Get the LOD object
	virtual IStatObj* GetLodObject( int nLodLevel,bool bReturnNearest=false )=0;

	// Description:
	//     Returns a light source specified by the id.
	// Arguments:
	//     nId - Id of the light source
	// Return Value:
	//     A pointer to a CDLight object will be returned. In case the id 
	//     specified as parameter was out of range, the value NULL will be 
	//     returned.
	// Summary:
	//     Get the light sources
//	virtual const CDLight * GetLightSources(int nId) = 0;

	// Summary:
	//     Returns the folder name of the object
	// Return Value:
	//     A null terminated string which contain the folder name of the object.
	virtual const char *GetFolderName()=0;

	// Summary:
	//     Returns the filename of the object
	// Return Value:
	//     A null terminated string which contain the filename of the object.
	virtual	const char *GetFilePath()=0;		

	// Summary:
	//     Set the filename of the object
	// Arguments:
	//     szFileName - New filename of the object
	// Return Value:
	//		 None
	virtual	void SetFilePath(const char * szFileName)=0;		

	// Summary:
	//     Returns the name of the geometry
	// Return Value:
	//     A null terminated string which contains the name of the geometry
	virtual	const char *GetGeoName()=0;		

	// Summary:
	//     Compares if another object is the same
	// Arguments:
	//     szFileName - Filename of the object to compare
	//     szGeomName - Geometry name of the object to compare (optional)
	// Return Value:
	//     A boolean which equals to true in case both object are the same, or false in the opposite case.
	virtual bool IsSameObject(const char * szFileName, const char * szGeomName)=0;

	// Description:
	//     Will return the position of the helper named in the argument. The 
	//     helper should have been specified during the exporting process of 
	//     the cgf file.
	// Arguments:
	//     szHelperName - A null terminated string holding the name of the helper
	// Return Value:
	//     A Vec3 object which contains the position.
	// Summary:
	//     Gets the position of a specified helper
	virtual Vec3 GetHelperPos(const char * szHelperName)=0;
 
  // Summary:
  //     Gets the transformation matrix of a specified helper, see GetHelperPos  
  virtual const Matrix34& GetHelperTM(const char* szHelperName)=0;

	//! Tell us if the object is not found 
	virtual bool IsDefaultObject()=0;

	// Summary:
	//     Free the geometry data
	virtual void FreeIndexedMesh() = 0;

	// Pushes the underlying tree of objects into the given Sizer object for statistics gathering
	virtual void GetMemoryUsage(class ICrySizer* pSizer) {} // TODO: implement

//DOC-IGNORE-BEGIN
	//! used for sprites
	virtual float & GetRadiusVert() = 0;

	//! used for sprites
	virtual float & GetRadiusHors() = 0;
//DOC-IGNORE-END

	// Summary:
	//     Determines if the object has physics capabilities
	virtual bool IsPhysicsExist() = 0;

	// Summary:
	//     Starts preloading textures, shaders and sprites
	virtual void PreloadResources(float fDist, float fTime, int dwFlags) = 0;

	// Summary: 
	//     Returns a pointer to the object
	// Return Value:
	//     A pointer to the current object, which is simply done like this "return this;"
	virtual struct IStatObj* GetIStatObj() {return this;}

	// Summary:
	//     Invalidates geometry inside IStatObj, will mark hosted IIndexedMesh as invalid.
	// Arguments:
	//     bPhysics - if true will also recreate physics for indexed mesh.
	virtual void Invalidate( bool bPhysics=false ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Interface to the Sub Objects.
	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    Retrieve number of sub-objects.
	virtual int GetSubObjectCount() const = 0;
	// Summary:
	//    Sets number of sub-objects.
	virtual void SetSubObjectCount( int nCount ) = 0;
	// Summary:
	//    Retrieve sub object by index, where 0 <= nIndex < GetSubObjectCount()
	virtual SSubObject* GetSubObject( int nIndex ) = 0;
	// Summary:
	//    Check if this object is sub object of another IStatObj.
	virtual bool IsSubObject() const = 0;
	// Summary:
	//    Retrieve parent static object, only relevant when this IStatObj is Sub-object.
	virtual IStatObj* GetParentObject() const = 0;
	// Summary:
	//    Retrieve the static object, from which this one was cloned (if that is the case)
	virtual IStatObj* GetCloneSourceObject() const = 0;
	// Summary:
	//    Find sub-pbject by name.
	virtual SSubObject* FindSubObject( const char *sNodeName ) = 0;
	//    Remove Sub-Object.
	virtual bool RemoveSubObject( int nIndex ) = 0;
	//    Copy Sub-Object.
	virtual bool CopySubObject( int nToIndex,IStatObj *pFromObj,int nFromIndex ) = 0;
	// Summary:
	//		Adds subobjects to pent, meshes as parts, joint helpers as breakable joints
	virtual void PhysicalizeSubobjects(IPhysicalEntity *pent, const Matrix34 *pMtx, float mass,float density=0.0f, int id0=0) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Save contents of static object to the CGF file.
	//////////////////////////////////////////////////////////////////////////
	// Save object to the CGF file.
	// Arguments:
	//    sFilename - Filename of the CGF file.
	//    pOutChunkFile - Optional output parameter if specified file will not be saved to disck instead it will return pointer to the 
	//                    IChunkFile interface with filled CGF chunks.
	//                    Caller of the function is responsible to call Release method of IChunkFile to release it later.
	virtual bool SaveToCGF( const char *sFilename,IChunkFile** pOutChunkFile=NULL ) = 0;
	
	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    Clones static geometry, Makes an exact copy of the Static object and the contained geometry.
	virtual IStatObj* Clone(bool bCloneChildren=true) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    Clones static geometry, Makes an exact copy of the Static object and the contained geometry.
	virtual IStatObj* Clone(bool bCloneGeometry,bool bCloneChildren,bool bMeshesOnly) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    prepares object to be used in smear calculations (via SmearStatObj)
	// Arguments:
	//    pInternalGeom - physicalized interio geometry, smeared vertices will not be able to go inside it
	virtual void MarkAsDeformable(IGeometry *pInternalGeom=0) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    returns a slot index of the deformation morph counterpart of this slot; -1 if none
	virtual int SubobjHasDeformMorph(int iSubObj) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    makes sure that both objects have one-to-one vertex correspondance
	//    sets MorphBuddy for this object's render mesh
	//    returns 0 if failed (due to objects having no vertex maps most likely)
	virtual int SetDeformationMorphTarget(IStatObj *pDeformed) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//		chages the weights of the deformation morphing according to point, radius, and strength
	//      (radius==0 updates all weights of all vertices)
	//    if the object is a compound object, updates the weights of its subobjects that have deformation morphs; clones the object if necessary
	//    otherwise, updates the weights passed as a pWeights param
	virtual IStatObj *DeformMorph(const Vec3 &pt,float r,float strength, IRenderMesh *pWeights=0) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//		hides all non-physicalized geometry, clones the object if necessary
	virtual IStatObj *HideFoliage() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    return amount of texture memory used for vegetation sprites
//	virtual int GetSpritesTexMemoryUsage() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Summary:
	//    serializes the StatObj's mesh into a stream
	virtual int Serialize(TSerialize ser) = 0;

	// Get object properties as loaded from CGF.
	virtual const char* GetProperties() = 0;

	// Get physical properties specified for object.
	virtual bool GetPhysicalProperties( float &mass,float &density ) = 0;

	// Returns the last B operand for this object as A, along with its relative scale
	virtual IStatObj *GetLastBooleanOp(float &scale) = 0;

	// Intersect ray with static object.
	// Ray must be in object local space.
	virtual bool RayIntersection( SRayHitInfo &hitInfo,IMaterial *pCustomMtl=0 ) = 0;

protected:
  virtual ~IStatObj() {}; // should be never called, use Release() instead
};


#endif // _IStatObj_H_
