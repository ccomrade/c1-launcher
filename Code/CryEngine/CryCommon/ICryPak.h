// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
// Interface to the crytek pack files management

#ifndef _ICRY_PAK_HDR_
#define _ICRY_PAK_HDR_

#if _MSC_VER > 1000
#pragma once
#endif

#include <smartptr.h>
#include <vector>					// STL vector<>

struct IResourceList; 

// this represents one particular archive filcare
struct ICryArchive: public _reference_target_t
{
	// Compression methods
	enum ECompressionMethods
	{
		METHOD_STORE    = 0,
		METHOD_COMPRESS = 8,
		METHOD_DEFLATE  = 8
	};

	// Compression levels
	enum ECompressionLevels
	{
		LEVEL_FASTEST = 0,
		LEVEL_FASTER  = 2,
		LEVEL_NORMAL  = 8,
		LEVEL_BETTER  = 8,
		LEVEL_BEST    = 9,
		LEVEL_DEFAULT = -1
	};

	enum EPakFlags
	{
		// support for absolute and other complex path specifications -
		// all paths will be treated relatively to the current directory (normally MasterCD)
		FLAGS_ABSOLUTE_PATHS = 1,

		// if this is set, the object will only understand relative to the zip file paths,
		// but this can give an opportunity to optimize for frequent quick accesses
		// FLAGS_SIMPLE_RELATIVE_PATHS and FLAGS_ABSOLUTE_PATHS are mutually exclusive
		FLAGS_RELATIVE_PATHS_ONLY = 1 << 1,

		// if this flag is set, the archive update/remove operations will not work
		// this is useful when you open a read-only or already opened for reading files.
		// If FLAGS_OPEN_READ_ONLY | FLAGS_SIMPLE_RELATIVE_PATHS are set, ICryPak 
		// will try to return an object optimized for memory, with long life cycle
		FLAGS_READ_ONLY = 1 << 2,

		// if this flag is set, FLAGS_OPEN_READ_ONLY
		// flags are also implied. The returned object will be optimized for quick access and
		// memory footprint
		FLAGS_OPTIMIZED_READ_ONLY = (1 << 3),

		// if this is set, the existing file (if any) will be overwritten
		FLAGS_CREATE_NEW = 1 << 4,

		// if this flag is set, and the file is opened for writing, and some files were updated
		// so that the archive is no more continuous, the archive will nevertheless NOT be compacted
		// upon closing the archive file. This can be faster if you open/close the archive for writing 
		// multiple times
		FLAGS_DONT_COMPACT   = 1 << 5,

		// if this is set, CryPak doesn't try to transform the path according to the MOD subdirectories
		FLAGS_IGNORE_MODS    = 1 << 6
	};

	virtual ~ICryArchive(){}

	// Summary:
	//   Adds a new file to the zip or update an existing one.
	// Description:
	//   Adds a new file to the zip or update an existing one
	//   adds a directory (creates several nested directories if needed)
	//   compression methods supported are METHOD_STORE == 0 (store) and
	//   METHOD_DEFLATE == METHOD_COMPRESS == 8 (deflate) , compression
	//   level is LEVEL_FASTEST == 0 till LEVEL_BEST == 9 or LEVEL_DEFAULT == -1
	//   for default (like in zlib)
	virtual int UpdateFile (const char* szRelativePath, void* pUncompressed, unsigned nSize, unsigned nCompressionMethod = 0, int nCompressionLevel = -1) = 0;


	// Summary:
	//   Adds a new file to the zip or update an existing one if it is not compressed - just stored  - start a big file
	//   ( name might be misleading as if nOverwriteSeekPos is used the update is not continuous )
	// Description:
	//   First step for the UpdateFileConinouseSegment
	virtual	int	StartContinuousFileUpdate( const char* szRelativePath, unsigned nSize ) = 0;

