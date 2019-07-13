/*************************************************************************
 Crytek Source File.
 Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
 $Id$
 $DateTime$
 Description:  Message definition to id management
 -------------------------------------------------------------------------
 History:
 - 07/25/2001   : Alberto Demichelis, Created
 - 07/20/2002   : Martin Mittring, Cleaned up
 - 09/08/2004   : Craig Tiller, Refactored
 - 17/09/2004   : Craig Tiller, Introduced contexts
*************************************************************************/

#ifndef __INETWORK_H__
#define __INETWORK_H__

#pragma once

#ifdef CRYNETWORK_EXPORTS
	#define CRYNETWORK_API DLL_EXPORT
#else
	#define CRYNETWORK_API DLL_IMPORT
#endif

#if _MSC_VER > 1000
#pragma warning(disable:4786)
#endif

#include "ISerialize.h"
#include "TimeValue.h"
#include "ITimer.h"

#include <vector>				// STL vector<>

#define SERVER_DEFAULT_PORT 64087
#define SERVER_DEFAULT_PORT_STRING "64087"

#define SERVER_MULTICAST_PORT	5678
//<<FIXME>> It can be changed
#define SERVER_MULTICAST_ADDRESS	"234.5.6.7"

////////////////////////////////////////////////////////////////////////////////////////
// Interfaces
////////////////////////////////////////////////////////////////////////////////////////
struct IRConSystem;
class ICrySizer;

class INetMessage;
struct INetMessageSink;

struct IGameNub;
struct IGameQuery;
struct IGameSecurity;
struct IGameChannel;
struct IGameContext;
struct IServerSnooperSink;
struct INetNub;
struct INetChannel;
struct INetContext;
struct IServerSnooper;
struct ICVar;
struct IRMIMessageBody;
struct ILanQueryListener;
struct IGameQueryListener;
struct IRMIListener;
struct INetAtSyncItem;
struct IRMICppLogger;
struct IContextEstablishTask;
struct IContextEstablisher;
struct INetSendable;

class CNetSerialize;

typedef _smart_ptr<IRMIMessageBody> IRMIMessageBodyPtr;

// this enum describes different reliability mechanisms for
// packet delivery
// do not change ordering here without updating ServerContextView, ClientContextView
enum ENetReliabilityType
{
	eNRT_ReliableOrdered,
	eNRT_ReliableUnordered,
	eNRT_UnreliableOrdered,
	eNRT_UnreliableUnordered,
	// must be last
	eNRT_NumReliabilityTypes
};

enum ERMIAttachmentType
{
	// note: implementation of CContextView relies on the first two values being
	// as they are
	eRAT_PreAttach = 0,
	eRAT_PostAttach = 1,
	eRAT_NoAttach,
	// must be last
	eRAT_NumAttachmentTypes
};

enum EContextViewState
{
	// we use this initially to ensure that eCVS_Begin is a real state 
	// change
	eCVS_Initial = 0,
	// begin setting up a context
	eCVS_Begin,
	// establish the context (load any data files)
	eCVS_EstablishContext,
	// configure any data-dependent things before binding
	eCVS_ConfigureContext,
	// spawn (bind) any objects
	eCVS_SpawnEntities,
	// post-spawn initialization for any entities
	eCVS_PostSpawnEntities,
	// now play the game (finally!)
	eCVS_InGame,
	// reset some game state without completely destroying the context
	eCVS_NUM_STATES
};

enum EChannelConnectionState
{
	eCCS_WaitingForResponse,
	eCCS_StartingConnection,
	eCCS_InContextInitiation,
	eCCS_InGame,
	eCCS_Disconnecting,
};

enum EPunkType
{
	ePT_NotResponding,
	ePT_ChangedVars,
	ePT_ModifiedCode,
	ePT_ModifiedFile,
	ePT_NetProtocol
};

enum ECheatProtectionLevel
{
	eCPL_Default = 0,
	eCPL_RandomChecks,
	eCPL_Code,
	eCPL_High
};

enum EDisconnectionCause
{
	// this cause must be first! - timeout occured
	eDC_Timeout = 0,
	// incompatible protocols
	eDC_ProtocolError,
	// failed to resolve an address
	eDC_ResolveFailed,
	// versions mismatch
	eDC_VersionMismatch,
	// server is full
	eDC_ServerFull,
	// user initiated kick
	eDC_Kicked,
	// teamkill ban/ admin ban
	eDC_Banned,
	// context database mismatch
	eDC_ContextCorruption,
	// password mismatch
	eDC_AuthenticationFailed,
	// login required to join
	eDC_NotLoggedIn,
	// cdkey check failed
	eDC_CDKeyChekFailed,
	// misc. game error
	eDC_GameError,
	// dx10 not found
	eDC_NotDX10Capable,
	// the nub has been destroyed
	eDC_NubDestroyed,
	// icmp reported error
	eDC_ICMPError,
	// NAT negotiation error
	eDC_NatNegError,
	// punk buster detected something bad
	eDC_PunkDetected,
	// demo playback finished
	eDC_DemoPlaybackFinished,
	// demo playback file not found
	eDC_DemoPlaybackFileNotFound,
  // user decided to stop playing
  eDC_UserRequested,
	// user should have controller connected
	eDC_NoController,
	// unable to connect to server
	eDC_CantConnect,
	// client running different mod to server (name or version)
	eDC_ModMismatch,
	// client doesn't have the map the server is running
	eDC_MapNotFound,
	// client map differs from server (checksum)
	eDC_MapVersion,
	// this cause must be last! - unknown cause
	eDC_Unknown
};

enum EAspectFlags
{
	eAF_Delegatable          = 0x10,
	eAF_ServerManagedProfile = 0x20,
	eAF_HashState            = 0x40,
	eAF_TimestampState       = 0x80,
};

enum EMessageParallelFlags
{
	eMPF_BlocksStateChange   = 0x0001,
	eMPF_DecodeInSync        = 0x0002,
	eMPF_NoSendDelay         = 0x0004, // lock the network thread and get this message sent as quickly as possible
	eMPF_DiscardIfNoEntity   = 0x0008,
	eMPF_StateChange         = 0x0010,
	eMPF_DontAwake           = 0x0020, // not a critical message - don't wake up sending
	eMPF_AfterSpawning       = 0x0040,
};

