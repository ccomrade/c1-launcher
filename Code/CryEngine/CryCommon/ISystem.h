// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _CRY_SYSTEM_H_
#define _CRY_SYSTEM_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef CRYSYSTEM_EXPORTS
	#define CRYSYSTEM_API DLL_EXPORT
#else
	#define CRYSYSTEM_API DLL_IMPORT
#endif

#include <platform.h> // Needed for LARGE_INTEGER (for consoles).

////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////////////////////
#include "IXml.h"
#include "IValidator.h"
#include "ILog.h"
#include "CryVersion.h"
#include "smartptr.h"

struct ISystem;
struct ILog;
struct IEntitySystem;
struct IEntity;
struct ICryPak;
struct IKeyboard;
struct IMouse;
struct IConsole;
struct IInput;
struct IRenderer;
struct IConsole;
struct IProcess;
struct I3DEngine;
struct ITimer;
struct IGame;
struct IScriptSystem;
struct IAISystem;
struct IFlash;
struct INetwork;
struct ICryFont;
struct IMovieSystem;
struct IPhysicalWorld;
struct IMemoryManager;
struct ISoundSystem;
struct IMusicSystem;
struct IFrameProfileSystem;
struct FrameProfiler;
struct IStreamEngine;
struct ICharacterManager;
struct SFileVersion;
struct IDataProbe;
struct INameTable;
struct IBudgetingSystem;
struct IFlowSystem;
struct IDialogSystem;
struct IMaterialEffects;
struct IHardwareMouse;
struct IAnimationGraphSystem;
struct IFlashPlayer;
struct IFlashLoadMovieHandler;
class  ICrySizer;
struct ITestSystem;
class  IXMLBinarySerializer;
struct IReadWriteXMLSink;
struct IGlobalTaskScheduler;
struct IThreadTaskManager;
struct ITextModeConsole;
struct IAVI_Reader;	
class Crc32Gen; 
class CPNoise3;

#ifndef EXCLUDE_GPU_PARTICLE_PHYSICS
struct IGPUPhysicsManager;
#endif

class ICmdLine;
class CFrameProfilerSection;

#ifndef DATA_FOLDER
#define DATA_FOLDER "game"
#endif

#define PROC_MENU		1
#define PROC_3DENGINE	2

//ID for script userdata typing (maybe they should be moved into the game.dll)
#define USER_DATA_SOUND			1
#define USER_DATA_TEXTURE		2
#define USER_DATA_OBJECT		3
#define USER_DATA_LIGHT			4
#define USER_DATA_BONEHANDLER	5
#define USER_DATA_POINTER		6

enum ESystemUpdateFlags
{
	ESYSUPDATE_IGNORE_AI			= 0x0001,
	ESYSUPDATE_IGNORE_PHYSICS = 0x0002,
	// Special update mode for editor.
	ESYSUPDATE_EDITOR					=	0x0004,
	ESYSUPDATE_MULTIPLAYER		= 0x0008
};

//////////////////////////////////////////////////////////////////////////
// Configuration specification, depends on user selected machine spec.
//////////////////////////////////////////////////////////////////////////
enum ESystemConfigSpec
{
	CONFIG_CUSTOM        = 0,
	CONFIG_LOW_SPEC      = 1,
	CONFIG_MEDIUM_SPEC   = 2,
	CONFIG_HIGH_SPEC     = 3,
	CONFIG_VERYHIGH_SPEC = 4,

	CONFIG_RESERVED1_SPEC = 5,
	CONFIG_RESERVED2_SPEC = 6,

	// Specialized detail config setting.
	CONFIG_DETAIL_SPEC  = 7,
};

enum ESubsystem
{
	ESubsys_3DEngine = 0,
	ESubsys_AI = 1,
	ESubsys_Physics = 2,
	ESubsys_Renderer = 3,
	ESubsys_Script = 4
};

//////////////////////////////////////////////////////////////////////////
// System wide events.
enum ESystemEvent
{
	// Seeds all random number generators to the same seed number, WParam will hold seed value.
	ESYSTEM_EVENT_RANDOM_SEED = 1,
	ESYSTEM_EVENT_RANDOM_ENABLE,
	ESYSTEM_EVENT_RANDOM_DISABLE,

	// Change to main window focus.
	// wparam is not 0 is focused, 0 if not focused
	ESYSTEM_EVENT_CHANGE_FOCUS = 10,
	// Move of the main window.
	// wparam=x, lparam=y
	ESYSTEM_EVENT_MOVE = 11,
	// Resize of the main window.
	// wparam=width, lparam=height
	ESYSTEM_EVENT_RESIZE = 12,

