// -------------------------------------------------------------------------
// Crytek Source File.
// Copyright (C) Crytek GmbH, 2001-2008.
// -------------------------------------------------------------------------
#ifndef _ILOG_H_
#define _ILOG_H_

#include <platform.h>



#include <string>
#include "IMiniLog.h"

//////////////////////////////////////////////////////////////////////////
// ILogCallback is a callback interface to the ILog.
//////////////////////////////////////////////////////////////////////////
struct ILogCallback
{
	virtual void OnWriteToConsole( const char *sText,bool bNewLine ) = 0;
	virtual void OnWriteToFile( const char *sText,bool bNewLine ) = 0;
};

// wiki documentation: http://server4/twiki/bin/view/CryEngine/CryLog
//////////////////////////////////////////////////////////////////////
struct ILog: public IMiniLog
{
	virtual void Release() = 0;

	//set the file used to log to disk
	virtual bool	SetFileName(const char *command = NULL) = 0;

	//
	virtual const char*	GetFileName() = 0;

	//all the following functions will be removed are here just to be able to compile the project ---------------------------

	//will log the text both to file and console
	virtual void	Log(const char *szCommand,...) PRINTF_PARAMS(2, 3) = 0;

	virtual void	LogWarning(const char *szCommand,...) PRINTF_PARAMS(2, 3) = 0;

	virtual void	LogError(const char *szCommand,...) PRINTF_PARAMS(2, 3) = 0;

	//will log the text both to the end of file and console
	virtual void	LogPlus(const char *command,...) PRINTF_PARAMS(2, 3) = 0;	

	//log to the file specified in setfilename
  virtual void	LogToFile(const char *command,...) PRINTF_PARAMS(2, 3) = 0;	

	//
	virtual void	LogToFilePlus(const char *command,...) PRINTF_PARAMS(2, 3) = 0;

	//log to console only
	virtual void	LogToConsole(const char *command,...) PRINTF_PARAMS(2, 3) = 0;

	//
	virtual void	LogToConsolePlus(const char *command,...) PRINTF_PARAMS(2, 3) = 0;

	//
	virtual void	UpdateLoadingScreen(const char *command,...) PRINTF_PARAMS(2, 3) = 0;	

	//
	virtual void RegisterConsoleVariables() {}

	//
	virtual void UnregisterConsoleVariables() {}

	//
	virtual void	SetVerbosity( int verbosity ) = 0;

	virtual int		GetVerbosityLevel()=0;

	virtual void  AddCallback( ILogCallback *pCallback ) = 0;
	virtual void  RemoveCallback( ILogCallback *pCallback ) = 0;
};

#endif //_ILOG_H_