enum ENetworkGameSync
{
	eNGS_FrameStart = 0,
	eNGS_FrameEnd,
	eNGS_Shutdown,
	// must be last
	eNGS_NUM_ITEMS
};

enum EMessageSendResult
{
	eMSR_SentOk, // message was successfully sent
	eMSR_NotReady, // message wasn't ready to be sent (try again later)
	eMSR_FailedMessage, // failed sending the message, but it's ok
	eMSR_FailedConnection, // failed sending so badly that we need to disconnect!
	eMSR_FailedPacket, // failed sending the message; don't try to send anything more this packet
};

ILINE EMessageSendResult WorstMessageSendResult( EMessageSendResult r1, EMessageSendResult r2 )
{
	if (r1 < r2)
		return r2;
	else
		return r1;
}

typedef uint16 TNetTypeID;
// must be at least the size of EntityId
typedef uint16 TNetObjectParamsID;
typedef uint32 TNetChannelID;
static const char * LOCAL_CONNECTION_STRING = "<local>";
static const char * NULL_CONNECTION_STRING = "<null>";
static const size_t MaxProfilesPerAspect = 8;

struct SSendableHandle
{
	SSendableHandle() : id(0), salt(0) {}

	uint32 id;
	uint32 salt;

	ILINE bool operator!() const
	{
		return id==0 && salt==0;
	}
	typedef uint32 (SSendableHandle::*unknown_bool_type);
	ILINE operator unknown_bool_type() const
	{
		return !!(*this)? &SSendableHandle::id : NULL;
	}
	ILINE bool operator!=( const SSendableHandle& rhs ) const
	{
		return !(*this == rhs);
	}
	ILINE bool operator==( const SSendableHandle& rhs ) const
	{
		return id==rhs.id && salt==rhs.salt;
	}
	ILINE bool operator<( const SSendableHandle& rhs ) const
	{
		return id<rhs.id || (id==rhs.id && salt<rhs.salt);
	}

	const char * GetText( char * tmpBuf = 0 )
	{
		static char singlebuf[64];
		if (!tmpBuf)
			tmpBuf = singlebuf;
		sprintf(tmpBuf, "%.8x:%.8x", id, salt);
		return tmpBuf;
	}
};

typedef std::pair<bool,INetAtSyncItem*> TNetMessageCallbackResult;

// this structure describes the meta-data we need to be able
// to dispatch/handle a message
struct SNetMessageDef
{
	typedef TNetMessageCallbackResult (*HandlerType)( 
		uint32 nUser,
		INetMessageSink*, 
		TSerialize,
		uint32 curSeq,
		uint32 oldSeq,
		EntityId* pRmiObject,
		INetChannel * pChannel
		);
	HandlerType handler;
	const char * description;
	ENetReliabilityType reliability;
	uint32 nUser;
	uint32 parallelFlags;
	bool CheckParallelFlag( EMessageParallelFlags flag ) const
	{
		return (parallelFlags & flag) != 0;
	}
	struct ComparePointer
	{
		bool operator () (const SNetMessageDef *p1, const SNetMessageDef *p2)
		{
			return strcmp(p1->description, p2->description) < 0;
		}
	};
};

// if we are playing back a demo, and simulating packets, oldSeq == curSeq == DEMO_PLAYBACK_SEQ_NUMBER
static const uint32 DEMO_PLAYBACK_SEQ_NUMBER = ~uint32(0);

// this structure is a nice cross-dll way of passing a table
// of SNetMessageDef's
struct SNetProtocolDef
{
	size_t nMessages;
	SNetMessageDef * vMessages;
};

struct INetBreakagePlayback : public CMultiThreadRefCount
{
	virtual void SpawnedEntity( int idx, EntityId id ) = 0;
	virtual EntityId GetEntityIdForIndex( int idx ) = 0;
};
typedef _smart_ptr<INetBreakagePlayback> INetBreakagePlaybackPtr;

struct ISerializableInfo : public CMultiThreadRefCount, public ISerializable {};
typedef _smart_ptr<ISerializableInfo> ISerializableInfoPtr;

struct INetSendableSink
{
	virtual void NextRequiresEntityEnabled( EntityId id ) = 0;
	virtual void SendMsg( INetSendable * pSendable ) = 0;
};

struct IBreakDescriptionInfo : public CMultiThreadRefCount
{
	virtual void GetAffectedRegion(AABB& aabb) = 0;
	virtual void AddSendables( INetSendableSink * pSink, int32 brkId ) = 0;
};
typedef _smart_ptr<IBreakDescriptionInfo> IBreakDescriptionInfoPtr;

struct SNetBreakDescription
{
	SNetBreakDescription() {}
	IBreakDescriptionInfoPtr pMessagePayload;
	EntityId * pEntities;
	int nEntities;
};

enum ENetworkServiceInterface
{
	eNSI_CDKeyValidation,
};

enum ENetSendableStateUpdate
{
	eNSSU_Ack,
	eNSSU_Requeue,
	eNSSU_Nack,
	eNSSU_Rejected
};

// Description:
//    An ancillary service provided by the network system
struct INetworkService;
typedef _smart_ptr<INetworkService> INetworkServicePtr;

struct IRemoteControlSystem;
struct ISimpleHttpServer;

struct SNetGameInfo
{
	SNetGameInfo() : maxPlayers(-1) {}
	int maxPlayers;
};

// Description:
//    Main access point for creating Network objects
struct INetwork
{
	virtual bool IsPbInstalled() = 0;

	virtual void SetNetGameInfo( SNetGameInfo ) = 0;
	virtual SNetGameInfo GetNetGameInfo() = 0;

	// Description:
	//    retrieves RCON system interface
	virtual IRemoteControlSystem* GetRemoteControlSystemSingleton() = 0;

	// Description:
	//    retrieves HTTP server interface
	virtual ISimpleHttpServer* GetSimpleHttpServerSingleton() = 0;

