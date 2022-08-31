#pragma once

#include <cstdio>

namespace CrashLogger
{
	typedef std::FILE* (*Handler)();

	void OnEngineError(const char* format, ...);

	void Enable(Handler handler);
}