	// Summary:
	//   Adds a new file to the zip or update an existing's segment if it is not compressed - just stored 
	//   adds a directory (creates several nested directories if needed)
	//   ( name might be misleading as if nOverwriteSeekPos is used the update is not continuous )
	// Arguments:
	//   nOverwriteSeekPos - 0xffffffff means the seek pos should not be overwritten (then it needs UpdateFileCRC() to update CRC)
	virtual int UpdateFileContinuousSegment (const char* szRelativePath, unsigned nSize, void* pUncompressed, unsigned nSegmentSize, unsigned nOverwriteSeekPos=0xffffffff ) = 0;

	// Summary:
	//   needed to update CRC if UpdateFileContinuousSegment() was used with nOverwriteSeekPos
	virtual int UpdateFileCRC( const char* szRelativePath, const uint32 dwCRC ) = 0;

	// Summary:
	//   Deletes the file from the archive.
	virtual int RemoveFile (const char* szRelativePath) = 0;

	// Summary:
	//   Deletes the directory, with all its descendants (files and subdirs).
	virtual int RemoveDir (const char* szRelativePath) = 0;

	// Summary:
	//   Deletes all files and directories in the archive.
	virtual int RemoveAll() = 0;

	typedef void* Handle;

	// Summary:
	//   Finds the file; you don't have to close the returned handle.
	// Returns:
	//   NULL if the file doesn't exist
	virtual Handle FindFile (const char* szPath) = 0;
	// Summary:
	//   Get the file size (uncompressed).
	// Returns:
	//   The size of the file (unpacked) by the handle
	virtual unsigned GetFileSize (Handle) = 0;
	// Summary:
	//   Reads the file into the preallocated buffer
	// Note:
	//    Must be at least the size returned by GetFileSize.
	virtual int ReadFile (Handle, void* pBuffer) = 0;

	// Summary:
	//   Get the full path to the archive file.
	virtual const char* GetFullPath() const = 0;

	// Summary:
	//   Get the flags of this object.
	// Description:
	//   The possibles flags are defined in EPakFlags.
	// See Also:
	//   SetFlags, ResetFlags
	virtual unsigned GetFlags() const = 0;

	// Summary:
	//   Sets the flags of this object.
	// Description:
	//   The possibles flags are defined in EPakFlags.
	// See Also:
	//   GetFlags, ResetFlags
	virtual bool SetFlags(unsigned nFlagsToSet) = 0;

	// Summary:
	//   Resets the flags of this object.
	// See Also:
	//   SetFlags, GetFlags
	virtual bool ResetFlags(unsigned nFlagsToSet) = 0;

	// Summary:
	//   Determines if the archive is read only.
	// Returns:
	//   true if this archive is read-only
	inline bool IsReadOnly()const {return (GetFlags() & FLAGS_READ_ONLY) != 0;}

	// Summary:
	//   Get the class id.
	virtual unsigned GetClassId()const = 0; 

};

TYPEDEF_AUTOPTR(ICryArchive);

struct ICryPakFileAcesssSink
{
	// Arguments:
	//   in - 0 if asyncronous read
	//   szFullPath - must not be 0
	virtual void ReportFileOpen( FILE *in, const char *szFullPath )=0;
};


// this special flag is used for findfirst/findnext routines
// to mark the files that were actually found in Archive
enum {_A_IN_CRYPAK = 0x80000000};

// Summary
//   Interface to the Pak file system
// See Also
//   CryPak
struct ICryPak
{
	typedef uint64 FileTime;
	// Flags used in file path resolution rules
	enum EPathResolutionRules
	{
		// If used, the source path will be treated as the destination path
		// and no transformations will be done. Pass this flag when the path is to be the actual
		// path on the disk/in the packs and doesn't need adjustment (or after it has come through adjustments already)
		FLAGS_PATH_REAL = 1L << 16,

		// AdjustFileName will always copy the file path to the destination path:
		// regardless of the returned value, szDestpath can be used
		FLAGS_COPY_DEST_ALWAYS = 1L << 17,

