// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IGAMEOBJECT_H__
#define __IGAMEOBJECT_H__

#pragma once

// FIXME: Cell SDK GCC bug workaround.
#ifndef __IGAMEOBJECTSYSTEM_H__
#include "IGameObjectSystem.h"
#endif

#include "IEntitySystem.h"
#include "SerializeFwd.h"
#include "IActionMapManager.h"
#include "PoolAllocator.h"

inline void GameWarning(const char * ,...) PRINTF_PARAMS(1, 2);

struct IGameObjectExtension;
struct IGameObjectView;
struct IActionListener;
struct IMovementController;
struct IGameObjectExtension;
struct IGameObjectProfileManager;
struct IWorldQuery;

enum EEntityAspects
{
	// 0x01u                       // aspect 0
	eEA_Script            = 0x02u, // aspect 1
	// 0x04u                       // aspect 2
	eEA_Physics           = 0x08u, // aspect 3
	eEA_GameClientStatic  = 0x10u, // aspect 4
	eEA_GameServerStatic  = 0x20u, // aspect 5
	eEA_GameClientDynamic = 0x40u, // aspect 6
	eEA_GameServerDynamic = 0x80u, // aspect 7
};

enum EEntityPhysicsEvents
{
	eEPE_OnCollisionLogged						= 1<<0,	// logged events on lower byte
	eEPE_OnPostStepLogged							= 1<<1,
	eEPE_OnStateChangeLogged					= 1<<2,
	eEPE_OnCreateEntityPartLogged			= 1<<3,
	eEPE_OnUpdateMeshLogged           = 1<<4,
	eEPE_AllLogged										= eEPE_OnCollisionLogged|eEPE_OnPostStepLogged|
																			eEPE_OnStateChangeLogged|eEPE_OnCreateEntityPartLogged|
																			eEPE_OnUpdateMeshLogged,

	eEPE_OnCollisionImmediate					= 1<<8,	// immediate events on higher byte
	eEPE_OnPostStepImmediate					= 1<<9,
	eEPE_OnStateChangeImmediate				= 1<<10,
	eEPE_OnCreateEntityPartImmediate	= 1<<11,
	eEPE_OnUpdateMeshImmediate        = 1<<12,
	eEPE_AllImmediate									= eEPE_OnCollisionImmediate|eEPE_OnPostStepImmediate|
																			eEPE_OnStateChangeImmediate|eEPE_OnCreateEntityPartImmediate|
																			eEPE_OnUpdateMeshImmediate,
};

static const int MAX_UPDATE_SLOTS_PER_EXTENSION = 4;

enum ERMInvocation
{
	eRMI_ToClientChannel =    0x01,
	eRMI_ToOwnClient     =    0x02,
	eRMI_ToOtherClients  =    0x04,
	eRMI_ToAllClients    =    0x08,

	eRMI_ToServer        =   0x100,

	eRMI_NoLocalCalls    = 0x10000,
	eRMI_NoRemoteCalls   = 0x20000,

	eRMI_ToRemoteClients = eRMI_NoLocalCalls | eRMI_ToAllClients
};

enum EUpdateEnableCondition
{
	eUEC_Never,
	eUEC_Always,
	eUEC_Visible,
	eUEC_InRange,
	eUEC_VisibleAndInRange,
	eUEC_VisibleOrInRange,
  eUEC_VisibleOrInRangeIgnoreAI,
	eUEC_VisibleIgnoreAI,
  eUEC_WithoutAI,
};

enum EPrePhysicsUpdate
{
	ePPU_Never,
	ePPU_Always,
	ePPU_WhenAIActivated
};

enum EGameObjectAIActivationMode
{
	eGOAIAM_Never,
	eGOAIAM_Always,
	eGOAIAM_VisibleOrInRange,
	// must be last
	eGOAIAM_COUNT_STATES,
};

enum EAutoDisablePhysicsMode
{
	eADPM_Never,
	eADPM_WhenAIDeactivated,
	eADPM_WhenInvisibleAndFarAway,
	// must be last
	eADPM_COUNT_STATES,
};

enum EBindToNetworkMode
{
	eBTNM_Normal,
	eBTNM_Force,
	eBTNM_NowInitialized
};

struct SGameObjectExtensionRMI
{
	typedef INetAtSyncItem * (*DecoderFunction)(TSerialize, EntityId*, INetChannel*);

