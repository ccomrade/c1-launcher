////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2004.
// -------------------------------------------------------------------------
//  File name:   IInterestSystem.h
//  Version:     v1.00
//  Created:     08/03/2007 by Matthew Jack
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IInterestSystem_h__
#define __IInterestSystem_h__
#pragma once

#include "IEntity.h"

// Forward declarations
struct SEntityInterest;
class CAIActor;

class ICentralInterestManager
{
public:
	// Reset the CIM system
	// Do this when all caches should be emptied, e.g. on loading levels
	// Resetting during game should do no serious harm
	virtual void Reset() = 0;

	// Enable or disable (suppress) the interest system
	virtual void Enable(bool bEnable) = 0;

	// Is the Interest System enabled?
	virtual bool IsEnabled(void) = 0;

	// Update the CIM
	virtual void Update( float fDelta ) = 0;

	// Register/update an interesting entity
	// Returns true if this was accepted as a useful interest target
	// If the object is already registered, it will be updated and still return true
	virtual bool RegisterInterestingEntity( IEntity *pEntity, float fBaseInterest ) = 0;

	// Register a potential interested AIActor
	// Returns true if this was accepted as a potentially useful interested entity
	// If the object is already registered, it will be updated and still return true
	virtual bool RegisterInterestedAIActor( IEntity *pEntity, float fForceUntilDistance ) = 0;

	// Set the maximum number of AIActors that can have a PIM at once, for performance control
	// Shouldn't change often, so throws out PIMs without prioritisation
	virtual void SetMaxNumInterested(int nMaxPIMs) = 0;

	// Set the maximum number of registered interesting objects, for performance control
	// This does prioritise when the number is reduced
	virtual void SetMaxNumInteresting(int nMaxInteresting ) = 0;
};

//---------------------------------------------------------------------//

class IPersonalInterestManager
{
public:
	// Clear tracking cache, don't clear assignment
	virtual void Reset(void) = 0;

	// Re(assign) to an actor (helps in object re-use)
	// NULL is acceptable, to leave unassigned
	// You must also ensure the PIM pointer in the CAIActor is set to this object
	virtual void Assign( CAIActor* pAIActor ) = 0;

	// Get the currently assigned AIActor
	virtual CAIActor *GetAssigned(void) = 0;

	// Get/set minimum relevant interest level
	virtual void SetMinimumInterest( float fMinInterest ) = 0;
	virtual float GetMinimumInterest( void ) = 0;

	// Update
	virtual void Update( float fDelta ) = 0;

	// Consider this interesting object
	// May store it in the tracking cache
	// Returns the initial interest evaluation
	virtual float ConsiderInterestEntity ( const SEntityInterest &entInterest ) = 0;

	// Do we have any interest target right now?
	// Allows us to move on as quickly as possible for the common (uninterested) case
	virtual bool IsInterested(void) = 0;

	// Get the position we should be looking at
	// Undefined if uninterested
	virtual Vec3 GetInterestPoint(void) = 0;

	// Which entity are we currently interested in?
	// Returns NULL if not currently interested in anything, or that target is not an entity
	virtual IEntity * GetInterestEntity(void) = 0;
};

#endif //__IInterestSystem_h__