/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Implements some utilities.
  
 -------------------------------------------------------------------------
  History:
  - 4:11:2004: Created by Filippo De Luca

*************************************************************************/
#ifndef __UTILS_H__
#define __UTILS_H__

#if _MSC_VER > 1000
# pragma once
#endif

//! normalize the val to 0-360 range 
ILINE f32 Snap_s360( f32 val ) {
	if( val < 0.0f )
		val =f32( 360.0f + cry_fmod(val,360.0f));
	else
		if(val >= 360.0f)
			val =f32(cry_fmod(val,360.0f));
	return val;
}

//! normalize the val to -180, 180 range 
ILINE f32 Snap_s180( f32 val ) {
	if( val > -180.0f && val < 180.0f)
		return val;
	val = Snap_s360( val );
	if( val>180.0f )
		return -(360.0f - val);
	return val;
}


//interpolate angle
inline void Interpolate(Ang3 &actual,const Ang3 &goal,float speed,float frameTime,float limit = 0)
{
	Ang3 delta(goal - actual);
	delta.x=Snap_s180(delta.x);
	delta.y=Snap_s180(delta.y);
	delta.z=Snap_s180(delta.z);
	
	if (limit>0.001f)
	{
		delta.x = max(min(delta.x,limit),-limit);
		delta.y = max(min(delta.y,limit),-limit);
		delta.z = max(min(delta.z,limit),-limit);
	}

	actual += delta * min(frameTime * speed,1.0f);
}

//interpolate vector
inline void Interpolate(Vec3 &actual,const Vec3 &goal,float speed,float frameTime,float limit = 0)
{
	Vec3 delta(goal - actual);
		
	if (limit>0.001f)
	{
		float len(delta.len());

		if (len>limit)
		{
			delta /= len;
			delta *= limit;
		}
	}

	actual += delta * min(frameTime * speed,1.0f);
}

//interpolate vector
inline void Interpolate(Vec4 &actual,const Vec4 &goal,float speed,float frameTime,float limit = 0)
{
	Vec4 delta(goal - actual);

	if (limit>0.001f)
	{
		float len(delta.GetLength());

		if (len>limit)
		{
			delta /= len;
			delta *= limit;
		}
	}

	actual += delta * min(frameTime * speed,1.0f);
}

//interpolate float
inline void Interpolate(float &actual,float goal,float speed,float frameTime,float limit = 0)
{
	float delta(goal - actual);

	if (limit>0.001f)
	{
		delta = max(min(delta,limit),-limit);
	}

	actual += delta * min(frameTime * speed,1.0f);
}

//interpolate float - wraps at minValue/maxValue
inline void InterpolateWrapped(float &actual,float goal,float minValue,float maxValue,float speed,float frameTime,float limit = 0)
{
	assert(minValue < maxValue);
	assert(minValue <= goal);
	assert(goal <= maxValue);
	assert(minValue <= actual);
	assert(actual <= maxValue);

	float range = maxValue - minValue;
	float movement = 0.0f;
	if (goal < actual)
	{
		if (actual - goal > range * 0.5f)
			movement = goal+range - actual;
		else
			movement = goal - actual;
	}
	else
	{
		if (goal - actual > range * 0.5f)
			movement = goal - (actual-range);
		else
			movement = goal - actual;
	}

	if (limit>0.001f)
	{
		movement = max(min(movement,limit),-limit);
	}

	actual += movement * min(frameTime * speed,1.0f);
}

// not the best place for this
namespace GameUtils
{
	inline void timeToString(time_t value, string& outString)
	{
		struct tm dateTime;

		// convert the time to a string
		gEnv->pTimer->SecondsToDateUTC(value, dateTime);

		// use ISO 8601 UTC
		outString.Format("%d-%02d-%02dT%02d:%02d:%02dZ",
			dateTime.tm_year + 1900, dateTime.tm_mon + 1, dateTime.tm_mday,
			dateTime.tm_hour, dateTime.tm_min, dateTime.tm_sec );
	}

	inline time_t stringToTime(const char* str)
	{
		if (str)
		{
			struct tm timePtr;
			// convert the time to from a string to a time struct
			int n = sscanf(str, "%i-%02d-%02dT%02d:%02d:%02d",
				&timePtr.tm_year, &timePtr.tm_mon, &timePtr.tm_mday,
				&timePtr.tm_hour, &timePtr.tm_min, &timePtr.tm_sec);
			if (n != 6)
				return (time_t) 0;

			timePtr.tm_year -= 1900;
			timePtr.tm_mon -= 1;
			timePtr.tm_isdst = -1;
			return gEnv->pTimer->DateToSecondsUTC(timePtr);
		}
		return (time_t) 0;
	}
};

#endif //__UTILS_H__
