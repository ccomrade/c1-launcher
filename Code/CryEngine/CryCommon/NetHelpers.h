/*************************************************************************
 Crytek Source File.
 Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
 $Id$
 $DateTime$
 Description:  contains helpers for implementing INetwork.h interfaces
 -------------------------------------------------------------------------
 History:
 - 26/07/2004   10:34 : Created by Craig Tiller
*************************************************************************/

#ifndef __NET_HELPERS_H__
#define __NET_HELPERS_H__

#include "INetwork.h"

#pragma once

struct SNoParams : public ISerializable
{
	virtual void SerializeWith( TSerialize ser )
	{
	}
};

////////////////////////////////////////////////////////////////////////////////////////
/*! helper class for implementing INetMessageSink
 *
 * class CMyMessageSink : public CNetMessageSinkHelper<CMyMessageSink, IServerSlotSink>
 * {
 * public:
 *   NET_DECLARE_MESSAGE(...);
 * };
 */

template <class T_Derived, class T_Parent, size_t MAX_STATIC_MESSAGES=32>
class CNetMessageSinkHelper : public T_Parent
{
public:
	// helper to implement GetProtocolDefs
	static SNetProtocolDef GetProtocolDef()
	{
		SNetProtocolDef def;
		def.nMessages = g_nMessages;
		def.vMessages = g_vMessages;
		return def;
	}

	// check whether a particular message belongs to this sink
	static bool ClassHasDef( const SNetMessageDef * pDef )
	{
		return pDef >= g_vMessages && pDef < g_vMessages + g_nMessages;
	}

	bool HasDef( const SNetMessageDef * pDef )
	{
		return ClassHasDef( pDef );
	}

protected:
	// this function should not be used, except through the supplied macros
	static SNetMessageDef * Helper_AddMessage( SNetMessageDef::HandlerType handler,
		ENetReliabilityType reliability, const char * description, uint32 parallelFlags, uint32 nUser = 0xBA5E1E55 )
	{
		// would use an assert here, but we don't want this to be compiled out
		// (if we exceed this limit, it's a big bug)
		if (g_nMessages == MAX_STATIC_MESSAGES)
		{
			assert(!"Maximum static messages exceeded");
			CryError("Maximum static messages exceeded");
			((void(*)())NULL)();
		}
		SNetMessageDef * def = &g_vMessages[g_nMessages++];
		def->handler = handler;
		def->reliability = reliability;
		def->description = description;
		def->nUser = nUser;
		def->parallelFlags = parallelFlags;
		return def;
	}

private:
	static size_t g_nMessages;
	static SNetMessageDef g_vMessages[MAX_STATIC_MESSAGES];
};

template <class T, class U, size_t N>
size_t CNetMessageSinkHelper<T,U,N>::g_nMessages;
template <class T, class U, size_t N>
SNetMessageDef CNetMessageSinkHelper<T,U,N>::g_vMessages[N];

// primitive message types:
// these are provided to allow direct reading and writing from network streams
// when this is necessary (e.g. UpdateEntity -> read+lookup entity id, call serialize)

// when implementing one of these messages (with NET_IMPLEMENT_MESSAGE), we
// write the message handling code directly after the macro; it will implement
// a function that takes parameters: 
//    (TSerialize pSer, uint32 nCurSeq, uint32 nOldSeq)
// you should return false if an error occurs during processing

// when using NET_*_MESSAGE style declarations, YOU must supply the implementation
// of INetMessage so that this message can be sent (be sure to keep the WritePayload,
// and the message handler code synchronised for reading and writing)

// for a simpler alternative, see NET_DECLARE_SIMPLE_MESSAGE

#define NET_DECLARE_IMMEDIATE_MESSAGE( name )                                  \
	private:                                                                     \
		static TNetMessageCallbackResult Trampoline##name( uint32,                 \
			INetMessageSink*, TSerialize,                                            \
			uint32, uint32, EntityId*, INetChannel* );                                \
		bool Handle##name( TSerialize, uint32, uint32, EntityId*, INetChannel* );                 \
	public:                                                                      \
		static const SNetMessageDef* const name

