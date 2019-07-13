// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _I_BUDGETING_SYSTEM_
#define _I_BUDGETING_SYSTEM_


struct IBudgetingSystem
{
	// set budget
	virtual void SetSysMemLimit( int sysMemLimitInMB ) = 0;
	virtual void SetVideoMemLimit( int videoMemLimitInMB ) = 0;
	virtual void SetFrameTimeLimit( float frameLimitInMS ) = 0;
	virtual void SetSoundChannelsPlayingLimit( int soundChannelsPlayingLimit ) = 0;
	virtual void SetSoundMemLimit( int SoundMemLimit ) = 0;
	virtual void SetNumDrawCallsLimit( int numDrawCallsLimit ) = 0;
	virtual void SetBudget( int sysMemLimitInMB, int videoMemLimitInMB, 
		float frameTimeLimitInMS, int soundChannelsPlayingLimit, int SoundMemLimitInMB, int numDrawCallsLimit ) = 0;
	
	// get budget
	virtual int GetSysMemLimit() const = 0;
	virtual int GetVideoMemLimit() const = 0;
	virtual float GetFrameTimeLimit() const = 0;
	virtual int GetSoundChannelsPlayingLimit() const = 0;
	virtual int GetSoundMemLimit() const = 0;
	virtual void GetBudget( int& sysMemLimitInMB, int& videoMemLimitInMB, 
		float& frameTimeLimitInMS, int& soundChannelsPlayingLimit, int& SoundMemLimitInMB, int& numDrawCallsLimit ) const = 0;

	// monitoring
	virtual void MonitorBudget() = 0;

	// destruction
	virtual void Release() = 0;

protected:
	virtual ~IBudgetingSystem() {}
};


#endif // #ifndef _I_BUDGETING_SYSTEM_