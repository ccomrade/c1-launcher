//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//  CrySound Source Code
//
//  File: ISound.h
//  Description: Sound interface.
// 
//  History:
//  - August 28, 2001: Created by Marco Corbetta
//
//////////////////////////////////////////////////////////////////////

#ifndef CRYSOUND_ISOUND_H
#define CRYSOUND_ISOUND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Cry_Math.h"
#include "TimeValue.h"
#include "SerializeFwd.h"

//forward declarations
//////////////////////////////////////////////////////////////////////
class		CCamera;
struct	IMusicSystem;
struct  IAudioDevice;
class		ICrySizer;
struct	IVisArea;
struct	ptParam;
struct ISoundMoodManager;
struct IReverbManager;
struct ISound;
struct ISoundBuffer;
struct IMicrophone;
struct INetworkSoundListener;
struct IMicrophoneStream;
struct IEntitySoundProxy;

struct IVisArea;

// need this explicit here to prevent circular includes to IEntity
typedef unsigned int EntityId;	//! unique identifier for each entity instance


// Unique ID of a sound
typedef uint32	tSoundID;
#define INVALID_SOUNDID 0

#define DECIBELTODISTANCE(_DB)	float(cry_powf(10,(_DB-11)/30.0f));

#define SOUND_MAX_SKIPPED_ENTITIES	5	// maximun number of skipped entites for obstruction calculation (defines cache size)
#define SOUND_MAX_OBSTRUCTION_TESTS 10

#define SOUND_INDOOR_TAILNAME "indoor"
#define SOUND_OUTDOOR_TAILNAME "outdoor"

//////////////////////////////////////////////////////////////////////
#define MAX_SFX													1024

//////////////////////////////////////////////////////////////////////
// Valid Event Sound Flags (only use these on a event)
//////////////////////////////////////////////////////////////////////
//#define FLAG_SOUND_RELATIVE							0x00000040	// sound position moves relative to player
//#define FLAG_SOUND_OUTDOOR							0x00000800	// play the sound only if the listener is in outdoor
//#define FLAG_SOUND_INDOOR	 							0x00001000	// play the sound only if the listener is in indoor
//#define FLAG_SOUND_CULLING		 					0x00004000	// the sound uses sound occlusion (based on VisAreas)
//#define FLAG_SOUND_LOAD_SYNCHRONOUSLY		0x00008000	// the loading of this sound will be synchronous (asynchronously by default).
//#define FLAG_SOUND_OBSTRUCTION			    0x00040000	// the sound uses sound obstruction (based on ray-world-intersects)
//#define FLAG_SOUND_SELFMOVING		        0x00080000	// sounds will be automatically moved controlled by direction vector in m/sec
//#define FLAG_SOUND_START_PAUSED					0x00100000  // start the sound paused, so an additional call to unpause is needed
//#define FLAG_SOUND_VOICE      					0x00200000  // Sound used as a voice (sub-titles and lip sync can be applied)

//////////////////////////////////////////////////////////////////////
// Valid Wave Sound Flags
//////////////////////////////////////////////////////////////////////
//#define FLAG_SOUND_LOOP									0x00000001	 
//#define FLAG_SOUND_2D										0x00000002	
//#define FLAG_SOUND_3D										0x00000004	 
//#define FLAG_SOUND_STEREO								0x00000008	 
//#define FLAG_SOUND_16BITS								0x00000010	 
//#define FLAG_SOUND_STREAM								0x00000020	// streamed wav 
//#define FLAG_SOUND_RELATIVE							0x00000040	// sound position moves relative to player
//#define FLAG_SOUND_DOPPLER							0x00000100	// use doppler effect for this sound	
//#define FLAG_SOUND_NO_SW_ATTENUATION		0x00000200	// doesn't use SW attenuation for this sound
//#define FLAG_SOUND_MUSIC								0x00000400	// pure music sound, to use to set pure music volume
//#define FLAG_SOUND_OUTDOOR							0x00000800	// play the sound only if the listener is in outdoor
//#define FLAG_SOUND_INDOOR	 							0x00001000	// play the sound only if the listener is in indoor
//#define FLAG_SOUND_UNSCALABLE						0x00002000	// for all sounds with this flag the volume can be scaled separately respect to the master volume
//#define FLAG_SOUND_CULLING		 					0x00004000	// the sound uses sound occlusion (based on VisAreas)
//#define FLAG_SOUND_LOAD_SYNCHRONOUSLY		0x00008000	// the loading of this sound will be synchronous (asynchronously by default).
//#define FLAG_SOUND_MANAGED              0x00010000  // Managed sounds life time is controlled by the sound system, when sound stops it will be deleted.
//#define FLAG_SOUND_FADE_OUT_UNDERWATER	0x00020000	//1<<16
//#define FLAG_SOUND_OBSTRUCTION			    0x00040000	// the sound uses sound obstruction (based on ray-world-intersects)
//#define FLAG_SOUND_SELFMOVING		        0x00080000	// sounds will be automatically moved controlled by direction vector in m/sec
//#define FLAG_SOUND_START_PAUSED					0x00100000  // start the sound paused, so an additional call to unpause is needed
//#define FLAG_SOUND_VOICE      					0x00200000  // Sound used as a voice (sub-titles and lip sync can be applied)
//#define FLAG_SOUND_EDITOR								0x01000000	// mark sound as being only used within the Editor (eg. Facial Editor)

