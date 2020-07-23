#pragma once

#include "Error.h"  // SystemError

struct DLL;

namespace System
{
	inline void *RVA(void *base, size_t offset)
	{
		return static_cast<unsigned char*>(base) + offset;
	}

	inline const void *RVA(const void *base, size_t offset)
	{
		return static_cast<const unsigned char*>(base) + offset;
	}

	const char *GetCmdLine();

	void ErrorBox(const char *message);
	void ErrorBox(const Error & error);

	unsigned int GetCrysisGameBuild(const DLL & dll);

	void FillNOP(void *address, size_t length);
	void FillMem(void *address, const void *data, size_t length);
}