#define NET_IMPLEMENT_IMMEDIATE_MESSAGE( cls, name, reliability, parallelFlags ) \
	const SNetMessageDef* const cls::name = cls::Helper_AddMessage(              \
		cls::Trampoline##name, reliability, #cls ":" #name, (parallelFlags), 0xba5e1e55 );   \
	TNetMessageCallbackResult cls::Trampoline##name(                             \
		uint32,                                                                    \
		INetMessageSink* handler,                                                  \
		TSerialize serialize,                                                      \
		uint32 curSeq,                                                             \
		uint32 oldSeq,                                                             \
		EntityId* pEntityId, INetChannel * pChannel )                                                        \
	{                                                                            \
		char * p = (char*) handler;                                                \
		p -= size_t((INetMessageSink*)(cls*)NULL);                                 \
		return TNetMessageCallbackResult( ((cls*)p)->Handle##name( serialize, curSeq, oldSeq, pEntityId, pChannel ), NULL ); \
	}                                                                            \
	ILINE bool cls::Handle##name( TSerialize ser,                                \
		uint32 nCurSeq, uint32 nOldSeq, EntityId * pEntityId, INetChannel * pNetChannel )

#define NET_DECLARE_ATSYNC_MESSAGE( name )                                     \
	private:                                                                     \
		static TNetMessageCallbackResult Trampoline##name( uint32,                 \
			INetMessageSink*, TSerialize,                                            \
			uint32, uint32, EntityId, INetChannel* );                                \
		bool Handle##name( TSerialize, uint32, uint32, EntityId, INetChannel* );   \
	public:                                                                      \
		static const SNetMessageDef* const name

#define NET_IMPLEMENT_ATSYNC_MESSAGE( cls, name, reliability, parallelFlags ) \
	const SNetMessageDef* const cls::name = cls::Helper_AddMessage(              \
		cls::Trampoline##name, reliability, #cls ":" #name, (parallelFlags) );   \
	TNetMessageCallbackResult cls::Trampoline##name(                             \
		uint32,                                                                    \
		INetMessageSink* handler,                                                  \
		TSerialize serialize,                                                      \
		uint32 curSeq,                                                             \
		uint32 oldSeq,                                                             \
		EntityId entityId, INetChannel* pChannel )                                          \
	{                                                                            \
		char * p = (char*) handler;                                                \
		p -= size_t((INetMessageSink*)(cls*)NULL);                                 \
		return TNetMessageCallbackResult(																					 \
			((cls*)p)->Handle##name(serialize,curSeq,oldSeq,entityId,pChannel), 0);	 \
	}                                                                            \
	ILINE bool cls::Handle##name( TSerialize ser,                                \
		uint32 nCurSeq, uint32 nOldSeq, EntityId entityId, INetChannel *pChannel )

// helper class for NET_DECLARE_SIMPLE_MESSAGE
template <class T>
class CSimpleNetMessage : public INetMessage
{
public:
	CSimpleNetMessage( const T& value, const SNetMessageDef * pDef ) : 
			INetMessage(pDef), m_value(value) 
	{
	}

	EMessageSendResult WritePayload( 
		TSerialize serialize,
		uint32, uint32 )
	{
		m_value.SerializeWith( serialize );
		return eMSR_SentOk;
	}
	void UpdateState( uint32 nFromSeq, ENetSendableStateUpdate )
	{
	}
	size_t GetSize() { return sizeof(*this); }

private:
	T m_value;
};

// simple message types:
// if we have a class that represents the parameters to a function, and that 
// implements the ISerializable interface, then we can use 
// NET_DECLARE_SIMPLE_MESSAGE to declare the message, and save some time and
// some bugs.

// it takes as parameters "name", which is the name of the message to declare,
// and "CClass" which is the name of the class holding our parameters

// it will declare in our class a static function:
//   static void Send"name"With( const CClass& msg, TChannelType * pChannel )
// to assist us with sending this message type; TChannelType is any class that
// implements a SendMsg function

// when implementing our message handler, we will be passed a single parameter,
// which is a "const CClass&" called param

#define NET_DECLARE_SIMPLE_IMMEDIATE_MESSAGE( name, CClass )                   \
	private:                                                                     \
		static TNetMessageCallbackResult Trampoline##name( uint32,                 \
			INetMessageSink*, TSerialize,                                            \
			uint32, uint32, EntityId*, INetChannel * );                               \
		bool Handle##name( const CClass&, EntityId*, bool bFromDemoSystem, INetChannel * pChannel );        \
		typedef CClass TParam##name;                                               \
	public:                                                                      \
		template <class T>                                                         \
		static void Send##name##With( const CClass& msg, T * pChannel )            \
		{ pChannel->SendMsg( new CSimpleNetMessage<CClass>(msg, name) ); }  \
		static const SNetMessageDef* const name