//////////////////////////////////////////////////////////////////////
// Internal Sound Flags
//////////////////////////////////////////////////////////////////////
#define FLAG_SOUND_LOOP									0x00000001	 
#define FLAG_SOUND_2D										0x00000002	
#define FLAG_SOUND_3D										0x00000004	 
#define FLAG_SOUND_STEREO								0x00000008	 
#define FLAG_SOUND_16BITS								0x00000010	 
#define FLAG_SOUND_STREAM								0x00000020	// streamed wav 
#define FLAG_SOUND_RELATIVE							0x00000040	// sound position moves relative to player
#define FLAG_SOUND_RADIUS								0x00000080	// sound has a radius, custom attenuation calculation
#define FLAG_SOUND_DOPPLER							0x00000100	// use doppler effect for this sound	
#define FLAG_SOUND_NO_SW_ATTENUATION		0x00000200	// doesn't use SW attenuation for this sound
#define FLAG_SOUND_MUSIC								0x00000400	// pure music sound, to use to set pure music volume
#define FLAG_SOUND_OUTDOOR							0x00000800	// play the sound only if the listener is in outdoor
#define FLAG_SOUND_INDOOR	 							0x00001000	// play the sound only if the listener is in indoor
#define FLAG_SOUND_UNSCALABLE						0x00002000	// for all sounds with this flag the volume can be scaled separately respect to the master volume
#define FLAG_SOUND_CULLING		 					0x00004000	// the sound uses sound occlusion (based on VisAreas)
#define FLAG_SOUND_LOAD_SYNCHRONOUSLY		0x00008000	// the loading of this sound will be synchronous (asynchronously by default).
#define FLAG_SOUND_MANAGED              0x00010000  // Managed sounds life time is controlled by the sound system, when sound stops it will be deleted.
#define FLAG_SOUND_FADE_OUT_UNDERWATER	0x00020000	//1<<16
#define FLAG_SOUND_OBSTRUCTION			    0x00040000	// the sound uses sound obstruction (based on ray-world-intersects)
#define FLAG_SOUND_SELFMOVING		        0x00080000	// sounds will be automatically moved controlled by direction vector in m/sec
#define FLAG_SOUND_START_PAUSED					0x00100000  // start the sound paused, so an additional call to unpause is needed
#define FLAG_SOUND_VOICE      					0x00200000  // Sound used as a voice (sub-titles and lip sync can be applied)
#define FLAG_SOUND_EVENT								0x00400000  // this sound is a sound event
#define FLAG_SOUND_EDITOR								0x00800000	// mark sound as being only used within the Editor (eg. Facial Editor)
#define FLAG_SOUND_SPREAD								0x01000000  // this sound has a spread parameter
#define FLAG_SOUND_DAYLIGHT							0x02000000  // this sound has a daylight parameter
#define FLAG_SOUND_SQUELCH							0x04000000  // this sound has a radio squelch parameter
#define FLAG_SOUND_DOPPLER_PARAM				0x08000000  // this sound has a doppler parameter


//#define FLAG_SOUND_DEFAULT_3D (FLAG_SOUND_3D | FLAG_SOUND_RADIUS | FLAG_SOUND_CULLING | FLAG_SOUND_OBSTRUCTION )
#define FLAG_SOUND_DEFAULT_3D ( FLAG_SOUND_CULLING | FLAG_SOUND_OBSTRUCTION )

#define FLAG_SOUND_ACTIVELIST	 	(FLAG_SOUND_RADIUS | FLAG_SOUND_CULLING | FLAG_SOUND_INDOOR | FLAG_SOUND_OUTDOOR)
#define SOUNDBUFFER_FLAG_MASK	(FLAG_SOUND_LOOP | FLAG_SOUND_2D | FLAG_SOUND_3D | FLAG_SOUND_STEREO | FLAG_SOUND_16BITS | FLAG_SOUND_STREAM | FLAG_SOUND_LOAD_SYNCHRONOUSLY)	// flags affecting the sound-buffer, not its instance
//////////////////////////////////////////////////////////////////////
// Precache Flags
#define FLAG_SOUND_PRECACHE_LOAD_SOUND				0x000001	 
#define FLAG_SOUND_PRECACHE_LOAD_GROUP				0x000002	 
#define FLAG_SOUND_PRECACHE_STAY_IN_MEMORY		0x000004	
#define FLAG_SOUND_PRECACHE_UNLOAD_AFTER_PLAY	0x000008	
#define FLAG_SOUND_PRECACHE_UNLOAD_NOW				0x000010	

#define FLAG_SOUND_PRECACHE_EVENT_DEFAULT (FLAG_SOUND_PRECACHE_LOAD_SOUND)
#define FLAG_SOUND_PRECACHE_DIALOG_DEFAULT (FLAG_SOUND_PRECACHE_LOAD_SOUND | FLAG_SOUND_PRECACHE_STAY_IN_MEMORY | FLAG_SOUND_PRECACHE_UNLOAD_AFTER_PLAY)
#define FLAG_SOUND_PRECACHE_READABILITY_DEFAULT (FLAG_SOUND_PRECACHE_LOAD_SOUND | FLAG_SOUND_PRECACHE_STAY_IN_MEMORY)

//////////////////////////////////////////////////////////////////////
// group stuff
#define MAX_SOUNDSCALE_GROUPS		8
#define SOUNDSCALE_MASTER				0
#define SOUNDSCALE_SCALEABLE		1
#define SOUNDSCALE_DEAFNESS			2
#define SOUNDSCALE_UNDERWATER		3
#define SOUNDSCALE_MISSIONHINT	4

// Update rates in MS = 1000/Hz
#define UPDATE_SOUND_ASSET_MANAGER_IN_MS	250
#define UPDATE_AUDIO_DEVICE_IN_MS					10
#define UPDATE_SOUNDS_IN_MS								15
#define UPDATE_SOUND_AUDIODEVICE_IN_MS		15

//////////////////////////////////////////////////////////////////////
// Sound Semantics
//////////////////////////////////////////////////////////////////////
enum ESoundSemantic
{
	eSoundSemantic_None											= 0x00000000,
	eSoundSemantic_OnlyVoice								= 0x00000001,
	eSoundSemantic_NoVoice									= 0x00000002,
	eSoundSemantic_Unused1									= 0x00000004,
	eSoundSemantic_Unused2									= 0x00000008,

	eSoundSemantic_Unused3									= 0x00000010,
	eSoundSemantic_Unused4									= 0x00000020,
	eSoundSemantic_Ambience									= 0x00000040, //a
	eSoundSemantic_Ambience_OneShot					= 0x00000080, //b

	eSoundSemantic_Physics_Collision				= 0x00000100, //c
	eSoundSemantic_Dialog										= 0x00000200, //d
	eSoundSemantic_MP_Chat									= 0x00000400, //e
	eSoundSemantic_Physics_Footstep					= 0x00000800, //f

	eSoundSemantic_Physics_General					= 0x00001000, //g
	eSoundSemantic_HUD											= 0x00002000, //h
	eSoundSemantic_Unused5									= 0x00004000, //i
	eSoundSemantic_FlowGraph								= 0x00008000, //j

	eSoundSemantic_Player_Foley_Voice				= 0x00010000, //k
	eSoundSemantic_Living_Entity						= 0x00020000, //l
	eSoundSemantic_Mechanic_Entity					= 0x00040000, //m
	eSoundSemantic_NanoSuit									= 0x00080000, //n

	eSoundSemantic_SoundSpot								= 0x00100000, //o
	eSoundSemantic_Particle									= 0x00200000, //p
	eSoundSemantic_AI_Pain_Death						= 0x00400000, //q
	eSoundSemantic_AI_Readability						= 0x00800000, //r

	eSoundSemantic_AI_Readability_Response	= 0x01000000, //s
	eSoundSemantic_TrackView								= 0x02000000, //t
	eSoundSemantic_Projectile								= 0x04000000, //u
	eSoundSemantic_Vehicle									= 0x08000000, //v

