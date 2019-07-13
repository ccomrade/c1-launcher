////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   IReverbManager.h
//  Version:     v1.00
//  Created:     15/8/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: ReverbManager offers an abstraction layer to handle reverb.
//							 New environments can be activated and automatically blending
//							 into old ones.
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __IREVERBMANAGER_H__
#define __IREVERBMANAGER_H__

#include "SerializeFwd.h"

#pragma once

#define REVERB_PRESETS_FILENAME_LUA	"Libs/ReverbPresets/ReverbPresetDB.lua"
#define REVERB_PRESETS_FILENAME_XML	"Libs/ReverbPresets/ReverbPresets.xml"

#define REVERB_TYPE_NONE 0
#define REVERB_TYPE_HARDWARE 1
#define REVERB_TYPE_SOFTWARE 2
#define REVERB_TYPE_FREEVERB 3  // built in FMOD reverb
#define REVERB_TYPE_VST_CLASSIC_REVERB 4
#define REVERB_TYPE_VST_PRINCETON2016 5
#define REVERB_TYPE_VST_ROOMMACHINE844 6

// Reverb Presets
//////////////////////////////////////////////////////////////////////////////////////////////
enum SOUND_REVERB_PRESETS{
	REVERB_PRESET_OFF=0,              
	REVERB_PRESET_GENERIC,          
	REVERB_PRESET_PADDEDCELL,       
	REVERB_PRESET_ROOM, 	           
	REVERB_PRESET_BATHROOM, 	       
	REVERB_PRESET_LIVINGROOM,       
	REVERB_PRESET_STONEROOM,        
	REVERB_PRESET_AUDITORIUM,       
	REVERB_PRESET_CONCERTHALL,      
	REVERB_PRESET_CAVE,             
	REVERB_PRESET_ARENA,            
	REVERB_PRESET_HANGAR,           
	REVERB_PRESET_CARPETTEDHALLWAY, 
	REVERB_PRESET_HALLWAY,          
	REVERB_PRESET_STONECORRIDOR,    
	REVERB_PRESET_ALLEY, 	       
	REVERB_PRESET_FOREST, 	       
	REVERB_PRESET_CITY,             
	REVERB_PRESET_MOUNTAINS,        
	REVERB_PRESET_QUARRY,           
	REVERB_PRESET_PLAIN,            
	REVERB_PRESET_PARKINGLOT,       
	REVERB_PRESET_SEWERPIPE,        
	REVERB_PRESET_UNDERWATER,
	REVERB_PRESET_DRUGGED,
	REVERB_PRESET_DIZZY,
};

struct IAudioDevice;
struct ISound;

