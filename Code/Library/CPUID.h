#pragma once

#include <bitset>
#include <cstring>

#ifdef _MSC_VER
#include <intrin.h>
#define SUPPRESS_STUPID_MSVC_WARNING_C4351 __pragma(warning(suppress:4351))
#else
#define SUPPRESS_STUPID_MSVC_WARNING_C4351
#endif

struct CPUID
{
	struct Query
	{
		unsigned int eax;
		unsigned int ebx;
		unsigned int ecx;
		unsigned int edx;

		explicit Query(unsigned int leaf)
		{
#ifdef _MSC_VER
			int regs[4];
			__cpuid(regs, leaf);

			this->eax = regs[0];
			this->ebx = regs[1];
			this->ecx = regs[2];
			this->edx = regs[3];
#else
			__asm__
			(
				"cpuid"
				: "=a" (this->eax), "=b" (this->ebx), "=c" (this->ecx), "=d" (this->edx)
				: "a" (leaf), "c" (0)
			);
#endif
		}
	};

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

	SUPPRESS_STUPID_MSVC_WARNING_C4351
	CPUID() : vendor(VENDOR_UNKNOWN), leaf_1_edx(), leaf_80000001_edx(), brand_string(), vendor_string()
	{
		Query query(0x0);
		const unsigned int maxBasicLeaf = query.eax;

		*reinterpret_cast<unsigned int*>(this->vendor_string + 0) = query.ebx;
		*reinterpret_cast<unsigned int*>(this->vendor_string + 4) = query.edx;
		*reinterpret_cast<unsigned int*>(this->vendor_string + 8) = query.ecx;

		static const struct { char string[12 + 1]; Vendor id; } vendors[] = {
			{ "AuthenticAMD", VENDOR_AMD },
			{ "GenuineIntel", VENDOR_INTEL },
		};

		for (unsigned int i = 0; i < (sizeof vendors / sizeof vendors[0]); i++)
		{
			if (std::memcmp(this->vendor_string, vendors[i].string, 12) == 0)
			{
				this->vendor = vendors[i].id;
				break;
			}
		}

		if (maxBasicLeaf >= 0x1)
		{
			query = Query(0x1);
			this->leaf_1_edx = query.edx;
		}

		query = Query(0x80000000);
		const unsigned int maxExtendedLeaf = query.eax;

		if (maxExtendedLeaf >= 0x80000001)
		{
			query = Query(0x80000001);
			this->leaf_80000001_edx = query.edx;
		}

		if (maxExtendedLeaf >= 0x80000004)
		{
			query = Query(0x80000002);
			std::memcpy(this->brand_string + 0, &query, 16);
			query = Query(0x80000003);
			std::memcpy(this->brand_string + 16, &query, 16);
			query = Query(0x80000004);
			std::memcpy(this->brand_string + 32, &query, 16);
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

extern const CPUID g_cpuid;
