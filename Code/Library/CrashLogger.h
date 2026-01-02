#pragma once

#include <cstdio>

namespace CrashLogger
{
	typedef std::FILE* (*LogFileProvider)();

	void OnEngineError(const char* format, va_list args);

	void Enable(LogFileProvider logFileProvider, const char* banner);

	struct ExtraProvider
	{
		ExtraProvider* next;

		ExtraProvider() : next(NULL) {}

		virtual void OnCrash(std::FILE* file) = 0;
	};

	void AddExtraProvider(ExtraProvider* provider);
}