	eSoundSemantic_Weapon										= 0x10000000, //w
	eSoundSemantic_Explosion								= 0x20000000, //x
	eSoundSemantic_Player_Foley							= 0x40000000, //y
	eSoundSemantic_Animation								= 0x80000000, //z
};

enum ESoundActiveState
{
	eSoundState_None,
	eSoundState_Active,
	eSoundState_Inactive,
	eSoundState_Stopped,
};

enum EFadeState
{
	eFadeState_None,
	eFadeState_FadingIn,
	eFadeState_FadingOut,
	eFadeState_JustFinished,
};

enum ESoundStopMode
{
	ESoundStopMode_AtOnce,
	ESoundStopMode_EventFade,
	ESoundStopMode_OnSyncPoint, // fallback: will be stopped after 0.1 sec or set spSYNCTIMEOUTINSEC before
};

enum ESoundUpdateMode
{
	eSoundUpdateMode_None					= 0x00000000,
	eSoundUpdateMode_Listeners    = 0x00000001,
	eSoundUpdateMode_Sounds			  = 0x00000002,
	eSoundUpdateMode_Rest				  = 0x00000004,
	eSoundUpdateMode_All					= 0x0000000F
};

enum EOutputHandle
{
	eOUTPUT_WINMM,			// Pointer to type HWAVEOUT is returned.
	eOUTPUT_DSOUND,			// Pointer to type DIRECTSOUND is returned.
	eOUTPUT_WASAPI,			// Pointer to type WASAPI is returned.
	eOUTPUT_OPENAL,			// Pointer to type OPENAL is returned.
	eOUTPUT_ASIO,				// NULL / 0 is returned.
	eOUTPUT_OSS,				// File handle is returned, (cast to int).
	eOUTPUT_ESD,				// Handle of type int is returned, as returned by so_esd_open_sound (cast to int). 
	eOUTPUT_ALSA,				// Pointer to type snd_pcm_t is returned.
	eOUTPUT_MAC,				// Handle of type SndChannelPtr is returned.
	eOUTPUT_Xbox,				// Pointer to type DIRECTSOUND is returned.
	eOUTPUT_PS2,				// NULL / 0 is returned.
	eOUTPUT_GC,					// NULL / 0 is returned.
	eOUTPUT_NOSOUND,		// NULL / 0 is returned.
	eOUTPUT_WAVWRITER,	// NULL / 0 is returned.
	eOUTPUT_MAX,
};

enum EPrecacheResult
{
	ePrecacheResult_None,
	ePrecacheResult_OK,
	ePrecacheResult_Error,
	ePrecacheResult_Delayed,
	ePrecacheResult_Max,
};


typedef struct 
{
	float fCurrentVolumeRatio;
	float fTargetVolumeRatio;

	float fCurrentObstructionRatio;
	float fTargetObstructionRatio;
	
	float fCurrentReverbFeed;
	float fTargetReverbFeed;
	
	float fTimeleft;
	bool  bHasChanged;
} SSoundGroupProperties;

typedef int32 ListenerID;

#define LISTENERID_STANDARD 0
#define LISTENERID_ALLACTIVE -1
#define LISTENERID_INVALID -2

#define MAX_VIS_AREAS 256 // maximum of visarea cache


typedef struct IListener 
{
	virtual ListenerID	GetID() const = 0;
	virtual EntityId		GetEntityID() const = 0;
	
	virtual bool GetActive() const = 0;
	virtual void SetActive(bool bActive) = 0;
	
	virtual void SetRecordLevel(float fRecord) = 0;
	virtual float GetRecordLevel() = 0;

	virtual Vec3 GetPosition() const = 0;
	virtual void SetPosition(const Vec3 Position) = 0;

	virtual Vec3 GetForward() const = 0;
	virtual Vec3 GetTop() const = 0;
	virtual Vec3 GetVelocity() const = 0;
	virtual void SetVelocity(Vec3 vVel) = 0;

	virtual void SetMatrix(const Matrix34 newTransformation) = 0;
	virtual Matrix34 GetMatrix() const = 0;

	virtual float GetUnderwater() const = 0;
	virtual void	SetUnderwater(const float fUnder) = 0;

	virtual IVisArea* GetVisArea() const = 0;
	virtual void SetVisArea(IVisArea* pVArea) = 0;

} IListener;



//These values are used with CS_FX_Enable to enable DirectX 8 FX for a channel.
//////////////////////////////////////////////////////////////////////////////////////////////
enum SOUND_FX_MODES
{
    S_FX_CHORUS,
    S_FX_COMPRESSOR,
    S_FX_DISTORTION,
    S_FX_ECHO,
    S_FX_FLANGER,
    S_FX_GARGLE,
    S_FX_I3DL2REVERB,
    S_FX_PARAMEQ,
    S_FX_WAVES_REVERB
};



//! Sound events sent to callback that can registered to every sound.
enum ESoundCallbackEvent
{
	SOUND_EVENT_ON_LOADED,						//!< Fired when sound is loaded.
	SOUND_EVENT_ON_LOAD_FAILED,				//!< Fired if sound loading is failed.
	SOUND_EVENT_ON_START,							//!< Fired when sound is started.
	SOUND_EVENT_ON_PLAYBACK_STARTED,	//!< Fired when sound's playback started.
	SOUND_EVENT_ON_PLAYBACK_UNPAUSED,	//!< Fired when sound's playback unpaused.
	SOUND_EVENT_ON_STOP,							//!< Fired when sound stops.
	SOUND_EVENT_ON_PLAYBACK_STOPPED,	//!< Fired when sound's playback stops.
	SOUND_EVENT_ON_PLAYBACK_PAUSED,		//!< Fired when sound's playback paused.
	SOUND_EVENT_ON_SYNCHPOINT					//!< Fired when sound reaches a syncpoint.
};

//! Soundsystem events sent to callback that registered to soundsystem.
//! Important the first elements need to be the same as in ESoundCallbackEvent
enum ESoundSystemCallbackEvent
{
	SOUNDSYSTEM_EVENT_ON_LOADED,						//!< Fired when sound is loaded.
	SOUNDSYSTEM_EVENT_ON_LOAD_FAILED,				//!< Fired if sound loading is failed.
	SOUNDSYSTEM_EVENT_ON_START,							//!< Fired when sound is started.
	SOUNDSYSTEM_EVENT_ON_PLAYBACK_STARTED,	//!< Fired when sound's playback started.
	SOUNDSYSTEM_EVENT_ON_PLAYBACK_UNPAUSED,	//!< Fired when sound's playback unpaused.
	SOUNDSYSTEM_EVENT_ON_STOP,							//!< Fired when sound stops.
	SOUNDSYSTEM_EVENT_ON_PLAYBACK_STOPPED,	//!< Fired when sound's playback stops.
	SOUNDSYSTEM_EVENT_ON_PLAYBACK_PAUSED,		//!< Fired when sound's playback paused.
	SOUNDSYSTEM_EVENT_ON_SYNCHPOINT					//!< Fired when sound reaches a syncpoint.
};


