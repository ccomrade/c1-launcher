/*************************************************************************
  Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  $Id$
  $DateTime$
  Description: Gathers level information. Loads a level.
  
 -------------------------------------------------------------------------
  History:
  - 18:8:2004   11:22 : Created by Márcio Martins

*************************************************************************/
#ifndef __ILEVELSYSTEM_H__
#define __ILEVELSYSTEM_H__

#if _MSC_VER > 1000
# pragma once
#endif

struct ILevelRotationFile;

struct ILevelRotation
{
	virtual bool Load(ILevelRotationFile* file) = 0;
	virtual bool Save(ILevelRotationFile* file) = 0;
	virtual void Reset() = 0;
	virtual int  AddLevel(const char *level, const char *gamerules) = 0;
  virtual void AddSetting(int level, const char* setting) = 0;
	
	virtual bool First() = 0;
	virtual bool Advance() = 0;
	virtual const char *GetNextLevel() const = 0;
	virtual const char *GetNextGameRules() const = 0;
  virtual int  GetNextSettingsNum() const = 0;
  virtual const char *GetNextSetting(int idx) = 0; 
	virtual int GetLength() const = 0;

  virtual void SetRandom(bool rnd) = 0;
  virtual bool IsRandom()const = 0;

	virtual void ChangeLevel(IConsoleCmdArgs* pArgs = NULL) = 0;
};


struct ILevelInfo
{
public:
	typedef std::vector<string>	TStringVec;

	typedef struct
	{
		string	name;
		string	xmlFile;
		int			cgfCount;
	} TGameTypeInfo;

	virtual const char *GetName() const = 0;
	virtual const char *GetPath() const = 0;
	virtual const char *GetPaks() const = 0;
  virtual const char* GetDisplayName()const = 0;
	virtual const TStringVec& GetMusicLibs() const = 0;
	virtual int GetHeightmapSize() const = 0;

	virtual int GetGameTypeCount() const = 0;
	virtual const TGameTypeInfo *GetGameType(int gameType) const = 0;
	virtual bool SupportsGameType(const char *gameTypeName) const = 0;
	virtual const TGameTypeInfo *GetDefaultGameType() const = 0;
};


struct ILevel
{
	virtual void Release() = 0;
	virtual ILevelInfo *GetLevelInfo() = 0;
};

struct ILevelSystemListener
{
	virtual void OnLevelNotFound(const char *levelName) = 0;
	virtual void OnLoadingStart(ILevelInfo *pLevel) = 0;
	virtual void OnLoadingComplete(ILevel *pLevel) = 0;
	virtual void OnLoadingError(ILevelInfo *pLevel, const char *error) = 0;
	virtual void OnLoadingProgress(ILevelInfo *pLevel, int progressAmount) = 0;
};

struct ILevelSystem :
	public ILevelSystemListener
{
	virtual void Rescan(const char *levelsFolder = 0) = 0;
  virtual void LoadRotation() = 0;
	virtual int GetLevelCount() = 0;
	virtual ILevelInfo *GetLevelInfo(int level) = 0;
	virtual ILevelInfo *GetLevelInfo(const char *levelName) = 0;
	virtual uint64 CalcLevelChecksum(const char *levelName) = 0;

	virtual void AddListener(ILevelSystemListener *pListener) = 0;
	virtual void RemoveListener(ILevelSystemListener *pListener) = 0;

	virtual ILevel *GetCurrentLevel() const = 0;
	virtual ILevel *LoadLevel(const char *levelName) = 0;
	virtual bool IsLevelLoaded() = 0;
	virtual void PrepareNextLevel(const char *levelName) = 0;

	virtual ILevelRotation *GetLevelRotation() = 0;

	// Retrieve`s last level level loading time.
	virtual float GetLastLevelLoadTime() = 0;
};

#endif //__ILEVELSYSTEM_H__