#define NET_IMPLEMENT_SIMPLE_IMMEDIATE_MESSAGE( cls, name, reliability, parallelFlags ) \
	const SNetMessageDef* const cls::name = cls::Helper_AddMessage(              \
		cls::Trampoline##name, reliability, #cls ":" #name, (parallelFlags) & ~eMPF_DecodeInSync );   \
	TNetMessageCallbackResult cls::Trampoline##name(                             \
		uint32,                                                                    \
		INetMessageSink* handler,                                                  \
		TSerialize serialize,                                                      \
		uint32 curSeq,                                                             \
		uint32 oldSeq,                                                             \
		EntityId* pEntityId, INetChannel* pChannel )                                          \
	{                                                                            \
		char * p = (char*) handler;                                                \
		p -= size_t((INetMessageSink*)(cls*)NULL);                                 \
		TParam##name param;                                                        \
		param.SerializeWith( serialize );                                          \
		return TNetMessageCallbackResult( ((cls*)p)->Handle##name( param, pEntityId, \
			curSeq == DEMO_PLAYBACK_SEQ_NUMBER &&                                    \
			oldSeq == DEMO_PLAYBACK_SEQ_NUMBER, pChannel ), 0 );                               \
	}                                                                            \
	ILINE bool cls::Handle##name( const TParam##name& param, EntityId * pEntityId, bool bFromDemoSystem, INetChannel * pNetChannel )

template <class T, class Obj>
class CNetSimpleAtSyncItem : public INetAtSyncItem
{
public:
	typedef bool (Obj::*CallbackFunc)( const T&, EntityId, bool, INetChannel * );
	CNetSimpleAtSyncItem( Obj* pObj, CallbackFunc callback, const T& data, EntityId id, bool fromDemo, INetChannel * pChannel ) : m_pObj(pObj), m_callback(callback), m_data(data), m_id(id), m_fromDemo(fromDemo), m_pChannel(pChannel) {}
	bool Sync()
	{
		if (!(m_pObj->*m_callback)(m_data, m_id, m_fromDemo, m_pChannel))
			return false;
		return true;
	}
	void DeleteThis()
	{
		delete this;
	}
private:
	Obj * m_pObj;
	CallbackFunc m_callback;
	T m_data;
	EntityId m_id;
	bool m_fromDemo;
	INetChannel * m_pChannel;
};

#define NET_DECLARE_SIMPLE_ATSYNC_MESSAGE( name, CClass )                      \
	private:                                                                     \
		static TNetMessageCallbackResult Trampoline##name( uint32,                 \
			INetMessageSink*, TSerialize,                                            \
			uint32, uint32, EntityId*, INetChannel * );                                              \
		bool Handle##name( const CClass&, EntityId, bool bFromDemoSystem, INetChannel* );        \
		typedef CClass TParam##name;                                               \
	public:                                                                      \
		template <class T>                                                         \
		static void Send##name##With( const CClass& msg, T * pChannel )            \
		{ pChannel->SendMsg( new CSimpleNetMessage<CClass>(msg, name) ); }  \
		static const SNetMessageDef* const name

#define NET_IMPLEMENT_SIMPLE_ATSYNC_MESSAGE( cls, name, reliability, parallelFlags ) \
	const SNetMessageDef* const cls::name = cls::Helper_AddMessage(              \
		cls::Trampoline##name, reliability, #cls ":" #name, (parallelFlags) & ~eMPF_DecodeInSync );   \
	TNetMessageCallbackResult cls::Trampoline##name(                             \
		uint32,                                                                    \
		INetMessageSink* handler,                                                  \
		TSerialize serialize,                                                      \
		uint32 curSeq,                                                             \
		uint32 oldSeq,                                                             \
		EntityId* pEntityId, INetChannel * pChannel )                                                        \
	{                                                                            \
		char * p = (char*) handler;                                                \
		p -= size_t((INetMessageSink*)(cls*)NULL);                                 \
		TParam##name param;                                                        \
		param.SerializeWith( serialize );                                          \
		return TNetMessageCallbackResult( true,                                    \
			new CNetSimpleAtSyncItem<TParam##name, cls>(                             \
				(cls*)p, &cls::Handle##name, param, *pEntityId,                          \
				curSeq == DEMO_PLAYBACK_SEQ_NUMBER && oldSeq == DEMO_PLAYBACK_SEQ_NUMBER, pChannel ) ); \
	}                                                                            \
	ILINE bool cls::Handle##name( const TParam##name& param, EntityId entityId, bool bFromDemoSystem, INetChannel *pNetChannel )