	// Sent before starting loading a new level.
	// Used for a more efficient resource management.
	ESYSTEM_EVENT_LEVEL_LOAD_START,
	
	// Sent after loading a level finished.
	// Used for a more efficient resource management.
	ESYSTEM_EVENT_LEVEL_LOAD_END,

	// Level reload. For cleanup code.
	ESYSTEM_EVENT_LEVEL_RELOAD,

	// When keyboard layout changed
	ESYSTEM_EVENT_LANGUAGE_CHANGE,

	// Toggled fullscreen
	// wparam is 1 means we switched to fullscreen, 0 if for windowed
	ESYSTEM_EVENT_TOGGLE_FULLSCREEN,
	ESYSTEM_EVENT_USER = 0x1000,
};

//////////////////////////////////////////////////////////////////////////
// Localized strings manager interface.
//////////////////////////////////////////////////////////////////////////

// Summary: Interface to the Localization Manager
struct ILocalizationManager
{
	// Localization Info structure
	struct SLocalizedInfo
	{
		const char* sKey;
		const char* sEnglish;
		const wchar_t* sLocalizedString;
		const char* sEnglishSubtitle;
		const wchar_t* sLocalizedSubtitle;
		const wchar_t* sWho;
		bool bUseSubtitle;
	};

	// Localization Sound Info structure, containing sound related parameters
	struct SLocalizedSoundInfo
	{
		const char* sEnglish;
		const wchar_t* sLocalizedString;
		const char* sEnglishSubtitle;
		const wchar_t* sLocalizedSubtitle;
		const char* sSoundEvent;
		const wchar_t* sWho;
		float fVolume;
		float fDucking;	
		float fRadioRatio;
		float fRadioBackground;
		float fRadioSquelch;
		bool  bUseSubtitle;
	};


	virtual bool SetLanguage( const char* sLanguage ) = 0;
	virtual const char* GetLanguage() = 0;

	virtual bool LoadExcelXmlSpreadsheet( const char* sFileName, bool bReload=false ) = 0;
	// Summary:
	//   Free localization data
	virtual	void FreeData() = 0;

	// Summary:
	//   Translate a string into the currently selected language
	// Description:
	//   Processes the input string and translates all labels contained into the currently selected language
	// Parameters:
	//   sString             - String to be translated
	//   outLocalizedString  - Translated version of the string.
	//   bEnglish            - if true, translates the string into the always present English language
	// Returns:
	//   true if localization was successful, false otherwise
	virtual bool LocalizeString( const char* sString, wstring& outLocalizedString, bool bEnglish=false ) = 0;

	// Summary:
	//   Same as LocalizeString( const char* sString, wstring& outLocalizedString, bool bEnglish=false )
	//   but at the moment this is faster
	virtual bool LocalizeString( const string& sString, wstring& outLocalizedString, bool bEnglish=false ) = 0;

	// Summary:
	//   Return the localized version corresponding to a label.
	// Description:
	//   A label has to start with '@' sign.
	// Parameters:
	//   sLabel              - Label to be translated, must start with '@' sign
	//   outLocalizedString  - Localized version of the label.
	//   bEnglish            - if true, returns the always present English version of the label
	// Returns:
	//   true if localization was successful, false otherwise
	virtual bool LocalizeLabel( const char* sLabel, wstring& outLocalizedString, bool bEnglish=false ) = 0;

	// Summary:
	//   Get localization info structure corresponding to a key (key=label without the '@' sign)
	// Parameters:
	//   sKey    - Key to be looked up. Key = Label without '@' sign
	//   outInfo - Reference to localization info structure to be filled in
	//  Returns:
	//    true if info for key was found, false otherwise
	virtual bool GetLocalizedInfo( const char* sKey, SLocalizedInfo& outInfo ) = 0;

	// Summary:
	//   Return number of localization entries
	virtual int  GetLocalizedStringCount() = 0;
	// Summary:
	//   Get the localization info structure at index nIndex
	// Parameters:
	//   nIndex  - index
	//   outInfo - reference to localization info structure to be filled in
	// Returns:
	//   true if successful, false otherwise (out of bounds)
	virtual bool GetLocalizedInfoByIndex( int nIndex, SLocalizedInfo& outInfo ) = 0;