	// Description:
	//    Disconnect everything before fast shutdown
	virtual void FastShutdown() = 0;

	// Description:
	//    Enable/disable multithreading
	virtual void EnableMultithreading( bool enable ) = 0;

	// Description:
	//    Initialize some optional service
	virtual INetworkServicePtr GetService( const char * name ) = 0;

	// Description:
	//    Allocates a nub for communication with another computer
	// See Also: INetNub, IGameNub
	// Arguments:
	//    address - specify an address for the nub to open, or NULL
	//    pGameNub - the game half of this nub, must be non-null (will be released by the INetNub)
	//    pSecurity - security callback interface (for banning/unbanning ip's, etc, can be null)
	//    pGameQuery - interface for querying information about the nub (for server snooping, etc, can be null)
	// Return:
	//    Pointer to the new INetNub, or NULL on failure
	virtual INetNub * CreateNub( const char * address, 
		IGameNub * pGameNub, 
		IGameSecurity * pSecurity,
		IGameQuery * pGameQuery ) = 0;

	// Description:
	//    Queries the local network for games that are running
	// Arguments:
	//    pGameQueryListener - the game half of the query listener, must be non-null (will be released by INetQueryListener)
	// Return:
	//    Pointer to the new INetQueryListener, or NULL on failure
	virtual ILanQueryListener * CreateLanQueryListener( IGameQueryListener * pGameQueryListener ) = 0;

	// Description:
	//    Create a server context with an associated game context
	enum ENetContextCreationFlags
	{
		eNCCF_Multiplayer = BIT(0)
	};
	virtual INetContext * CreateNetContext( IGameContext * pGameContext, uint32 flags ) = 0;

	// Description:
	//    release the interface (and delete the object that implements it)
  virtual void Release() = 0;
	// Description:
	//    Gather memory statistics for the network module
	virtual void GetMemoryStatistics(ICrySizer *pSizer) = 0;

	// Description:
	//    Update all nubs and contexts
	// Arguments:
	//    blocking - time to block for network input (zero to not block)
	virtual void SyncWithGame( ENetworkGameSync syncType ) = 0; 

	// Description:
	//    Get the local host name
	virtual const char * GetHostName() = 0;

	// Description:
	//    Get the local ip of host
	virtual uint32 GetHostLocalIp() = 0;

  // Description:
  //    Set CD key string for online validation
  virtual void SetCDKey(const char*) = 0;

	// Description:
	//     Sends a console command event to PunkBuster
	virtual bool PbConsoleCommand(const char*, int length) = 0;  // EvenBalance - M. Quinn
	virtual void PbCaptureConsoleLog(const char* output, int length) = 0;  // EvenBalance - M. Quinn
	virtual void PbServerAutoComplete(const char*, int length) = 0;  // EvenBalance - M. Quinn
	virtual void PbClientAutoComplete(const char*, int length) = 0;  // EvenBalance - M. Quinn

	virtual bool IsPbSvEnabled() = 0;
	virtual bool HasNetworkConnectivity() = 0;

	virtual void StartupPunkBuster(bool server) = 0;
	virtual void CleanupPunkBuster() = 0;

	virtual bool IsPbClEnabled() = 0;
};

// This interface is implemented by CryNetwork, and is used by 
// INetMessageSink::DefineProtocol to find all of the message sinks that should
// be attached to a channel
struct IProtocolBuilder
{
	virtual void AddMessageSink( 
		INetMessageSink * pSink, 
		const SNetProtocolDef& protocolSending,
		const SNetProtocolDef& protocolReceiving ) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////
/*! this interface must be implemented by anyone who wants to receive CTP messages
*/
struct INetMessageSink
{
	// called on setting up an endpoint to figure out what kind of messages can be sent
	// and received
	virtual void DefineProtocol( IProtocolBuilder * pBuilder ) = 0;
	virtual bool HasDef( const SNetMessageDef * pDef ) = 0;
};

struct IVoiceGroup
{
	virtual void AddEntity( const EntityId id ) = 0;
	virtual void RemoveEntity( const EntityId id ) = 0;
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};

// Description:
//    An IVoiceReader provides voice samples to be sent over network
//
struct IVoiceDataReader
{
	virtual bool Update() = 0;
	virtual uint32 GetSampleCount() = 0;
	virtual int16* GetSamples() = 0;
};

// Description:
// An IVoiceContext provides interface for network voice data manipulation
//

struct IVoiceContext
{
	//Description:
	//    Sets voice data reader for entity, voice context requests it when it needs new data
	virtual void SetVoiceDataReader(EntityId id,IVoiceDataReader*) = 0;

	//Description:
	//	Retrieve voice packet from network system
	//	NOTE: called directly from the network thread to reduce voice transmission latency
	virtual bool GetDataFor( EntityId id, uint32 numSamples, int16 * samples ) = 0;

	// Description:
	// Create new voice group
	virtual IVoiceGroup* CreateVoiceGroup() = 0;

	virtual void Mute(EntityId requestor, EntityId id, bool mute) = 0;
	virtual bool IsMuted(EntityId requestor, EntityId id) = 0;
	virtual void PauseDecodingFor(EntityId id, bool pause) = 0;

	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual bool IsEnabled() = 0;

	virtual void GetMemoryStatistics(ICrySizer* pSizer) = 0;

	// Description:
	//	Force recreation of routing table (when players change voice group, for instance)
	virtual void InvalidateRoutingTable() = 0;
};

// Description:
//    An INetContext manages the list of objects synchronized over the network
//    ONLY to be implemented in CryNetwork
struct INetContext
{
	// Description:
	//    Release this context
	virtual void DeleteContext() = 0;
	
	// Description:
	//    Record this context as a demo file
	virtual void ActivateDemoRecorder( const char * filename ) = 0;
	// Description:
	//    Record this context as a demo file
	virtual void ActivateDemoPlayback( const char * filename, INetChannel * pClient, INetChannel * pServer ) = 0;
	// Description:
	//		Are we playing back a demo session?
	virtual bool IsDemoPlayback() const = 0;
	// Description:
	//    If we're recording, log an RMI call to a file
	virtual void LogRMI( const char * function, ISerializable * pParams ) = 0;
	// Description:
	//    Log a custom (C++ based) RMI call to the demo file
	virtual void LogCppRMI( EntityId id, IRMICppLogger * pLogger ) = 0;

