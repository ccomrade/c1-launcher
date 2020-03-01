/**
 * @file
 * @brief Crysis launcher.
 */

#pragma once

struct DLL;
struct SSystemInitParams;  // ISystem.h

namespace Launcher
{
	bool InitEngineParams(SSystemInitParams & params, void *hInstance, const char *logFileName);

	bool Run(const DLL & libCryGame, SSystemInitParams & params);
}