typedef struct CRYSOUND_REVERB_PROPERTIES /* MIN     MAX    DEFAULT   DESCRIPTION */
{          
	int					 Instance;
	int					 Environment;            /* 0     , 25    , 0      , sets all listener properties (WIN32/PS2 only) */
	float        EnvSize;                /* 1.0   , 100.0 , 7.5    , environment size in meters (WIN32 only) */
	float        EnvDiffusion;           /* 0.0   , 1.0   , 1.0    , environment diffusion (WIN32/XBOX) */
	int          Room;                   /* -10000, 0     , -1000  , room effect level (at mid frequencies) (WIN32/XBOX/PS2) */
	int          RoomHF;                 /* -10000, 0     , -100   , relative room effect level at high frequencies (WIN32/XBOX) */
	int          RoomLF;                 /* -10000, 0     , 0      , relative room effect level at low frequencies (WIN32 only) */
	float        DecayTime;              /* 0.1   , 20.0  , 1.49   , reverberation decay time at mid frequencies (WIN32/XBOX) */
	float        DecayHFRatio;           /* 0.1   , 2.0   , 0.83   , high-frequency to mid-frequency decay time ratio (WIN32/XBOX) */
	float        DecayLFRatio;           /* 0.1   , 2.0   , 1.0    , low-frequency to mid-frequency decay time ratio (WIN32 only) */
	int          Reflections;            /* -10000, 1000  , -2602  , early reflections level relative to room effect (WIN32/XBOX) */
	float        ReflectionsDelay;       /* 0.0   , 0.3   , 0.007  , initial reflection delay time (WIN32/XBOX) */
	float        ReflectionsPan[3];      /*       ,       , [0,0,0], early reflections panning vector (WIN32 only) */
	int          Reverb;                 /* -10000, 2000  , 200    , late reverberation level relative to room effect (WIN32/XBOX) */
	float        ReverbDelay;            /* 0.0   , 0.1   , 0.011  , late reverberation delay time relative to initial reflection (WIN32/XBOX) */
	float        ReverbPan[3];           /*       ,       , [0,0,0], late reverberation panning vector (WIN32 only) */
	float        EchoTime;               /* .075  , 0.25  , 0.25   , echo time (WIN32/PS2 only.  PS2 = Delay time for ECHO/DELAY modes only) */
	float        EchoDepth;              /* 0.0   , 1.0   , 0.0    , echo depth (WIN32/PS2 only.  PS2 = Feedback level for ECHO mode only) */
	float        ModulationTime;         /* 0.04  , 4.0   , 0.25   , modulation time (WIN32 only) */
	float        ModulationDepth;        /* 0.0   , 1.0   , 0.0    , modulation depth (WIN32 only) */
	float        AirAbsorptionHF;        /* -100  , 0.0   , -5.0   , change in level per meter at high frequencies (WIN32 only) */
	float        HFReference;            /* 1000.0, 20000 , 5000.0 , reference high frequency (hz) (WIN32/XBOX) */
	float        LFReference;            /* 20.0  , 1000.0, 250.0  , reference low frequency (hz) (WIN32 only) */
	float        RoomRolloffFactor;      /* 0.0   , 10.0  , 0.0    , like CS_3D_SetRolloffFactor but for room effect (WIN32/XBOX) */
	float        Diffusion;              /* 0.0   , 100.0 , 100.0  , Value that controls the echo density in the late reverberation decay. (XBOX only) */
	float        Density;                /* 0.0   , 100.0 , 100.0  , Value that controls the modal density in the late reverberation decay (XBOX only) */
	unsigned int Flags;                  /* CS_REVERB_FLAGS - modifies the behavior of above properties (WIN32/PS2 only) */
}CRYSOUND_REVERB_PROPERTIES;

typedef struct CRYSOUND_REVERB_CHANNELPROPERTIES  
{                                      /*          MIN     MAX    DEFAULT  DESCRIPTION */
	int          Direct;               /* [in/out] -10000, 1000,  0,       direct path level (at low and mid frequencies) (win32/Xbox) */
	int          DirectHF;             /* [in/out] -10000, 0,     0,       relative direct path level at high frequencies (win32/Xbox) */
	int          Room;                 /* [in/out] -10000, 1000,  0,       room effect level (at low and mid frequencies) (win32/Xbox/Gamecube/Xbox360) */
	int          RoomHF;               /* [in/out] -10000, 0,     0,       relative room effect level at high frequencies (win32/Xbox) */
	int          Obstruction;          /* [in/out] -10000, 0,     0,       main obstruction control (attenuation at high frequencies)  (win32/Xbox) */
	float        ObstructionLFRatio;   /* [in/out] 0.0,    1.0,   0.0,     obstruction low-frequency level re. main control (win32/Xbox) */
	int          Occlusion;            /* [in/out] -10000, 0,     0,       main occlusion control (attenuation at high frequencies) (win32/Xbox) */
	float        OcclusionLFRatio;     /* [in/out] 0.0,    1.0,   0.25,    occlusion low-frequency level re. main control (win32/Xbox) */
	float        OcclusionRoomRatio;   /* [in/out] 0.0,    10.0,  1.5,     relative occlusion control for room effect (win32) */
	float        OcclusionDirectRatio; /* [in/out] 0.0,    10.0,  1.0,     relative occlusion control for direct path (win32) */
	int          Exclusion;            /* [in/out] -10000, 0,     0,       main exlusion control (attenuation at high frequencies) (win32) */
	float        ExclusionLFRatio;     /* [in/out] 0.0,    1.0,   1.0,     exclusion low-frequency level re. main control (win32) */
	int          OutsideVolumeHF;      /* [in/out] -10000, 0,     0,       outside sound cone level at high frequencies (win32) */
	float        DopplerFactor;        /* [in/out] 0.0,    10.0,  0.0,     like DS3D flDopplerFactor but per source (win32) */
	float        RolloffFactor;        /* [in/out] 0.0,    10.0,  0.0,     like DS3D flRolloffFactor but per source (win32) */
	float        RoomRolloffFactor;    /* [in/out] 0.0,    10.0,  0.0,     like DS3D flRolloffFactor but for room effect (win32/Xbox) */
	float        AirAbsorptionFactor;  /* [in/out] 0.0,    10.0,  1.0,     multiplies AirAbsorptionHF member of FMOD_REVERB_PROPERTIES (win32) */
	unsigned int Flags;                /* [in/out] FMOD_REVERB_CHANNELFLAGS - modifies the behavior of properties (win32) */
} CRYSOUND_REVERB_CHANNELPROPERTIES;