	// Description:
	//    Enable on a server to lower pass-through message latency
	virtual void EnableBackgroundPassthrough( bool enable ) = 0;

	// Description:
	//    Call just after construction to declare which aspect bits have which characteristics
	// Arguments:
	//    aspectBit - the bit we are changing (1,2,4,8,16,32,64 or 128)
	//    aspectFlags - some combination of eAF_* that describes how this parameter will change
	//    basePriority - base priority for this aspect (TODO: may be removed)
	virtual void DeclareAspect( const char * name, uint8 aspectBit, uint8 aspectFlags ) = 0;
	// Description:
	//    Modify the profile of an aspect
	// Arguments:
	//    aspectBit - the aspect we are changing
	//    profile - the new profile of the aspect
	virtual void SetAspectProfile( EntityId id, uint8 aspectBit, uint8 profile ) = 0;
	// Description:
	//    Fetch the profile on an aspect; this is a very heavyweight method as it must completely flush the state of network queues before operating
	virtual uint8 GetAspectProfile( EntityId id, uint8 aspectBit ) = 0;
	// Description:
	//    Bind an object to the network so it starts synchronizing its state
	// Arguments:
	//    id - a user supplied id for this object (probably the entity id :))
	//    aspectBits - a bit mask specifying which aspects are enabled now
	virtual void BindObject( EntityId id, uint8 aspectBits, bool bStatic ) = 0;
	virtual void SafelyUnbind( EntityId id ) = 0;
	// Description:
	//    Remove the binding of an object to the network
#ifdef SP_DEMO
	virtual bool UnbindObject( EntityId id ) = 0;
#else
	virtual bool IsBound( EntityId id ) = 0;
#endif
	// Description:
	//    Must be called ONCE in response to a message sent from a SendSpawnObject call on the server
	//    (from the client)
	virtual void SpawnedObject( EntityId id ) = 0;
	// Description:
	//    Determine if an object is bound or not
#ifdef SP_DEMO
	virtual bool IsBound( EntityId id ) = 0;
#else
	virtual bool UnbindObject( EntityId id ) = 0;
#endif
	// Description:
	//    Enable/disable the synchronization of some aspects over the network
	// Arguments:
	//    id - the id of a *bound* object
	//    aspectBits - the aspects to enable/disable
	//    enabled - are we enabling new aspects, or disabling old ones
	virtual void EnableAspects( EntityId id, uint8 aspectBits, bool enabled ) = 0;
	// Description:
	//    Some aspects of an object have been changed - and those aspects should be
	//    updated shortly (the only way to get eAF_UpdateOccasionally aspects updated)
	// Arguments:
	//    id - the id of the object changed
	//    aspectBits - a bit field describing which aspects have been changed
	//    pChannel - NULL if changed for all channels, or the specific channel to mark the aspect as
	//               changed for
	virtual void ChangedAspects( EntityId id, uint8 aspectBits, INetChannel * pChannel ) = 0;
	// Description:
	//    Update the location of an object (allows correct priorities to be assigned to it)
	virtual void ChangedTransform( EntityId id, const Vec3& pos, const Quat& rot, float drawDist ) = 0;
	virtual void ChangedFov( EntityId id, float fov ) = 0;
	// Description:
	//    Pass authority for updating an object to some remote channel;
	//    This channel must have had SetServer() called on it at construction time
	//    (only those aspects marked as eAF_Delegatable are passed on)
	// Arguments:
	//    id - the id of a *bound* object to change authority for
	//    pControlling - the channel who will now control the object (or NULL if we wish to
	//                   take control)
	virtual void DelegateAuthority( EntityId id, INetChannel * pControlling ) = 0;

	// Description:
	//    Change the game context (destroy all objects, and cause all channels to load
	//    a new level, and reinitialize state)
	virtual bool ChangeContext() = 0;

	// Description:
	//    The level has finished loading (ie the slow part of context establishment is complete)
	//    Call this after a call to IGameContext::EstablishContext
	virtual void EstablishedContext( int establishToken ) = 0;

	// Description:
	//    Removing an RMI listener -- make sure there's no pointers left to it
	virtual void RemoveRMIListener( IRMIListener * pListener ) = 0;

	// Description:
	//    Determine if the context on the remote end of a channel has authority over an object
	virtual bool RemoteContextHasAuthority( INetChannel * pChannel, EntityId id ) = 0;

	// Description:
	//    Specify an objects 'network parent' 
	//    - child objects are unspawned after the parent object is
	//    - child objects are spawned after the parent object is
	virtual void SetParentObject( EntityId objId, EntityId parentId ) = 0;

	virtual void RequestRemoteUpdate( EntityId id, uint8 aspects ) = 0;

	virtual void LogBreak( const SNetBreakDescription& des ) = 0;

	virtual bool SetSchedulingParams( EntityId objId, uint32 normal, uint32 owned ) = 0;
	virtual void PulseObject( EntityId objId, uint32 pulseType ) = 0;

	virtual IVoiceContext* GetVoiceContext() = 0;

	virtual int RegisterPredictedSpawn( INetChannel * pChannel, EntityId id ) = 0;
	virtual void RegisterValidatedPredictedSpawn( INetChannel * pChannel, int predictionHandle, EntityId id ) = 0;

	virtual void SetLevelName( const char * levelName ) = 0;

	virtual void GetMemoryStatistics(ICrySizer* pSizer) = 0;
};

struct INetSender
{
	INetSender( TSerialize sr, uint32 nCurrentSeq, uint32 nBasisSeq, bool isServer ) : ser(sr)
	{
		this->nCurrentSeq = nCurrentSeq;
		this->nBasisSeq = nBasisSeq;
		this->isServer = isServer;
	}
	virtual void BeginMessage( const SNetMessageDef * pDef ) = 0;
	virtual void BeginUpdateMessage( SNetObjectID ) = 0;
	virtual void EndUpdateMessage() = 0;
	TSerialize ser;
	bool isServer;
	uint32 nCurrentSeq;
	uint32 nBasisSeq;
};

class CSendUpdateMessageHelper
{
public:
	CSendUpdateMessageHelper( INetSender * pSender, SNetObjectID id ) : m_pSender(pSender)
	{
		m_pSender->BeginUpdateMessage(id);
	}
	~CSendUpdateMessageHelper()
	{
		m_pSender->EndUpdateMessage();
	}

private:
	CSendUpdateMessageHelper( const CSendUpdateMessageHelper& );
	CSendUpdateMessageHelper& operator=( const CSendUpdateMessageHelper& );