	// Summary:		
	//   Get the sound localization info structure corresponding to a key
	// Parameters:
	//   sKey         - Key to be looked up. Key = Label without '@' sign
	//   outSoundInfo - reference to sound info structure to be filled in
	// Returns:
	//   true if successful, false otherwise (key not found)
	virtual bool GetLocalizedSoundInfo( const char* sKey, SLocalizedSoundInfo& outSoundInfo ) = 0;


	// Summary:
	//   Get the english localization info structure corresponding to a key
	// Parameters:
	//   sKey         - Key to be looked up. Key = Label without '@' sign
	//   sLocalizedString - corresponding english language string
	// Returns:
	//   true if successful, false otherwise (key not found)
	virtual bool GetEnglishString( const char *sKey, string &sLocalizedString ) = 0;
	
	// Summary:
	//   Get Subtitle for Key or Label 
	// Parameters:
	//   sKeyOrLabel    - Key or Label to be used for subtitle lookup. Key = Label without '@' sign.
	//   outSubtitle    - Subtitle (untouched if Key/Label not found)
	//   bForceSubtitle - if true, get subtitle (sLocalized or sEnglish) even if not specified in Data file
	// Returns:
	//   true if subtitle found (and outSubtitle filled in), false otherwise
	virtual bool GetSubtitle( const char* sKeyOrLabel, wstring& outSubtitle, bool bForceSubtitle = false) = 0;

	// these methods format outString depending on sString with ordered arguments
	// FormatStringMessage(outString, "This is %2 and this is %1", "second", "first");
	// outString -> "This is first and this is second";
	virtual void FormatStringMessage( string& outString, const string& sString, const char** sParams, int nParams ) = 0;
	virtual void FormatStringMessage( string& outString, const string& sString, const char* param1, const char* param2=0, const char* param3=0, const char* param4=0 ) = 0;
	virtual void FormatStringMessage( wstring& outString, const wstring& sString, const wchar_t** sParams, int nParams ) = 0;
	virtual void FormatStringMessage( wstring& outString, const wstring& sString, const wchar_t* param1, const wchar_t* param2=0, const wchar_t* param3=0, const wchar_t* param4=0 ) = 0;

	virtual wchar_t ToUpperCase(wchar_t c) = 0;
	virtual wchar_t ToLowerCase(wchar_t c) = 0;
	virtual void LocalizeTime(time_t t, bool bMakeLocalTime, bool bShowSeconds, wstring& outTimeString) = 0;
	virtual void LocalizeDate(time_t t, bool bMakeLocalTime, bool bShort, bool bIncludeWeekday, wstring& outDateString) = 0;
	virtual void LocalizeDuration(int seconds, wstring& outDurationString) = 0;
};

// User defined callback, which can be passed to ISystem.
struct ISystemUserCallback
{
	/** Signals to User that engine error occured.
			@return true to Halt execution or false to ignore this error.
	*/
	virtual bool OnError( const char *szErrorString ) = 0;
	/** If working in Editor environment notify user that engine want to Save current document.
			This happens if critical error have occured and engine gives a user way to save data and not lose it
			due to crash.
	*/
	virtual void OnSaveDocument() = 0;
	
	/** Notify user that system wants to switch out of current process.
			(For ex. Called when pressing ESC in game mode to go to Menu).
	*/
	virtual void OnProcessSwitch() = 0;

	// Notify user, usually editor about initialization progress in system.
	virtual void OnInitProgress( const char *sProgressMsg ) = 0;

	// Initialization callback.  This is called early in CSystem::Init(), before
	// any of the other callback methods is called.
	virtual void OnInit(ISystem *) { }

	// Shutdown callback.
	virtual void OnShutdown() { }

	// Notify user of an update iteration.  Called in the update loop.
	virtual void OnUpdate() { }

	// to collect the memory information in the user program/application
	virtual void GetMemoryUsage( ICrySizer* pSizer ) = 0;
};

// interface used for getting notified when a system event occurs
struct ISystemEventListener
{
	virtual void OnSystemEvent( ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam ) = 0;
};


// structure used for getting notified when a system event occurs
struct ISystemEventDispatcher
{
	virtual bool RegisterListener(ISystemEventListener *pListener) = 0;
	virtual bool RemoveListener(ISystemEventListener *pListener) = 0;

	virtual void OnSystemEvent( ESystemEvent event,UINT_PTR wparam,UINT_PTR lparam ) = 0;

	//virtual void OnLocaleChange() = 0;
};



