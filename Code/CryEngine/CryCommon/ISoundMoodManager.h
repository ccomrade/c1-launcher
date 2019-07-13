////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2002.
// -------------------------------------------------------------------------
//  File name:   ISoundMoodManager.h
//  Version:     v1.00
//  Created:     13/5/2005 by Tomas.
//  Compilers:   Visual Studio.NET
//  Description: SoundManager is responsible for organising groups of sounds,
//							 creating new sounds by combinging sound settings and assets
//							 and caching sound settings and distance curves 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ISOUNDMOODMANAGER_H__
#define __ISOUNDMOODMANAGER_H__

#include "SerializeFwd.h"

#pragma once

#define SOUNDMOOD_FILENAME	"Libs/SoundMoods/SoundMoods.xml"

#define FLAG_SOUNDMOOD_MUTED						1				//1<<1
#define FLAG_SOUNDMOOD_SOLO							2				//1<<2 

#define FLAG_SOUNDMOOD_MOODTYPE					256			//1<<8 	
#define FLAG_SOUNDMOOD_CATEGORYTYPE			512			//1<<9 	
#define FLAG_SOUNDMOOD_CATEGORYSUBTYPE	1024		//1<<10 

enum enumSG_SEARCHDIRECTION
{
	eELEMENTONLY,
	eSEARCH_PARENTS,
	eSEARCH_CHILDREN
};

enum enumGroupParamSemantics
{
	gspNONE,
	gspFLAGS,
	gspFVOLUME,
	gspFPITCH,
	gspFLOWPASS,
	gspFHIGHPASS,
	gspMUTED,
	gspSOLO,
	gspLOOPED,
	gspLAST
};
// forward declaration

struct ISoundMoodManager;

struct ICategory : public _i_reference_target_t
{
	// Name
	virtual const char*	GetName() const  = 0;
	virtual void				SetName(const char* sCategoryName) = 0;
	virtual const char* GetMoodName() const = 0;

	// Platform Category
	virtual void*				GetPlatformCategory() const = 0;
	virtual void				SetPlatformCategory(void *pPlatformCategory) = 0;

	// Muted
	virtual bool				GetMuted() const = 0;
	virtual void				SetMuted(bool bMuted) = 0;

	// Parent
	virtual ICategory*	GetParentCategory() const = 0;
	virtual void				SetParentCategory(ICategory* pParent) = 0;

	// Children
	virtual uint32			GetChildCount() = 0;
	virtual ICategory*	GetCategoryByIndex(uint32 nCategoryCount) const = 0;
	virtual ICategory*	GetCategory(const char* sCategoryName) const = 0;
	virtual ICategory*	GetCategoryByPtr(void *pPlatformCategory) const = 0;
	virtual ICategory*	AddCategory(const char* sCategoryName) = 0;
	virtual bool				RemoveCategory(const char *sCategoryName) = 0;

	// Parameter
	virtual bool				GetParam(enumGroupParamSemantics eSemantics, ptParam* pParam) const = 0;
	virtual bool				SetParam(enumGroupParamSemantics eSemantics, ptParam* pParam) = 0;

	virtual bool				InterpolateCategories(ICategory *pCat1, float fFade1, ICategory *pCat2, float fFade2, bool bForceDefault) = 0;

	// Import Export
	virtual bool				Serialize(XmlNodeRef &node, bool bLoading) = 0;
	virtual bool				RefreshCategories(const bool bAddNew) = 0;

	// Functionality
	virtual bool ApplyCategory(float fFade) = 0;

	// Other
	virtual ISoundMoodManager* GetIMoodManager() = 0;

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, bool bAppliedValues) = 0;
};

struct IMood : public _i_reference_target_t
{
	// Name
	virtual const char*	GetName() const = 0;
	virtual void SetName(const char* sMoodName) = 0;

	// Master Category
	virtual ICategory*	GetMasterCategory() const = 0;
	
