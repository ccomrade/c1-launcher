#include "ILog.h"
#include "ISystem.h"

SSystemGlobalEnvironment* gEnv;

void CryLog(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eMessage, format, args);
	va_end(args);
}

void CryLogWarning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eWarning, format, args);
	va_end(args);
}

void CryLogError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eError, format, args);
	va_end(args);
}

void CryLogAlways(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eAlways, format, args);
	va_end(args);
}

void CryLogWarningAlways(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eWarningAlways, format, args);
	va_end(args);
}

void CryLogErrorAlways(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eErrorAlways, format, args);
	va_end(args);
}

void CryLogComment(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	gEnv->pLog->LogV(ILog::eComment, format, args);
	va_end(args);
}