/**
 * @brief Reverse engineered init parameters.
 * Crytek removed content of this structure from the game SDK. Total size is 2384 B in 32-bit code and 2464 B in 64-bit code.
 */
struct SSystemInitParams
{
	void *hInstance;                     // executable handle
	void *hWnd;                          // optional window handle
	ILog *pLog;                          // optional custom log
	ILogCallback *pLogCallback;          // optional log callback
	ISystemUserCallback *pUserCallback;  // optional engine callback
	const char *sLogFileName;            // usually "Game.log" or "Server.log"
	IValidator *pValidator;              // optional custom validator
	char szSystemCmdLine[2048];          // process command line obtained with GetCommandLineA
	char szUserPath[256];                // optional custom user folder in %USERPROFILE%\Documents
	bool bEditor;                        // editor mode
	bool bMinimal;                       // minimal mode - skip initialization of some subsystems
	bool bTesting;                       // test mode
	bool bDedicatedServer;               // launch dedicated server
	ISystem *pSystem;                    // initialized by IGameStartup::Init
	void *pCheckFunc;                    // not used
	void *pProtectedFunctions[10];       // probably not used
};



// Typedef for frame profile callback function.
typedef void (*FrameProfilerSectionCallback)( class CFrameProfilerSection *pSection );

// can be used for LoadConfiguration()
struct ILoadConfigurationEntrySink
{
	virtual void OnLoadConfigurationEntry( const char *szKey, const char *szValue, const char *szGroup )=0;
	virtual void OnLoadConfigurationEntry_End() {}
};

struct SPlatformInfo
{
	unsigned int numCoresAvailableToProcess;

#if defined(WIN32) || defined(WIN64)
	enum EWinVersion
	{
		WinUndetected,
		Win2000,
		WinXP,
		WinSrv2003,
		WinVista
	};

	EWinVersion winVer;
	bool win64Bit;
	bool vistaKB940105Required;
#endif
};

//////////////////////////////////////////////////////////////////////////
// Global environment.
// Contain pointers to all global often needed interfaces.
// This is a faster way to get interface pointer then calling ISystem interface to retrieve one.
// Some pointers can be NULL, use with care.
//////////////////////////////////////////////////////////////////////////
struct SSystemGlobalEnvironment
{
	ISystem*                   pSystem;

	IGame*                     pGame;
	INetwork*                  pNetwork;
	IRenderer*                 pRenderer;
	IInput*                    pInput;
	ITimer*                    pTimer;
	IConsole*                  pConsole;
	IScriptSystem*             pScriptSystem;
	I3DEngine*                 p3DEngine;
	ISoundSystem*              pSoundSystem;
	IMusicSystem*              pMusicSystem;
	IPhysicalWorld*            pPhysicalWorld;
	IMovieSystem*              pMovieSystem;
	IAISystem*                 pAISystem;
	IEntitySystem*             pEntitySystem;
	ICryFont*                  pCryFont;
	ICryPak*                   pCryPak;
	ILog*                      pLog;
	ICharacterManager*         pCharacterManager;
	IFrameProfileSystem*       pFrameProfileSystem;
	INameTable*                pNameTable;
	IFlowSystem*               pFlowSystem;
	IAnimationGraphSystem*     pAnimationGraphSystem;
	IDialogSystem*             pDialogSystem;
	IHardwareMouse*            pHardwareMouse;
	IMaterialEffects*          pMaterialEffects;

	//////////////////////////////////////////////////////////////////////////
	// Used to tell if this is a server/client/multiplayer instance
	bool										   bClient;
	bool                       bServer;
	bool											 bMultiplayer;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Used by frame profiler.
	bool                       bProfilerEnabled;
	FrameProfilerSectionCallback callbackStartSection;
	FrameProfilerSectionCallback callbackEndSection;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Indicate Editor status.
	//////////////////////////////////////////////////////////////////////////
	bool                       bEditor;          // Engine is running under editor.
	bool                       bEditorGameMode;  // Engine is in editor game mode.
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Used by CRY_ASSERT
	bool											 bIgnoreAllAsserts;
	//////////////////////////////////////////////////////////////////////////

	SPlatformInfo pi;

};


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// Main Engine Interface
// initialize and dispatch all engine's subsystems 
struct ISystem
{ 
	struct ILoadingProgressListener
	{
		virtual void OnLoadingProgress(int steps) = 0;
	};

	// Release ISystem.
	virtual void Release() = 0;

