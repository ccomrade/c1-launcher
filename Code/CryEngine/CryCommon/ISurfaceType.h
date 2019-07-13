////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   ISurfaceType.h
//  Version:     v1.00
//  Created:     30/9/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: Defines interfaces to access Surface Types.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ISurfaceType_h__
#define __ISurfaceType_h__
#pragma once

//////////////////////////////////////////////////////////////////////////
// Flags that ISurfaceType::GetFlags() can return.
//////////////////////////////////////////////////////////////////////////
enum ESurfaceTypeFlags
{
	SURFACE_TYPE_NO_PHYSICALIZE         = BIT(1), // This surface should not be physicalized.
	SURFACE_TYPE_NO_COLLIDE             = BIT(2), // Should only be set for vegetation canopy mats
	SURFACE_TYPE_VEHICLE_ONLY_COLLISION = BIT(3),
	SURFACE_TYPE_CAN_SHATTER            = BIT(4), // This surface type can shatter
};

// Parameter structure passed to ISurfaceType::Execute
struct SSurfaceTypeExecuteParams
{
	Vec3 hitPoint;
	Vec3 hitNormal;
	int hitType;
};

#define SURFACE_BREAKAGE_TYPE(x) x

//////////////////////////////////////////////////////////////////////////
// Surface definition.
//////////////////////////////////////////////////////////////////////////
struct ISurfaceType
{
	//////////////////////////////////////////////////////////////////////////
	struct SSurfaceTypeAIParams
	{
		float fImpactRadius;
		float fImpactSoundRadius;
		float fFootStepRadius;
		float proneMult;
		float crouchMult;
		float movingMult;

		SSurfaceTypeAIParams()
		{
			fImpactRadius = 2.5f;
			fImpactSoundRadius = 20.0f;
			fFootStepRadius = 20.0f;
			proneMult = 0.2f;
			crouchMult = 0.5f;
			movingMult = 2.5f;
		}
	};
	struct SPhysicalParams
	{
		int breakable_id;
		int break_energy;
		float hole_size;
		float hit_radius;
		float hit_points;
		float hit_maxdmg;
		float hit_lifetime;
		int pierceability;
		float friction;
		float bouncyness;
		int iBreakability;
	};
	struct SBreakable2DParams
	{
		string particle_effect;
		float blast_radius;
		int rigid_body;
		float life_time;
		float cell_size;
		int max_patch_tris;
		float shard_density;

		SBreakable2DParams() : blast_radius(0),rigid_body(0),life_time(0),cell_size(0),max_patch_tris(0),shard_density(0) {}
	};
	struct SBreakageParticles
	{
		string type;
		string particle_effect;
		int count_per_unit;
		float count_scale;
		float scale;
		SBreakageParticles() : count_per_unit(1),count_scale(1),scale(1) {}
	};

	// Releases surface type.
	virtual void Release() = 0;

	// Return unique Id of this surface type.
	// Maximum of 65535 simultanious surface types can exist.
	virtual uint16 GetId() = 0;

	// Unique name of the surface type.
	virtual const char *GetName() = 0;

	// Typename of this surface type.
	virtual const char *GetType() = 0;

	// Flags of the surface type.
	// Return:
	//    A combination of ESurfaceTypeFlags flags.
	virtual int  GetFlags() = 0;

	// Execute material.
	virtual void Execute( SSurfaceTypeExecuteParams &params ) = 0;

	// returns a some cached properties for faster access
	virtual int GetBreakability() = 0;
	virtual float GetBreakEnergy() = 0;
	virtual int GetHitpoints() = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual const SPhysicalParams& GetPhyscalParams() = 0;

	// Optional AI Params.
	virtual const SSurfaceTypeAIParams* GetAIParams() = 0;

	// Optional params for 2D breakable plane.
	virtual SBreakable2DParams* GetBreakable2DParams() = 0;
	virtual SBreakageParticles* GetBreakageParticles( const char *sType,bool bLookInDefault=true ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Called by Surface manager.
	//////////////////////////////////////////////////////////////////////////
	// Loads surface, (do not use directly).
	virtual bool Load( int nId ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    This interface is used to enumerate all items registered to the surface type manager.
//////////////////////////////////////////////////////////////////////////
struct ISurfaceTypeEnumerator
{
	virtual void Release() = 0;
	virtual ISurfaceType* GetFirst() = 0;
	virtual ISurfaceType* GetNext() = 0;
};

// Description:
//    Manages loading and mapping of physical surface materials to Ids and materials scripts.
// Behaviour:
//    At start will enumerate all material names.
//    When the surface is first time requested by name it will be loaded and cached 
//    and new unique id will be generated for it.
struct ISurfaceTypeManager
{
	// Description:
	//    Return surface type by name.
	//    If surface is not yet loaded it will be loaded and and cached.
	// Arguments:
	//    sName - Name of the surface type ("mat_metal","mat_wood", etc..)
  //    warn - print warning message if surface not found
	virtual ISurfaceType* GetSurfaceTypeByName( const char *sName,const char *sWhy=NULL,bool warn=true ) = 0;

	// Description:
	//    Return surface type by id.
	//    If surface is not yet loaded it will be loaded and and cached.
	// Arguments:
	//    sName - Name of the surface type ("mat_metal","mat_wood", etc..)
	virtual ISurfaceType* GetSurfaceType( int nSurfaceId,const char *sWhy=NULL ) = 0;

	// Description:
	//    Retrieve an interface to the enumerator class that allow to iterate over all surface types.
	virtual ISurfaceTypeEnumerator* GetEnumerator() = 0;

	// Register a new surface type.
	virtual bool RegisterSurfaceType( ISurfaceType *pSurfaceType,bool bDefault=false ) = 0;
	virtual void UnregisterSurfaceType( ISurfaceType *pSurfaceType ) = 0;
};

#endif // __ISurfaceType_h__

