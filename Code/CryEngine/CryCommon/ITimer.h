// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _ITIMER_H_
#define _ITIMER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TimeValue.h"				// CTimeValue
#include "SerializeFwd.h"

struct tm;
/*! Interface to the Timer System.
*/

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
struct ITimer
{
	enum ETimer
	{
		ETIMER_GAME = 0, // pausable, serialized, frametime is smoothed/scaled/clamped, 
		ETIMER_UI,       // non-pausable, non-serialized, frametime unprocessed
		ETIMER_LAST
	};

	//! reset the timer (only needed because float precision wasn't last that long - can be removed if 64bit is used everywhere)
	virtual void ResetTimer() = 0;	

	//! update the timer every frame
	virtual void UpdateOnFrameStart() = 0;

	//! return the time at the last UpdateOnFrameStart() call 
	virtual float GetCurrTime(ETimer which = ETIMER_GAME) const = 0;

	//! return the time at the last UpdateOnFrameStart() call 
	virtual CTimeValue GetFrameStartTime(ETimer which = ETIMER_GAME) const = 0;

	//! return the current time
	virtual CTimeValue GetAsyncTime() const = 0;

	//! return the current time at the moment of the call
	virtual float GetAsyncCurTime()= 0;

	//! return the time passed from the last UpdateOnFrameStart() in seconds
	virtual float GetFrameTime(ETimer which = ETIMER_GAME) const = 0;

	//! return the time passed from the last UpdateOnFrameStart() in seconds without any dilation, smoothing, clamping, etc...
	virtual float GetRealFrameTime() const = 0;

	//! return the time scale applied to time values
	virtual float GetTimeScale() const = 0;

	//! set the time scale applied to time values
	virtual void SetTimeScale(float s) = 0;

	//! enable/disable timer
	virtual void EnableTimer( const bool bEnable ) = 0;

	//! if timer is engable
	virtual bool IsTimerEnabled() const = 0;

	//! return the current framerate in frames/second
	virtual float	GetFrameRate() = 0;

	//! return the fraction to blend current frame in profiling stats.
	virtual float GetProfileFrameBlending( float* pfBlendTime = 0, int* piBlendMode = 0 ) = 0;

	//! serialization
	virtual void Serialize( TSerialize ser ) = 0;

	//! try to pause/unpause a timer
	//  returns true if successfully paused/unpaused, false otherwise
	virtual bool PauseTimer(ETimer which, bool bPause) = 0;

	//! determine if a timer is paused
	//  returns true if paused, false otherwise
	virtual bool IsTimerPaused(ETimer which) = 0;

	//! try to set a timer
	//  return true if successful, false otherwise
	virtual bool SetTimer(ETimer which, float timeInSeconds) = 0;

	//! make a tm struct from a time_t in UTC (like gmtime)
	virtual void SecondsToDateUTC(time_t time, struct tm& outDateUTC) = 0;

	//! make a UTC time from a tm (like timegm, but not available on all platforms)
	virtual time_t DateToSecondsUTC(struct tm& timePtr) = 0;
	
};

// this class is used for automatic profiling of a section of the code.
// create an instance of this class, and upon exiting from the code section
template <typename time>
class CITimerAutoProfiler
{
public:
	CITimerAutoProfiler (ITimer* pTimer, time& rTime):
		m_pTimer (pTimer),
		m_rTime (rTime)
	{
		rTime -= pTimer->GetAsyncCurTime();
	}

	~CITimerAutoProfiler ()
	{
		m_rTime += m_pTimer->GetAsyncCurTime();
	}

protected:
	ITimer* m_pTimer;
	time& m_rTime;
};

// include this string AUTO_PROFILE_SECTION(pITimer, g_fTimer) for the section of code where the profiler timer must be turned on and off
// The profiler timer is just some global or static float or double value that accumulates the time (in seconds) spent in the given block of code
// pITimer is a pointer to the ITimer interface, g_fTimer is the global accumulator
#define AUTO_PROFILE_SECTION(pITimer, g_fTimer) CITimerAutoProfiler<double> __section_auto_profiler(pITimer, g_fTimer)

#endif //_ITIMER_H_