		// Adds trailing slash to the path
		FLAGS_ADD_TRAILING_SLASH = 1L << 18,

		// Doesn't take mods into account
		FLAGS_IGNORE_MOD_DIRS    = 1L << 19,

		// Search only in MODs; if not found, return NULL; may not be used with FLAGS_IGNORE_MOD_DIRS
		FLAGS_ONLY_MOD_DIRS      = 1L << 20,

		// if this is set, AdjustFileName will not make relative paths into full paths
		FLAGS_NO_FULL_PATH	 = 1L << 21,

		// if this is set, AdjustFileName will not map the input path into the master folder (Ex: Shaders will not be converted to Game\Shaders)
		FLAGS_NO_MASTER_FOLDER_MAPPING = 1L << 22,

		// if this is set, AdjustFileName will not adjust path for writing files
		FLAGS_FOR_WRITING	 = 1L << 23
	};

	// Used for widening FOpen functionality. They're ignored for the regular File System files.
	enum EFOpenFlags
	{
		// If possible, will prevent the file from being read from memory.
		FOPEN_HINT_DIRECT_OPERATION = 1,
		// Will prevent a "missing file" warnings to be created.
		FOPEN_HINT_QUIET = 1 << 1
	};

	// Notification events for the pak system.
	enum ENotifyEvent
	{
		EVENT_BEGIN_LOADLEVEL,
		EVENT_END_LOADLEVEL,
	};

	//
	enum ERecordFileOpenList
	{
		RFOM_Disabled,							// file open are not recorded (fast, no extra memory)
		RFOM_EngineStartup,					// before a level is loaded
		RFOM_Level,									// during level loading till export2game -> resourcelist.txt, used to generate the list for level2level loading
		RFOM_NextLevel							// used for level2level loading
	};
	// the size of the buffer that receives the full path to the file
	enum {g_nMaxPath = 0x800};

	// given the source relative path, constructs the full path to the file according to the flags
	// returns the pointer to the constructed path (can be either szSourcePath, or szDestPath, or NULL in case of error
	virtual const char* AdjustFileName (const char *src, char dst[g_nMaxPath], unsigned nFlags,bool *bFoundInPak=NULL) = 0;

  virtual bool Init (const char *szBasePath)=0;
  virtual void Release()=0;
	// after this call, the pak file will be searched for files when they aren't on the OS file system
	// Arguments:
	//   pName - must not be 0
  virtual bool OpenPack(const char *pName, unsigned nFlags = FLAGS_PATH_REAL)=0;
	// after this call, the pak file will be searched for files when they aren't on the OS file system
	virtual bool OpenPack(const char* pBindingRoot, const char *pName, unsigned nFlags = FLAGS_PATH_REAL)=0;
	// after this call, the file will be unlocked and closed, and its contents won't be used to search for files
	virtual bool ClosePack(const char* pName, unsigned nFlags = FLAGS_PATH_REAL) = 0;
	// opens pack files by the path and wildcard
	virtual bool OpenPacks(const char *pWildcard, unsigned nFlags = FLAGS_PATH_REAL)=0;
	// opens pack files by the path and wildcard
	virtual bool OpenPacks(const char* pBindingRoot, const char *pWildcard, unsigned nFlags = FLAGS_PATH_REAL)=0;
	// closes pack files by the path and wildcard
	virtual bool ClosePacks(const char* pWildcard, unsigned nFlags = FLAGS_PATH_REAL) = 0;

	// adds a mod to the list
	virtual void AddMod(const char* szMod)=0;
	// removes a mod from the list
	virtual void RemoveMod(const char* szMod)=0;
	// Processes an alias command line containing multiple aliases.
	virtual void ParseAliases(const char* szCommandLine)=0;
	// adds or removes an alias from the list 
	virtual void SetAlias(const char* szName,const char* szAlias,bool bAdd)=0;
	// gets an alias from the list, if any exist. 
	// if bReturnSame==true, it will return the input name if an alias doesn't exist. Otherwise returns NULL
	virtual const char *GetAlias(const char* szName,bool bReturnSame=true)=0;