	INetSender * m_pSender;
};

struct INetBaseSendable
{
	INetBaseSendable() : m_cnt(0) {}
	virtual ~INetBaseSendable() {}

	virtual size_t GetSize() = 0;
	virtual EMessageSendResult Send( INetSender * pSender ) = 0;
	// callback for when we know what happened to a packet
	virtual void UpdateState( uint32 nFromSeq, ENetSendableStateUpdate update ) = 0;

	void AddRef()
	{
		CryInterlockedIncrement(&m_cnt);
	}
	void Release()
	{
		if (CryInterlockedDecrement(&m_cnt) <= 0)
			DeleteThis();
	}

private:
	volatile int m_cnt;

	virtual void DeleteThis()
	{
		delete this;
	}
};

static const int MAXIMUM_PULSES_PER_STATE = 6;
class CPriorityPulseState : public CMultiThreadRefCount
{
public:
	CPriorityPulseState() : m_count(0) {}

	struct SPulse
	{
		SPulse(uint32 k=0, CTimeValue t=0.0f) : key(k), tm(t) {}
		uint32 key;
		CTimeValue tm;
		bool operator<( const SPulse& rhs ) const
		{
			return key < rhs.key;
		}
	};

	void Pulse( uint32 key )
	{
		CTimeValue tm = gEnv->pTimer->GetAsyncTime();
		SPulse * pPulse = std::lower_bound(m_pulses, m_pulses+m_count, key);
		if (pPulse == m_pulses+m_count || key != pPulse->key)
		{
			if (m_count == MAXIMUM_PULSES_PER_STATE)
			{
#ifndef NDEBUG
				CryLog("Pulse ignored due to pulse overpopulation; increase MAXIMUM_PULSES_PER_STATE in INetwork.h");
#endif
				return;
			}
			else
			{
				m_pulses[m_count++] = SPulse(key, tm);
				std::sort(m_pulses, m_pulses+m_count);
			}
		}
		else
		{
			pPulse->tm = tm;
		}
	}

	const SPulse * GetPulses() const { return m_pulses; }
	uint32 GetNumPulses() const { return m_count; }

private:
	uint32 m_count;
	SPulse m_pulses[MAXIMUM_PULSES_PER_STATE];
};
typedef _smart_ptr<CPriorityPulseState> CPriorityPulseStatePtr;

struct SMessagePositionInfo
{
	SMessagePositionInfo() : havePosition(false), haveDrawDistance(false) {}
	bool haveDrawDistance;
	bool havePosition;
	float drawDistance;
	Vec3 position;
	SNetObjectID obj;
};

struct INetSendable : public INetBaseSendable
{
public:
	INetSendable( uint32 flags, ENetReliabilityType reliability ) : m_flags(flags), m_group(0), m_priorityDelta(0.0f), m_reliability(reliability) {}

	virtual const char * GetDescription() = 0;
	virtual void GetPositionInfo( SMessagePositionInfo& pos ) = 0;

	ENetReliabilityType GetReliability() const { return m_reliability; }
	uint32 GetGroup() const { return m_group; }
	float GetPriorityDelta() const { return m_priorityDelta; }

	bool CheckParallelFlag( EMessageParallelFlags f )
	{
		return (m_flags & f) != 0;
	}

	void SetGroup( uint32 group ) { m_group = group; }
	void SetPriorityDelta( float prioDelta ) { m_priorityDelta = prioDelta; }

	// should only be called by the network engine
	void SetPulses( CPriorityPulseStatePtr pulses ) { m_pulses = pulses; }
	const CPriorityPulseState * GetPulses() { return m_pulses; }

	uint32 GetFlags() const { return m_flags; }

private:
	ENetReliabilityType m_reliability;
	uint32 m_group;
	uint32 m_flags;
	float m_priorityDelta;
	CPriorityPulseStatePtr m_pulses;
};

typedef _smart_ptr<INetSendable> INetSendablePtr;

class INetSendableHook : public INetBaseSendable {};
typedef _smart_ptr<INetSendableHook> INetSendableHookPtr;

enum ESynchObjectResult
{
	eSOR_Ok,
	eSOR_Failed,
	eSOR_Skip,
};

// Description:
//    Interface for a channel to call in order to create/destroy objects, and when changing
//    context, to properly configure that context
struct IGameContext
{
	// Description:
	//    Initialize global tasks that we need to perform to establish the game context
	virtual bool InitGlobalEstablishmentTasks( IContextEstablisher * pEst, int establishedToken ) = 0;
	// Description:
	//    Initialize tasks that we need to perform on a channel to establish the game context
	virtual bool InitChannelEstablishmentTasks( IContextEstablisher * pEst, INetChannel * pChannel, int establishedToken ) = 0;

	virtual INetSendableHookPtr CreateObjectSpawner( EntityId id, INetChannel * pChannel ) = 0;
	virtual bool SendPostSpawnObject( EntityId id, INetChannel * pChannel ) = 0;

	// Description:
	//    We have control of an objects delegatable aspects (or not)
	virtual void ControlObject( EntityId id, bool bHaveControl ) = 0;

