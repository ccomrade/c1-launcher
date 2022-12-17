#pragma once

#include <cstdio>

namespace CrashLogger
{
	typedef std::FILE* (*Handler)();

	void OnEngineError(const char* format, va_list args);

	void Enable(Handler handler);
}