	struct PakInfo
	{
		struct Pak
		{
			const char* szFilePath;
			const char* szBindRoot;
			size_t nUsedMem;
		};
		// the number of packs in the arrPacks array
		unsigned numOpenPaks;
		// the packs
		Pak arrPaks[1];
	};
	// returns an array of PackInfo structures inside OpenPacks structure
	// you MUST call FreeOpenPackInfo
	virtual PakInfo* GetPakInfo() = 0;
	virtual void FreePakInfo (PakInfo*) = 0;

	// Open file handle, file can be on disk or in PAK file.
	// Possible mode is r,b,x
	// ex: FILE *f = FOpen( "test.txt","rbx" );
	// mode x is a direct access mode, when used file reads will go directly into the low level file system without any internal data caching.
	// Text mode is not supported for files in PAKs.
	// for nFlags @see ICryPak::EFOpenFlags
  virtual FILE *FOpen(const char *pName, const char *mode, unsigned nFlags = 0)=0;
	virtual FILE *FOpen(const char *pName, const char *mode,char *szFileGamePath,int nLen)=0;
	
	// Read raw data from file, no endian conversion.
	virtual size_t FReadRaw(void *data, size_t length, size_t elems, FILE *handle) = 0;

	// Read all file contents into the provided memory, nSizeOfFile must be the same as returned by GetFileSize(handle)
	// Current seek pointer is ignored and reseted to 0.
	// no endian conversion.
	virtual size_t FReadRawAll(void *data, size_t nFileSize, FILE *handle) = 0;

	// Get pointer to the internally cached, loaded data of the file.
	// WARNING! requesting cached file data of the another file will invalidate previously retrieved pointer.
	virtual void* FGetCachedFileData( FILE *handle,size_t &nFileSize ) = 0;

	// Write file data, cannot be used for writing into the PAK.
	// Use ICryArchive interface for writing into the pak files.
  virtual size_t FWrite(const void *data, size_t length, size_t elems, FILE *handle)=0;

#ifdef SP_DEMO
  virtual size_t FSeek(FILE *handle, long seek, int mode)=0;
  virtual long FTell(FILE *handle)=0;
  virtual int FClose(FILE *handle)=0;
  virtual int FEof(FILE *handle)=0;
	virtual int FError(FILE *handle)=0;
	virtual int FGetErrno()=0;
  virtual int FFlush(FILE *handle)=0;
#endif
  //virtual int FScanf(FILE *, const char *, ...) SCANF_PARAMS(2, 3) =0;
  virtual int FPrintf(FILE *handle, const char *format, ...) PRINTF_PARAMS(3, 4)=0;
  virtual char *FGets(char *, int, FILE *)=0;
  virtual int Getc(FILE *)=0;
  virtual size_t FGetSize(FILE* f)=0;
	virtual int Ungetc(int c, FILE *)=0;
	virtual bool IsInPak(FILE *handle)=0;
	virtual bool RemoveFile(const char* pName) = 0; // remove file from FS (if supported)
	virtual bool RemoveDir(const char* pName, bool bRecurse) = 0;  // remove directory from FS (if supported)
	virtual bool IsAbsPath(const char* pPath) = 0; // determines if pPath is an absolute or relative path
#ifndef SP_DEMO
	virtual size_t FSeek(FILE *handle, long seek, int mode)=0;
	virtual long FTell(FILE *handle)=0;
	virtual int FClose(FILE *handle)=0;
	virtual int FEof(FILE *handle)=0;
	virtual int FError(FILE *handle)=0;
	virtual int FGetErrno()=0;
	virtual int FFlush(FILE *handle)=0;
#endif