	// Description:
	//    Synchronize a single aspect of an entity (nAspect will have exactly one bit set
	//    describing which aspect to synch)
	virtual ESynchObjectResult SynchObject( EntityId id, uint8 nAspect, uint8 nCurrentProfile, TSerialize ser, bool verboseLogging ) = 0;
	// Description:
	//    Change the current profile of an object (game code should ensure that things work out correctly - ie change physicalization)
	virtual bool SetAspectProfile( EntityId id, uint8 nAspect, uint8 nProfile ) = 0;
	// Description:
	//    An entity has been unbound (we may wish to destroy it)
	virtual void UnboundObject( EntityId id ) = 0;
	// Description:
	//    An entity has been bound (we may wish to do something with that info)
	virtual void BoundObject( EntityId id, uint8 nAspects ) = 0;
	// Description:
	//    Handle a remote method invocation
	virtual INetAtSyncItem * HandleRMI( bool bClient, EntityId objID, uint8 funcID, TSerialize ser ) = 0;
	// Description:
	//		Passes current demo playback mapped entity ID of the original demo recording server (local) player
	virtual void PassDemoPlaybackMappedOriginalServerPlayer(EntityId id) = 0;
	// Description:
	//    Fetch the default (spawned) profile for an aspect
	virtual uint8 GetDefaultProfileForAspect( EntityId id, uint8 aspectID ) = 0;

	virtual CTimeValue GetPhysicsTime() = 0;
	virtual void BeginUpdateObjects( CTimeValue physTime, INetChannel * pChannel ) = 0;
	virtual void EndUpdateObjects() = 0;

	virtual void OnEndNetworkFrame() = 0;
	virtual void OnStartNetworkFrame() = 0;

	virtual uint32 HashAspect( EntityId id, uint8 nAspect ) = 0;
	virtual void PlaybackBreakage( int breakId, INetBreakagePlaybackPtr pBreakage ) = 0;

	virtual string GetConnectionString(bool fake = false) const = 0;

	virtual void CompleteUnbind( EntityId id ) = 0;

	virtual void GetMemoryStatistics(ICrySizer* pSizer) = 0;
};


static const int CREATE_CHANNEL_ERROR_SIZE = 256;
struct SCreateChannelResult
{
	explicit SCreateChannelResult(IGameChannel* ch):pChannel(ch){errorMsg[0] = 0;}
	explicit SCreateChannelResult(EDisconnectionCause dc):pChannel(0),cause(dc){errorMsg[0] = 0;}
	IGameChannel*				pChannel;
	EDisconnectionCause cause;
	char								errorMsg[CREATE_CHANNEL_ERROR_SIZE];
};

struct IGameNub
{
	virtual void Release() = 0;
	// Description:
	//    Create a new game channel to handle communication with a client or server
	// Arguments:
	//    pChannel - the INetChannel implementing the communications part of this channel; you
	//               should call INetChannel::SetChannelType if you want client/server state
	//               machine based context setup semantics
	//    connectionString - NULL if we're creating a channel in response to a call to ConnectTo
	//               locally, otherwise it's the connectionString passed to ConnectTo remotely
	// Return:
	//
	//    Pointer to the new game channel; Release() will be called on it by the network
	//    engine when it's no longer required
	virtual SCreateChannelResult CreateChannel( INetChannel * pChannel, const char * connectionString ) = 0;

	// Description:
	//		Notifies the GameNub that an active connection attempt failed (before a NetChannel is created)
	//		By implementing this interface function, the game can effectively capture pre-channel connection failures
	//		NOTE: the GameNub should be prepared to be destroyed shortly after this call is finished
	virtual void FailedActiveConnect( EDisconnectionCause cause, const char * description ) = 0;
};

struct IGameChannel : public INetMessageSink
{
	// Description:
	//    Network engine will no longer use this object; it should be deleted
	virtual void Release() = 0;
	// Description:
	//    The other side has disconnected from us; cleanup is occuring, and we'll soon be released
	// Arguments:
	//    cause - why the disconnection occured
	virtual void OnDisconnect( EDisconnectionCause cause, const char * description ) = 0;
};

struct INetNub
{
	struct SStatistics
	{
		SStatistics() : bandwidthUp(0), bandwidthDown(0) {}
		float bandwidthUp;
		float bandwidthDown;
	};

	// Description:
	//    Game will no longer use this object; it should be deleted
	virtual void DeleteNub() = 0;
	// Description:
	//    Connect to a remote host
	// Arguments:
	//    address - the address to connect to ("127.0.0.1:20319" for example)
	//    connectionString - an arbitrary string to pass to the other end of this connection saying what we'd like to do
	// Returns:
	//    true for success
	virtual bool ConnectTo( const char * address, const char * connectionString ) = 0;
	// Description:
	//    Fetch current nub-wide statistics
	virtual const SStatistics& GetStatistics() = 0;
  // Description:
  //   Returns true if we nub is connecting now
  virtual bool IsConnecting() = 0;
  // Description:
  //  Initializes NAT negotiation process on server side
  virtual void OnNatCookieReceived(int cookie) = 0;
  // Description:
  //  An authorization result comes from master server
  virtual void OnCDKeyAuthResult(EntityId plr_id, bool success, const char* description) = 0;
  // Description:
  //  An authorization result come from master server
  virtual void DisconnectPlayer(EDisconnectionCause cause, EntityId plr_id, const char* reason) = 0;
  // Description:
  //  Collect memory usage info
	virtual void GetMemoryStatistics(ICrySizer* pSizer) = 0;
};

struct INetChannel : public INetMessageSink
{
	// see CNetwork::CreateConsoleVars for defaults!
	struct SPerformanceMetrics
	{
		SPerformanceMetrics() :
			pBitRateDesired(NULL),
			pBitRateToleranceHigh(NULL),
			pBitRateToleranceLow(NULL),
			pPacketRateDesired(NULL),
			pIdlePacketRateDesired(NULL),
			pPacketRateToleranceHigh(NULL),
			pPacketRateToleranceLow(NULL)
		{
		}
		// desired bit rate (in bits-per-second)
		ICVar * pBitRateDesired;
		// bit rate targets can climb to bitRateDesired * (1.0f + bitRateToleranceHigh)
		ICVar * pBitRateToleranceHigh;
		// bit rate targets can fall to bitRateDesired * (1.0f - bitRateToleranceLow)
		ICVar * pBitRateToleranceLow;
		// desired packet rate (in packets-per-second)
		ICVar * pPacketRateDesired;
		// desired packet rate when nothing urgent needs to be sent (in packets-per-second)
		ICVar * pIdlePacketRateDesired;
		// packet rate targets can climb to packetRateDesired * (1.0f + packetRateToleranceHigh)
		ICVar * pPacketRateToleranceHigh;
		// packet rate targets can fall to packetRateDesired * (1.0f - packetRateToleranceLow)
		ICVar * pPacketRateToleranceLow;
	};