	// Returns pointer to the global environment structure.
	virtual SSystemGlobalEnvironment* GetGlobalEnvironment() = 0;

	// Returns the root folder specified by the command line option "-root <path>"
	virtual const char* GetRootFolder() const = 0;

	// Update all subsystems (including the ScriptSink() )
	// Arguments:
	//   flags - one or more flags from ESystemUpdateFlags structure
	//   nPauseMode - 0=normal(no pause), 1=menu/pause, 2=cutscene
	virtual bool Update( int updateFlags=0, int nPauseMode=0 ) = 0;

	// Begin rendering frame.
	virtual void	RenderBegin() = 0;
	// Render subsystems.
	virtual void	Render() = 0;
	// End rendering frame and swap back buffer.
	virtual void	RenderEnd() = 0;

	// Renders the statistics; this is called from RenderEnd, but if the 
	// Host application (Editor) doesn't employ the Render cycle in ISystem,
	// it may call this method to render the essencial statistics
	virtual void RenderStatistics () = 0;

	// Common (cross-module) memory allocation function.
	virtual void* AllocMem( void* oldptr, size_t newsize ) = 0;

	// Returns the current used memory
	virtual uint32 GetUsedMemory() = 0;

	// Retrieve the name of the user currently logged in to the computer
	virtual const char *GetUserName() = 0;

  // Gets current supported CPU features flags. (CPUF_SSE, CPUF_SSE2, CPUF_3DNOW, CPUF_MMX)
  virtual int GetCPUFlags() = 0;

  // Get seconds per processor tick
  virtual double GetSecondsPerCycle() = 0;

  // dumps the memory usage statistics to the log. default MB (can be KB)
	virtual void DumpMemoryUsageStatistics(bool bUseKB=false) = 0;

	// Quit the application
	virtual void	Quit() = 0;
	// Tells the system if it is relaunching or not
	virtual void	Relaunch(bool bRelaunch) = 0;
	virtual bool IsRelaunch() const = 0;
	// Tells the system in which way we are using the serialization system.
	virtual void  SerializingFile(int mode) = 0;	
	virtual int IsSerializingFile() const = 0;
	// return true if the application is in the shutdown phase
	virtual bool	IsQuitting() = 0;

	// Display error message.
	// Logs it to console and file and error message box.
	// Then terminates execution.
	virtual void Error( const char *sFormat,... ) PRINTF_PARAMS(2, 3) = 0;
	
	// Report warning to current Validator object.
	// Not terminates execution.
	virtual void Warning( EValidatorModule module,EValidatorSeverity severity,int flags,const char *file,const char *format,... ) PRINTF_PARAMS(6, 7) = 0;
	// Compare specified verbosity level to the one currently set.
	virtual bool CheckLogVerbosity( int verbosity ) = 0;

	// returns true if this is dedicated server application
	virtual bool IsDedicated() {return false;}

	// returns true if this is editor application
	virtual bool IsEditor() {return false;}

	// returns true if this is editor application and not in game mode
	virtual bool IsEditorMode() {return false;}

	// return the related subsystem interface
#ifdef SP_DEMO
	virtual IGame *GetIGame() = 0;
	virtual INetwork *GetINetwork() = 0;
	virtual IRenderer *GetIRenderer() = 0;
	virtual IInput *GetIInput() = 0;
	virtual ITimer *GetITimer() = 0;
#endif
	virtual IConsole *GetIConsole() = 0;
	virtual IScriptSystem *GetIScriptSystem() = 0;
	virtual I3DEngine *GetI3DEngine() = 0;
	virtual ISoundSystem *GetISoundSystem() = 0;
	virtual IMusicSystem *GetIMusicSystem() = 0;
  virtual IPhysicalWorld *GetIPhysicalWorld() = 0;
	virtual IMovieSystem *GetIMovieSystem() = 0;
	virtual IAISystem *GetAISystem() = 0;
	virtual IMemoryManager *GetIMemoryManager() = 0;
	virtual IEntitySystem *GetIEntitySystem() = 0;
	virtual ICryFont *GetICryFont()	= 0;
	virtual ICryPak *GetIPak()	= 0;
	virtual ILog *GetILog() = 0;
	virtual ICmdLine *GetICmdLine() = 0;
	virtual IStreamEngine *GetStreamEngine() = 0;
	virtual ICharacterManager *GetIAnimationSystem() = 0;
	virtual IValidator *GetIValidator() = 0;
	virtual IFrameProfileSystem *GetIProfileSystem() = 0;	
	virtual INameTable *GetINameTable() = 0;
	virtual IBudgetingSystem *GetIBudgetingSystem() = 0;
	virtual IFlowSystem *GetIFlowSystem() = 0;
	virtual IAnimationGraphSystem *GetIAnimationGraphSystem() = 0;
	virtual IDialogSystem *GetIDialogSystem() = 0;
	virtual IHardwareMouse *GetIHardwareMouse() = 0;
	virtual ISystemEventDispatcher *GetISystemEventDispatcher() = 0;
	virtual IGlobalTaskScheduler *GetIGlobalTaskScheduler() = 0;
	virtual IThreadTaskManager *GetIThreadTaskManager() = 0;
#ifndef SP_DEMO
	virtual IGame *GetIGame() = 0;
	virtual INetwork *GetINetwork() = 0;
	virtual IRenderer *GetIRenderer() = 0;
	virtual IInput *GetIInput() = 0;
	virtual ITimer *GetITimer() = 0;
#endif

#ifndef EXCLUDE_GPU_PARTICLE_PHYSICS
	virtual IGPUPhysicsManager *GetIGPUPhysicsManager() = 0;
#endif

