// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

#include <cstddef>
#include <cstdio>

struct ICryArchive;
struct IResourceList;

struct ICryPakFileAcesssSink
{
	virtual void ReportFileOpen(std::FILE* file, const char* path) = 0;
};

/**
 * Interface to the Pak file system.
 */
struct ICryPak
{
	typedef unsigned __int64 FileTime;

	enum EPathResolutionRules
	{
		FLAGS_PATH_REAL = (1 << 16),
		FLAGS_COPY_DEST_ALWAYS = (1 << 17),
		FLAGS_ADD_TRAILING_SLASH = (1 << 18),
		FLAGS_IGNORE_MOD_DIRS = (1 << 19),
		FLAGS_ONLY_MOD_DIRS = (1 << 20),
		FLAGS_NO_FULL_PATH = (1 << 21),
		FLAGS_NO_MASTER_FOLDER_MAPPING = (1 << 22),
		FLAGS_FOR_WRITING = (1L << 23),
	};

	enum EFOpenFlags
	{
		FOPEN_HINT_DIRECT_OPERATION = (1 << 0),
		FOPEN_HINT_QUIET = (1 << 1),
	};

	enum ENotifyEvent
	{
		EVENT_BEGIN_LOADLEVEL,
		EVENT_END_LOADLEVEL,
	};

	enum ERecordFileOpenList
	{
		RFOM_Disabled,
		RFOM_EngineStartup,
		RFOM_Level,
		RFOM_NextLevel,
	};

	enum { MAX_PATH = 0x800 };

	virtual const char* AdjustFileName(const char* src, char dst[MAX_PATH], unsigned int flags, bool* foundInPak = NULL) = 0;

	virtual bool Init(const char* basePath) = 0;
	virtual void Release() = 0;

	virtual bool OpenPack(const char* name, unsigned int flags = FLAGS_PATH_REAL) = 0;
	virtual bool OpenPack(const char* bindingRoot, const char* name, unsigned int flags = FLAGS_PATH_REAL) = 0;
	virtual bool ClosePack(const char* name, unsigned int flags = FLAGS_PATH_REAL) = 0;
	virtual bool OpenPacks(const char* wildcard, unsigned int flags = FLAGS_PATH_REAL) = 0;
	virtual bool OpenPacks(const char* bindingRoot, const char* wildcard, unsigned int flags = FLAGS_PATH_REAL) = 0;
	virtual bool ClosePacks(const char* wildcard, unsigned int flags = FLAGS_PATH_REAL) = 0;

	virtual void AddMod(const char* mod) = 0;
	virtual void RemoveMod(const char* mod) = 0;

	virtual void ParseAliases(const char* commandLine) = 0;
	virtual void SetAlias(const char* name, const char* alias, bool add) = 0;
	virtual const char* GetAlias(const char* name, bool returnNameIfMissing = true) = 0;

	struct PakInfo
	{
		struct Pak
		{
			const char* path;
			const char* bindingRoot;
			std::size_t usedMemory;
		};

		unsigned int pakCount;
		Pak paks[1];
	};

	virtual PakInfo* GetPakInfo() = 0;
	virtual void FreePakInfo(PakInfo* pakInfo) = 0;

	virtual std::FILE* FOpen(const char* name, const char* mode, unsigned int flags = 0) = 0;
	virtual std::FILE* FOpen(const char* name, const char* mode, char* fileGamePath, int length) = 0;

	virtual std::size_t FReadRaw(void* data, std::size_t length, std::size_t elementCount, std::FILE* file) = 0;
	virtual std::size_t FReadRawAll(void* data, std::size_t fileSize, std::FILE* file) = 0;

	virtual void* FGetCachedFileData(std::FILE* file, std::size_t& fileSize) = 0;

	virtual std::size_t FWrite(const void* data, std::size_t length, std::size_t elementCount, std::FILE* file) = 0;

	virtual int FPrintf(std::FILE* file, const char* format, ...) = 0;
	virtual char* FGets(char* buffer, int bufferSize, std::FILE* file) = 0;
	virtual int Getc(std::FILE* file) = 0;
	virtual std::size_t FGetSize(std::FILE* file) = 0;
	virtual int Ungetc(int ch, std::FILE* file) = 0;
	virtual bool IsInPak(std::FILE* file) = 0;
	virtual bool RemoveFile(const char* name) = 0;
	virtual bool RemoveDir(const char* name, bool recurse) = 0;
	virtual bool IsAbsPath(const char* path) = 0;
	virtual std::size_t FSeek(std::FILE* file, long seek, int mode) = 0;
	virtual long FTell(std::FILE* file) = 0;
	virtual int FClose(std::FILE* file) = 0;
	virtual int FEof(std::FILE* file) = 0;
	virtual int FError(std::FILE* file) = 0;
	virtual int FGetErrno() = 0;
	virtual int FFlush(std::FILE* file) = 0;

	virtual void* PoolMalloc(std::size_t size) = 0;
	virtual void PoolFree(void* pool) = 0;

	virtual std::size_t FindFirst(const char* dir, struct _finddata_t* fd, unsigned int flags = 0) = 0;
	virtual int FindNext(std::size_t handle, struct _finddata_t* fd) = 0;
	virtual int FindClose(std::size_t handle) = 0;

	virtual FileTime GetModificationTime(std::FILE* file) = 0;

	virtual bool IsFileExist(const char* name) = 0;

	virtual bool MakeDir(const char* path) = 0;

	virtual ICryArchive* OpenArchive(const char* path, unsigned int flags = FLAGS_PATH_REAL) = 0;

	virtual const char* GetFileArchivePath(std::FILE* file) = 0;

	virtual int RawCompress(const void* uncompressed, unsigned long* dstSize, void* compressed, unsigned long srcSize, int level = -1) = 0;
	virtual int RawUncompress(void* uncompressed, unsigned long* dstSize, const void* compressed, unsigned long srcSize) = 0;

	virtual void RecordFileOpen(const ERecordFileOpenList list) = 0;
	virtual void RecordFile(std::FILE* file, const char* name) = 0;
	virtual IResourceList* GetRecorderdResourceList(const ERecordFileOpenList list) = 0;
	virtual ERecordFileOpenList GetRecordFileOpenList() = 0;

	virtual void Notify(ENotifyEvent event) = 0;

	virtual unsigned long ComputeCRC(const char* path) = 0;
	virtual bool ComputeMD5(const char* path, unsigned char* md5) = 0;

	virtual void RegisterFileAccessSink(ICryPakFileAcesssSink* pSink) = 0;
	virtual void UnregisterFileAccessSink(ICryPakFileAcesssSink* pSink) = 0;

	virtual bool GetLvlResStatus() const = 0;

	virtual const char* GetModDir() const = 0;
};