// struct for sound obstruction
// a single obstruction test
typedef struct  
{
	Vec3 vOrigin;
	Vec3 vDirection;
	int  nHits;
	int  nPierceability;
	int  nTestForTest;
	tSoundID SoundID;
	float fDistance;
	bool bDirect;
	bool bResult;
} SObstructionTest;

// obstruction test
typedef struct SObstruction
{
	public: 

		// Constructor
		SObstruction() 
		{
			fDirectOcclusion			= 0.0f;
			fDirectOcclusionAccu	= 0.0f;
			fReverbOcclusion			= 0.0f;
			fReverbOcclusionAccu	= 0.0f;
			nRaysShot							= 0;
			bProcessed						= false;
			bAssigned							= false;
			bDelayPlayback				= false;
			bFirstTime						= true;
			bDontAveragePrevious	= true;


			for (int i=0; i< SOUND_MAX_OBSTRUCTION_TESTS; ++i)
			{
				ObstructionTests[i].SoundID = INVALID_SOUNDID;
				ObstructionTests[i].vOrigin = Vec3(0);
				ObstructionTests[i].vDirection = Vec3(0);
				ObstructionTests[i].nHits = 0;
				ObstructionTests[i].nPierceability = 0;
				ObstructionTests[i].nTestForTest = 0;
				ObstructionTests[i].fDistance = 0.0f;
				ObstructionTests[i].bDirect = false;
				ObstructionTests[i].bResult = false;
			}

			nObstructionSkipEnts = 0;
			for (int i=0; i< SOUND_MAX_SKIPPED_ENTITIES; ++i)
				pOnstructionSkipEntIDs[i] = 0;
		}

		float GetDirect() const
		{
			return fDirectOcclusion;
		}

		void SetDirect(const float fObstruct)
		{
			fDirectOcclusion = min(max(fObstruct,0.0f), 1.0f);
		}

		void AddDirect(const float fObstruct)
		{
			fDirectOcclusionAccu += fObstruct;
		}

		float GetAccumulatedDirect() const
		{
			return fDirectOcclusionAccu;
		}

		void ResetDirect() 
		{
			fDirectOcclusion			= 0.0f;
			fDirectOcclusionAccu	= 0.0f;
		}

		float GetReverb() const
		{
			return fReverbOcclusion;
		}

		void SetReverb(const float fObstruct)
		{
			fReverbOcclusion = min(max(fObstruct,0.0f), 1.0f);
		}

		void AddReverb(const float fObstruct)
		{
			fReverbOcclusionAccu += fObstruct;
		}

		float GetAccumulatedReverb() const
		{
			return fReverbOcclusionAccu;
		}

		void ResetReverb() 
		{
			fReverbOcclusion			= 0.0f;
			fReverbOcclusionAccu	= 0.0f;
		}

	//members
	int nRaysShot;

	SObstructionTest ObstructionTests[SOUND_MAX_OBSTRUCTION_TESTS];

	CTimeValue	LastUpdateTime;

	EntityId pOnstructionSkipEntIDs[SOUND_MAX_SKIPPED_ENTITIES];
	int nObstructionSkipEnts;
	bool bAddObstruction;
	bool bProcessed;
	bool bAssigned;
	bool bDelayPlayback;
	bool bFirstTime;
	bool bDontAveragePrevious;

private:
	float fDirectOcclusion;				// level of obstruction from 0 = none, to 1 = full obstructed
	float fDirectOcclusionAccu;		// accumulates level of obstruction from 0 = none, to 1 = full obstructed
	float fReverbOcclusion;				// level of obstruction from 0 = none, to 1 = full obstructed
	float fReverbOcclusionAccu;		// accumulates level of obstruction from 0 = none, to 1 = full obstructed


} SObstruction;


//////////////////////////////////////////////////////////////////////////
//! Listener interface for the sound.
//////////////////////////////////////////////////////////////////////////
struct ISoundEventListener
{
	//! Callback event.
	virtual void OnSoundEvent( ESoundCallbackEvent event,ISound *pSound ) = 0;
};

//////////////////////////////////////////////////////////////////////////
//! Listener interface for the soundsystem.
//////////////////////////////////////////////////////////////////////////
struct ISoundSystemEventListener
{
	//! Callback event.
	virtual void OnSoundSystemEvent( ESoundSystemCallbackEvent event,ISound *pSound ) = 0;
};

struct SSoundEventListenerInfo
{
	ISoundEventListener	*pListener;
//#ifdef _DEBUG
	char sWho[64];
//#endif
};

// Marco's NOTE: this is a redefine of the EAX preset OFF, since it seems
// that audigy cards are having problems when the default EAX off preset
#define MY_CS_PRESET_OFF  {0,	1.0f,	0.00f, -10000, -10000, -10000,   0.1f,  0.1f, 0.1f,  -10000, 0.0f, { 0.0f,0.0f,0.0f }, -10000, 0.0f, { 0.0f,0.0f,0.0f }, 0.0750f, 0.00f, 0.04f, 0.000f, 0.0f, 1000.0f, 20.0f, 0.0f,   0.0f,   0.0f, 0 }


//////////////////////////////////////////////////////////////////////////////////////////////
// Wavebanbk interface
struct IWavebank
{
	struct SWavebankInfo
	{
	public: 
		SWavebankInfo()
		{
			nTimesAccessed			= 0;
			nFileSize						= 0;
			nMemCurrentlyInByte = 0;
			nMemPeakInByte			= 0;
		}
		
		uint32 nTimesAccessed;
		uint32 nFileSize;
		uint32 nMemCurrentlyInByte;
		uint32 nMemPeakInByte;
	};

	virtual const char *GetName() = 0;
	virtual const char* GetPath() = 0;
	virtual void SetPath(const char* sWavebankPath) = 0;
	virtual SWavebankInfo* GetInfo() = 0;
	virtual void AddInfo(SWavebankInfo &WavebankInfo) = 0;
};

struct ISoundProfileInfo
{
	struct SSoundProfileInfo
	{
	public: 
		SSoundProfileInfo()
		{
			nTimesPlayed					= 0;
			nTimesPlayedOnChannel	= 0;
			nMemorySize						= 0;
			nPeakSpawn						= 0;
			nChannelsUsed					= 0;
			bWavebanksLogged			= false;
		}

