#pragma once

#include <bitset>
#include <cstring>

#include <intrin.h>

struct CPUID
{
	enum Vendor
	{
		VENDOR_UNKNOWN = 0,
		VENDOR_AMD,
		VENDOR_INTEL,
	};

	Vendor vendor;
	std::bitset<32> leaf_1_edx;
	std::bitset<32> leaf_80000001_edx;
	char brand_string[48 + 1];
	char vendor_string[12 + 1];

	#pragma warning (suppress:4351)
	CPUID() : vendor(VENDOR_UNKNOWN), leaf_1_edx(), leaf_80000001_edx(), brand_string(), vendor_string()
	{
		int regs[4];

		__cpuid(regs, 0x0);
		const unsigned int maxBasicLeaf = regs[0];

		*reinterpret_cast<int*>(this->vendor_string + 0) = regs[1];
		*reinterpret_cast<int*>(this->vendor_string + 4) = regs[3];
		*reinterpret_cast<int*>(this->vendor_string + 8) = regs[2];

		if (std::memcmp(this->vendor_string, "AuthenticAMD", 12) == 0)
		{
			this->vendor = VENDOR_AMD;
		}
		else if (std::memcmp(this->vendor_string, "GenuineIntel", 12) == 0)
		{
			this->vendor = VENDOR_INTEL;
		}

		if (maxBasicLeaf >= 0x1)
		{
			__cpuid(regs, 0x1);
			this->leaf_1_edx = regs[3];
		}

		__cpuid(regs, 0x80000000);
		const unsigned int maxExtendedLeaf = regs[0];

		if (maxExtendedLeaf >= 0x80000001)
		{
			__cpuid(regs, 0x80000001);
			this->leaf_80000001_edx = regs[3];
		}

		if (maxExtendedLeaf >= 0x80000004)
		{
			__cpuid(regs, 0x80000002);
			std::memcpy(this->brand_string + 0, regs, 16);
			__cpuid(regs, 0x80000003);
			std::memcpy(this->brand_string + 16, regs, 16);
			__cpuid(regs, 0x80000004);
			std::memcpy(this->brand_string + 32, regs, 16);
		}
	}

	bool HasMMX() const
	{
		return this->leaf_1_edx[23];
	}

	bool HasSSE() const
	{
		return this->leaf_1_edx[25];
	}

	bool HasSSE2() const
	{
		return this->leaf_1_edx[26];
	}

	bool Has3DNow() const
	{
		return this->vendor == VENDOR_AMD && this->leaf_80000001_edx[31];
	}
};

extern CPUID g_cpuid;
