// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _IAIRECORDER_H_
#define _IAIRECORDER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef LINUX
#	include "platform.h"
#endif

// Luciano - remove these lines to remove the log signals
#ifndef AI_LOG_SIGNALS
	#define AI_LOG_SIGNALS
#endif
////////////////////////////////////////////////////

struct IAIRecorder
{
	virtual bool Load(const char *filename = NULL) = 0;
	virtual bool Save(const char *filename = NULL) = 0;
	virtual void Update(void) = 0;

};

class CStream;

struct IAIDebugRecord;

struct IAIRecordable
{
	struct RecorderEventData
	{
		const char* pString;
		Vec3	pos;
		float	val;
		void* pExtraData;

		RecorderEventData(const char *pStr) : pString(pStr), pos(0,0,0), val(0), pExtraData(NULL) {}
		RecorderEventData(const Vec3& p) : pString(0), pos(p), val(0), pExtraData(NULL) {}
		RecorderEventData(float v) : pString(0), pos(0,0,0), val(v), pExtraData(NULL) {}
	};

	enum e_AIDbgEvent{
		E_NONE,
		E_RESET,
		E_SIGNALRECIEVED,
		E_SIGNALRECIEVEDAUX,
		E_SIGNALEXECUTING,
		E_GOALPIPESELECTED,
		E_GOALPIPEINSERTED,
		E_GOALPIPERESETED,
		E_BEHAVIORSELECTED,
		E_BEHAVIORDESTRUCTOR,
		E_BEHAVIORCONSTRUCTOR,
		E_ATTENTIONTARGET,
		E_ATTENTIONTARGETPOS,
		E_HANDLERNEVENT,
		E_ACTIONSTART,
		E_ACTIONSUSPEND,
		E_ACTIONRESUME,
		E_ACTIONEND,
		E_EVENT,
		E_REFPOINTPOS,
		E_AGENTPOS,
		E_LUACOMMENT,
		E_HEALTH,
		E_HIT_DAMAGE,
		E_DEATH,
		E_SIGNALEXECUTEDWARNING
	};

	virtual void	RecordEvent(e_AIDbgEvent event, const RecorderEventData* pEventData=NULL)=0;
	virtual void	RecordSnapshot()=0;
	virtual IAIDebugRecord* GetAIDebugRecord()=0;
};

struct IAIDebugStream
{
	virtual void	Seek(float whereTo)=0;
	virtual int		GetCurrentIdx()=0;	
	virtual int		GetSize()=0;
	virtual void*	GetCurrent(float &startingFrom)=0;
	virtual void*	GetNext(float &startingFrom)=0;
	virtual float	GetStartTime()=0;
	virtual float	GetEndTime()=0;
};

struct IAIDebugRecord
{
	virtual IAIDebugStream* GetStream(IAIRecordable::e_AIDbgEvent streamTag)=0;
};


#endif //_IAIRECORDER_H_