	//! Return pointer to pool if available
	virtual void * PoolMalloc(size_t size) = 0;
	//! Free pool
	virtual void PoolFree(void * p) = 0;
	// Type-safe endian conversion read.
	template<class T>
  size_t FRead(T *data, size_t elems, FILE *handle, bool bSwap = true)
	{
		size_t count = FReadRaw(data, sizeof(T), elems, handle);
		if (bSwap)
			SwapEndian(data, count);
		return count;
	}
	// Type-independent Write.
	template<class T>
	void FWrite(T *data, size_t elems, FILE *handle)
	{
		FWrite((void*)data, sizeof(T), elems, handle);
	}

	// Arguments:
	//   nFlags is a combination of EPathResolutionRules flags.
  virtual intptr_t FindFirst( const char *pDir,struct _finddata_t *fd,unsigned int nFlags=0 )=0;
  virtual int FindNext(intptr_t handle, struct _finddata_t *fd)=0;
  virtual int FindClose(intptr_t handle)=0;
//	virtual bool IsOutOfDate(const char * szCompiledName, const char * szMasterFile)=0;
	//returns file modification time
	virtual FileTime GetModificationTime(FILE*f)=0;

	// Description:
	//    Checks if specified file exist in filesystem.
	virtual bool IsFileExist( const char *sFilename ) = 0;

	// creates a directory
	virtual bool MakeDir (const char* szPath) = 0;
		
	// open the physical archive file - creates if it doesn't exist
	// returns NULL if it's invalid or can't open the file
	// nFlags can have the following flag set:
	//   FLAGS_ABSOLUTE_PATHS	
	virtual ICryArchive* OpenArchive (const char* szPath, unsigned nFlags = FLAGS_PATH_REAL) = 0;

	// returns the path to the archive in which the file was opened
	// returns NULL if the file is a physical file, and "" if the path to archive is unknown (shouldn't ever happen)
	virtual const char* GetFileArchivePath (FILE* f) = 0;

	// compresses the raw data into raw data. The buffer for compressed data itself with the heap passed. Uses method 8 (deflate)
	// returns one of the Z_* errors (Z_OK upon success)
	// MT-safe
	virtual int RawCompress (const void* pUncompressed, unsigned long* pDestSize, void* pCompressed, unsigned long nSrcSize, int nLevel = -1) = 0;

	// Uncompresses raw (without wrapping) data that is compressed with method 8 (deflated) in the Zip file
	// returns one of the Z_* errors (Z_OK upon success)
	// This function just mimics the standard uncompress (with modification taken from unzReadCurrentFile)
	// with 2 differences: there are no 16-bit checks, and 
	// it initializes the inflation to start without waiting for compression method byte, as this is the 
	// way it's stored into zip file
	virtual int RawUncompress (void* pUncompressed, unsigned long* pDestSize, const void* pCompressed, unsigned long nSrcSize) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Files collector.
	//////////////////////////////////////////////////////////////////////////

	// Turn on/off recording of filenames of opened files.
	virtual void RecordFileOpen( const ERecordFileOpenList eList ) = 0;
	// Record this file if recording is enabled.
	// Arguments:
	//   in - 0 if asynchronous read
	virtual void RecordFile( FILE *in, const char *szFilename ) = 0;
	// Summary:
	//    Get resource list of all recorded files, the next level, ...
	virtual IResourceList* GetRecorderdResourceList( const ERecordFileOpenList eList ) = 0;
	// get the current mode, can be set by RecordFileOpen()
	virtual ERecordFileOpenList GetRecordFileOpenList() = 0;

	// Summary:
	//    Notify Pak system of important events in the game, used for logging and time recording.
	virtual void Notify( ENotifyEvent event ) = 0;

	// computes CRC (zip compatible) for a file
	// useful if a huge uncompressed file is generation in non continuous way
	// good for big files - low memory overhead (1MB)
	// Arguments:
	//   szPath - must not be 0
	// Returns:
	//   error code
	virtual uint32 ComputeCRC( const char* szPath )=0;

