//////////////////////////////////////////////////////////////////////
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
//
//  CrySound Source Code
//
//  File: ISound.h
//  Description: Music System Interface.
// 
//  History:
//  - August 25, 2005: seperate file created by Tomas Neuman
//  - August 28, 2001: Created by Marco Corbetta (as part of ISound)
//
//////////////////////////////////////////////////////////////////////

#ifndef CRYSOUND_IMUSICSYSTEM_H
#define CRYSOUND_IMUSICSYSTEM_H

#include "StlUtils.h"
#include "ISerialize.h"
//#include "SerializeFwd.h"

//////////////////////////////////////////////////////////////////////////
// String Iterator
struct IStringItVec
{
	virtual bool IsEnd() = 0;
	virtual const char* Next() = 0;
	virtual void MoveFirst() = 0;
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};

//////////////////////////////////////////////////////////////////////////
// MusicSystem

// Different layers
#define MUSICLAYER_NONE							0x00000000
#define MUSICLAYER_MAIN							0x00000001
#define MUSICLAYER_RHYTHMIC					0x00000002
#define MUSICLAYER_INCIDENTAL				0x00000004
#define MUSICLAYER_STINGER					0x00000008
#define MUSICLAYER_START						0x00000010
#define MUSICLAYER_END							0x00000020
#define MUSICLAYER_MAIN_ANY					0x000000FF    
#define MUSICLAYER_PATTERN_BY_NAME	0x00000100
//#define MUSICLAYER_STINGER					0x00000200
#define MUSICLAYER_ANY              0xFFFFFFFF                      

// Type of blending
enum EBlendingType
{
	EBlend_None,
	EBlend_PlayOnFadePoint,
	EBlend_EndOnFadePoint,
	EBlend_FadeIn,
	EBlend_FadeInOnFadePoint,
	EBlend_FadeOut,
	EBlend_FadeOutOnFadePoint,
	EBlend_FadeOutOnMainEnd,
	EBlend_Any,
	EBlend_MAX,
};

// Type of playing
enum EPlayingType
{
	EPlaying_Loop,
	EPlaying_Once,
	EPlaying_Trigger_Next,
	EPlaying_Stop,
	EPlaying_Any,
	EPlaying_MAX,
};

// Type of ThemeFading
enum EThemeFadeType
{
	EThemeFade_StopAtOnce,
	EThemeFade_FadeOut,
	EThemeFade_PlayEnd,
	EThemeFade_PlayEndAtFadePoint,
	EThemeFade_KeepMood,
	EThemeFade_MAX,
};

//////////////////////////////////////////////////////////////////////////
// Structures to pass as data-entry for music system
//////////////////////////////////////////////////////////////////////////

struct SPatternDef;
struct SMusicPatternSet;
struct SMusicMood;
struct SMusicTheme;
struct IRenderer;

// Helper integer-vector
typedef std::vector<int>	TIntVec;
typedef TIntVec::iterator	TIntVecIt;

typedef std::vector<SPatternDef*>							TPatternDefVec;
typedef TPatternDefVec::iterator							TPatternDefVecIt;

typedef std::map<string,string,stl::less_stricmp<string> > TThemeBridgeMap;
typedef TThemeBridgeMap::iterator							TThemeBridgeMapIt;

typedef std::map<string,SMusicMood*,stl::less_stricmp<string> > TMoodMap;
typedef TMoodMap::iterator										TMoodMapIt;
typedef TMoodMap::const_iterator							TMoodMapItConst;

typedef std::vector<SMusicPatternSet*>				TPatternSetVec;
typedef TPatternSetVec::iterator							TPatternSetVecIt;

typedef std::map<string,SMusicTheme*,stl::less_stricmp<string> >	TThemeMap;
typedef TThemeMap::iterator										TThemeMapIt;
typedef TThemeMap::const_iterator							TThemeMapItConst;

// Pattern-definition
struct SPatternDef
{
	string sName;
	string sFilename;
	TIntVec vecFadePoints;
	float fLayeringVolume;
	float fProbability;
	int   nPreFadeIn;

	SPatternDef()
	{
		fLayeringVolume = 1.0f;
		fProbability = 1.0f;
		nPreFadeIn = 0;
	}
};