	virtual void SetLoadingProgressListener(ILoadingProgressListener *pListener) = 0;
	virtual ILoadingProgressListener *GetLoadingProgressListener() const = 0;

	//
	// Game is created after System init, so has to be set explicitly
	virtual void						SetIGame(IGame* pGame) = 0;
	virtual void            SetIFlowSystem(IFlowSystem* pFlowSystem) = 0;
	virtual void SetIAnimationGraphSystem(IAnimationGraphSystem* pAnimationGraphSystem) = 0;
	virtual void SetIDialogSystem(IDialogSystem* pDialogSystem) = 0;
	virtual void SetIMaterialEffects(IMaterialEffects* pMaterialEffects) = 0;
	// Change current user sub path, the path is always relative to the user documents folder. (ex: "My Games\Crysis")
	virtual void ChangeUserPath( const char *sUserPath ) = 0;

	//virtual	const char			*GetGamePath()=0;

	virtual void DebugStats(bool checkpoint, bool leaks) = 0;
	virtual void DumpWinHeaps() = 0;
	virtual int DumpMMStats(bool log) = 0;

	//////////////////////////////////////////////////////////////////////////
	// @param bValue set to true when running on a cheat protected server or a client that is connected to it (not used in singlplayer)
	virtual void SetForceNonDevMode( const bool bValue )=0;
	// @return is true when running on a cheat protected server or a client that is connected to it (not used in singlplayer)
	virtual bool GetForceNonDevMode() const=0;
	virtual bool WasInDevMode() const=0;
	virtual bool IsDevMode() const=0;
	virtual bool IsMODValid(const char *szMODName) const=0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// IXmlNode interface.
	//////////////////////////////////////////////////////////////////////////
	
	// Creates new xml node.
	virtual XmlNodeRef CreateXmlNode( const char *sNodeName="" ) = 0;
	// Load xml file, return 0 if load failed.
	virtual XmlNodeRef LoadXmlFile( const char *sFilename ) = 0;
	// Load xml from string, return 0 if load failed.
	virtual XmlNodeRef LoadXmlFromString( const char *sXmlString ) = 0;
	// Retrieve access to XML utilities interface.
	virtual IXmlUtils* GetXmlUtils() = 0;

	virtual void SetViewCamera(class CCamera &Camera) = 0;
	virtual CCamera& GetViewCamera() = 0;

	// When ignore update sets to true, system will ignore and updates and render calls.
	virtual void IgnoreUpdates( bool bIgnore ) = 0;

	/* Set the active process
		@param process a pointer to a class that implement the IProcess interface
	*/
	virtual void SetIProcess(IProcess *process) = 0;
	/* Get the active process
		@return a pointer to the current active process
	*/
	virtual IProcess* GetIProcess() = 0;

	// Returns true if system running in Test mode.
	virtual bool IsTestMode() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Frame profiler functions
	virtual void SetFrameProfiler(bool on, bool display, char *prefix) = 0;

	//////////////////////////////////////////////////////////////////////////
	// VTune Profiling interface.
	// Resume vtune data collection.
	virtual void VTuneResume() = 0;
	// Pauses vtune data collection.
	virtual void VTunePause() = 0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// File version.
	//////////////////////////////////////////////////////////////////////////
	virtual const SFileVersion& GetFileVersion() = 0;
	virtual const SFileVersion& GetProductVersion() = 0;
	