	struct SStatistics
	{
		SStatistics() : bandwidthUp(0), bandwidthDown(0) {}
		float bandwidthUp;
		float bandwidthDown;
	};

	virtual void SetClient( INetContext * pServerContext, bool cheatProtection ) = 0;
	virtual void SetServer( INetContext * pServerContext, bool cheatProtection ) = 0;
	// Description:
	//    Set/reset the server password
	// Arguments:
	//    password - the new password string; will be checked at every context change if the length>0
	virtual void SetPassword( const char * password ) = 0;

	// Description:
	//    Set tolerances on packet delivery rate, bandwidth consumption, packet size, etc...
	// Arguments:
	//    pMetrics - An SPerformanceMetrics structure describing these tolerances
	virtual void SetPerformanceMetrics( SPerformanceMetrics * pMetrics ) = 0;
	// Description:
	//    Disconnect this channel
	virtual void Disconnect( EDisconnectionCause cause, const char * description ) = 0;
	// Description:
	//    Send a message to the other end of this channel
	virtual void SendMsg( INetMessage * ) = 0;
	// Description:
	//    Like AddSendable, but removes an old message if it still exists
	virtual bool SubstituteSendable( INetSendablePtr pMsg, int numAfterHandle, const SSendableHandle * afterHandle, SSendableHandle * handle ) = 0;
	// Description:
	//    Lower level, more advanced sending interface; enforce sending after afterHandle, and return a handle to this message in handle
	virtual bool AddSendable( INetSendablePtr pMsg, int numAfterHandle, const SSendableHandle * afterHandle, SSendableHandle * handle ) = 0;
	// Description:
	//    Undo a sent message if possible
	virtual bool RemoveSendable( SSendableHandle handle ) = 0;
	// Description:
	//    Get current channel based statistics for this channel
	virtual const SStatistics& GetStatistics() = 0;
	// Description:
	//    Get the remote time
	virtual CTimeValue GetRemoteTime() const = 0;
	// Description:
	//    Get the current ping
	virtual float GetPing( bool smoothed ) const = 0;
	// Description:
	//    is the system suffering high latency
	virtual bool IsSufferingHighLatency(CTimeValue nTime) const = 0;
	// Description:
	//    Dispatch a remote method invocation
	virtual void DispatchRMI( IRMIMessageBodyPtr pBody ) = 0;
	// Description:
	//    Declare an entity that "witnesses" the world... allows prioritization
	virtual void DeclareWitness( EntityId id ) = 0;
	// Description:
	//    Is this channel connected locally?
	virtual bool IsLocal() const = 0;
	// Description:
	//    Has this connection been successfully established?
	virtual bool IsConnectionEstablished() const = 0;
	// Description:
	//    Is this channel a fake one? (e.g. demorecording, debug channel etc.)
	//    ContextView extensions will not be created for fake channels
	virtual bool IsFakeChannel() const = 0;
	// Description:
	//    Get a descriptive string describing the channel
	virtual const char*  GetNickname() = 0;
  // Description:
  //    Get a descriptive string describing the channel
  virtual const char * GetName() = 0;
	// Description:
	//    Set a persistent nickname for this channel (MP playername)
	virtual void SetNickname(const char* name) = 0;
	// Description:
	//    Get the local channel ID
	virtual TNetChannelID GetLocalChannelID() = 0;
	// Description:
	//    Get the remote channel ID
	virtual TNetChannelID GetRemoteChannelID() = 0;

	virtual IGameChannel * GetGameChannel() = 0;

	virtual bool IsInTransition() = 0;

	virtual EContextViewState GetContextViewState() const = 0;
	virtual EChannelConnectionState GetChannelConnectionState() const = 0;
	virtual int GetContextViewStateDebugCode() const = 0;

	virtual bool IsTimeReady() const = 0;

	virtual CTimeValue TimeSinceVoiceTransmission() = 0;
	virtual CTimeValue TimeSinceVoiceReceipt( EntityId id ) = 0;
	virtual void AllowVoiceTransmission( bool allow ) = 0;
	
	// Description:
	//    Get the unique and persistent profile id for this client (profile id is associated with the user account)
	virtual int GetProfileId() const = 0;

	// Description:
	//    Does remote channel have pre-ordered copy?
	virtual bool IsPreordered() const = 0;

	virtual void GetMemoryStatistics(ICrySizer* pSizer, bool countingThis = false) = 0;
};

struct IGameSecurity
{
	// Description:
	//    Callback for making sure we're not communicating with a banned IP address
	virtual bool IsIPBanned( uint32 ip ) = 0;
	// Description:
	//    Called when a cheater is detected
	virtual void OnPunkDetected( const char * addr, EPunkType punkType ) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////
/*! this interface defines what goes into a CTP message
 */
class INetMessage : public INetSendable
{
public:
	INetMessage( const SNetMessageDef * pDef ) : INetSendable(pDef->parallelFlags, pDef->reliability), m_pDef(pDef) {}
	// returns the message definition - a static structure describing this message
	inline const SNetMessageDef * GetDef() const { return m_pDef; }
	// writes the packets payload to a stream (possibly using the pSerialize helper)
	virtual EMessageSendResult WritePayload( TSerialize ser, uint32 nCurrentSeq, uint32 nBasisSeq ) = 0;

	// auto-implementation of INetSendable
	virtual EMessageSendResult Send( INetSender * pSender )
	{
		pSender->BeginMessage( m_pDef );
		return WritePayload( pSender->ser, pSender->nCurrentSeq, pSender->nBasisSeq );
	}
	virtual const char * GetDescription() { return m_pDef->description; }
	virtual ENetReliabilityType GetReliability() { return m_pDef->reliability; }