	DecoderFunction decoder;
	const char * description;
	const void * pBase;
	const SNetMessageDef * pMsgDef;
	ERMIAttachmentType attach;
	bool isServerCall;
	bool lowDelay;
	ENetReliabilityType reliability;
};

template <size_t N>
class CRMIAllocator
{
public:
	static ILINE void * Allocate()
	{
		if (!m_pAllocator)
			m_pAllocator = new stl::PoolAllocator<N>;
		return m_pAllocator->Allocate();
	}
	static ILINE void Deallocate(void * p)
	{
		assert(m_pAllocator);
		m_pAllocator->Deallocate(p);
	}

private:
	static stl::PoolAllocator<N> * m_pAllocator;
};
template <size_t N> stl::PoolAllocator<N> * CRMIAllocator<N>::m_pAllocator = 0;

// Summary
//   Interface used to interact with a game object
// See Also
//   IGameObjectExtension
struct IGameObject : public IActionListener
{
protected:
	class CRMIBody : public IRMIMessageBody
	{
	public:
		CRMIBody( const SGameObjectExtensionRMI * method, EntityId id, IRMIListener * pListener, int userId, EntityId dependentId ) :
				IRMIMessageBody(method->reliability, method->attach, id, method->pMsgDef, pListener, userId, dependentId)
		{
		}
	};

	template <class T>
	class CRMIBodyImpl : public CRMIBody
	{
	public:
		void SerializeWith( TSerialize ser )
		{
			m_params.SerializeWith(ser);
		}

		size_t GetSize()
		{
			return sizeof(*this);
		}

		static CRMIBodyImpl * Create( const SGameObjectExtensionRMI * method, EntityId id, const T& params, IRMIListener * pListener, int userId, EntityId dependentId )
		{
			return new (CRMIAllocator<sizeof(CRMIBodyImpl)>::Allocate()) CRMIBodyImpl(method, id, params, pListener, userId, dependentId);
		}

		void DeleteThis()
		{
			this->~CRMIBodyImpl();
			CRMIAllocator<sizeof(CRMIBodyImpl)>::Deallocate(this);
		}

	private:
		T m_params;

