////////////////////////////////////////////////////////////////////////////
//
//  Crytek Engine Source File.
//  Copyright (C), Crytek Studios, 2001-2005.
// -------------------------------------------------------------------------
//  File name:   Linux_Win32Wrapper.h
//  Version:     v1.00
//  Created:     23/6/2005 by Timur.
//  Compilers:   Visual Studio.NET 2003
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __Linux_Win32Wrapper_h__
#define __Linux_Win32Wrapper_h__
#pragma once

//#include <asm/msr.h>
#include <sys/types.h>
//#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
//#include <aio.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
//#include <sys/time.h>
#include <errno.h>
//#include <dirent.h>
//#include <fnmatch.h>
//#include <termios.h>
//#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <vector>
#include <string>

/* Memory block identification */
#define _FREE_BLOCK      0
#define _NORMAL_BLOCK    1
#define _CRT_BLOCK       2
#define _IGNORE_BLOCK    3
#define _CLIENT_BLOCK    4
#define _MAX_BLOCKS      5

typedef void *HMODULE;

#if 0
//for compatibility reason we got to create a class which actually contains an int rather than a void* and make sure it does not get mistreated
template <class T, T U>//U is default type for invalid handle value, T the encapsulated handle type to be used instead of void* (as under windows and never linux)
class CHandle
{
public:
	typedef T			HandleType;
	typedef void* PointerType;	//for compatibility reason to encapsulate a void* as an int

	static const HandleType sciInvalidHandleValue = U;

	CHandle(const CHandle<T,U>& cHandle) : m_Value(cHandle.m_Value){}
	CHandle(const HandleType cHandle = U) : m_Value(cHandle){}
	//CHandle(const PointerType cpHandle) : m_Value(reinterpret_cast<HandleType>(cpHandle)){}
	//CHandle(INVALID_HANDLE_VALUE_ENUM) : m_Value(U){}//to be able to use a common value for all InvalidHandle - types

	operator HandleType(){return m_Value;}
	bool operator!() const{return m_Value == sciInvalidHandleValue;}
	const CHandle& operator =(const CHandle& crHandle){m_Value = crHandle.m_Value;return *this;}
	const CHandle& operator =(const PointerType cpHandle){m_Value = reinterpret_cast<HandleType>(cpHandle);return *this;}
	const bool operator ==(const CHandle& crHandle)		const{return m_Value == crHandle.m_Value;}
	const bool operator ==(const HandleType cHandle)	const{return m_Value == cHandle;}
	//const bool operator ==(const PointerType cpHandle)const{return m_Value == reinterpret_cast<HandleType>(cpHandle);}
	const bool operator !=(const HandleType cHandle)	const{return m_Value != cHandle;}
	const bool operator !=(const CHandle& crHandle)		const{return m_Value != crHandle.m_Value;}
	//const bool operator !=(const PointerType cpHandle)const{return m_Value != reinterpret_cast<HandleType>(cpHandle);}
	const bool operator <	(const CHandle& crHandle)		const{return m_Value < crHandle.m_Value;}
	HandleType Handle()const{return m_Value;}

private:
	HandleType m_Value;	//the actual value, remember that file descriptors are ints under linux

	typedef void	ReferenceType;//for compatibility reason to encapsulate a void* as an int
	//forbid these function which would actually not work on an int
	PointerType operator->();
	PointerType operator->() const;
	ReferenceType operator*();
	ReferenceType operator*() const;
	operator PointerType();
};

typedef CHandle<INT_PTR, (INT_PTR)0> HANDLE;

typedef HANDLE EVENT_HANDLE;
typedef HANDLE THREAD_HANDLE;

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)
#endif

//-------------------------------------socket stuff------------------------------------------
// These are already in LinuxSpecific.h ...
//#define SOCKET int
//#define INVALID_SOCKET (-1)
//#define SOCKET_ERROR (-1)

//-------------------------------------end socket stuff------------------------------------------

#define __TIMESTAMP__ __DATE__" "__TIME__

// stdlib.h stuff
#define _MAX_DRIVE  3   // max. length of drive component
#define _MAX_DIR    256 // max. length of path component
#define _MAX_FNAME  256 // max. length of file name component
#define _MAX_EXT    256 // max. length of extension component