		uint32 nTimesPlayed;
		uint32 nTimesPlayedOnChannel;
		uint32 nMemorySize;
		uint32 nPeakSpawn;
		uint32 nChannelsUsed;
		bool   bWavebanksLogged;
	};
	virtual const char *GetName() = 0;
	virtual SSoundProfileInfo* GetInfo() = 0;
	virtual void AddInfo(SSoundProfileInfo &SoundInfo) = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////
// Sound system interface
struct ISoundSystem
{
	virtual bool Init() = 0; // Need to call this after the AudioDevice was set and initialized
	virtual void Release() = 0;
	virtual void Update(ESoundUpdateMode UpdateMode) = 0;

	/*! Create a music-system. You should only create one music-system at a time.
	*/
	virtual IMusicSystem* CreateMusicSystem() = 0;

	/*! Returns IAudioDevice Interface.
	*/
	virtual IAudioDevice*		GetIAudioDevice()	const = 0;
	
	/*! Returns ISoundMoodManager Interface.
	*/
	virtual ISoundMoodManager* GetIMoodManager() const = 0;
	
	/*! Returns IReverbManager Interface.
	*/
	virtual IReverbManager* GetIReverbManager() const = 0;

	// Register listener to the sound.
	virtual void AddEventListener( ISoundSystemEventListener *pListener, bool bOnlyVoiceSounds ) = 0;
	virtual void RemoveEventListener( ISoundSystemEventListener *pListener ) = 0;
	
	/*! // returns the output handle if possible, else NULL
	// HandleType will specify if this a pointer to DirectX LPDIRECTSOUND or a WINMM handle
	// HandleType will be eOUTPUT_MAX if invalid
	*/
	virtual void	GetOutputHandle( void **pHandle, EOutputHandle *HandleType)	const = 0;

	/*! Load a sound from disk
	@param szfile filename
	@param nFlags sound flags combination
	@return	sound interface
	*/	
	//virtual struct ISound* LoadSound(const char *szFile, int nFlags) = 0;

	/*! SetMasterVolume
	@param fVol volume (0.0f - 1.0f)
	*/		
	virtual void SetMasterVolume(float fVol) = 0;

	/*! Set the volume scale for all sounds with FLAG_SOUND_SCALABLE
	@param fScale volume scale (default 1.0)
	*/			
	virtual void SetMasterVolumeScale(float fScale, bool bForceRecalc=false) = 0;

	/*! GetSFXVolume
	@param fVol volume (0.0f - 1.0f)
	*/		
	virtual float GetSFXVolume() = 0;

	virtual void SetSoundActiveState(ISound *pSound, ESoundActiveState State) = 0;

	/*! SetMasterPitch
	@param fPitch pitch (in octaves)
	*/		
	virtual void SetMasterPitch(float fPitch) = 0;

	/*! Get a sound interface from the sound system
	@param nSoundId sound id
	*/		
	virtual struct ISound* GetSound(tSoundID nSoundID) const = 0;

	/*! Play a sound from the sound system
	@param nSoundId sound id
	*/		
	//virtual void PlaySound(tSoundID nSoundID) = 0;

	/*! Precaches the definition and the buffer of a group, subgroup or a sound 
	@param sGroupAndSoundName name of group (with /path/) and name of sound split by ":"
	@param nSoundFlags some Flags that can be set, needs to be defined yet
	@param nPrecacheFlags some Flags that can be set to change precaching behaviour
	@return EPrecacheResult result code
	*/	
	virtual EPrecacheResult Precache( const char *sGroupAndSoundName, uint32 nSoundFlags, uint32 nPrecacheFlags ) = 0;

	/*! Creates a sound object by combining the definition of the sound and the buffer of sound data
	@param SSGHandle the Handle to the sound definition
	@param int Sound Flags
	@return ISound* For now this will return a Ptr to a Sound, this will change to the unique Sound ID later
	*/	
	virtual ISound* CreateSound				( const char *sGroupAndSoundName, uint32 nFlags ) = 0;
	virtual ISound* CreateLineSound		( const char *sGroupAndSoundName, uint32 nFlags, const Vec3 &vStart, const Vec3 &VEnd ) = 0;
	virtual ISound* CreateSphereSound	( const char *sGroupAndSoundName, uint32 nFlags, const float fRadius ) = 0;

	/*! Set the listener transformation matrix, velocity and recordlevel for a special listener
	@param nListenerID ListenerID of the listener
	@param matOrientation Transformation matrix
	@param vVel velocity	vector
	@param fRecordLevel level of input sensitivity
	@return	bool value if listener was successfully found and updated
	*/			
	virtual bool SetListener(const ListenerID nListenerID, const Matrix34 &matOrientation, const Vec3 &vVel, bool bActive, float fRecordLevel)=0;

	virtual void SetListenerEntity( ListenerID nListenerID, EntityId nEntityID ) = 0;

	/*! Creates a new Listener 
	@return ListenerID of new created Listener or INVALID_LISTENERID on failure
	*/
	virtual ListenerID CreateListener()=0;

	/*! Removes a Listener 
	@param nListenerID ListenerID of the listener which should be removed
	@return bool if Listener was removed successfully 
	*/
	virtual bool RemoveListener(ListenerID nListenerID)=0;


	/*! Retrieves the ListenerID of the closest Listener to a 3D Point
	@param vPosition Position in 3D
	@return nListenerID ListenerID of the active listener which is closest
	*/
	virtual ListenerID	GetClosestActiveListener(Vec3 vPosition) const = 0;

	/*! Retrieves a pointer to a Listener struct of the Listener defined by ID
	@param nListenerID ListenerID of a valid Listener
	@return IListener* Pointer to Listener struct or NULL if not found
	*/
	virtual	IListener*	GetListener(ListenerID nListenerID) = 0;

	/*! Retrieves a pointer to a Listener struct of the Listener who has a higher ID than provided
	@param nListenerID ListenerID or LISTENERID_STANDARD to start
	@return IListener* Pointer to Listener struct or NULL if no Listener has a higher ID
	*/
	virtual IListener*	GetNextListener(ListenerID nListenerID) = 0;

	/*! Retrieves the number of active Listeners
	@return uint32 Number of active Listeners
	*/
	virtual uint32			GetNumActiveListeners() const = 0;


	/*! to be called when something changes in the environment which could affect
	sound occlusion, for example a door closes etc.
	@param	bRecomputeListener recomputes the listener vis area
	@param	bForceRecompute forces to recompute the vis area connections even if
					the listener didn't move (useful for moving objects that can occlude)
	*/
	virtual void	RecomputeSoundOcclusion(bool bRecomputeListener, bool bForceRecompute, bool bReset=false)=0;
	
	//! Check for EAX support.
	virtual bool IsEAX(void) = 0;

	//! Set EAX listener environment; one of the predefined presets
	//! listened above or a custom environmental reverb set
	//virtual bool SetEaxListenerEnvironment(SOUND_REVERB_PRESETS nPreset, CRYSOUND_REVERB_PROPERTIES *pProps=NULL, int nFlags=0) = 0;