		CRMIBodyImpl( const SGameObjectExtensionRMI * method, EntityId id, const T& params, IRMIListener * pListener, int userId, EntityId dependentId ) : 
			CRMIBody( method, id, pListener, userId, dependentId ),
			m_params(params)
		{
		}
	};

public:
	// bind this entity to the network system (it gets synchronized then...)
	virtual bool BindToNetwork(EBindToNetworkMode mode = eBTNM_Normal) = 0;
	// flag that we have changed the state of the game object aspect
	virtual void ChangedNetworkState( uint8 aspects ) = 0;
	// enable/disable network aspects on game object
	virtual void EnableAspect(uint8 aspects, bool enable) = 0;
	// query extension. returns 0 if extension is not there.
	virtual IGameObjectExtension *QueryExtension(const char *name) const= 0;
	// set extension parameters
	virtual bool SetExtensionParams( const char * extension, SmartScriptTable params ) = 0;
	// get extension parameters
	virtual bool GetExtensionParams( const char * extension, SmartScriptTable params ) = 0;
	// send a game object event
	virtual void SendEvent( const SGameObjectEvent& ) = 0;
	// force the object to update even if extensions' slots are "sleeping"...
	virtual void ForceUpdate(bool force) = 0;
	virtual void ForceUpdateExtension( IGameObjectExtension * pGOE, int slot ) = 0;
	// get/set network channel
	virtual uint16 GetChannelId() const = 0;
	virtual void SetChannelId( uint16 ) = 0;
	virtual INetChannel *GetNetChannel() const = 0;
	// serialize some aspects of the game object
	virtual void FullSerialize( TSerialize ser ) = 0;
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) = 0;
	// in case things have to be set after serialization
	virtual void PostSerialize() = 0;
	// is the game object probably visible?
	virtual bool IsProbablyVisible() = 0;
	virtual bool IsProbablyDistant() = 0;
	// change the profile of an aspect
	virtual bool SetAspectProfile( EEntityAspects aspect, uint8 profile, bool fromNetwork = false ) = 0;
	virtual uint8 GetAspectProfile( EEntityAspects aspect ) = 0;
	virtual IGameObjectExtension * GetExtensionWithRMIBase( const void * pBase ) = 0;
	virtual void EnablePrePhysicsUpdate( EPrePhysicsUpdate updateRule ) = 0;
	virtual void SetNetworkParent( EntityId id ) = 0;
	virtual void Pulse( uint32 pulse ) = 0;
	virtual void RegisterAsPredicted() = 0;
	virtual void RegisterAsValidated(IGameObject* pGO, int predictionHandle) = 0;
	virtual int GetPredictionHandle() = 0;

	// enable/disable sending physics events to this game object
	virtual void EnablePhysicsEvent(bool enable, int events) = 0;
	virtual bool WantsPhysicsEvent(int events) = 0;
	virtual void AttachDistanceChecker() = 0;

	// enable/disable AI activation flag
	virtual bool SetAIActivation( EGameObjectAIActivationMode mode ) = 0;
	// enable/disable auto-disabling of physics
	virtual void SetAutoDisablePhysicsMode( EAutoDisablePhysicsMode mode ) = 0;
	// for debugging updates
	virtual bool ShouldUpdate( ) = 0;

	// register a partial update in the netcode without actually serializing - useful only for working around other bugs
	virtual void RequestRemoteUpdate( uint8 aspectMask ) = 0;

	// WARNING: there *MUST* be at least one frame between spawning ent and using this function to send an RMI if
	// that RMI is _FAST, otherwise the dependent entity is ignored
	template <class MI, class T>
	void InvokeRMIWithDependentObject( const MI method, const T& params, unsigned where, EntityId ent, int channel = -1 )
	{
		InvokeRMI_Primitive(method, params, where, 0, 0, channel, ent);
	}

	template <class MI, class T>
	void InvokeRMI( const MI method, const T& params, unsigned where, int channel = -1 )
	{
		InvokeRMI_Primitive(method, params, where, 0, 0, channel, 0);
	}

	template <class MI, class T>
	void InvokeRMI_Primitive( const MI method, const T& params, unsigned where, IRMIListener * pListener, int userId, int channel, EntityId dependentId )
	{
		method.Verify(params);
		DoInvokeRMI( CRMIBodyImpl<T>::Create(method.pMethodInfo, GetEntityId(), params, pListener, userId, dependentId), where, channel );
	}

	// turn an extension on
	ILINE bool ActivateExtension( const char * extension ) { return ChangeExtension( extension, eCE_Activate ) != 0; }
	// turn an extension off
	ILINE void DeactivateExtension( const char * extension ) { ChangeExtension( extension, eCE_Deactivate ); }
	// forcefully get a pointer to an extension (may instantiate if needed)
	ILINE IGameObjectExtension * AcquireExtension( const char * extension ) { return ChangeExtension( extension, eCE_Acquire ); }
	// release a previously acquired extension
	ILINE void ReleaseExtension( const char * extension ) { ChangeExtension( extension, eCE_Release ); }

	// retrieve the hosting entity
	ILINE IEntity *GetEntity() const
	{
		return m_pEntity;
	}

	ILINE EntityId GetEntityId() const
	{
		return m_entityId;
	}

	// for extensions to register for special things
	virtual bool CaptureView( IGameObjectView * pGOV ) = 0;
	virtual void ReleaseView( IGameObjectView * pGOV ) = 0;
	virtual bool CaptureActions( IActionListener * pAL ) = 0;
	virtual void ReleaseActions( IActionListener * pAL ) = 0;
	virtual bool CaptureProfileManager( IGameObjectProfileManager * pPH ) = 0;
	virtual void ReleaseProfileManager( IGameObjectProfileManager * pPH ) = 0;
	virtual void EnableUpdateSlot( IGameObjectExtension * pExtension, int slot ) = 0;
	virtual void DisableUpdateSlot( IGameObjectExtension * pExtension, int slot ) = 0;
  virtual uint8 GetUpdateSlotEnables( IGameObjectExtension * pExtension, int slot ) = 0;
	virtual void EnablePostUpdates( IGameObjectExtension * pExtension ) = 0;
	virtual void DisablePostUpdates( IGameObjectExtension * pExtension ) = 0;
	virtual void SetUpdateSlotEnableCondition( IGameObjectExtension * pExtension, int slot, EUpdateEnableCondition condition ) = 0;
	virtual void PostUpdate( float frameTime ) = 0;
	virtual IWorldQuery* GetWorldQuery() = 0;

	virtual bool IsJustExchanging() = 0;

	ILINE void SetMovementController( IMovementController * pMC ) { m_pMovementController = pMC; }
	ILINE IMovementController * GetMovementController() { return m_pMovementController; }