// PatternSet-Structure used by moods
struct SMusicPatternSet
{
	//Pattern set
	float fMinTimeout;
	float fMaxTimeout;
	
	//Main
	float fTotalMainPatternProbability;	// added probabilities of all containing patterns
	TPatternDefVec vecMainPatterns;

	//Rhythmic
	int nMaxSimultaneousRhythmicPatterns;
	float fRhythmicLayerProbability;
	float fTotalRhythmicPatternProbability;	// added probabilities of all containing patterns
	TPatternDefVec vecRhythmicPatterns;
	
	//Incidental
	int nMaxSimultaneousIncidentalPatterns;
	float fIncidentalLayerProbability;
	float fTotalIncidentalPatternProbability;	// added probabilities of all containing patterns
	TPatternDefVec vecIncidentalPatterns;

	//Start
	TPatternDefVec vecStartPatterns;

	//End
	TPatternDefVec vecEndPatterns;

	//Stinger
	TPatternDefVec vecStingerPatterns;
	float fTotalStingerPatternProbability;

	SMusicPatternSet()
	{
		fMinTimeout = 0;
		fMaxTimeout = 0;
		fRhythmicLayerProbability = 0;
		fIncidentalLayerProbability = 0;

		fTotalMainPatternProbability = 0;
		fTotalRhythmicPatternProbability = 0;
		fTotalIncidentalPatternProbability = 0;
		fTotalStingerPatternProbability = 0;

		nMaxSimultaneousRhythmicPatterns = 1;
		nMaxSimultaneousIncidentalPatterns = 1;
	}
};

// Mood-Structure
struct SMusicMood
{
	string sName;
	int nPriority;
	float fFadeOutTime;
	TPatternSetVec vecPatternSets;
	bool bPlaySingle;
	bool bPlayFromStart;
	bool bMainSpawned;
	// internal
	SMusicPatternSet *pCurrPatternSet;
	float fCurrPatternSetTime;
	float fCurrPatternSetTimeout;
	int nPatternRandIndex;
	SMusicTheme* pSpawnedFromTheme;

	SMusicMood()
	{
		nPriority = 0;
		fFadeOutTime = 0;
		bPlaySingle			= false;
		bPlayFromStart	= false;
		bMainSpawned		= false;
		pCurrPatternSet = NULL;
		fCurrPatternSetTime = 0;
		fCurrPatternSetTimeout = 0;
		nPatternRandIndex = 0;
		pSpawnedFromTheme = NULL;
	}
};

// Theme-Structure
struct SMusicTheme
{
	string sName;
	TMoodMap mapMoods;
	TThemeBridgeMap mapBridges;
	// default mood
	string sDefaultMood;
	float fDefaultMoodTimeout;

	SMusicTheme()
	{
		fDefaultMoodTimeout = 0;
	}
};


//////////////////////////////////////////////////////////////////////////
// Music interface structures.
//////////////////////////////////////////////////////////////////////////
struct SMusicInfo
{
	struct Pattern
	{
		const char *sTheme;
		const char *sMood;
		int nPatternSetIndex;  // Index in mood pattern set.
		int nPatternType;      // Type of pattern (Main/Rhythmic/Incidental)

		const char *sName;
		const char *sFilename;
		int *pFadePoints;
		int nFadePointsCount;
		int nPreFadeIn;
		float fLayeringVolume;
		float fProbability;
	};

	//////////////////////////////////////////////////////////////////////////
	// PatternSet-Structure used by moods
	struct PatternSet
	{
		float fMinTimeout;
		float fMaxTimeout;
		float fTotalMainPatternProbability;	// added probabilities of all containing patterns

		int nMaxSimultaneousRhythmicPatterns;
		float fRhythmicLayerProbability;
		float fTotalRhythmicPatternProbability;	// added probabilities of all containing patterns

		int nMaxSimultaneousIncidentalPatterns;
		float fIncidentalLayerProbability;
		float fTotalIncidentalPatternProbability;	// added probabilities of all containing patterns

		int nMaxSimultaneousStartPatterns;
		float fStartLayerProbability;
		float fTotalStartPatternProbability;	// added probabilities of all containing patterns