#define CRYSOUND_REVERB_PRESET_ZERO    {0, 0,		0.0f,		0.0f,		0,			0,			0,	0.0f,		0.0f,		0.0f, 0,			0.0f,		{ 0.0f,0.0f,0.0f },		0,		0.0f,		{ 0.0f,0.0f,0.0f }, 0.0f,		0.0f,		0.0f,		0.0f,		0.0f,		0.0f,			0.0f,		0.0f, 0.0f,		0.0f,		0x3f}
#define CRYSOUND_REVERB_PRESET_OFF     {0, -1,	7.5f,		1.00f,	-10000, -10000, 0,  1.00f,  1.00f,	1.0f, -2602,	0.007f, { 0.0f,0.0f,0.0f },   200,	0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f,	0.25f,	0.000f, -5.0f,	5000.0f,	250.0f, 0.0f, 0.0f,   0.0f,		0x33f}
#define CRYSOUND_REVERB_PRESET_GENERIC {0, 0,		7.5f,		1.00f,	-1000,  -100,   0,  1.49f,  0.83f,	1.0f, -2602,	0.007f, { 0.0f,0.0f,0.0f },   200,	0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f,	0.25f,	0.000f, -5.0f,	5000.0f,	250.0f, 0.0f, 100.0f, 100.0f, 0x3f}
#define CRYSOUND_REVERB_UNDERWATER		 {0, 22,	1.8f,		1.00f,	-1000,	-4000,  0,  1.49f,	0.10f,	1.0f, -449,		0.007f, { 0.0f,0.0f,0.0f },		1700, 0.011f, { 0.0f,0.0f,0.0f }, 0.250f, 0.00f,	1.18f,	0.348f, -5.0f,	5000.0f,	250.0f, 0.0f, 100.0f, 100.0f, 0x3f}
#define CRYSOUND_REVERB_PRESET_OUTDOOR {0, 0,		100.0f,	1.0f,		-2000,	-100,		0,	0.2f,		0.83f,	1.0f,	-2602,	0.015f,	{ 0.0f,0.0f,0.0f },		-500,	0.011f,	{ 0.0f,0.0f,0.0f }, 0.25f,	0.0f,		0.25f,	0.0f,		-5.0f,	5000.0f,	250.0f,	0.0f, 100.0f,	100.0f,	0x3f}

#define CRYSOUND_REVERB_CHANNEL_ZERO			{0, 0, 0,				0,			0, 0.0f, 0, 0.0f,		0.0f,		0.0f,		0, 0.0f, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0}
#define CRYSOUND_REVERB_CHANNEL_GENERIC		{0, 0, 0,				0,			0, 0.0f, 0, 0.25f,	1.5f,		1.0f,		0, 1.0f, 0, 0.0f, 0.0f, 0.0f, 1.0f, 0}
#define CRYSOUND_REVERB_CHANNEL_NOREVERB	{0, 0, -10000, -10000,	0, 0.0f, 0, 0.25f,	1.5f,		1.0f,		0, 1.0f, 0, 0.0f, 0.0f, 0.0f, 1.0f, 0}

//#define FMOD_REVERB_CHANNELFLAGS_DIRECTHFAUTO  0x00000001
//#define FMOD_REVERB_CHANNELFLAGS_ROOMAUTO  0x00000002
//#define FMOD_REVERB_CHANNELFLAGS_ROOMHFAUTO  0x00000004
//#define FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0  0x00000008
//#define FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1  0x00000010
//#define FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT2  0x00000020
//#define FMOD_REVERB_CHANNELFLAGS_DEFAULT  (FMOD_REVERB_CHANNELFLAGS_DIRECTHFAUTO | FMOD_REVERB_CHANNELFLAGS_ROOMAUTO| FMOD_REVERB_CHANNELFLAGS_ROOMHFAUTO| FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0)


