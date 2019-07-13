////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IEntitySerialize.h
//  Version:     v1.00
//  Created:     24/5/2004 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IEntitySerialize_h__
#define __IEntitySerialize_h__
#pragma once

struct EntityCloneState
{
	//! constructor
	EntityCloneState()
	{
		m_bLocalplayer=false; 
		m_bSyncYAngle=true;
		m_bSyncAngles=true;
		m_bSyncPosition=true;
		m_fWriteStepBack=0;
		m_bOffSync=true;
		m_pServerSlot=0;
	}

	//! destructor 
	EntityCloneState(const EntityCloneState& ecs)
	{
		m_pServerSlot=ecs.m_pServerSlot;
		m_v3Angles=ecs.m_v3Angles;
		m_bLocalplayer=ecs.m_bLocalplayer;
		m_bSyncYAngle=ecs.m_bSyncYAngle;
		m_bSyncAngles=ecs.m_bSyncAngles;
		m_bSyncPosition=ecs.m_bSyncPosition;
		m_fWriteStepBack=ecs.m_fWriteStepBack;
		m_bOffSync=ecs.m_bOffSync;
	}

	// ------------------------------------------------------------------------------

	class CXServerSlot*	m_pServerSlot;			//!< destination serverslot, 0 if not used
	Vec3						m_v3Angles;					//!<
	bool						m_bLocalplayer;			//!< say if this entity is the entity of the player
	bool						m_bSyncYAngle;			//!< can be changed dynamically (1 bit), only used if m_bSyncAngles==true, usually not used by players (roll)
	bool						m_bSyncAngles;			//!< can be changed dynamically (1 bit)
	bool						m_bSyncPosition;		//!< can be changed dynamically (1 bit)
	float						m_fWriteStepBack;		//!<
	bool						m_bOffSync;					//!<
};

//////////////////////////////////////////////////////////////////////////
// Description:
//    IEntitySerializer interface is passed to IEntity::Serialize method.
//    It provides entity with all the data needed to serialize or deserialize an entity.
//////////////////////////////////////////////////////////////////////////
struct IEntitySerializationContext
{
	// Description:
	//    Call to release this interface.
	virtual void Release() = 0;

	// Description:
	//    Assign stream to save or load entity.
	virtual void SetStream( CStream *pStream );

	// Description:
	//    Returns currently assigned stream.
	// Returns:
	//    Always return valid CStream pointer, no need to check if stream is NULL.
	virtual CStream* GetStream() const = 0;

	// Description:
	//    Assign current clone state.
	virtual void SetCloneState( EntityCloneState *pCloneState );

	// Description:
	//    Returns currently Assign current clone state.
	virtual EntityCloneState* GetCloneState() const = 0;
};

#endif // __IEntitySerialize_h__