		int nMaxSimultaneousEndPatterns;
		float fEndLayerProbability;
		float fTotalEndPatternProbability;	// added probabilities of all containing patterns

		float fTotalStingerPatternProbability; // added probabilities of all containing patterns
	};

	//////////////////////////////////////////////////////////////////////////
	struct Mood
	{
		const char *sTheme;
		const char *sName;
		int nPriority;
		float fFadeOutTime;

		bool bPlaySingle;

		PatternSet *pPatternSets;
		int nPatternSetsCount;
	};

	//////////////////////////////////////////////////////////////////////////
	struct Theme
	{
		const char *sName;
		const char *sDefaultMood;
		float fDefaultMoodTimeout;

		const char **pBridges; // Pairs of strings.
		int nBridgeCount;
	};

	struct Data
	{
		Theme *pThemes;
		int nThemes;
		Mood *pMoods;
		int nMoods;
		Pattern *pPatterns;
		int nPatterns;
	};
};
//////////////////////////////////////////////////////////////////////////

#define DEFAULT_CROSSFADE_TIME			3.0

//////////////////////////////////////////////////////////////////////////
// Status struct
//////////////////////////////////////////////////////////////////////////

struct SPlayingPatternsStatus
{
	string sName;
	int nLayer;
	float fVolume;
	float fPhase;
	EBlendingType		eBlendType; 
	EPlayingType		ePlayingType; 
};

typedef std::vector<SPlayingPatternsStatus>		TPatternStatusVec;
typedef TPatternStatusVec::iterator						TPatternStatusVecIt;

struct SMusicSystemStatus
{
	bool bPlaying;
	string sTheme;
	string sMood;
	TPatternStatusVec m_vecPlayingPatterns;
};


//////////////////////////////////////////////////////////////////////////
// Main music-interface
//////////////////////////////////////////////////////////////////////////

struct IMusicSystem
{
	virtual void Release() = 0;
	virtual void Update() = 0;

	virtual int GetBytesPerSample() = 0;
	virtual struct IMusicSystemSink* SetSink(struct IMusicSystemSink *pSink) = 0;
	virtual bool SetData( SMusicInfo::Data *pMusicData ) = 0;
	virtual void Unload() = 0;
	virtual void Pause(bool bPause) = 0;
	virtual void EnableEventProcessing(bool bEnable) = 0;
	
	// theme stuff
	virtual bool SetTheme(const char *pszTheme, bool bForceChange=true, bool bKeepMood=true, int nDelayInSec=-1) = 0;
	virtual bool EndTheme(EThemeFadeType ThemeFade=EThemeFade_FadeOut, int nForceEndLimitInSec=10, bool bEndEverything=true) = 0;
	virtual const char* GetTheme() const = 0;
	virtual IStringItVec* GetThemes() const = 0;
	virtual CTimeValue GetThemeTime() const = 0;

	// Mood stuff
	virtual bool SetMood(const char *pszMood, const bool bPlayFromStart=true, const bool bForceChance=false ) = 0;
	virtual bool SetDefaultMood(const char *pszMood) = 0;
	virtual const char* GetMood() const = 0;
	virtual IStringItVec* GetMoods(const char *pszTheme) const = 0;
	virtual bool AddMusicMoodEvent(const char *pszMood, float fTimeout) = 0;
	virtual CTimeValue GetMoodTime() const = 0;
	
	// general
	virtual SMusicSystemStatus* GetStatus() = 0;		// retrieve status of music-system... dont keep returning pointer !
	virtual void GetMemoryUsage(class ICrySizer* pSizer) const = 0;
	virtual bool StreamOGG() = 0;
	virtual void LogMsg( const int nVerbosity, const char *pszFormat, ... ) PRINTF_PARAMS(3, 4) = 0;

	//////////////////////////////////////////////////////////////////////////
	//! Load music data from XML.
	//! @param bAddAdata if true data from XML will be added to currently loaded music data.
	virtual bool LoadFromXML( const char *sFilename, bool bAddData, bool bReplaceData) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Editing support.
	//////////////////////////////////////////////////////////////////////////
	virtual void UpdatePattern( SMusicInfo::Pattern *pPattern ) = 0;
	virtual void RenamePattern( const char *sOldName,const char *sNewName ) = 0;
	virtual void PlayPattern( const char *sPattern, bool bStopPrevious, bool bPlaySynched ) = 0;
	virtual void DeletePattern( const char *sPattern ) = 0;
	virtual void Silence() = 0;

