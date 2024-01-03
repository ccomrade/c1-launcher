#include <cstring>

#include "CryCommon/CrySystem/ISystem.h"
#include "Library/CPUID.h"
#include "Library/OS.h"

#include "CPUInfo.h"
#include "LauncherCommon.h"

static unsigned int GetCoreCount()
{
	unsigned int coreCount = OS::GetLogicalProcessorCount();

	// fix crash on systems with more than 32 logical processors
	if (coreCount > CPUInfo::MAX_CORE_COUNT)
	{
		coreCount = CPUInfo::MAX_CORE_COUNT;
	}

	return coreCount;
}

static unsigned int GetFeatures()
{
	unsigned int features = 0;

	if (g_cpuid.HasMMX())
	{
		features |= CPUInfo::FLAG_MMX;
	}

	if (g_cpuid.Has3DNow())
	{
		features |= CPUInfo::FLAG_3DNOW;
	}

	if (g_cpuid.HasSSE())
	{
		features |= CPUInfo::FLAG_SSE;
	}

	if (g_cpuid.HasSSE2())
	{
		features |= CPUInfo::FLAG_SSE2;
	}

	return features;
}

void CPUInfo::Detect(CPUInfo* self, ISystem* pSystem)
{
	LauncherCommon::OnEarlyEngineInit(pSystem);

	const unsigned int coreCount = GetCoreCount();
	const unsigned int features = GetFeatures();

	std::memset(self, 0, sizeof(CPUInfo));

	// only the following is needed
	self->coreCountTotal = coreCount;
	self->coreCountAvailable = coreCount;
	self->coreCountPhysical = coreCount;
	self->cores[0].flags = features;

	CryLogAlways("%s [Count: %u] [Features:%s%s%s%s]",
		g_cpuid.brand_string,
		coreCount,
		(features & FLAG_MMX)   ? " MMX"    : "",
		(features & FLAG_3DNOW) ? " 3DNow!" : "",
		(features & FLAG_SSE)   ? " SSE"    : "",
		(features & FLAG_SSE2)  ? " SSE2"   : ""
	);
}