// fcntl.h
#define _O_RDONLY       0x0000  /* open for reading only */
#define _O_WRONLY       0x0001  /* open for writing only */
#define _O_RDWR         0x0002  /* open for reading and writing */
#define _O_APPEND       0x0008  /* writes done at eof */
#define _O_CREAT        0x0100  /* create and open file */
#define _O_TRUNC        0x0200  /* open and truncate */
#define _O_EXCL         0x0400  /* open only if file doesn't already exist */
#define _O_TEXT         0x4000  /* file mode is text (translated) */
#define _O_BINARY       0x8000  /* file mode is binary (untranslated) */
#define _O_RAW  _O_BINARY
#define _O_NOINHERIT    0x0080  /* child process doesn't inherit file */
#define _O_TEMPORARY    0x0040  /* temporary file bit */
#define _O_SHORT_LIVED  0x1000  /* temporary storage file, try not to flush */
#define _O_SEQUENTIAL   0x0020  /* file access is primarily sequential */
#define _O_RANDOM       0x0010  /* file access is primarily random */


//////////////////////////////////////////////////////////////////////////
// io.h stuff
#if !defined(PS3)
extern int errno;
#endif
typedef unsigned int _fsize_t;

struct _OVERLAPPED;
typedef _OVERLAPPED* LPOVERLAPPED;

typedef void (*LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, struct _OVERLAPPED *lpOverlapped);

typedef struct tagRECT
{
	LONG    left;
	LONG    top;
	LONG    right;
	LONG    bottom;
} RECT, *PRECT;

typedef struct tagPOINT
{
  LONG  x;
  LONG  y;
} POINT, *PPOINT;