protected:
	enum EChangeExtension
	{
		eCE_Activate,
		eCE_Deactivate,
		eCE_Acquire,
		eCE_Release
	};

	IGameObject() : m_pEntity(0), m_entityId(0), m_pMovementController(0) {}
	EntityId m_entityId;
	IMovementController * m_pMovementController;
	IEntity * m_pEntity;

private:
	// change extension activation/reference somehow
	virtual IGameObjectExtension * ChangeExtension( const char * extension, EChangeExtension change ) = 0;
	// invoke an RMI call
	virtual void DoInvokeRMI( _smart_ptr<CRMIBody> pBody, unsigned where, int channel ) = 0;
};


struct IRMIAtSyncItem : public INetAtSyncItem, public IRMICppLogger {};

template <class T, class Obj>
class CRMIAtSyncItem : public IRMIAtSyncItem
{
public:
	typedef bool (Obj::*CallbackFunc)(const T&, INetChannel*);

	// INetAtSyncItem
	// INetAtSyncItem

	static ILINE CRMIAtSyncItem * Create( const T& params, EntityId id, const SGameObjectExtensionRMI * pRMI, CallbackFunc callback, INetChannel * pChannel )
	{
		return new (CRMIAllocator<sizeof(CRMIAtSyncItem)>::Allocate()) CRMIAtSyncItem(params, id, pRMI, callback, pChannel);
	}

	bool Sync()
	{
		bool ok = false;
		bool foundObject = false;
		char msg[256];
		msg[0] = 0;

		if (IGameObject * pGameObject = gEnv->pGame->GetIGameFramework()->GetGameObject(m_id))
		{
			if (Obj * pGameObjectExtension = (Obj*)pGameObject->GetExtensionWithRMIBase(m_pRMI->pBase))
			{
				ok = (pGameObjectExtension->*m_callback)(m_params, m_pChannel);
				foundObject = true;
			}
			else
			{
				sprintf(msg, "Game object extension with base %.8x for entity %s for RMI %s not found", (uint32)m_pRMI->pBase, pGameObject->GetEntity()->GetName(), m_pRMI->pMsgDef->description);
				GameWarning("%s", msg);
			}
		}
		else
		{
			sprintf(msg, "Entity %d for RMI %s not found", m_id, m_pRMI->pMsgDef->description);
		}

		if (!ok)
		{
			GameWarning("Error handling RMI %s", m_pRMI->pMsgDef->description);

			if (!foundObject && !gEnv->bServer && !m_pChannel->IsInTransition())
			{
				assert(msg[0]);
				m_pChannel->Disconnect( eDC_ContextCorruption, msg );
			}
			else
			{
				ok = true; 
				// fake for singleplayer/multiplayer server
				// singleplayer - 'impossible' to get right during quick-load
				// multiplayer server - object can be deleted while the message is in flight
			}
		}

		if (!foundObject)
			return true; // lie about success if we cant find the object... editor workaround
		else
			return ok;
	}


	void DeleteThis()
	{
		this->~CRMIAtSyncItem();
		CRMIAllocator<sizeof(CRMIAtSyncItem)>::Deallocate(this);
	}
	// ~INetAtSyncItem

	// IRMICppLogger
	virtual const char * GetName()
	{
		return m_pRMI->description;
	}
	virtual void SerializeParams( TSerialize ser )
	{
		m_params.SerializeWith(ser);
	}
	// ~IRMICppLogger

private:
	CRMIAtSyncItem( const T& params, EntityId id, const SGameObjectExtensionRMI * pRMI, CallbackFunc callback, INetChannel * pChannel ) : m_params(params), m_id(id), m_pRMI(pRMI), m_callback(callback), m_pChannel(pChannel) {}

	T m_params;
	EntityId m_id;
	const SGameObjectExtensionRMI * m_pRMI;
	CallbackFunc m_callback;
	INetChannel * m_pChannel;
};

struct IGameObject;
struct SViewParams;

template <class T_Derived, class T_Parent, size_t MAX_STATIC_MESSAGES = 32>
class CGameObjectExtensionHelper : public T_Parent
{
public:
	static void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )
	{
		*ppRMI = g_vMessages;
		*nCount = g_nMessages;
	}

	const void * GetRMIBase() const
	{
		return g_vMessages;
	}
	