	//! Set the scaling factor for a specific scale group (0-7)
	virtual bool SetGroupScale(int nGroup, float fScale) = 0;

	//! Stop all sounds and music
	virtual bool	Silence(bool bStopLoopingSounds, bool bUnloadData) = 0;

	//! calls to be able to overwrite sound files and clear any loaded dependency
	virtual bool DeactivateAudioDevice() = 0;
	virtual bool ActivateAudioDevice() = 0;

	//! pause all sounds
	virtual void	Pause(bool bPause, bool bResetVolume=false) = 0; 
	virtual bool  IsPaused() = 0;

	//! Mute/unmute all sounds
	virtual void	Mute(bool bMute)=0;

	//! get memory usage info
	virtual void	GetSoundMemoryUsageInfo(int *nCurrentMemory,int *nMaxMemory)const = 0;

  // added to set and get music effect value for volume
  virtual void SetMusicEffectsVolume(float v ) = 0;
  
	//! get number of voices playing
	virtual int	GetUsedVoices() const = 0;

	//! get cpu-usuage
	virtual float	GetCPUUsage() const = 0;

	//! get music-volume
	virtual float GetMusicVolume() const = 0;

	//! sets parameters for directional attenuation (for directional microphone effect); set fConeInDegree to 0 to disable the effect
	virtual void CalcDirectionalAttenuation(const Vec3 &Pos, const Vec3 &Dir, const float fConeInRadians) = 0;

	//! returns the maximum sound-enhance-factor to use it in the binoculars as "graphical-equalizer"...
	virtual float GetDirectionalAttenuationMaxScale() = 0;

	//! returns if directional attenuation is used
	virtual bool UsingDirectionalAttenuation() = 0;

	//! sets the weather condition that affect acoustics
	virtual bool SetWeatherCondition(float fWeatherTemperatureInCelsius, float fWeatherHumidityAsPercent, float fWeatherInversion) = 0;

	//! gets the weather condition that affect acoustics
	virtual bool GetWeatherCondition(float &fWeatherTemperatureInCelsius, float &fWeatherHumidityAsPercent, float &fWeatherInversion) = 0;

	virtual void GetMemoryUsage(class ICrySizer* pSizer) const = 0;
	virtual int  GetMemoryUsageInMB() = 0;

	//! get the current area the listener is in
	//virtual IVisArea	*GetListenerArea()=0;

	//! returns true if sound is being debugged
	virtual bool DebuggingSound()=0;	

	//! Set minimal priority for sounds to be played.
	//! Sound`s with priority less then that will not be played.
	//! @return previous minimal priority.
	virtual int SetMinSoundPriority( int nPriority ) = 0;

	//! Lock all sound buffer resources to prevent them from unloading (when restoring checkpoint).
	virtual void LockResources() = 0;
	//! Unlock all sound buffer resources to prevent them from unloading.
	virtual void UnlockResources() = 0;


	//! This one is to debug some of FMOD mem allocation
	 virtual void TraceMemoryUsage(int nMemUsage) = 0;

	 //! Profiling Sounds
	 virtual ISoundProfileInfo* GetSoundInfo(int nIndex) = 0;
	 virtual ISoundProfileInfo* GetSoundInfo(const char* sSoundName) = 0;
	 virtual int GetSoundInfoCount() = 0;

	// check for multiple record devices and expose them
	virtual bool GetRecordDeviceInfo(const int nRecordDevice, char* sName, int nNameLength) = 0;

	// creates a new Microphone
	virtual IMicrophone* CreateMicrophone(
		const unsigned int nRecordDevice,
		const unsigned int nBitsPerSample, 
		const unsigned int nSamplesPerSecond,
		const unsigned int nBufferSizeInSamples) = 0;

	virtual bool RemoveMicrophone( IMicrophone *pMicrophone ) = 0;

	// creates a 3D sound emitting the talk of PlayerName that belongs to PlayerMicro
	// probably it even gets routed to the headset's speaker, so we need to identify target player
	virtual ISound* CreateNetworkSound(	INetworkSoundListener *pNetworkSoundListener,
		const unsigned int nBitsPerSample, 
		const unsigned int nSamplesPerSecond,
		const unsigned int nBufferSizeInSamples,
		EntityId PlayerID) = 0;

	// removes this network sound, probably ISound:Release can replace this later
	virtual void RemoveNetworkSound (ISound *pSound) = 0;

	// for serialization
	virtual void Serialize(TSerialize ser) = 0;
};

// special asset ParamSemantics
enum enumSoundParamSemantics
{
	// review and clean this list, check implementation
	spNONE,
	spSOUNDID,
	spSOUNDTYPE, // ?
	spSAMPLETYPE, //?
	spFREQUENCY,
	spPITCH,

	spVOLUME,
	spISPLAYING,	
	spPAUSEMODE,
	spLOOPMODE,
	spLENGTHINMS,
	spLENGTHINBYTES,
	spLENGTHINSAMPLES,
	spSAMPLEPOSITION,
	spTIMEPOSITION,
	sp3DPOSITION,
	sp3DVELOCITY,
	spMINRADIUS,
	spMAXRADIUS,
	spPRIORITY,
	spFXEFFECT,
	spAMPLITUDE,
	spSPEAKERPAN,
	spREVERBWET,
	spREVERBDRY,
	spSYNCTIMEOUTINSEC
};

//////////////////////////////
// A sound...
struct ISound
{

	// Register listener to the sound.
	virtual void AddEventListener( ISoundEventListener *pListener, const char *sWho ) = 0;
	virtual void RemoveEventListener( ISoundEventListener *pListener ) = 0;

	virtual bool IsPlaying() const = 0;
	virtual bool IsPlayingVirtual() const = 0;
	//! Return true if sound is now in the process of asynchronous loading of sound buffer.
	virtual bool IsLoading()const = 0;
	//! Return true if sound have already loaded sound buffer.
	virtual bool IsLoaded() const = 0;
	//! Unloads the soundbuffer of this sound.
	virtual bool UnloadBuffer() = 0;

	virtual void Play(float fVolumeScale=1.0f, bool bForceActiveState=true, bool bSetRatio=true, IEntitySoundProxy *pEntitySoundProxy=NULL) = 0;
	virtual void Stop(ESoundStopMode eStopMode=ESoundStopMode_EventFade) = 0; // Sound will be invalid after stopped, unless being static on a soundproxy
	virtual void SetPaused(bool bPaused) = 0;
	virtual bool GetPaused() const = 0;

	//! Fading In/Out - 0.0f is Out 1.0f is In
	virtual void				SetFade(const float fFadeGoal, const int nFadeTimeInMS) = 0;
	virtual EFadeState	GetFadeState() const = 0;

