/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
	$Id$
	$DateTime$
  Description:	This is the interface which the launcher.exe will interact
								with to start the game framework. For an implementation of
								this interface refer to CryAction.
  
 -------------------------------------------------------------------------
  History:
  - 20:7:2004   10:34 : Created by Marco Koegler
	- 3:8:2004		11:29 : Taken-over by Márcio Martins

*************************************************************************/
#ifndef __IGAMEFRAMEWORK_H__
#define __IGAMEFRAMEWORK_H__

#if _MSC_VER > 1000
#	pragma once
#endif

#include "IGameStartup.h"
#include "Cry_Color.h"
#include "TimeValue.h"

struct pe_explosion;
struct IPhysicalEntity;

// Summary
//   Generic factory creation
// Description
//   This macro is used to register new game object extension classes. 
#define REGISTER_FACTORY(host, name, impl, isAI)												\
	(host)->RegisterFactory((name), (impl *)0, (isAI), (impl *)0)								\


#define DECLARE_GAMEOBJECT_FACTORY(impl)																	\
public:																																		\
	virtual void RegisterFactory(const char *name, impl *(*)(), bool isAI) = 0;				\
	template <class T> void RegisterFactory(const char *name, impl *, bool isAI, T *)	\
	{																																				\
		struct Factory																												\
		{																																			\
			static impl *Create()																								\
			{																																		\
				return new T();																										\
			}																																		\
		};																																		\
		RegisterFactory(name, Factory::Create, isAI);																\
	}

// game object extensions need more information than the generic interface can provide
struct IGameObjectExtension;

struct IGameObjectExtensionCreatorBase
{
	virtual IGameObjectExtension * Create() = 0;
	virtual void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount ) = 0;
};