	// computes MD5 checksum for a file
	// good for big files - low memory overhead (1MB)
	// Arguments:
	//   szPath - must not be 0
	//	 md5 - destination array of unsigned char [16]
	// Returns:
	//   true if success, false on failure
	virtual bool ComputeMD5(const char* szPath, unsigned char* md5) = 0;

	// useful for gathering file access statistics, assert if it was inserted already but then it does not become insersted
	// Arguments:
	//   pSink - must not be 0
	virtual void RegisterFileAccessSink( ICryPakFileAcesssSink *pSink )=0;
	// assert if it was not registered already
	// Arguments:
	//   pSink - must not be 0
	virtual void UnregisterFileAccessSink( ICryPakFileAcesssSink *pSink )=0;
	
	// LvlRes can be enabled by command line - then asset resource recording is enabled
	// and some modules can do more asset tracking work based on that
	// Returns:
	//   true=on, false=off
	virtual bool GetLvlResStatus() const=0;

	//Returns path to current active mod or NULL.
	virtual const char* GetModDir() const = 0;
};

// The IResourceList provides an access to the collection of the resource`s file names.
// Client can add a new file names to the resource list and check if resource already in the list.
struct IResourceList : public _reference_target_t
{
	// Description:
	//    Adds a new resource to the list.
	virtual void Add( const char *sResourceFile ) = 0;

	// Description:
	//    Clears resource list.
	virtual void Clear() = 0;

	// Description:
	//    Checks if specified resource exist in the list.
	virtual bool IsExist( const char *sResourceFile ) = 0;

	// Description:
	//    Loads a resource list from the resource list file.
	virtual void Load( const char *sResourceListFilename ) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Enumeration.
	//////////////////////////////////////////////////////////////////////////

	// Description:
	//    Returns the file name of the first resource or NULL if resource list is empty.
	virtual const char* GetFirst() = 0;
	// Description:
	//    Returns the file name of the next resource or NULL if reached the end.
	//    Client must call GetFirst before calling GetNext.
	virtual const char* GetNext() = 0;
};

//////////////////////////////////////////////////////////////////////////
// Include File helpers.
//////////////////////////////////////////////////////////////////////////
#include "CryPath.h"
#include "CryFile.h"

//////////////////////////////////////////////////////////////////////




































//! Everybody should use fxopen instead of fopen
//! so it will work both on PC and XBox
inline FILE * fxopen(const char *file, const char *mode)
{
	//SetFileAttributes(file,FILE_ATTRIBUTE_ARCHIVE);
	//	FILE *pFile = fopen("C:/MasterCD/usedfiles.txt","a");
	//	if (pFile)
	//	{
	//		fprintf(pFile,"%s\n",file);
	//		fclose(pFile);
	//	}


















#if defined(LINUX)
	char adjustedName[MAX_PATH];
	bool createFlag = false;
	if (strchr(mode, 'w') || strchr(mode, 'a'))
		createFlag = true;
	GetFilenameNoCase(file, adjustedName, createFlag);
	return fopen(adjustedName, mode);
#else
	// This is on windows.
	if (gEnv && gEnv->pCryPak)
	{
		bool bWriteAccess = false;
		for (const char *s = mode; *s; s++) { if (*s == 'w' || *s == 'W' || *s == 'a' || *s == 'A' || *s == '+') { bWriteAccess = true; break; }; }
		int nAdjustFlags = ICryPak::FLAGS_NO_MASTER_FOLDER_MAPPING;
		if (bWriteAccess)
			nAdjustFlags |= ICryPak::FLAGS_FOR_WRITING;
		char path[_MAX_PATH];
		const char* szAdjustedPath = gEnv->pCryPak->AdjustFileName(file,path,nAdjustFlags);
		return fopen(szAdjustedPath,mode);
	}
	else
		return 0;
#endif //LINUX


}
//////////////////////////////////////////////////////////////////////////

#endif
