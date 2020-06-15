/**
 * @file
 * @brief Launcher API interface.
 */

#pragma once

#include <stddef.h>
#include <stdarg.h>

struct ILauncher
{
	/**
	 * @brief Function exported by the launcher as "GetILauncher".
	 */
	typedef ILauncher *(*TGetFunc)();

	virtual const char *GetName() = 0;

	virtual int GetVersionMajor() = 0;
	virtual int GetVersionMinor() = 0;
	virtual int GetFPSCap() = 0;
	virtual void SetFPSCap(int fps) {return;};
};

