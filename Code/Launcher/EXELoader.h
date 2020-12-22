#pragma once

struct DLL;

namespace EXELoader
{
	void FillIAT(const DLL & exe);

	void CallGlobalInitializers(const DLL & exe);

	inline void Init(const DLL & exe)
	{
		FillIAT(exe);

		CallGlobalInitializers(exe);
	}
}