// helpers for writing channel establishment tasks
class CCET_Base : public IContextEstablishTask
{
public:
	virtual ~CCET_Base()
	{
	}
	virtual void Release()
	{
		delete this;
	}
	virtual void OnFailLoading( bool hasEntered )
	{
	}
	virtual void OnLoadingComplete()
	{
	}
};

template <class T>
class CCET_SetValue : public CCET_Base
{
public:
	CCET_SetValue( T * pWhere, T what, string name ) : m_pSetWhere(pWhere), m_setWhat(what), m_name(name) {}

	const char * GetName() { return m_name.c_str(); }

	EContextEstablishTaskResult OnStep( SContextEstablishState& )
	{
		*m_pSetWhere = m_setWhat;
		return eCETR_Ok;
	}

private:
	T * m_pSetWhere;
	T m_setWhat;
	string m_name;
};

template <class T>
void AddSetValue( IContextEstablisher * pEst, EContextViewState state, T * pWhere, T what, const char * name )
{
	pEst->AddTask( state, new CCET_SetValue<T>(pWhere, what, name) );
}

template <class T>
class CCET_StoreValueThenChangeTo : public CCET_Base
{
public:
	CCET_StoreValueThenChangeTo( T init, T what, string name ) : m_value(init), m_setWhat(what), m_name(name) {}

	const char * GetName() { return m_name.c_str(); }

	EContextEstablishTaskResult OnStep( SContextEstablishState& )
	{
		m_value = m_setWhat;
		return eCETR_Ok;
	}

private:
	T m_value;
	T m_setWhat;
	string m_name;
};

template <class T>
void AddStoreValueThenChangeTo( IContextEstablisher * pEst, EContextViewState state, T init, T what, const char * name )
{
	pEst->AddTask( state, new CCET_StoreValueThenChangeTo<T>(init, what, name) );
}

template <class T>
class CCET_WaitValue : public CCET_Base
{
public:
	CCET_WaitValue( T * pWhere, T what, string name ) : m_pWaitWhere(pWhere), m_waitWhat(what), m_name(name) {}

	const char * GetName() { return m_name.c_str(); }

	EContextEstablishTaskResult OnStep( SContextEstablishState& )
	{
		if (*m_pWaitWhere != m_waitWhat)
			return eCETR_Wait;
		else
			return eCETR_Ok;
	}

private:
	T * m_pWaitWhere;
	T m_waitWhat;
	string m_name;
};

template <class T>
void AddWaitValue( IContextEstablisher * pEst, EContextViewState state, T * pWhere, T what, const char * name )
{
	pEst->AddTask( state, new CCET_WaitValue<T>(pWhere, what, name) );
}

template <class T>
class CCET_AddValue : public CCET_Base
{
public:
	CCET_AddValue( T * pWhere, T what, string name ) : m_pWaitWhere(pWhere), m_waitWhat(what), m_name(name) {}

	const char * GetName() { return m_name.c_str(); }

	EContextEstablishTaskResult OnStep( SContextEstablishState& )
	{
		*m_pWaitWhere += m_waitWhat;
		return eCETR_Ok;
	}

private:
	T * m_pWaitWhere;
	T m_waitWhat;
	string m_name;
};

template <class T>
void AddAddValue( IContextEstablisher * pEst, EContextViewState state, T * pWhere, T what, const char * name )
{
	pEst->AddTask( state, new CCET_AddValue<T>(pWhere, what, name) );
}

template <class T>
class CCET_CallMemberFunction : public CCET_Base
{
public:
	CCET_CallMemberFunction( T * pObj, void (T::*memberFunc)(), string name ) : m_pObj(pObj), m_memberFunc(memberFunc), m_name(name) {}

	const char * GetName() { return m_name.c_str(); }

	EContextEstablishTaskResult OnStep( SContextEstablishState& )
	{
		(m_pObj->*m_memberFunc)();
		return eCETR_Ok;
	}

private:
	T * m_pObj;
	void (T::*m_memberFunc)();
	string m_name;
};

template <class T>
void AddCallMemberFunction( IContextEstablisher * pEst, EContextViewState state, T * pObj, void (T::*memberFunc)(), const char * name )
{
	pEst->AddTask( state, new CCET_CallMemberFunction<T>(pObj, memberFunc, name) );
}

#endif