	virtual void						SetSemantic(ESoundSemantic eSemantic) = 0;
	virtual ESoundSemantic	GetSemantic() = 0;

	//virtual void SetFadeTime(const int nFadeTimeInMS) = 0;
	//virtual int  GetFadeTime() const = 0;

	//! Get name of sound file.
	virtual const char*	GetName() = 0;
	//! Get unique id of sound.
	virtual const tSoundID	GetId()const = 0;
	virtual	void						SetId(tSoundID SoundID) = 0;

	//! Set looping mode of sound.
	virtual void SetLoopMode(bool bLoop) = 0;

	virtual bool Preload() = 0;
 
	//! retrieves the currently played sample-pos, in milliseconds or bytes
	virtual unsigned int GetCurrentSamplePos(bool bMilliSeconds=false) const = 0;
	//! set the currently played sample-pos in bytes or milliseconds
	virtual void SetCurrentSamplePos(unsigned int nPos,bool bMilliSeconds) = 0;

	//! sets automatic pitching amount (0-1000)
	virtual void SetPitching(float fPitching) = 0;

	//! sets the volume ratio
	virtual void SetRatio(float fRatio)=0;

	//! Return frequency of sound.
	virtual int	 GetFrequency() const = 0;
	
	//! Set sound pitch.
	//! 1000 is default pitch.
	virtual void SetPitch(int nPitch) = 0;

	//! Set panning values (-1.0 left, 0.0 center, 1.0 right)
	virtual void	SetPan(float fPan) = 0;
	virtual float GetPan() const = 0;

	//! Set 3d panning values (0.0 no directional, 1.0 full 3d directional effect)
	virtual void	Set3DPan(float f3DPan) = 0;
	virtual float	Get3DPan() const = 0;

	//! set the maximum distance / the sound will be stopped if the
	//! distance from the listener and this sound is bigger than this max distance
//	virtual void SetMaxSoundDistance(float fMaxSoundDistance)=0;

	//! Set Minimal/Maximal distances for sound.
	//! Sound is not attenuated below minimal distance and not heared outside of max distance.
	virtual void SetMinMaxDistance(float fMinDist, float fMaxDist) = 0;

	//! Retrieves the maximum distance of a sound
	virtual float GetMaxDistance() const = 0;

	//! Sets a distance multiplier so sound event's distance can be tweak (sadly pretty workaround feature)
	virtual void SetDistanceMultiplier(const float fMultiplier) = 0;

	//! Define sound cone.
	//! Angles are in degrees, in range 0-360.
	virtual void SetConeAngles(const float fInnerAngle,const float fOuterAngle) = 0;
	virtual void GetConeAngles(float &fInnerAngle, float &fOuterAngle) = 0;

	// group stuff
	//! Add sound to specific sound-scale-group (0-31)
	virtual void AddToScaleGroup(int nGroup) = 0;
	//! Remove sound from specific sound-scale-group (0-31)
	virtual void RemoveFromScaleGroup(int nGroup) = 0;
	//! Set sound-scale-groups by bitfield.
	virtual void SetScaleGroup(unsigned int nGroupBits) = 0;

	//! Set sound volume.
	//! Range: 0-1
	virtual	void	SetVolume( const float fVolume ) = 0;

	//! Get sound volume.
	virtual	float	GetVolume() const = 0;

	//! Set sound source position.
	//IVO
	virtual void	SetPosition(const Vec3 &pos) = 0;

	//! Get sound source position.
	virtual Vec3 GetPosition() const = 0;

	// modify a line sound
	virtual void SetLineSpec(const Vec3 &vStart, const Vec3 &vEnd) = 0;
	virtual bool GetLineSpec(  Vec3 &vStart,   Vec3 &vEnd) = 0;

	// modify a sphere sound
	virtual void SetSphereSpec(const float fRadius) = 0;
	
	//! Set sound source velocity.
	virtual void	SetVelocity(const Vec3 &vel) = 0;
	//! Get sound source velocity.
	virtual Vec3	GetVelocity( void ) const = 0;

	//! Get sound obstruction struct.
	virtual SObstruction* GetObstruction( void ) = 0;
	//! Set to-be-skipped entities for obstruction calculation
	virtual void SetPhysicsToBeSkipObstruction(EntityId *pSkipEnts,int nSkipEnts) = 0;

	//! Set orientation of sound.
	//! Only relevant when cone angles are specified.
	virtual void	SetDirection( const Vec3 &dir ) = 0;
	virtual Vec3	GetDirection() const = 0;

	//virtual void SetLoopPoints(const int iLoopStart, const int iLoopEnd) = 0;
	virtual bool IsRelative() const = 0;

	// Add/remove sounds. // This is used in the smart_ptr
	virtual int	AddRef() = 0;
	virtual int	Release() = 0;

	// Sets and Gets the Sound Flags, SetFlags will overwrite all old Flags!
	virtual void		SetFlags(uint32 nFlags) = 0;
	virtual uint32	GetFlags() const = 0;

	//! enable fx effects for this sound
	//! must be called after each play
	virtual	void	FXEnable(int nEffectNumber)=0;

	virtual	void	FXSetParamEQ(float fCenter,float fBandwidth,float fGain)=0;

	//! returns the size of the stream in ms
	virtual int GetLengthMs() const = 0; //returns 0 on looping sound or if buffer is not loaded (dialog)
	//virtual int GetLengthInMs()=0;

	//! returns the size of the stream in bytes
	virtual int GetLength()const = 0; // will soon change to:
	//virtual int GetLengthInBytes()=0;

	//! set sound priority (0-255)
	virtual void	SetSoundPriority(int nSoundPriority)=0;	

	virtual bool IsInCategory(const char* sCategory) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// Gets and Sets Parameter defined in the enumAssetParam list
	virtual bool	GetParam(enumSoundParamSemantics eSemantics, ptParam* pParam) const = 0;
	virtual bool	SetParam(enumSoundParamSemantics eSemantics, ptParam* pParam) = 0;

	// Gets and Sets Parameter defined by string and float value, returns the index of that parameter
	virtual int	GetParam(const char *sParameter, float *fValue, bool bOutputWarning=true) const = 0;
	virtual int	SetParam(const char *sParameter, float fValue, bool bOutputWarning=true) = 0;

	// Gets and Sets Parameter defined by index and float value
	virtual bool	GetParam(int nIndex, float *fValue, bool bOutputWarning=true) const = 0;
	virtual bool	SetParam(int nIndex, float fValue, bool bOutputWarning=true) = 0;

};


//////////////////////////////
// A network sound listener
struct INetworkSoundListener
{
	// ask for buffer to be filled with data
	virtual bool FillDataBuffer(	unsigned int nBitsPerSample,
																unsigned int nSamplesPerSecond,		
																unsigned int nNumSamples, 
																void* pData ) = 0;

