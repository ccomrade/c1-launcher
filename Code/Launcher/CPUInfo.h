#pragma once

struct ISystem;

/**
 * Reverse engineered CryEngine 2 structure with CPU information.
 *
 * Do not change the content.
 * Total size is 8720 bytes in both 32-bit and 64-bit code.
 *
 * The purpose of this is to replace the problematic CPU detection mechanism in CrySystem.
 */
struct CPUInfo
{
	enum
	{
		FLAG_MMX   = (1 << 1),
		FLAG_3DNOW = (1 << 2),
		FLAG_SSE   = (1 << 3),
		FLAG_SSE2  = (1 << 4),
	};

	enum
	{
		MAX_CORE_COUNT = 32,
	};

	unsigned int coreCountTotal;
	unsigned int coreCountAvailable;
	unsigned int coreCountPhysical;
	unsigned int reserved;

	struct Core
	{
		unsigned int reserved1[2];
		unsigned int flags;
		unsigned int reserved2[65];
	};

	Core cores[MAX_CORE_COUNT];

	static void Detect(CPUInfo* self, ISystem* pSystem);
};