protected:
	static const SGameObjectExtensionRMI * Helper_AddMessage( SGameObjectExtensionRMI::DecoderFunction decoder, const char * description, ERMIAttachmentType attach, bool isServerCall, ENetReliabilityType reliability, bool lowDelay )
	{
		if (g_nMessages >= MAX_STATIC_MESSAGES)
		{
			// Assert or CryError here uses gEnv, which is not yet initialized.
			((void(*)())NULL)();
			return NULL;
		}
		SGameObjectExtensionRMI& rmi = g_vMessages[g_nMessages++];
		rmi.decoder = decoder;
		rmi.description = description;
		rmi.attach = attach;
		rmi.isServerCall = isServerCall;
		rmi.pBase = g_vMessages;
		rmi.reliability = reliability;
		rmi.pMsgDef = 0;
		rmi.lowDelay = lowDelay;
		return &rmi;
	}

private:
	static size_t g_nMessages;
	static SGameObjectExtensionRMI g_vMessages[MAX_STATIC_MESSAGES];
};

#define DECLARE_RMI(name, params, reliability, attachment, isServer, lowDelay) \
	public: \
		struct MethodInfo_##name \
		{ \
			MethodInfo_##name( const SGameObjectExtensionRMI * pMethodInfo ) { this->pMethodInfo = pMethodInfo; } \
			const SGameObjectExtensionRMI * pMethodInfo; \
			ILINE void Verify( const params& p ) const \
			{ \
			} \
		}; \
	private: \
		static INetAtSyncItem * Decode_##name( TSerialize, EntityId*, INetChannel* ); \
		bool Handle_##name( const params&, INetChannel* ); \
		static const ERMIAttachmentType Attach_##name = attachment; \
		static const bool ServerCall_##name = isServer; \
		static const ENetReliabilityType Reliability_##name = reliability; \
		static const bool LowDelay_##name = lowDelay; \
		typedef params Params_##name; \
		static MethodInfo_##name m_info##name; \
	public: \
		static const MethodInfo_##name& name() { return m_info##name; }

#define DECLARE_INTERFACE_RMI(name, params, reliability, attachment, isServer, lowDelay) \
	protected: \
		static const ERMIAttachmentType Attach_##name = attachment; \
		static const bool ServerCall_##name = isServer; \
		static const ENetReliabilityType Reliability_##name = reliability; \
		static const bool LowDelay_##name = lowDelay; \
		typedef params Params_##name; \
	public: \
		struct MethodInfo_##name \
		{ \
			MethodInfo_##name( const SGameObjectExtensionRMI * pMethodInfo ) { this->pMethodInfo = pMethodInfo; } \
			const SGameObjectExtensionRMI * pMethodInfo; \
			ILINE void Verify( const params& p ) const \
			{ \
			} \
		}; \
		virtual const MethodInfo_##name& name() = 0

#define DECLARE_IMPLEMENTATION_RMI(name) \
	private: \
		INetAtSyncItem * Decode_##name( TSerialize, EntityId, INetChannel* ); \
		static bool Handle_##name( const Params_##name&, INetChannel* ); \
		static MethodInfo_##name m_info##name; \
	public: \
		const MethodInfo_##name& name() { return m_info##name; }

#define IMPLEMENT_RMI(cls, name) \
	cls::MethodInfo_##name cls::m_info##name = cls::Helper_AddMessage( &cls::Decode_##name, "RMI:" #cls ":" #name, cls::Attach_##name, cls::ServerCall_##name, cls::Reliability_##name, cls::LowDelay_##name ); \
	INetAtSyncItem * cls::Decode_##name( TSerialize ser, EntityId * pID, INetChannel* pChannel ) \
	{ \
		assert(pID); \
		Params_##name params; \
		params.SerializeWith( ser ); \
		return CRMIAtSyncItem<Params_##name, cls>::Create( params, *pID, m_info##name.pMethodInfo, &cls::Handle_##name, pChannel ); \
	} \
	ILINE bool cls::Handle_##name( const Params_##name& params, INetChannel* pNetChannel )