//////////////////////////////////////////////////////////////////////////
// Reverb interface structures.
//////////////////////////////////////////////////////////////////////////
struct SReverbInfo
{
	//struct SReverbProps
	//{
	//	CRYSOUND_REVERB_PROPERTIES EAX;
	//	int nPreset;
	//};

	struct SReverbPreset
	{
		string sPresetName;
		string sTailName;			 /* internal string to switch between different tails */
		CRYSOUND_REVERB_PROPERTIES EAX;
	};

	struct SWeightedReverbPreset
	{
		SReverbPreset* pReverbPreset;
		float fWeight;
		bool bFullEffectWhenInside;
	};

	struct Data
	{
		SReverbPreset *pPresets;
		int nPresets;
		//Mood *pMoods;
		//int nMoods;
		//Pattern *pPatterns;
		//int nPatterns;
	};

};

struct IReverbManager
{
	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////
	virtual void Init(IAudioDevice *pAudioDevice, int nInstanceNumber) = 0;
	virtual bool SelectReverb(int nReverbType) = 0;

	virtual void Reset() = 0;
	virtual void Release() = 0;

	virtual bool SetData( SReverbInfo::Data *pReverbData ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// needs to be called regularly
	virtual bool Update(bool bInside) = 0;

	virtual bool SetListenerReverb(SOUND_REVERB_PRESETS nPreset, CRYSOUND_REVERB_PROPERTIES *tpProps=NULL, uint32 nFlags=0) = 0;
	
	// Preset management
	virtual bool RegisterReverbPreset(const char *sPresetName=NULL, CRYSOUND_REVERB_PROPERTIES *pProps=NULL, const char *sTailName=NULL) = 0;
	virtual bool UnregisterReverbPreset(const char *sPresetName=NULL) = 0;

	//! Registers an EAX Preset Area with the current blending weight (0-1) as being active
	//! morphing of several EAX Preset Areas is done internally
	//! Registering the same Preset multiple time will only overwrite the old one
	virtual bool RegisterWeightedReverbEnvironment(const char *sPreset=NULL, bool bFullEffectWhenInside=false, uint32 nFlags=0) = 0;
	//virtual bool RegisterWeightedReverbEnvironment(const char *sPreset=NULL, CRYSOUND_REVERB_PROPERTIES *pProps=NULL, bool bFullEffectWhenInside=false, uint32 nFlags=0) = 0;
	//virtual bool RegisterWeightedReverbEnvironment(const char *sPreset=NULL, SOUND_REVERB_PRESETS nPreset, bool bFullEffectWhenInside=false, uint32 nFlags=0) = 0;

	//! Updates an EAX Preset Area with the current blending ratio (0-1)
	virtual bool UpdateWeightedReverbEnvironment(const char *sPreset=NULL, float fWeight=0.0) = 0;

	//! Unregisters an active EAX Preset Area 
	virtual bool UnregisterWeightedReverbEnvironment(const char *sPreset=NULL) = 0;

	//! Gets current EAX listener environment.
	virtual bool GetCurrentReverbEnvironment(SOUND_REVERB_PRESETS &nPreset, CRYSOUND_REVERB_PROPERTIES &Props) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Dependency with loading a sound
	//////////////////////////////////////////////////////////////////////////
	//! returns boolean if hardware reverb (EAX) is used or not
	virtual bool UseHardwareVoices() = 0;


	virtual const char* GetTailName() = 0;

	virtual bool RegisterSound(ISound *pSound) = 0;
	virtual int  GetReverbInstance(ISound *pSound) const = 0;

	virtual bool RegisterPlayer(EntityId PlayerID) = 0;
	virtual bool UnregisterPlayer(EntityId PlayerID) = 0;

	// virtual bool EvaluateSound(ISound *pSound);

	//virtual SSGHandle ImportXMLFile(const string sGroupName) = 0;

	virtual void SerializeState(TSerialize ser) = 0;


	//////////////////////////////////////////////////////////////////////////
	// ISoundEventListener implementation
	//////////////////////////////////////////////////////////////////////////
//	virtual void OnSoundEvent( ESoundCallbackEvent event,ISound *pSound );

};
#endif