#define DECLARE_GAMEOBJECTEXTENSION_FACTORY(name) \
	struct I##name##Creator : public IGameObjectExtensionCreatorBase \
	{ \
	}; \
	template <class T> \
	struct C##name##Creator : public I##name##Creator \
	{ \
		I##name * Create() \
		{ \
			return new T(); \
		} \
		void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount ) \
		{ \
			T::GetGameObjectExtensionRMIData( ppRMI, nCount ); \
		} \
	}; \
	virtual void RegisterFactory(const char *name, I##name##Creator*, bool isAI) = 0; \
	template <class T> void RegisterFactory(const char *name, I##name *, bool isAI, T *) \
	{ \
		static C##name##Creator<T> creator; \
		RegisterFactory(name, &creator, isAI); \
	}

struct ISystem;
struct IUIDraw;
struct ILanQueryListener;
struct IActor;
struct IActorSystem;
struct IItem;
struct IGameRules;
struct IWeapon;
struct IItemSystem;
struct ILevelSystem;
struct IActionMapManager;
struct IGameChannel;
struct IViewSystem;
struct IVehicle;
struct IVehicleSystem;
struct IGameRulesSystem;
struct IFlowSystem;
struct IGameTokenSystem;
struct IEffectSystem;
struct IGameObject;
struct IGameObjectExtension;
struct IGameObjectSystem;
struct IGameplayRecorder;
struct IAnimationStateNodeFactory;
struct ISaveGame;
struct ILoadGame;
struct IGameObject;
struct IMaterialEffects;
struct INetChannel;
struct IPlayerProfileManager;
struct IMusicLogic;
struct IAnimationGraphState;
struct INetNub;
struct ISaveGame;
struct ILoadGame;
struct IDebugHistoryManager;
struct IDebrisMgr;
struct ISubtitleManager;
struct IDialogSystem;

struct INeuralNet;
typedef _smart_ptr<INeuralNet> INeuralNetPtr;

enum EGameStartFlags
{
	eGSF_NoLevelLoading            = 0x0001,
	eGSF_Server                    = 0x0002,
	eGSF_Client                    = 0x0004,
	eGSF_NoDelayedStart            = 0x0008,
	eGSF_BlockingClientConnect     = 0x0010,
	eGSF_NoGameRules               = 0x0020,
	eGSF_LocalOnly                 = 0x0040,
	eGSF_NoQueries                 = 0x0080,
	eGSF_NoSpawnPlayer             = 0x0100,
	eGSF_BlockingMapLoad           = 0x0200,

	eGSF_DemoRecorder              = 0x0400,
	eGSF_DemoPlayback              = 0x0800,

	eGSF_ImmersiveMultiplayer      = 0x1000,
	eGSF_RequireController         = 0x2000,
	eGSF_RequireKeyboardMouse      = 0x4000,
};

enum ESaveGameReason
{
	eSGR_LevelStart,
	eSGR_FlowGraph,
	eSGR_Command,
	eSGR_QuickSave
};

static const EntityId LOCAL_PLAYER_ENTITY_ID = 0x7777u; // 30583 between static and dynamic EntityIDs

struct SGameContextParams
{
	const char * levelName;
	const char * gameRules;
	const char * demoRecorderFilename;
	const char * demoPlaybackFilename;

	SGameContextParams()
	{
		levelName = 0;
		gameRules = 0;
		demoRecorderFilename = 0;
		demoPlaybackFilename = 0;
	}
};

struct SGameStartParams
{
	// a combination of EGameStartFlags - needed if bServer==true
	unsigned flags;
	// UDP port to connect to
	uint16 port;
	// ip address/hostname of server to connect to - needed if bClient==true
	const char * hostname;
	// optional connection string for client
	const char * connectionString;
	// context parameters - needed if bServer==true
	const SGameContextParams * pContextParams;
	// maximum players to allow to connect
	int maxPlayers;
	
	SGameStartParams()
	{
		flags = 0;
		port = 0;
		hostname = 0;
		connectionString = 0;
		pContextParams = NULL;
		maxPlayers = 32;
	}
};

struct SModInfo
{
	const char *m_name, *m_version, *m_description, *m_url, *m_screenshot;
};

//provides an interface to game so game will be able to display numeric stats in user-friendly way
struct IGameStatsConfig
{
	virtual ~IGameStatsConfig(){}
	virtual int GetStatsVersion() = 0;
	virtual int GetCategoryMod(const char* cat) = 0;
	virtual const char* GetValueNameByCode(const char* cat, int id) = 0;
};

struct IPersistantDebug
{
	virtual void Begin( const char * name, bool clear ) = 0;
	virtual void AddSphere( const Vec3& pos, float radius, ColorF clr, float timeout ) = 0;
	virtual void AddDirection( const Vec3& pos, float radius, const Vec3& dir, ColorF clr, float timeout ) = 0;
	virtual void AddLine( const Vec3& pos1, const Vec3& pos2, ColorF clr, float timeout ) = 0;
	virtual void AddPlanarDisc( const Vec3& pos, float innerRadius, float outerRadius, ColorF clr, float timeout ) = 0;
	virtual void Add2DText ( const char * text, float size, ColorF clr, float timeout ) = 0;
	virtual void AddText ( float x, float y, float size, ColorF clr, float timeout, const char * fmt, ... ) = 0;
	virtual void Add2DLine( float x1, float y1, float x2, float y2, ColorF clr, float timeout ) = 0;
	virtual void AddQuat( const Vec3& pos, const Quat& q, float r, ColorF clr, float timeout ) = 0;
};

// when you add stuff here, also update in CCryAction::Init
enum EGameFrameworkEvent
{
	eGFE_PauseGame,
	eGFE_ResumeGame,
	eGFE_OnCollision,
	eGFE_OnPostStep,
	eGFE_OnStateChange,
	eGFE_ResetAnimationGraphs,
	eGFE_OnBreakable2d,
	eGFE_OnBecomeVisible,
	eGFE_PreFreeze,
	eGFE_PreShatter,
	eGFE_BecomeLocalPlayer,
	eGFE_DisablePhysics,
	eGFE_EnablePhysics,
};

//all events game should be aware of need to be added here
enum EActionEvent
{
  eAE_channelCreated,
  eAE_channelDestroyed,
  eAE_connectFailed,
  eAE_connected,
  eAE_disconnected,
  eAE_clientDisconnected,
	// map resetting
	eAE_resetBegin,
	eAE_resetEnd,
	eAE_resetProgress,
	eAE_preSaveGame,  // m_value -> ESaveGameReason
	eAE_postSaveGame, // m_value -> ESaveGameReason, m_description: 0 (failed), != 0 (successful)
	eAE_inGame,

	eAE_serverName, //started server 
	eAE_serverIp,		//obtained server ip
	eAE_earlyPreUpdate,  // called from CryAction's PreUpdate loop after System has been updated, but before subsystems
};

struct SActionEvent
{
  SActionEvent(EActionEvent e, int val=0,const char* des = 0):
    m_event(e),
    m_value(val),
    m_description(des)
  {}
  EActionEvent  m_event;
  int           m_value;
  const char*   m_description;
};

// We must take care of order in which listeners are called.
// Priority order is from low to high.
// As an example, menu must follow hud as it must be drawn on top of the rest.
enum EFRAMEWORKLISTENERPRIORITY
{
	// Default priority should not be used unless you don't care about order (it will be called first)
	FRAMEWORKLISTENERPRIORITY_DEFAULT,

	// Add your order somewhere here if you need to be called between one of them
	FRAMEWORKLISTENERPRIORITY_GAME,
	FRAMEWORKLISTENERPRIORITY_HUD,
	FRAMEWORKLISTENERPRIORITY_MENU
};

struct IGameFrameworkListener
{
	virtual void OnPostUpdate(float fDeltaTime) = 0;
	virtual void OnSaveGame(ISaveGame* pSaveGame) = 0;
	virtual void OnLoadGame(ILoadGame* pLoadGame) = 0;
	virtual void OnLevelEnd(const char* nextLevel) = 0;
  virtual void OnActionEvent(const SActionEvent& event) = 0;
};

// Summary
//   Interface which exposes the CryAction subsystems
struct IGameFramework
{
	DECLARE_GAMEOBJECT_FACTORY(IAnimationStateNodeFactory);
	DECLARE_GAMEOBJECT_FACTORY(ISaveGame);
	DECLARE_GAMEOBJECT_FACTORY(ILoadGame);
	DECLARE_GAMEOBJECTEXTENSION_FACTORY(Actor);
	DECLARE_GAMEOBJECTEXTENSION_FACTORY(Item);
	DECLARE_GAMEOBJECTEXTENSION_FACTORY(Vehicle);
	DECLARE_GAMEOBJECTEXTENSION_FACTORY(GameObjectExtension);
	
	// Summary
	//   Entry function to the game framework
	// Description
	//   Entry function used to create a new instance of the game framework from 
	//   outside its own DLL.
	// Returns
	//   a new instance of the game framework
	typedef IGameFramework *(*TEntryFunction)();

	// Summary:
	//		Initialize CryENGINE with every system needed for a general action game.
	//		Independently of the success of this method, Shutdown must be called.
	// Arguments:
	//		startupParams - Pointer to SSystemInitParams structure containg system initialization setup!
	// Return Value:
	//		0 if something went wrong with initialization, non-zero otherwise.
	virtual bool Init(SSystemInitParams &startupParams) = 0;
	// Summary:
	//    Complete initialization of game framework with things that can only be done
	//    after all entities have been registered.
	virtual bool CompleteInit() = 0;

	// Description:
	//		Updates CryENGINE before starting a game frame.
	// Arguments:
	//		haveFocus - Boolean describing if the game has the input focus or not.
	//		updateFlags - Flags specifying how to update.
	// Return Value:
	//		0 if something went wrong with initialization, non-zero otherwise.
	virtual bool PreUpdate(bool haveFocus, unsigned int updateFlags) = 0;

	// Description:
	//		Updates CryENGINE after a game frame.
	// Arguments:
	//		haveFocus - Boolean describing if the game has the input focus or not.
	//		updateFlags - Flags specifying how to update.
	virtual void PostUpdate(bool haveFocus, unsigned int updateFlags) = 0;

	// Description:
	//		Shuts down CryENGINE and any other subsystem created during initialization.
	virtual void Shutdown() = 0;

	// Description:
	//		Resets the current game
	virtual void Reset(bool clients) = 0;

	// Description:
	//		Pauses the game
	// Arguments:
	//		pause - Boolean describing if it's pausing or not.
	virtual void PauseGame(bool pause, bool force) = 0;

	// Description:
	//		Returns the pause status
	// Return Value:
	//		true - if the game is pause, false - otherwise
	virtual bool IsGamePaused() = 0;
	// Description:
	//    Are we completely into game mode?
	virtual bool IsGameStarted() = 0;

	// Description:
	//		Returns a pointer to the ISystem interface.
	// Return Value:
	//		Pointer to ISystem interface.
	virtual ISystem *GetISystem() = 0;

	// Description:
	//		Retrieve a pointer to the ILanQueryListener interface
	// Return Value:
	//		Pointer to ILanQueryListener interface.
	virtual ILanQueryListener *GetILanQueryListener() = 0;

	// Description:
	//		Returns a pointer to the IUIDraw interface.
	// Return Value:
	//		Pointer to IUIDraw interface.
	virtual IUIDraw *GetIUIDraw() = 0;

	// Description:
	//		Returns a pointer to the IGameObjectSystem interface.
	// Return Value:
	//		Pointer to IGameObjectSystem interface.
	virtual IGameObjectSystem *GetIGameObjectSystem() = 0;
	// Description:
	//		Returns a pointer to the ILevelSystem interface.
	// Return Value:
	//		Pointer to ILevelSystem interface.
	virtual ILevelSystem *GetILevelSystem() = 0;
	// Description:
	//		Returns a pointer to the IActorSystem interface.
	// Return Value:
	//		Pointer to IActorSystem interface.
	virtual IActorSystem *GetIActorSystem() = 0;
	// Description:
	//		Returns a pointer to the IItemSystem interface.
	// Return Value:
	//		Pointer to IItemSystem interface.
	virtual IItemSystem *GetIItemSystem() = 0;
	// Description:
	//		Returns a pointer to the IActionMapManager interface.
	// Return Value:
	//		Pointer to IActionMapManager interface.
	virtual IActionMapManager *GetIActionMapManager() = 0;
	// Description:
	//		Returns a pointer to the IViewSystem interface.
	// Return Value:
	//		Pointer to IViewSystem interface.
	virtual IViewSystem *GetIViewSystem() = 0;
	// Description:
	//		Returns a pointer to the IGameplayRecorder interface.
	// Return Value:
	//		Pointer to IGameplayRecorder interface.
	virtual IGameplayRecorder *GetIGameplayRecorder() = 0;
	// Description:
	//		Returns a pointer to the IVehicleSystem interface.
	// Return Value:
	//		Pointer to IVehicleSystem interface.
	virtual IVehicleSystem *GetIVehicleSystem() = 0;
	// Description:
	//		Returns a pointer to the IGameRulesSystem interface.
	// Return Value:
	//		Pointer to IGameRulesSystem interface.
	virtual IGameRulesSystem *GetIGameRulesSystem() = 0;
	// Description:
	//		Returns a pointer to the IFlowSystem interface.
	// Return Value:
	//		Pointer to IFlowSystem interface.
	virtual IFlowSystem *GetIFlowSystem() = 0;
	// Description:
	//		Returns a pointer to the IGameTokenSystem interface
	// Return Value:
	//		Pointer to IGameTokenSystem interface.
	virtual IGameTokenSystem *GetIGameTokenSystem() = 0;
	// Description:
	//		Returns a pointer to the IEffectSystem interface
	// Return Value:
	//		Pointer to IEffectSystem interface.
	virtual IEffectSystem *GetIEffectSystem() = 0;

	// Description:
	//		Returns a pointer to the IMaterialEffects interface.
	// Return Value:
	//		Pointer to IMaterialEffects interface.
	virtual IMaterialEffects *GetIMaterialEffects() = 0;

	// Description:
	//		Returns a pointer to the IDialogSystem interface
	// Return Value:
	//		Pointer to IDialogSystem interface.
	virtual IDialogSystem *GetIDialogSystem() = 0;

	// Description:
	//		Returns a pointer to the IPlayerProfileManager interface.
	// Return Value:
	//		Pointer to IPlayerProfileManager interface.
	virtual IPlayerProfileManager *GetIPlayerProfileManager() = 0;

	virtual IDebrisMgr *GetDebrisMgr () = 0;

	// Description:
	//		Returns a pointer to the ISubtitleManager interface.
	// Return Value:
	//		Pointer to ISubtitleManager interface.
	virtual ISubtitleManager *GetISubtitleManager() = 0;

	// Description:
	//    Initialises a game context
	// Arguments:
	//    pGameStartParams - parameters for configuring the game
	// Return Value:
	//    true if successful
	virtual bool StartGameContext( const SGameStartParams * pGameStartParams ) = 0;
	// Description:
	//    Changes a game context (levels and rules, etc); only allowed on the server
	// Arguments:
	//    pGameContextParams - parameters for configuring the context
	// Return Value:
	//    true if successful
	virtual bool ChangeGameContext( const SGameContextParams * pGameContextParams ) = 0;
	// Description:
	//    Finished a game context (no game running anymore)
	virtual void EndGameContext() = 0;
	// Description:
	//    Detect if a context is currently running
	// Return Value:
	//    true if a game context is running
	virtual bool StartedGameContext() const = 0;

	// Description:
	//    For the editor: spawn a player and wait for connection
	virtual bool BlockingSpawnPlayer() = 0;

	// Description:
	//    For the game : fix the broken game objects (to restart the map)
	virtual void ResetBrokenGameObjects() = 0;

	// Description:
	//		Inform the GameFramework of the current level loaded in the editor.
	virtual void SetEditorLevel(const char *levelName, const char *levelFolder) = 0;

	// Description:
	//		Retrieves the current level loaded by the editor.
	// Arguments:
	//		Pointers to receive the level infos.
	virtual void GetEditorLevel(char **levelName, char **levelFolder) = 0;

	// Description:
	//    Begin a query on the LAN for games
	virtual void BeginLanQuery() = 0;
	// Description:
	//    End the current game query
	virtual void EndCurrentQuery() = 0;

	// Description:
	//    Returns the Actor associated with the client (or NULL)
	virtual IActor * GetClientActor() const = 0;
	// Description:
	//    Returns the Actor Id associated with the client (or NULL)
	virtual EntityId GetClientActorId() const = 0;
	// Description:
	//    Returns the INetChannel associated with the client (or NULL)
	virtual INetChannel * GetClientChannel() const = 0;
	// Description:
	//    Returns the (synched) time of the server (so use this for timed events, such as MP round times)
	virtual CTimeValue GetServerTime() = 0;
	// Description:
	//    Retrieve the Game Server Channel Id associated with the specified INetChannel.
	//	Return Value:
	//		The Game Server ChannelId associated with the specified INetChannel.
	virtual uint16 GetGameChannelId(INetChannel *pNetChannel) = 0;
	// Description:
	//    Check if the game server channel has lost connection but still on hold and able to recover...
	//	Return Value:
	//		Returns true if the specified game server channel has lost connection but it's stil able to recover...
	virtual bool IsChannelOnHold(uint16 channelId) = 0;
	// Description:
	//    Retrieve a pointer to the INetChannel associated with the specified Game Server Channel Id.
	//	Return Value:
	//		Pointer to INetChannel associated with the specified Game Server Channel Id.
	virtual INetChannel *GetNetChannel(uint16 channelId) = 0;
	// Description:
	//    Retrieve an IGameObject from an entity id
	//	Return Value:
	//		Pointer to IGameObject of the entity if it exists (or NULL otherwise)
	virtual IGameObject * GetGameObject(EntityId id) = 0;

	// Description:
	//    Retrieve a network safe entity class id, that will be the same in client and server
	//	Return Value:
	//		true if an entity class with this name has been registered
	virtual bool GetNetworkSafeClassId(uint16 &id, const char *className) = 0;
	// Description:
	//    Retrieve a network safe entity class name, that will be the same in client and server
	//	Return Value:
	//		true if an entity class with this id has been registered
	virtual bool GetNetworkSafeClassName(char *className, size_t maxn, uint16 id) = 0;

	// Description:
	//    Retrieve an IGameObjectExtension by name from an entity
	//	Return Value:
	//		Pointer to IGameObjectExtension of the entity if it exists (or NULL otherwise)
	virtual IGameObjectExtension * QueryGameObjectExtension( EntityId id, const char * name) = 0;

	// Description:
	//    Save the current game to disk
	virtual bool SaveGame( const char * path, bool quick = false, bool bForceImmediate = true, ESaveGameReason reason = eSGR_QuickSave, bool ignoreDelay = false, const char* checkPoint = NULL) = 0;
	// Description:
	//    Load a game from disk (calls StartGameContext...)
	virtual bool LoadGame( const char * path, bool quick = false, bool ignoreDelay = false) = 0;

	// Description:
	//    Notification that game mode is being entered/exited
	//    iMode values: 0-leave game mode, 1-enter game mode, 3-leave AI/Physics mode, 4-enter AI/Physics mode
	virtual void OnEditorSetGameMode( int iMode ) = 0;

	virtual bool IsEditing() = 0;

	virtual bool IsInLevelLoad() = 0;

	virtual bool IsLoadingSaveGame() = 0;

	virtual bool IsInTimeDemo() = 0;

	virtual void AllowSave(bool bAllow = true) = 0;
	virtual void AllowLoad(bool bAllow = true) = 0;
	virtual bool CanSave() = 0;
	virtual bool CanLoad() = 0;

	// Description:
	//		Check if the current game can activate cheats (flymode, godmode, nextspawn)
	virtual bool CanCheat() = 0;

	// Returns:
	//    path relative to the levels folder e.g. "Multiplayer\PS\Shore"
	virtual const char * GetLevelName() = 0;

	// OUTDATED: Description:
	// OUTDATED:   absolute because downloaded content might anywhere
	// OUTDATED:   e.g. "c:/MasterCD/Game/Levels/Testy"
	// Description:
	//   relative to the MasterCD folder e.g. "game/levels/!Code/AutoTest0"
	// Returns
	//   0 if no level is loaded
	virtual const char * GetAbsLevelPath() = 0;

	virtual IPersistantDebug * GetIPersistantDebug() = 0;
	virtual IGameStatsConfig * GetIGameStatsConfig() = 0;

	// Music Logic
	virtual IAnimationGraphState * GetMusicGraphState() = 0;
	virtual IMusicLogic * GetMusicLogic() = 0;

	virtual void RegisterListener		(IGameFrameworkListener *pGameFrameworkListener, const char * name,EFRAMEWORKLISTENERPRIORITY eFrameworkListenerPriority) = 0;
	virtual void UnregisterListener	(IGameFrameworkListener *pGameFrameworkListener) = 0;

	virtual INetNub * GetServerNetNub() = 0;

	virtual void SetGameGUID( const char * gameGUID) = 0;
	virtual const char* GetGameGUID() = 0;
	virtual INetContext *GetNetContext() = 0;

	virtual void GetMemoryStatistics( ICrySizer * ) = 0;

	virtual void EnableVoiceRecording(const bool enable) = 0;

	virtual void MutePlayerById(EntityId mutePlayer) = 0;

  virtual IDebugHistoryManager* CreateDebugHistoryManager() = 0;

	virtual void DumpMemInfo(const char* format, ...) PRINTF_PARAMS(2, 3) = 0;

  // Description:
  //		Check whether the client actor is using voice communication.
	virtual bool IsVoiceRecordingEnabled() = 0;

	virtual bool IsImmersiveMPEnabled() = 0;

  // Description:
  //		Executes console command on next frame's beginning
  virtual void ExecuteCommandNextFrame(const char*) = 0;
  // Description:
  //		Opens a page in default browser
  virtual void ShowPageInBrowser(const char* URL) = 0;

  // Description:
  //		Opens a page in default browser
  virtual bool StartProcess(const char* cmd_line) = 0;

  // Description:
  //		Saves dedicated server console variables in server config file
  virtual bool SaveServerConfig(const char* path) = 0;

	// Description:
	//    to avoid stalls during gameplay and to get a list of all assets needed for the level (bEnforceAll=true)
	// Arguments:
	//   bEnforceAll - true to ensure all possible assets become registered (list should not be too conservative - to support level stripification)
	virtual void PrefetchLevelAssets( const bool bEnforceAll ) = 0;

	// Description:
	//	finds given mod or currently active mod (default) and returns mod meta data
	//	returns false if mod not found
	virtual bool GetModInfo(SModInfo *modInfo, const char *modPath = 0) = 0;
};

ILINE bool IsDemoPlayback()
{
	ISystem* pSystem = GetISystem();
	IGame* pGame = pSystem->GetIGame();
	IGameFramework* pFramework = pGame->GetIGameFramework();
	INetContext* pNetContext = pFramework->GetNetContext();
	return pNetContext ? pNetContext->IsDemoPlayback() : false;
}

#endif //__IGAMEFRAMEWORK_H__