#define IMPLEMENT_INTERFACE_RMI(cls, name) \
	cls::MethodInfo_##name cls::m_info##name = cls::Helper_AddMessage( &cls::Decode_##name, "RMI:" #cls ":" #name, cls::Attach_##name, cls::ServerCall_##name, cls::Reliability_##name, cls::LowDelay_##name ); \
	INetAtSyncItem * cls::Decode_##name( TSerialize ser, INetChannel * pChannel ) \
	{ \
		Params_##name params; \
		params.SerializeWith( ser ); \
		return CRMIAtSyncItem<Params_##name, cls>::Create( params, id, m_info##name.pMethodInfo, &cls::Handle_##name, pChannel ); \
	} \
	ILINE bool cls::Handle_##name( const Params_##name& params, INetChannel* pNetChannel )

/*
 * _FAST versions may send the RMI without waiting for the frame to end; be sure that consistency with the entity is not important!
 */

#define DECLARE_SERVER_RMI_PREATTACH(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PreAttach, true, false)
#define DECLARE_CLIENT_RMI_PREATTACH(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PreAttach, false, false)
#define DECLARE_SERVER_RMI_POSTATTACH(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PostAttach, true, false)
#define DECLARE_CLIENT_RMI_POSTATTACH(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PostAttach, false, false)
#define DECLARE_SERVER_RMI_NOATTACH(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_NoAttach, true, false)
#define DECLARE_CLIENT_RMI_NOATTACH(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_NoAttach, false, false)

#define DECLARE_SERVER_RMI_PREATTACH_FAST(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PreAttach, true, true)
#define DECLARE_CLIENT_RMI_PREATTACH_FAST(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PreAttach, false, true)
#define DECLARE_SERVER_RMI_POSTATTACH_FAST(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PostAttach, true, true)
#define DECLARE_CLIENT_RMI_POSTATTACH_FAST(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_PostAttach, false, true)
#define DECLARE_SERVER_RMI_NOATTACH_FAST(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_NoAttach, true, true)
#define DECLARE_CLIENT_RMI_NOATTACH_FAST(name, params, reliability) DECLARE_RMI(name, params, reliability, eRAT_NoAttach, false, true)
/*
// TODO: temporary, until a good solution for sending noattach fast messages can be found
#define DECLARE_SERVER_RMI_NOATTACH_FAST(a,b,c) DECLARE_SERVER_RMI_NOATTACH(a,b,c)
#define DECLARE_CLIENT_RMI_NOATTACH_FAST(a,b,c) DECLARE_CLIENT_RMI_NOATTACH(a,b,c)
*/

#define DECLARE_INTERFACE_SERVER_RMI_PREATTACH(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PreAttach, true, false)
#define DECLARE_INTERFACE_CLIENT_RMI_PREATTACH(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PreAttach, false, false)
#define DECLARE_INTERFACE_SERVER_RMI_POSTATTACH(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PostAttach, true, false)
#define DECLARE_INTERFACE_CLIENT_RMI_POSTATTACH(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PostAttach, false, false)

#define DECLARE_INTERFACE_SERVER_RMI_PREATTACH_FAST(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PreAttach, true, true)
#define DECLARE_INTERFACE_CLIENT_RMI_PREATTACH_FAST(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PreAttach, false, true)
#define DECLARE_INTERFACE_SERVER_RMI_POSTATTACH_FAST(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PostAttach, true, true)
#define DECLARE_INTERFACE_CLIENT_RMI_POSTATTACH_FAST(name, params, reliability) DECLARE_INTERFACE_RMI(name, params, reliability, eRAT_PostAttach, false, true)

template <class T, class U, size_t N>
size_t CGameObjectExtensionHelper<T,U,N>::g_nMessages = 0;
template <class T, class U, size_t N>
SGameObjectExtensionRMI CGameObjectExtensionHelper<T,U,N>::g_vMessages[N];

struct IGameObjectView
{
	virtual void UpdateView( SViewParams& params ) = 0;
	virtual void PostUpdateView( SViewParams& params ) = 0;
};

struct IGameObjectProfileManager
{
	virtual bool SetAspectProfile( EEntityAspects aspect, uint8 profile ) = 0;
	virtual uint8 GetDefaultProfile( EEntityAspects aspect ) = 0;
};

// Summary
//   Interface used to implement a game object extension
struct IGameObjectExtension
{
	IGameObjectExtension() : m_pGameObject(0), m_entityId(0), m_pEntity(0) {}

	// Summary
	//   Initialize the extension
	// Parameters
	//   pGameObject - a pointer to the game object which will use the extension
	// Remarks
	//   IMPORTANT: It's very important that the implementation of this function 
	//   call the protected function SetGameObject() during the execution of the 
	//   Init() function. Unexpected results would happen otherwise.
	virtual bool Init( IGameObject * pGameObject ) = 0;
	