	virtual void OnActivate( bool active ) = 0;



	// set the distance at which sound playback switches between 2d and 3d.
	// closer than max3dDistance: sound plays in 3d
	// further than min2dDistance: sound plays in 2d
	// between the two: linear transition.
	// NB: asserts max3dDistance >= min2dDistance
	virtual void SetSoundPlaybackDistances(float max3dDistance, float min2dDistance) = 0;

	// recalculate the sound's 3d pan based on position
	virtual void UpdateSound3dPan() = 0;
};

//////////////////////////////
// A single microphone 
struct IMicrophone
{
	// start recording and calling ReadDataBuffer on IMicrophoneStream
	virtual bool Record(const unsigned int nRecordDevice, const unsigned int nBitsPerSample, 
											const unsigned int nSamplesPerSecond, const unsigned int nBufferSizeInSamples) = 0;

	// stop recording and calling the callback. Can restart with Record
	virtual bool Stop() = 0;

	// regular call to fill internal buffer
	virtual void Update() = 0;

	// returns size of data in the internal buffer(valid until next call to Update())
	virtual int GetDataSize()=0;

	// returns pointer to internal buffer (valid until next call to Update())
	virtual int16* GetData()=0;

	// will tear down this microphone
	virtual void Release() = 0;

};


//////////////////////////////////////////////////////////////////////////////////////////////
typedef ISoundSystem* (*PFNCREATESOUNDSYSTEM)(struct ISystem*, void*);

#ifdef CRYSOUNDSYSTEM_EXPORTS
	#define CRYSOUND_API DLL_EXPORT
#else
	#define CRYSOUND_API DLL_IMPORT
#endif

extern "C"
{
	CRYSOUND_API ISoundSystem* CreateSoundSystem(struct ISystem*, void *pInitData);
}

//////////////////////////////////////////////////////////////////////////
// interface that passes all parameters via a structures
// this allows having stable interface methods and flexible default arguments system
// see physics code for the original idea, tried to do this a bit nicer
//////////////////////////////////////////////////////////////////////////
// Generic ptParam structure that offers Get/SetValue, which returns bool
// if that query was successful and the type checking was ok
//
// Use this by implementing an abstract interface to get/set parameter
// bool	GetParam(enumParamSemantics eSemantics, ptParam* pParam);
// bool	SetParam(enumParamSemantics eSemantics, ptParam* pParam);
// 
// ptParamINT32 NewParam(pspNumberOfSomething, 42);
// m_pMyImpl->SetParam(&NewParam);
//
// Then internally use the Semantics to tell what the value means
// switch (eSemantics)
//
//	case pspNumberOfSomething:
//		int32 nTemp;
//		if (!(pParam->GetValue(nTemp))) return (false);
//		if (!CS_SetSomethingToFmod(m_nChannel,nTemp)) return (false);
//		break;
//////////////////////////////////////////////////////////////////////////

// Some Generic Parameter Types
enum enumParamType
{
	ptBOOL,
	ptVOIDP,
	ptF32,
	ptINT32,
	ptCRYSTRING,
	ptVEC3F32
};

////////////////////////// Params structures /////////////////////

struct ptParamBOOL;
struct ptParamVOIDP;
struct ptParamF32;
struct ptParamINT32;
struct ptParamCRYSTRING;
struct ptParamVEC3F32;

template<typename T> struct Map2ptParam;
template<> struct Map2ptParam<bool>			{ typedef bool Tkey; typedef ptParamBOOL Tval; enum { TypeId = ptBOOL }; };
template<> struct Map2ptParam<void*>		{ typedef void* Tkey; typedef ptParamVOIDP Tval; enum { TypeId = ptVOIDP }; };
template<> struct Map2ptParam<f32>			{ typedef f32 Tkey; typedef ptParamF32 Tval; enum { TypeId = ptF32 }; };
template<> struct Map2ptParam<int32>		{ typedef int32 Tkey; typedef ptParamINT32 Tval; enum { TypeId = ptINT32 }; };
template<> struct Map2ptParam<string>		{ typedef string Tkey; typedef ptParamCRYSTRING Tval; enum { TypeId = ptCRYSTRING }; };
template<> struct Map2ptParam<Vec3>			{ typedef Vec3 Tkey; typedef ptParamVEC3F32 Tval; enum { TypeId = ptVEC3F32 }; };

struct ptParam // todo rename
{
protected:
	ptParam(const enumParamType eType) : m_eType(eType) {}

	enumParamType m_eType;

public:
	// template class to the given type with the value and return if type check was ok
	template<typename T> bool GetValue(T &outVal) const
	{
		if (m_eType == (enumParamType)Map2ptParam<T>::TypeId)
		{
			outVal = ( *static_cast<const typename Map2ptParam<T>::Tval*>(this) ).m_val;
			return true;
		}
		return false; // type check failed
	}

	// template class to set the given type with the value and return if type check was ok
	template<typename T> bool SetValue(const T &inVal)
	{
		if (m_eType == (enumParamType)Map2ptParam<T>::TypeId)
		{
			( *static_cast<typename Map2ptParam<T>::Tval*>(this) ).m_val = inVal;
			return true;
		}
		return false; // type check failed
	}
};

struct ptParamBOOL : ptParam 
{ 
	friend struct ptParam;
	ptParamBOOL(bool newVal) : ptParam(ptBOOL), m_val(newVal) {}
private:
	bool m_val;
};

struct ptParamVOIDP : ptParam
{
	friend struct ptParam;
	ptParamVOIDP(void * newVal) : ptParam(ptVOIDP), m_val(newVal) {}
private:
	void* m_val;
};

struct ptParamF32 : ptParam 
{ 
	friend struct ptParam;
	ptParamF32(f32 newVal) : ptParam(ptF32), m_val(newVal) {} 
private:
	f32 m_val;
};

struct ptParamINT32 : ptParam 
{ 
	friend struct ptParam;
	ptParamINT32(int32 newVal) : ptParam(ptINT32), m_val(newVal) {}
private:
	int32 m_val;
};

struct ptParamCRYSTRING : ptParam 
{ 
	friend struct ptParam;
	ptParamCRYSTRING(const string& newVal) : ptParam(ptCRYSTRING), m_val(newVal) {}
private:
	string m_val;
};

struct ptParamVEC3F32 : ptParam 
{ 
	friend struct ptParam;
	ptParamVEC3F32(const Vec3& newVal) : ptParam(ptVEC3F32), m_val(newVal) {}
private:
	Vec3 m_val;
};

#endif // CRYSOUND_ISOUND_H
