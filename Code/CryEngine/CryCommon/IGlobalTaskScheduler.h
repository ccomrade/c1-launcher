// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#pragma once

struct IGlobalTaskScheduler
{
	// Arguments:
	//   szTaskTypeName - !=0, needed to display debug info and to esimate further task of the same type, the scheduler keeps the pointer, does not make a copy or frees the memory
	//   fDesiredDelayTimeInMS - <= fMaxDelayTimeInMS, in seconds
	//   fMaxDelayTimeInMS - >= fDesiredDelayTimeInMS, in seconds
	//   dwUnits - !=0
	//   fInitalTimePerUnitEstimate - this time is the inital estimate - it becomes corrected once we execute the task
	// Returns:
	//   true=task was successfully added, false=task existed already
	virtual bool AddTask( void *pObjectIdentifier, const char *szTaskTypeName,
		const float fDesiredDelayTimeInMS, const float fMaxDelayTimeInMS, const uint32 dwUnits, const float fInitalTimePerUnitEstimate )=0;

	// to query if a registered task can run this frame
	// needs to be called for every task every frame
	// if this returns !=0, call FinishQuery() afterwards
	// this is needed to gather timing statistics
	// only one query can be started, call FinishQuery() before starting the next one
	virtual uint32 StartQuery( void *pObjectIdentifier )=0;

	// needed after call StartQuery()
	virtual void FinishQuery()=0;

	// needs to be called per frame once - in the beginning of each frame would be good
	// Arguments:
	//   fFrameTime - >0, in seconds
	virtual void Update( const float fFrameTime )=0;

	//
	virtual void Reset()=0;

	// mostly debugging purpose
	// Returns:
	//   0 is task does not exist
	virtual float GetEstimatedTimePerUnit( void *pObjectIdentifier)=0;

	// good to display statistics
	virtual float GetCurrentScheduledWorkload()=0;

	// mostly debugging purpose
	// Returns:
	//   0=not registered, 1=waiting, 2=in progress
	virtual uint32 GetTaskStatus( void *pObjectIdentifier )=0;
};