#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME
{
	DWORD dwLowDateTime;
	DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#endif

typedef union _ULARGE_INTEGER
{
	struct
	{
		DWORD LowPart;
		DWORD HighPart;
	};
	unsigned long long QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER *PULARGE_INTEGER;

#ifdef __cplusplus
inline LONG CompareFileTime(const FILETIME *lpFileTime1, const FILETIME *lpFileTime2)
#else
static LONG CompareFileTime(const FILETIME *lpFileTime1, const FILETIME *lpFileTime2)
#endif
{
	ULARGE_INTEGER u1, u2;
	memcpy(&u1, lpFileTime1, sizeof u1);
	memcpy(&u2, lpFileTime2, sizeof u2);
	if(u1.QuadPart < u2.QuadPart)
		return -1;
	else
		if(u1.QuadPart > u2.QuadPart)
			return 1;
	return 0;
}

typedef struct _SYSTEMTIME{
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _TIME_FIELDS
{
	short Year;
	short Month;
	short Day;
	short Hour;
	short Minute;
	short Second;
	short Milliseconds;
	short Weekday;
} TIME_FIELDS, *PTIME_FIELDS;

#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12

inline void ZeroMemory(void *pPtr, int nSize)
{
  memset(pPtr, 0, nSize);
}

inline BOOL InflateRect(RECT *pRect, int dx, int dy)
{
  pRect->left -= dx;
  pRect->right += dx;
  pRect->top -= dy;
  pRect->bottom += dy;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
extern BOOL SystemTimeToFileTime( const SYSTEMTIME *syst, LPFILETIME ft );
//Win32API function declarations actually used 
extern bool IsBadReadPtr(void* ptr, unsigned int size );

// Defined in the launcher.
DLL_IMPORT void OutputDebugString(const char *);
DLL_IMPORT void DebugBreak();

/*
//critical section stuff
#define pthread_attr_default NULL

typedef pthread_mutex_t CRITICAL_SECTION;
#ifdef __cplusplus
inline void InitializeCriticalSection(CRITICAL_SECTION *lpCriticalSection)
{
pthread_mutexattr_t pthread_mutexattr_def;
pthread_mutexattr_settype(&pthread_mutexattr_def, PTHREAD_MUTEX_RECURSIVE_NP);
pthread_mutex_init(lpCriticalSection, &pthread_mutexattr_def);
}
inline void EnterCriticalSection(CRITICAL_SECTION *lpCriticalSection){pthread_mutex_lock(lpCriticalSection);}
inline void LeaveCriticalSection(CRITICAL_SECTION *lpCriticalSection){pthread_mutex_unlock(lpCriticalSection);}
inline void DeleteCriticalSection(CRITICAL_SECTION *lpCriticalSection){}
#else
static void InitializeCriticalSection(CRITICAL_SECTION *lpCriticalSection)
{
pthread_mutexattr_t pthread_mutexattr_def;
pthread_mutexattr_settype(&pthread_mutexattr_def, PTHREAD_MUTEX_RECURSIVE_NP);
pthread_mutex_init(lpCriticalSection, &pthread_mutexattr_def);
}
static void EnterCriticalSection(CRITICAL_SECTION *lpCriticalSection){pthread_mutex_lock(lpCriticalSection);}
static void LeaveCriticalSection(CRITICAL_SECTION *lpCriticalSection){pthread_mutex_unlock(lpCriticalSection);}
static void DeleteCriticalSection(CRITICAL_SECTION *lpCriticalSection){}
#endif
*/

extern bool QueryPerformanceCounter(LARGE_INTEGER *counter);
extern bool QueryPerformanceFrequency(LARGE_INTEGER *frequency);

#define GetTickCount CryGetTicks
#define GetCurrentTime CryGetTicks

#define IGNORE              0       // Ignore signal
#define INFINITE            0xFFFFFFFF  // Infinite timeout

//begin--------------------------------findfirst/-next declaration/implementation----------------------------------------------------

#ifdef __cplusplus

//////////////////////////////////////////////////////////////////////////

typedef int64 __time64_t;     /* 64-bit time value */

typedef struct __finddata64_t
{
	//!< atributes set by find request
	unsigned    int attrib;			//!< attributes, only directory and readonly flag actually set
	__time64_t	time_create;		//!< creation time, cannot parse under linux, last modification time is used instead (game does nowhere makes decision based on this values)
	__time64_t	time_access;		//!< last access time
	__time64_t	time_write;			//!< last modification time
	__time64_t	size;						//!< file size (for a directory it will be the block size)
	char        name[256];			//!< file/directory name

private:
	int									m_LastIndex;					//!< last index for findnext
	char								m_DirectoryName[260];			//!< directory name, needed when getting file attributes on the fly
	char								m_ToMatch[260];						//!< pattern to match with
	DIR*								m_Dir;									//!< directory handle
	std::vector<string>	m_Entries;						//!< all file entries in the current directories
public:

	inline __finddata64_t():
	  attrib(0), time_create(0), time_access(0), time_write(0),
		size(0), m_LastIndex(-1), m_Dir(NULL)
	{
		memset(name, '0', 256);	
	}
	~__finddata64_t();
	
	//!< copies and retrieves the data for an actual match (to not waste any effort retrioeving data for unused files)
	void CopyFoundData(const char * rMatchedFileName);

public:
	//!< global _findfirst64 function using struct above, can't be a member function due to required semantic match
	friend intptr_t _findfirst64(const char *pFileName, __finddata64_t *pFindData);
	//!< global _findnext64 function using struct above, can't be a member function due to required semantic match
	friend int _findnext64(intptr_t last, __finddata64_t *pFindData);
}__finddata64_t;

typedef struct _finddata_t : public __finddata64_t
{}_finddata_t;//!< need inheritance since in many places it get used as struct _finddata_t
extern int _findnext64(intptr_t last, __finddata64_t *pFindData);
extern intptr_t _findfirst64(const char *pFileName, __finddata64_t *pFindData);
#endif
//end--------------------------------findfirst/-next declaration/implementation----------------------------------------------------

extern BOOL GetUserName(LPSTR lpBuffer, LPDWORD nSize);

//error code stuff
//not thread specific, just a coarse implementation for the main thread
inline DWORD GetLastError() { return errno; }
inline void SetLastError( DWORD dwErrCode ) { errno = dwErrCode; }

//////////////////////////////////////////////////////////////////////////
#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define FILE_SHARE_READ						0x00000001
#define FILE_SHARE_WRITE					0x00000002
#define OPEN_EXISTING							3
#define FILE_FLAG_OVERLAPPED			0x40000000
#define INVALID_FILE_SIZE					((DWORD)0xFFFFFFFFl)
#define FILE_BEGIN								0
#define FILE_CURRENT							1
#define FILE_END									2
#define ERROR_NO_SYSTEM_RESOURCES 1450L
#define ERROR_INVALID_USER_BUFFER	1784L
#define ERROR_NOT_ENOUGH_MEMORY   8L
#define ERROR_PATH_NOT_FOUND      3L
#define FILE_FLAG_SEQUENTIAL_SCAN 0x08000000

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateFile(
												 const char* lpFileName,
												 DWORD dwDesiredAccess,
												 DWORD dwShareMode,
												 void* lpSecurityAttributes,
												 DWORD dwCreationDisposition,
												 DWORD dwFlagsAndAttributes,
												 HANDLE hTemplateFile
												 );

//////////////////////////////////////////////////////////////////////////
extern DWORD GetFileAttributes(LPCSTR lpFileName);

//////////////////////////////////////////////////////////////////////////
extern BOOL SetFileAttributes(LPCSTR,DWORD attributes);


//////////////////////////////////////////////////////////////////////////
extern BOOL SetFileTime(
						HANDLE hFile,
						const FILETIME *lpCreationTime,
						const FILETIME *lpLastAccessTime,
						const FILETIME *lpLastWriteTime );
//////////////////////////////////////////////////////////////////////////
extern const uint64 GetFileModifTime(FILE* hFile);

//////////////////////////////////////////////////////////////////////////
extern DWORD GetFileSize(HANDLE hFile,DWORD *lpFileSizeHigh );

//////////////////////////////////////////////////////////////////////////
extern BOOL CloseHandle( HANDLE hObject );

//////////////////////////////////////////////////////////////////////////
extern BOOL CancelIo( HANDLE hFile );
//////////////////////////////////////////////////////////////////////////
extern HRESULT GetOverlappedResult( HANDLE hFile,void* lpOverlapped,LPDWORD lpNumberOfBytesTransferred, BOOL bWait );
//////////////////////////////////////////////////////////////////////////
extern BOOL ReadFile(
							HANDLE hFile,
							LPVOID lpBuffer,
							DWORD nNumberOfBytesToRead,
							LPDWORD lpNumberOfBytesRead,
							LPOVERLAPPED lpOverlapped
							);

//////////////////////////////////////////////////////////////////////////
extern BOOL ReadFileEx(
											 HANDLE hFile,
											 LPVOID lpBuffer,
											 DWORD nNumberOfBytesToRead,
											 LPOVERLAPPED lpOverlapped,
											 LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
											 );

//////////////////////////////////////////////////////////////////////////
extern DWORD SetFilePointer(
										 HANDLE hFile,
										 LONG lDistanceToMove,
										 PLONG lpDistanceToMoveHigh,
										 DWORD dwMoveMethod
										 );

//////////////////////////////////////////////////////////////////////////
extern DWORD GetCurrentThreadId();

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateEvent(
						 LPSECURITY_ATTRIBUTES lpEventAttributes,
						 BOOL bManualReset,
						 BOOL bInitialState,
						 LPCSTR lpName
						 );

//////////////////////////////////////////////////////////////////////////
extern DWORD Sleep( DWORD dwMilliseconds );

//////////////////////////////////////////////////////////////////////////
extern DWORD SleepEx( DWORD dwMilliseconds,BOOL bAlertable );

//////////////////////////////////////////////////////////////////////////
extern DWORD WaitForSingleObjectEx(
											HANDLE hHandle,
											DWORD dwMilliseconds,
											BOOL bAlertable );

//////////////////////////////////////////////////////////////////////////
extern DWORD WaitForMultipleObjectsEx(
												 DWORD nCount,
												 const HANDLE *lpHandles,
												 BOOL bWaitAll,
												 DWORD dwMilliseconds,
												 BOOL bAlertable );

//////////////////////////////////////////////////////////////////////////
extern DWORD WaitForSingleObject( HANDLE hHandle,DWORD dwMilliseconds );

//////////////////////////////////////////////////////////////////////////
extern BOOL SetEvent( HANDLE hEvent );

//////////////////////////////////////////////////////////////////////////
extern BOOL ResetEvent( HANDLE hEvent );

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateMutex(
						 LPSECURITY_ATTRIBUTES lpMutexAttributes,
						 BOOL bInitialOwner,
						 LPCSTR lpName
						 );

//////////////////////////////////////////////////////////////////////////
extern BOOL ReleaseMutex( HANDLE hMutex );

//////////////////////////////////////////////////////////////////////////
typedef DWORD (*PTHREAD_START_ROUTINE)( LPVOID lpThreadParameter );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

//////////////////////////////////////////////////////////////////////////
extern HANDLE CreateThread(
						 LPSECURITY_ATTRIBUTES lpThreadAttributes,
						 SIZE_T dwStackSize,
						 LPTHREAD_START_ROUTINE lpStartAddress,
						 LPVOID lpParameter,
						 DWORD dwCreationFlags,
						 LPDWORD lpThreadId
						 );

extern BOOL DeleteFile(LPCSTR lpFileName);
extern BOOL MoveFile( LPCSTR lpExistingFileName,LPCSTR lpNewFileName );
extern BOOL RemoveDirectory(LPCSTR lpPathName);
extern DWORD GetCurrentDirectory( DWORD nBufferLength, char* lpBuffer );
//extern BOOL SetCurrentDirectory(LPCSTR lpPathName);

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus

//helper function
extern const bool GetFilenameNoCase(const char *file, char*, const bool cCreateNew = false);
extern void adaptFilenameToLinux( char *rAdjustedFilename);
extern const int comparePathNames(const char* cpFirst, const char* cpSecond, const unsigned int len);//returns 0 if identical
extern void replaceDoublePathFilename(char *szFileName);//removes "\.\" to "\" and "/./" to "/"


//////////////////////////////////////////////////////////////////////////
inline LONG _InterlockedCompareExchange( LONG volatile* Destination,LONG Exchange,LONG Comperand )
{
	LONG prev = *Destination;
	if (*Destination == Comperand)
		*Destination = Exchange;
	return prev;
}

//////////////////////////////////////////////////////////////////////////
inline LONG _InterlockedExchangeAdd( LONG volatile* Addend,LONG Value )
{
	LONG prev = *Addend;
	*Addend += Value;
	return prev;
}

//////////////////////////////////////////////////////////////////////////
extern char *_fullpath( char *absPath,const char *relPath,size_t maxLength );
extern intptr_t _findfirst64( const char *filespec,struct __finddata64_t *fileinfo );
extern int _findnext64( intptr_t handle,struct __finddata64_t *fileinfo );
extern int _findclose( intptr_t handle );

//////////////////////////////////////////////////////////////////////////
extern int _mkdir( const char *dirname );
extern void _makepath(char * path, const char * drive, const char *dir, const char * filename, const char * ext);
extern void _splitpath(const char* inpath, char * drv, char * dir, char* fname, char * ext);

//////////////////////////////////////////////////////////////////////////
extern int memicmp( LPCSTR s1, LPCSTR s2, DWORD len );
extern int strcmpi( const char *str1, const char *str2 );

extern "C" char* strlwr (char * str);
extern "C" char* strupr(char * str);

extern char * _ui64toa(unsigned long long value,	char *str, int radix);
extern long long _atoi64( char *str );

template <size_t SIZE>
int strcpy_s(char (&dst)[SIZE], const char *src)
{
  strncpy(dst, src, SIZE - 1);
  dst[SIZE - 1] = 0;
  return 0;
}

inline int strcpy_s(char *dst, size_t size, const char *src)
{
	strncpy(dst, src, size);
	dst[size - 1] = 0;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// function renaming
#define _chmod chmod 
#define _snprintf snprintf
#define _isnan isnan
#define stricmp strcasecmp
#define _stricmp strcasecmp
#define strnicmp strncasecmp
#define _strnicmp strncasecmp

#define _strlwr	strlwr 
#define _strlwr_s strlwr 
#define _strups	strupr

#define _vsnprintf vsnprintf
#define _wtof( str ) wcstod( str, 0 )
//////////////////////////////////////////////////////////////////////////

#ifndef __TRLTOA__
#define __TRLTOA__
extern char *ltoa ( long i , char *a , int radix );
#endif
#define itoa ltoa

//////////////////////////////////////////////////////////////////////////
#if 0
inline long int abs(long int x)
{
	return labs(x);
}

inline float abs(float x)
{
	return fabsf(x);
}

inline double abs(double x)
{
	return fabs(x);
}

inline float sqrt(float x)
{
	return sqrtf(x);
}
#else
#include <cmath>
using std::abs;
using std::sqrt;
#endif

extern char* _strtime(char* date);
extern char* _strdate(char* date);

#define _MM_HINT_T0     (1)
#define _MM_HINT_T1     (2)
#define _MM_HINT_T2     (3)
#define _MM_HINT_NTA    (0)
inline void _mm_prefetch(const char *, int) { }

#endif //__cplusplus

#endif // __Linux_Win32Wrapper_h__

// vim:ts=2