	// Category
	virtual uint32			GetCategoryCount() const = 0;
	virtual ICategory*	GetCategoryByIndex(uint32 nCategoryCount) const = 0;
	virtual ICategory*	GetCategory(const char *sCategoryName)  const = 0;
	virtual ICategory*	GetCategoryByPtr(void* pPlatformCategory) const = 0;
	virtual ICategory*	AddCategory(const char *sCategoryName) = 0;
	virtual bool				RemoveCategory(const char *sCategoryName) = 0;

	// Priority
	virtual float           GetPriority() const = 0;
	virtual void            SetPriority(float fPriority) = 0;
	// Music Volume
	virtual float           GetMusicVolume() const = 0;
	virtual void            SetMusicVolume(float fMusicVolume) = 0;

	// Import Export
	virtual bool Serialize(XmlNodeRef &node, bool bLoading) = 0;
	virtual void SetIsMixMood(const bool bIsMixMood) = 0;
	virtual bool GetIsMixMood() = 0;

	// Functionality
	virtual bool InterpolateMoods(const IMood *pMood1, float fFade1, const IMood *pMood2, float fFade2, bool bForceDefault) = 0;
	virtual bool ApplyMood(float fFade) = 0;

	// Other
  virtual ISoundMoodManager* GetIMoodManager() = 0;

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, bool bAppliedValues) = 0;
};



struct ISoundMoodManager
{
	//////////////////////////////////////////////////////////////////////////
	// Initialization
	//////////////////////////////////////////////////////////////////////////

	virtual bool RefreshCategories(const char* sMoodName) = 0;
	virtual bool RefreshPlatformCategories() = 0;
	virtual void Release() = 0;

	virtual void Reset() = 0; // removes all soundmoods except "default" and resets the effect

	//////////////////////////////////////////////////////////////////////////
	// Memory Usage
	//////////////////////////////////////////////////////////////////////////

	// Returns current Memory Usage
	virtual uint32 GetMemUsage(void) const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Information
	//////////////////////////////////////////////////////////////////////////

	virtual uint32 GetNumberSoundsLoaded(void) const = 0;

	// writes output to screen in debug
	virtual void DrawInformation(IRenderer* pRenderer, float &xpos, float &ypos, int nSoundInfo) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Management
	//////////////////////////////////////////////////////////////////////////

	// adds/removes a Mood to the MoodManager database
	virtual IMood*			AddMood(const char *sMoodName) = 0;
	virtual bool				RemoveMood(const char *sMoodName) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Real Time Manipulation
	//////////////////////////////////////////////////////////////////////////

	// needs to be updated regularly
	virtual bool Update() = 0;

	// registers a Mood to be actively processes (starts with 0 fade value)
	virtual bool	RegisterSoundMood(const char *sMoodName) = 0;

	// updates the fade value of a registered mood
	virtual bool	UpdateSoundMood(const char *sMoodName, float fFade, uint32 nFadeTimeInMS, bool bUnregistedOnFadeOut=true) = 0;
	
	// get current fade value of a registered mood
	virtual float	GetSoundMoodFade(const char *sMoodName) = 0;

	// unregisters a Mood and removes it from the active ones
	virtual bool	UnregisterSoundMood(const char *sMoodName) = 0;

		//////////////////////////////////////////////////////////////////////////
	// Access
	//////////////////////////////////////////////////////////////////////////

	virtual IMood*			GetMoodPtr(uint32 nGroupCount) const = 0;
	virtual IMood*			GetMoodPtr(const char *sMoodName) const = 0;

	//////////////////////////////////////////////////////////////////////////
	// Import/Export
	//////////////////////////////////////////////////////////////////////////

	virtual bool Serialize(XmlNodeRef &node, bool bLoading) = 0;
	virtual void SerializeState(TSerialize ser) = 0;

	//Other
	virtual bool RefreshCategories() = 0;



};
#endif