	virtual void PlayStinger() = 0;

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos) = 0;

	// for serialization
	virtual void Serialize(TSerialize ser) = 0;
	virtual bool SerializeInternal(bool bSave) = 0;
};

//////////////////////////////////////////////////////////////////////////
// Sink to release data (if allocated in a different DLL
//////////////////////////////////////////////////////////////////////////

struct IMusicSystemSink
{
	virtual void ReleaseData(struct SMusicData *pData) = 0;
};


#define UPDATE_MUSICLOGIC_IN_MS 200

enum EMusicLogicEvents
{
	eMUSICLOGICEVENT_SET_MULTIPLIER = 0,
	eMUSICLOGICEVENT_SET_AI_MULTIPLIER,
	eMUSICLOGICEVENT_SET_AI,
	eMUSICLOGICEVENT_CHANGE_AI,
	eMUSICLOGICEVENT_SET_PLAYER,
	eMUSICLOGICEVENT_CHANGE_PLAYER,
	eMUSICLOGICEVENT_SET_GAME,
	eMUSICLOGICEVENT_CHANGE_GAME,
	eMUSICLOGICEVENT_SET_ALLOWCHANGE,
	eMUSICLOGICEVENT_CHANGE_ALLOWCHANGE,
	eMUSICLOGICEVENT_VEHICLE_ENTER,
	eMUSICLOGICEVENT_VEHICLE_LEAVE,
	eMUSICLOGICEVENT_WEAPON_MOUNT,
	eMUSICLOGICEVENT_WEAPON_UNMOUNT,
	eMUSICLOGICEVENT_SNIPERMODE_ENTER,
	eMUSICLOGICEVENT_SNIPERMODE_LEAVE,
	eMUSICLOGICEVENT_CLOAKMODE_ENTER,
	eMUSICLOGICEVENT_CLOAKMODE_LEAVE,
	eMUSICLOGICEVENT_ENEMY_SPOTTED,
	eMUSICLOGICEVENT_ENEMY_KILLED,
	eMUSICLOGICEVENT_ENEMY_HEADSHOT,
	eMUSICLOGICEVENT_ENEMY_OVERRUN,
	eMUSICLOGICEVENT_PLAYER_WOUNDED,
	eMUSICLOGICEVENT_PLAYER_KILLED,
	eMUSICLOGICEVENT_PLAYER_SPOTTED,
	eMUSICLOGICEVENT_PLAYER_TURRET_ATTACK,
	eMUSICLOGICEVENT_PLAYER_SWIM_ENTER,
	eMUSICLOGICEVENT_PLAYER_SWIM_LEAVE,
	eMUSICLOGICEVENT_EXPLOSION,
	eMUSICLOGICEVENT_FACTORY_CAPTURED,
	eMUSICLOGICEVENT_FACTORY_LOST,
	eMUSICLOGICEVENT_FACTORY_RECAPTURED,
	eMUSICLOGICEVENT_VEHICLE_CREATED,
	eMUSICLOGICEVENT_MAX
};


#define MUSICLOGIC_FILENAME	"Libs/MusicLogic/MusicLogic.xml"

struct IMusicLogic
{
public:

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	virtual bool Init() = 0;

	virtual bool Start() = 0;
	virtual bool Stop() = 0;

	virtual void Update() = 0;

	// incoming events
	virtual void SetEvent(EMusicLogicEvents MusicEvent, const float fValue=0.0f)  = 0;

	//IHitListener
	//virtual void OnHit(const HitInfo&);
	//virtual void OnExplosion(const ExplosionInfo&);
	//virtual void OnServerExplosion(const ExplosionInfo&);

	virtual void GetMemoryStatistics( ICrySizer * ) = 0;
	virtual void Serialize(TSerialize ser) = 0;

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float xpos, float ypos, int nSoundInfo) = 0;

};



#endif // CRYSOUND_IMUSICSYSTEM_H
