#pragma once

#include <cstddef>

using std::size_t;

namespace Util
{
	const char *GetCmdLine();

	int GetCrysisGameBuild(void *lib);

	void ErrorBox(const char *msg);

	bool FillNOP(void *address, size_t length);
	bool FillMem(void *address, const void *data, size_t length);

	inline void *RVA(void *base, size_t offset)
	{
		return static_cast<unsigned char*>(base) + offset;
	}

	inline const void *RVA(const void *base, size_t offset)
	{
		return static_cast<const unsigned char*>(base) + offset;
	}
}