	// Compressed file read & write
	virtual bool WriteCompressedFile(const char *filename, void *data, unsigned int bitlen) = 0;
	virtual unsigned int ReadCompressedFile(const char *filename, void *data, unsigned int maxbitlen) = 0;
	virtual unsigned int GetCompressedFileSize(const char *filename)=0;
	virtual bool CompressDataBlock( const void * input, size_t inputSize, void * output, size_t& outputSize, int level = 3 ) = 0;
	virtual bool DecompressDataBlock( const void * input, size_t inputSize, void * output, size_t& outputSize ) = 0;

	// Retrieve IDataProbe interface.
	virtual IDataProbe* GetIDataProbe() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Configuration.
	//////////////////////////////////////////////////////////////////////////
	// Saves system configuration.
	virtual void SaveConfiguration() = 0;
	// Loads system configuration
	// Arguments:
	//   pCallback - 0 means nomal LoadConfigVar behaviour is used
	virtual void LoadConfiguration( const char *sFilename, ILoadConfigurationEntrySink *pSink=0 )=0;

	// Retrieve current configuration specification for client or server.
	// Arguments:
	//   bClient - if True returns local client config spec, if false returns server config spec.
	virtual ESystemConfigSpec GetConfigSpec( bool bClient=true ) = 0;
	
	virtual ESystemConfigSpec GetMaxConfigSpec() const = 0;

	// Changes current configuration specification for client or server.
	// Arguments:
	//   bClient - if True changes client config spec (sys_spec variable changed), 
	//               if false changes only server config spec (as known on the client).
	virtual void SetConfigSpec( ESystemConfigSpec spec,bool bClient ) = 0;
	//////////////////////////////////////////////////////////////////////////

	// detect and set optimal spec
	virtual void AutoDetectSpec() = 0;

	// Thread management for subsystems
	// returns non-0 if the state was indeed changed, 0 if already in that state
	virtual int SetThreadState(ESubsystem subsys, bool bActive) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Creates and returns a usable object implementing ICrySizer interface.
	virtual ICrySizer* CreateSizer() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Query if system is now paused.
	// Pause flag is set when calling system update with pause mode.
	virtual bool IsPaused() const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Retrieve localized strings manager interface.
	virtual ILocalizationManager* GetLocalizationManager() = 0;

	//////////////////////////////////////////////////////////////////////////
	// Creates an instance of the IFlashPlayer interface.
	virtual IFlashPlayer* CreateFlashPlayerInstance() const = 0;
	virtual void SetFlashLoadMovieHandler(IFlashLoadMovieHandler* pHandler) const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Creates an instance of the AVI Reader class.
	virtual IAVI_Reader *CreateAVIReader() = 0;
	// Release the AVI reader
	virtual void ReleaseAVIReader(IAVI_Reader *pAVIReader) = 0;

	//////////////////////////////////////////////////////////////////////////
	// For debugging use only!, query current C++ call stack.
	//////////////////////////////////////////////////////////////////////////
	// Pass nCount to indicate maximum number of functions to get.
	// Fill array of function pointers, nCount return number of functions.
	virtual void debug_GetCallStack( const char **pFunctions,int &nCount ) = 0;
	// Logs current callstack.
	virtual void debug_LogCallStack( int nMaxFuncs=32,int nFlags=0 ) = 0;

	// can be called through console e.g. #System.ApplicationTest("testcase0")
	// Arguments:
	//   szParam - 0 generates error
	virtual void ApplicationTest( const char *szParam ) = 0;
	// Returns:
	//  0 if not activated, activate through #System.ApplicationTest
	virtual ITestSystem *GetITestSystem() = 0;

	// execute command line arguments e.g. +g_gametype ASSAULT +map "testy"
	// should be after init game
	virtual void ExecuteCommandLine() = 0;

	// useful to investigate memory fragmentation
	// every time you call this from the console: #System.DumpMemoryCoverage()
	// it adds a line to "MemoryCoverage.bmp" (generated the first time, there is a max line count)
	virtual void DumpMemoryCoverage() = 0;

	virtual ITextModeConsole * GetITextModeConsole() = 0;

	//////////////////////////////////////////////////////////////////////////
	// retrieves the crc32 singleton instance
	virtual Crc32Gen* GetCrc32Gen() = 0;