	virtual void GetPositionInfo( SMessagePositionInfo& pos ) {}

protected:
	void ResetMessageDef( const SNetMessageDef * pDef )
	{
		m_pDef = pDef;
	}

private:
	const SNetMessageDef * m_pDef;
};

struct INetAtSyncItem
{
public:
	virtual bool Sync() = 0;
	virtual void DeleteThis() = 0;
};

struct IRMIListener
{
	virtual void OnSend( INetChannel * pChannel, int userId, uint32 nSeq ) = 0;
	virtual void OnAck( INetChannel * pChannel, int userId, uint32 nSeq, bool bAck ) = 0;
};

// kludgy, record C++ RMI's for the demo recorder
struct IRMICppLogger
{
public:
	virtual const char * GetName() = 0;
	virtual void SerializeParams( TSerialize ser ) = 0;
};

// this class defines a remote procedure call message
struct IRMIMessageBody
{
	IRMIMessageBody( 
		ENetReliabilityType reliability_, 
		ERMIAttachmentType attachment_, 
		EntityId objId_, 
		uint8 funcId_, 
		IRMIListener * pListener_, 
		int userId_,
		EntityId dependentId_ ) :
		m_cnt(0),
		reliability(reliability_), 
		attachment(attachment_), 
		objId(objId_),
		dependentId(dependentId_),
		funcId(funcId_),
		pMessageDef(0), 
		userId(userId_),
		pListener(pListener_)
	{
	}
	IRMIMessageBody( 
		ENetReliabilityType reliability_, 
		ERMIAttachmentType attachment_, 
		EntityId objId_, 
		const SNetMessageDef * pMessageDef_, 
		IRMIListener * pListener_, 
		int userId_,
		EntityId dependentId_ ) :
		m_cnt(0),
		reliability(reliability_), 
		attachment(attachment_), 
		objId(objId_),
		dependentId(dependentId_),
		funcId(0),
		pMessageDef(pMessageDef_), 
		userId(userId_),
		pListener(pListener_)
	{
	}
	virtual void SerializeWith( TSerialize ser ) = 0;
	virtual size_t GetSize() = 0;

	const ENetReliabilityType reliability;
	const ERMIAttachmentType attachment;
	const EntityId objId;
	const EntityId dependentId;
	const uint8 funcId;
	// can optionally set this to send a defined message instead of a script style function
	const SNetMessageDef * pMessageDef;
	// these two define a listening interface for really advance user stuff
	const int userId;
	IRMIListener * pListener;

	void AddRef()
	{
		CryInterlockedIncrement(&m_cnt);
	}
	void Release()
	{
		if (CryInterlockedDecrement(&m_cnt) <= 0)
			DeleteThis();
	}

private:
	volatile int m_cnt;

	virtual void DeleteThis()
	{
		delete this;
	}
};

// this class provides a mechanism for the network library to obtain information
// about the game being played
struct IGameQuery
{
	virtual XmlNodeRef GetGameState() = 0;
};

// this interface should is implemented by CryNetwork and provides the information
// for an IGameQueryListener... releasing this will release the game query listener
struct INetQueryListener
{
	virtual void DeleteNetQueryListener() = 0;
};

struct SServerData
{
	int iNumPlayers;
	int iMaxPlayers;
	string strMap;
	string strMode;
	string strVersion;
};

// this interface should be implemented by the game to receive asynchronous game
// query results
struct IGameQueryListener
{
	//add a server to intern list if not already existing, else just update data
	virtual void AddServer(const char* description, const char* target, const char* additionalText, uint32 ping) = 0;
	//remove a server independently from last answer etc.
	virtual void RemoveServer(string address) = 0;
	//add a received server pong
	virtual void AddPong(string address, uint32 ping) = 0;
	//returns a vector of running servers (as const char*)
	//please delete the list (not the servers!) afterwards!
	virtual void GetCurrentServers(char*** pastrServers, int& o_amount) = 0;
	//return a specific server by number (NULL if not available)
	virtual void GetServer(int number, char** server, char** data, int& ping) = 0;
	//returns the game server's data as a string and it's ping as an integer by reference
	virtual const char* GetServerData(const char* server, int& o_ping) = 0;
	//refresh pings of all servers in the list
	virtual void RefreshPings() = 0;
	virtual void OnReceiveGameState( const char *, XmlNodeRef ) = 0;
	virtual void Update() = 0;
	virtual void Release() = 0;
	//connects a network game to the specified server
	virtual void ConnectToServer(const char* server) = 0;
	//retrieve infos from the data string
	virtual void GetValuesFromData(char *strData,SServerData *pServerData) = 0;
	virtual void GetMemoryStatistics(ICrySizer* pSizer) = 0;
};

struct ILanQueryListener : public INetQueryListener
{
	//send a ping to the specified server
	virtual void SendPingTo(const char * addr) = 0;
	//this returns a pointer to the game query listener (game-side code of the listener)
	virtual IGameQueryListener* GetGameQueryListener() = 0;
	virtual void GetMemoryStatistics(ICrySizer * pSizer) = 0;
};

struct SContextEstablishState
{
	SContextEstablishState() 
		: contextState(eCVS_Initial)
		, pSender(NULL)
	{
	}
	EContextViewState contextState;
	INetChannel * pSender;
};

enum EContextEstablishTaskResult
{
	eCETR_Ok = 1,
	eCETR_Failed = 0,
	eCETR_Wait = 2
};

struct IContextEstablishTask
{
	virtual void Release() = 0;
	virtual EContextEstablishTaskResult OnStep( SContextEstablishState& ) = 0;
	virtual void OnFailLoading( bool hasEntered ) = 0;
	virtual const char * GetName() = 0;
};

struct IContextEstablisher
{
	virtual void GetMemoryStatistics(ICrySizer* pSizer) = 0;
	virtual void AddTask( EContextViewState state, IContextEstablishTask * pTask ) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////
// other stuff


// exports;
extern "C"{
	CRYNETWORK_API INetwork *CreateNetwork(ISystem *pSystem, int ncpu);
	typedef INetwork *(*PFNCREATENETWORK)(ISystem *pSystem, int ncpu);
}

#endif //_INETWORK_H_