	// Summary
	//   Post-initialize the extension
	// Description
	//   During the post-initialization, the extension is now contained in the 
	//   game object
	// Parameters
	//   pGameObject - a pointer to the game object which own the extension
	virtual void PostInit( IGameObject * pGameObject ) = 0;

	// Summary
	//   Initialize the extension (client only)
	// Description
	//   This initialization function should be use to initialize resource only 
	//   used in the client
	// Parameters
	//   channelId - id of the server channel of the client to receive the 
	//               initialization
	virtual void InitClient(int channelId) = 0;

	// Summary
	//   Post-initialize the extension (client only)
	// Description
	//   This initialization function should be use to initialize resource only 
	//   used in the client. During the post-initialization, the extension is now 
	//   contained in the game object
	// Parameters
	//   channelId - id of the server channel of the client to receive the 
	//               initialization
	virtual void PostInitClient(int channelId) = 0;
	
	// Summary
	//   Releases the resources used by the object
	// Remarks
	//   This function should also take care of freeing the instance once the 
	//   resource are freed.
	virtual void Release() = 0;

	// Summary
	//   Performs the serialization the extension
	// Parameters
	//   ser - object used to serialize values
	//   aspect - serialization aspect, used for network serialization
	//   profile - which profile to serialize; 255 == don't care
	//   flags - physics flags to be used for serialization
	// See Also
	//   ISerialize
	virtual void FullSerialize( TSerialize ser ) = 0;
	virtual bool NetSerialize( TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags ) = 0;
	
	// Summary
	//   Performs post serialization fixes
	virtual void PostSerialize() = 0;

	// Summary
	//   Performs the serialization of special spawn information
	// Parameters
	//   ser - object used to serialize values
	// See Also
	//   Serialize, ISerialize
	virtual void SerializeSpawnInfo( TSerialize ser ) = 0;

	virtual ISerializableInfoPtr GetSpawnInfo() = 0;

	// Summary
	//   Performs frame dependent extension updates
	// Parameters
	//   ctx - Update context
	//   updateSlot - updateSlot
	// See Also
	//   PostUpdate, SEntityUpdateContext, IGameObject::EnableUpdateSlot
	virtual void Update( SEntityUpdateContext& ctx, int updateSlot ) = 0;

	// Summary
	//   Processes game specific events
	// Parameters
	//   event - game event
	// See Also
	//   SGameObjectEvent
	virtual void HandleEvent( const SGameObjectEvent& event ) = 0;

	// Summary
	//   Processes entity specific events
	// Parameters
	//   event - entity event, see SEntityEvent for more information
	virtual void ProcessEvent( SEntityEvent& event ) = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) = 0;

	virtual void SetChannelId(uint16 id) = 0;
	virtual void SetAuthority( bool auth ) = 0;

	// Summary
	//   Retrieves the RMI Base pointer
	// Description
	//   Internal function used for RMI. It's usually implemented by 
	//   CGameObjectExtensionHelper provides a way of checking who should 
	//   receive some RMI call.
	virtual const void * GetRMIBase() const = 0;

	// Summary
	//   Performs an additional update
	// Parameters
	//   frameTime - time elapsed since the last frame update
	// See Also
	//   Update, IGameObject::EnablePostUpdates, IGameObject::DisablePostUpdates
	virtual void PostUpdate( float frameTime ) = 0;

	// Summary
	virtual void PostRemoteSpawn() = 0;

	// Summary
	//   Retrieves the pointer to the game object
	// Returns
	//   A pointer to the game object which hold this extension
	ILINE IGameObject * GetGameObject() const { return m_pGameObject; }

	// Summary
	//   Retrieves the pointer to the entity
	// Returns
	//   A pointer to the entity which hold this game object extension
	ILINE IEntity * GetEntity() const { return m_pEntity; }

	// Summary
	//   Retrieves the EntityId
	// Returns
	//   An EntityId to the entity which hold this game object extension
	ILINE EntityId GetEntityId() const { return m_entityId; }

protected:
	void SetGameObject( IGameObject * pGameObject )
	{
		m_pGameObject = pGameObject;
		m_pEntity = pGameObject->GetEntity();
		m_entityId = pGameObject->GetEntityId();
	}

private:
	IGameObject * m_pGameObject;
	EntityId m_entityId;
	IEntity * m_pEntity;
};

#endif
