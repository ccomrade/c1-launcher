// Copyright (C) 2001-2008 Crytek GmbH

#pragma once

#include <cstdarg>

struct ILogCallback
{
	virtual void OnWriteToConsole(const char* text, bool newLine) = 0;
	virtual void OnWriteToFile(const char* text, bool newLine) = 0;
};

struct ILog
{
	enum ELogType
	{
		eMessage,
		eWarning,
		eError,
		eAlways,
		eWarningAlways,
		eErrorAlways,
		eInput,
		eInputResponse,
		eComment
	};

	virtual void LogV(ELogType type, const char* format, va_list args) = 0;

	virtual ~ILog()
	{
	}

	virtual void Log(const char* format, ...) = 0;
	virtual void LogWarning(const char* format, ...) = 0;
	virtual void LogError(const char* format, ...) = 0;

	virtual void Release() = 0;

	virtual bool SetFileName(const char* filename) = 0;
	virtual const char* GetFileName() = 0;

	virtual void LogPlus(const char* format, ...) = 0;
	virtual void LogToFile(const char* format, ...) = 0;
	virtual void LogToFilePlus(const char* format, ...) = 0;
	virtual void LogToConsole(const char* format, ...) = 0;
	virtual void LogToConsolePlus(const char* format, ...) = 0;

	virtual void UpdateLoadingScreen(const char* format, ...) = 0;

	virtual void RegisterConsoleVariables() = 0;
	virtual void UnregisterConsoleVariables() = 0;

	virtual void SetVerbosity(int verbosity) = 0;
	virtual int GetVerbosityLevel() = 0;

	virtual void AddCallback(ILogCallback* pCallback) = 0;
	virtual void RemoveCallback(ILogCallback* pCallback) = 0;
};