	//////////////////////////////////////////////////////////////////////////
	// retrieves the perlin noise singleton instance
	virtual CPNoise3* GetNoiseGen() = 0;

	// Retrieve system update counter.
	virtual uint64 GetUpdateCounter() = 0;
};




//////////////////////////////////////////////////////////////////////////
// Global environment variable.
//////////////////////////////////////////////////////////////////////////
extern SSystemGlobalEnvironment* gEnv;

//////////////////////////////////////////////////////////////////////////
// Get the system interface
inline ISystem *GetISystem()
{
	return gEnv->pSystem;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Interface that allow access to the CryEngine memory manager.
//////////////////////////////////////////////////////////////////////////
struct IMemoryManager
{
	struct SProcessMemInfo
	{
		uint64 PageFaultCount;
		uint64 PeakWorkingSetSize;
		uint64 WorkingSetSize;
		uint64 QuotaPeakPagedPoolUsage;
		uint64 QuotaPagedPoolUsage;
		uint64 QuotaPeakNonPagedPoolUsage;
		uint64 QuotaNonPagedPoolUsage;
		uint64 PagefileUsage;
		uint64 PeakPagefileUsage;
	};
	virtual bool GetProcessMemInfo( SProcessMemInfo &minfo ) = 0;
};


// This function must be called once by each module at the begining, to setup global pointers.
extern void ModuleInitISystem( ISystem *pSystem );
extern bool g_bProfilerEnabled;
extern bool g_bTraceAllocations;



//////////////////////////////////////////////////////////////////////////
// Display error message.
// Logs it to console and file and error message box.
// Then terminates execution.
void CryError(const char *, ...) PRINTF_PARAMS(1, 2);
inline void CryError( const char *format,... )
{ 
	if (!gEnv || !gEnv->pSystem)
		return;

	va_list	ArgList;
	char szBuffer[MAX_WARNING_LENGTH];
	va_start(ArgList, format);
	vsprintf(szBuffer, format, ArgList);
	va_end(ArgList);

	gEnv->pSystem->Error( "%s",szBuffer );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Display warning message.
// Logs it to console and file and display a warning message box.
// Not terminates execution.
void CryWarning(EValidatorModule, EValidatorSeverity, const char *, ...) PRINTF_PARAMS(3, 4);
inline void CryWarning( EValidatorModule module,EValidatorSeverity severity,const char *format,... )
{
	if (!gEnv || !gEnv->pSystem)
		return;

	if (!GetISystem() || !format)
		return;
	va_list	ArgList;
	char		szBuffer[MAX_WARNING_LENGTH];
	va_start(ArgList, format);
	vsprintf(szBuffer, format, ArgList);
	va_end(ArgList);
	GetISystem()->Warning( module,severity,0,0,szBuffer );
}
//////////////////////////////////////////////////////////////////////////
// Simple log of data with low verbosity.
void CryLog(const char *, ...) PRINTF_PARAMS(1, 2);
inline void CryLog( const char *format,... )
{
//	return;
	if (gEnv && gEnv->pSystem)
	{
		va_list args;
		va_start(args,format);
		gEnv->pLog->LogV( ILog::eMessage,format,args );
		va_end(args);
	}
}

//////////////////////////////////////////////////////////////////////////
// Very rarely used log comment.
void CryComment(const char *, ...) PRINTF_PARAMS(1, 2);
inline void CryComment( const char *format,... )
{
	if (gEnv && gEnv->pSystem)
	{
		va_list args;
		va_start(args,format);
		gEnv->pLog->LogV( ILog::eComment,format,args );
		va_end(args);
	}
}


//////////////////////////////////////////////////////////////////////////
// Logs important data that must be printed regardless verbosity.
void CryLogAlways(const char *, ...) PRINTF_PARAMS(1, 2);
inline void CryLogAlways( const char *format,... )
{
	if (gEnv && gEnv->pSystem)
	{
		va_list args;
		va_start(args,format);
		gEnv->pLog->LogV( ILog::eAlways,format,args );
		va_end(args);
	}
}

// Allocation functor, for use in templates.
struct FSystemAlloc
{
	static void* Alloc( void* oldptr, int oldsize,int newsize )
	{ 
		return gEnv ? gEnv->pSystem->AllocMem( oldptr, newsize ) : 0; 
	}
};


//////////////////////////////////////////////////////////////////////////
// Additional headers.
//////////////////////////////////////////////////////////////////////////
#include <FrameProfiler.h>

#endif //_CRY_SYSTEM_H_
