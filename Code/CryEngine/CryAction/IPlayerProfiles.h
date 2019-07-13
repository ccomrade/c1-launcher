// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef __IPLAYERPROFILES_H__
#define __IPLAYERPROFILES_H__

#if _MSC_VER > 1000
#	pragma once
#endif

#include <IFlowSystem.h>

struct IPlayerProfile;
struct IActionMap;

struct ISaveGameThumbnail
{
	// a thumbnail is a image in BGR or BGRA format
	// uint8* p; p[0] = B; p[1] = G; p[2] = R; p[3] = A;

	virtual const char* GetSaveGameName() = 0;
	// image access
	virtual void  GetImageInfo(int& width, int& height, int& depth) = 0;
	virtual int   GetWidth() = 0;
	virtual int   GetHeight() = 0;
	virtual int   GetDepth() = 0;
	virtual const uint8* GetImageData() = 0;

	// smart ptr
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<ISaveGameThumbnail> ISaveGameThumbailPtr;

struct ISaveGameEnumerator
{
	struct SGameMetaData
	{
		const char* levelName;
		const char* gameRules;
		int         fileVersion;
		const char* buildVersion;
		time_t      saveTime;
		XmlNodeRef  xmlMetaDataNode;
	};

	struct SGameDescription
	{
		const char* name;
		const char* humanName;
		const char* description;
		SGameMetaData metaData;
	};

	virtual int  GetCount() = 0;
	virtual bool GetDescription(int index, SGameDescription& desc) = 0;

	// Get thumbnail (by index or save game name)
	virtual ISaveGameThumbailPtr GetThumbnail(int index) = 0;
	virtual ISaveGameThumbailPtr GetThumbnail(const char* saveGameName) = 0;

	// smart ptr
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<ISaveGameEnumerator> ISaveGameEnumeratorPtr;

struct IAttributeEnumerator
{
	struct SAttributeDescription
	{
		const char* name;
	};

	virtual bool Next(SAttributeDescription& desc) = 0;

	// smart ptr
	virtual void AddRef() = 0;
	virtual void Release() = 0;
};
typedef _smart_ptr<IAttributeEnumerator> IAttributeEnumeratorPtr;

struct IPlayerProfileManager
{
	struct SProfileDescription
	{
		const char* name;
	};

	struct SUserInfo
	{
		const char* userId;
	};

	virtual bool Initialize() = 0;
	virtual bool Shutdown() = 0;

	virtual void GetMemoryStatistics( ICrySizer * ) = 0;

	// win32:    currently logged on user
	// xbox360:  more than one user can be signed in
	// ps3:      several connected pads possible

	// login the user
	virtual int  GetUserCount() = 0;
	virtual bool GetUserInfo(int index, IPlayerProfileManager::SUserInfo& outInfo) = 0;
	virtual bool LoginUser(const char* userId, bool& bOutFirstTime) = 0;
	virtual bool LogoutUser(const char* userId) = 0;

	virtual int  GetProfileCount(const char* userId) = 0;
	virtual bool GetProfileInfo(const char* userId, int index, IPlayerProfileManager::SProfileDescription& outInfo) = 0;

	enum EProfileOperationResult
	{
		ePOR_Success					= 0,
		ePOR_NotInitialized		= 1,
		ePOR_NameInUse				= 2,
		ePOR_UserNotLoggedIn	= 3,
		ePOR_NoSuchProfile		= 4,
		ePOR_ProfileInUse			= 5,
		ePOR_NoActiveProfile	= 6,
		ePOR_DefaultProfile   = 7,
		ePOR_Unknown					=	255,
	};

	// create a new profile for a user 
	virtual bool CreateProfile(const char* userId, const char* profileName, bool bOverrideIfPresent, EProfileOperationResult& result) = 0;

	// delete a profile of an user 
	virtual bool DeleteProfile(const char* userId, const char* profileName, EProfileOperationResult& result) = 0;

	// rename the current profile of the user
	virtual bool RenameProfile(const char* userId, const char* newName, EProfileOperationResult& result) = 0;

	// save a profile
	virtual bool SaveProfile(const char* userId, EProfileOperationResult& result) = 0;

	// load and activate a profile, returns the IPlayerProfile if successful
	virtual IPlayerProfile* ActivateProfile(const char* userId, const char* profileName) = 0;

	// get the current profile of the user
	virtual IPlayerProfile* GetCurrentProfile(const char* userId) = 0;

	// get the current profile of the user
	virtual const char* GetCurrentUser() = 0;

	// reset the current profile
	// reset actionmaps and attributes, don't delete save games! 
	virtual bool ResetProfile(const char* userId) = 0;

	// get the (always present) default profile (factory default)
	virtual IPlayerProfile* GetDefaultProfile() = 0;

	// load a profile for previewing only. there is exactly one preview profile for a user
	// subsequent calls will invalidate former profiles. profileName can be "" or NULL which will
	// delete the preview profile from memory
	virtual const IPlayerProfile* PreviewProfile(const char* userId, const char* profileName) = 0;

	// Set a shared savegame folder for all profiles
	// this means all savegames get prefixed with the profilename and '_'
	// by default: SaveGame folder is shared "%USER%/SaveGames/"
	virtual void SetSharedSaveGameFolder(const char* sharedSaveGameFolder) = 0;

	// Get the shared savegame folder
	virtual const char* GetSharedSaveGameFolder() = 0;
};

struct ILevelRotationFile
{
  virtual bool Save(XmlNodeRef r) = 0;
  virtual XmlNodeRef Load() = 0;
  virtual void Complete() = 0;
};

struct IPlayerProfile
{
	// reset the profile
	virtual bool Reset() = 0; 

	// is this the default profile? it cannot be modified
	virtual bool IsDefault() const = 0;

	// name of the profile
	virtual const char* GetName() = 0;

	// Id of the profile user
	virtual const char* GetUserId() = 0;

	// retrieve an action map 
	virtual IActionMap* GetActionMap(const char* name) = 0;

	// set the value of an attribute
	virtual bool SetAttribute(const char* name, const TFlowInputData& value) = 0;

	// re-set attribute to default value (basically removes it from this profile)
	virtual bool ResetAttribute(const char* name) = 0;

	// get the value of an attribute. if not specified optionally lookup in default profile
	virtual bool GetAttribute(const char* name, TFlowInputData& val, bool bUseDefaultFallback = true) const = 0;

	template<typename T> bool GetAttribute(const char* name, T& outVal, bool bUseDefaultFallback = true) const
	{
		TFlowInputData val;
		if (GetAttribute(name, val, bUseDefaultFallback) == false)
			return false;
		return val.GetValueWithConversion(outVal);
	}

	template<typename T> bool SetAttribute(const char* name, const T& val)
	{
		TFlowInputData data (val);
		return SetAttribute(name, data);
	}

	// get name all attributes available 
	// all in this profile and inherited from default profile
	virtual IAttributeEnumeratorPtr CreateAttributeEnumerator() = 0;

	// save game stuff
	virtual ISaveGameEnumeratorPtr CreateSaveGameEnumerator() = 0;
	virtual ISaveGame* CreateSaveGame() = 0;
	virtual ILoadGame* CreateLoadGame() = 0;
	virtual bool DeleteSaveGame(const char* name) = 0;

  virtual ILevelRotationFile* GetLevelRotationFile(const char* name) = 0;
};

#endif
