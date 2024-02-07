#include <cstring>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>
#undef NO_ERROR

#include "EXELoader.h"

static void* RVA(void* base, std::size_t offset)
{
	return static_cast<unsigned char*>(base) + offset;
}

static const void* RVA(const void* base, std::size_t offset)
{
	return static_cast<const unsigned char*>(base) + offset;
}

static const IMAGE_NT_HEADERS* GetPEHeader(HMODULE exe)
{
	const IMAGE_DOS_HEADER* dosHeader = static_cast<const IMAGE_DOS_HEADER*>(RVA(exe, 0));
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return NULL;
	}

	const IMAGE_NT_HEADERS* peHeader = static_cast<const IMAGE_NT_HEADERS*>(RVA(exe, dosHeader->e_lfanew));
	if (peHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		return NULL;
	}

	return peHeader;
}

static const IMAGE_OPTIONAL_HEADER* GetOptionalHeader(HMODULE exe)
{
	const IMAGE_NT_HEADERS* peHeader = GetPEHeader(exe);
	if (!peHeader)
	{
		return NULL;
	}

	const IMAGE_OPTIONAL_HEADER* optionalHeader = &peHeader->OptionalHeader;
	if (optionalHeader->Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
	{
		return NULL;
	}

	return optionalHeader;
}

static const IMAGE_SECTION_HEADER* GetSectionHeader(HMODULE exe, const char* name)
{
	const std::size_t nameLength = std::strlen(name);
	if (nameLength > IMAGE_SIZEOF_SHORT_NAME)
	{
		return NULL;
	}

	const IMAGE_NT_HEADERS* peHeader = GetPEHeader(exe);
	if (!peHeader)
	{
		return NULL;
	}

	const IMAGE_SECTION_HEADER* sections = static_cast<const IMAGE_SECTION_HEADER*>(
		RVA(peHeader, sizeof(peHeader->Signature) + sizeof(peHeader->FileHeader)
			+ peHeader->FileHeader.SizeOfOptionalHeader));

	const unsigned int sectionCount = peHeader->FileHeader.NumberOfSections;

	for (unsigned int i = 0; i < sectionCount; ++i)
	{
		if (std::memcmp(sections[i].Name, name, nameLength) == 0)
		{
			return sections + i;
		}
	}

	return NULL;
}

static const IMAGE_DATA_DIRECTORY* GetDirectoryData(const IMAGE_OPTIONAL_HEADER* optionalHeader, unsigned int id)
{
	if (optionalHeader->NumberOfRvaAndSizes <= id)
	{
		return NULL;
	}

	const IMAGE_DATA_DIRECTORY* directoryData = &optionalHeader->DataDirectory[id];
	if (directoryData->VirtualAddress == 0 || directoryData->Size == 0)
	{
		return NULL;
	}

	return directoryData;
}

static void* __stdcall FakeSetUnhandledExceptionFilter(void*)
{
	return NULL;
}

static const char* GetThunkFunctionName(HMODULE exe, const IMAGE_THUNK_DATA* thunk)
{
	if (IMAGE_SNAP_BY_ORDINAL(thunk->u1.Ordinal))
	{
		return NULL;
	}

	IMAGE_IMPORT_BY_NAME* data = static_cast<IMAGE_IMPORT_BY_NAME*>(RVA(exe, thunk->u1.AddressOfData));

	return reinterpret_cast<const char*>(data->Name);
}

static void* FindThunkFunction(HMODULE exe, HMODULE dll, const IMAGE_THUNK_DATA* thunk)
{
	const char* name = GetThunkFunctionName(exe, thunk);

	if (!name)
	{
		return GetProcAddress(dll, reinterpret_cast<const char*>(IMAGE_ORDINAL(thunk->u1.Ordinal)));
	}

	if (std::strcmp(name, "SetUnhandledExceptionFilter") == 0)
	{
		// prevent the EXE from disabling our crash logger
		return &FakeSetUnhandledExceptionFilter;
	}

	return GetProcAddress(dll, name);
}

typedef int  (*PIFV)();
typedef void (*PVFV)();

static int initterm_e(PIFV* begin, PIFV* end)
{
	for (PIFV* fn = begin; fn < end; ++fn)
	{
		if (*fn)
		{
			int ret = (**fn)();
			if (ret != 0)
			{
				return ret;
			}
		}
	}

	return 0;
}

static void initterm(PVFV* begin, PVFV* end)
{
	for (PVFV* fn = begin; fn < end; ++fn)
	{
		if (*fn)
		{
			(**fn)();
		}
	}
}

void* EXELoader::Load(const char* name)
{
	this->error = NO_ERROR;
	this->sysError = 0;
	this->errorValue = NULL;

	HMODULE exe = LoadLibraryA(name);
	if (!exe)
	{
		this->error = ERROR_OPEN;
		this->sysError = GetLastError();
		return NULL;
	}

	const IMAGE_OPTIONAL_HEADER* optionalHeader = GetOptionalHeader(exe);
	if (!optionalHeader)
	{
		this->error = ERROR_OPTIONAL_HEADER;
		return NULL;
	}

	const IMAGE_DATA_DIRECTORY* importData = GetDirectoryData(optionalHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!importData)
	{
		this->error = ERROR_IMPORT_TABLE_MISSING;
		return NULL;
	}

	const IMAGE_DATA_DIRECTORY* iatData = GetDirectoryData(optionalHeader, IMAGE_DIRECTORY_ENTRY_IAT);
	if (!iatData)
	{
		this->error = ERROR_IAT_MISSING;
		return NULL;
	}

	void* iat = RVA(exe, iatData->VirtualAddress);

	// temporarily make IAT writable
	DWORD iatProtection;
	if (!VirtualProtect(iat, iatData->Size, PAGE_READWRITE, &iatProtection))
	{
		this->error = ERROR_IAT_VIRTUAL_PROTECT;
		this->sysError = GetLastError();
		return NULL;
	}

	const IMAGE_IMPORT_DESCRIPTOR* importDescriptor =
		static_cast<const IMAGE_IMPORT_DESCRIPTOR*>(RVA(exe, importData->VirtualAddress));

	// fill IAT
	for (; importDescriptor->Name && importDescriptor->FirstThunk; ++importDescriptor)
	{
		const char* dllName = static_cast<const char*>(RVA(exe, importDescriptor->Name));

		HMODULE dll = LoadLibraryA(dllName);
		if (!dll)
		{
			this->error = ERROR_IMPORT_LOAD_LIBRARY;
			this->sysError = GetLastError();
			this->errorValue = dllName;
			return NULL;
		}

		IMAGE_THUNK_DATA* thunk = static_cast<IMAGE_THUNK_DATA*>(RVA(exe, importDescriptor->FirstThunk));

		for (; thunk->u1.Ordinal; ++thunk)
		{
			void* func = FindThunkFunction(exe, dll, thunk);
			if (!func)
			{
				this->error = ERROR_IAT_GET_PROC_ADDRESS;
				this->sysError = GetLastError();
				this->errorValue = GetThunkFunctionName(exe, thunk);
				return NULL;
			}

			thunk->u1.Function = reinterpret_cast<DWORD_PTR>(func);
		}
	}

	// restore IAT protection
	if (!VirtualProtect(iat, iatData->Size, iatProtection, &iatProtection))
	{
		this->error = ERROR_IAT_VIRTUAL_PROTECT_RESTORE;
		this->sysError = GetLastError();
		return NULL;
	}

	const IMAGE_SECTION_HEADER* textSection = GetSectionHeader(exe, ".text");
	if (!textSection)
	{
		this->error = ERROR_TEXT_SECTION_MISSING;
		return NULL;
	}

	void* textBegin = RVA(exe, textSection->VirtualAddress);
	void* textEnd = RVA(exe, textSection->VirtualAddress + textSection->Misc.VirtualSize);

	void* iatEnd = RVA(exe, iatData->VirtualAddress + iatData->Size);

	// make sure .rdata section exists, see below
	if (!GetSectionHeader(exe, ".rdata"))
	{
		this->error = ERROR_RDATA_SECTION_MISSING;
		return NULL;
	}

	// there is no way to obtain location of the two global constructor arrays
	// VS2005 linker puts them after IAT in .rdata
	PVFV* xc_a = static_cast<PVFV*>(iatEnd);
	PVFV* xc_z = xc_a;

	// from the beginning of the first array, find the end of the second array
	for (PVFV* next = xc_z + 1; (*next >= textBegin && *next < textEnd) || *next == NULL; ++next)
	{
		xc_z = next;
	}

	// find the real end of the second array by skipping trailing null pointers
	while (xc_z > xc_a && *xc_z == NULL)
	{
		--xc_z;
	}

	PIFV* xi_z = reinterpret_cast<PIFV*>(xc_z + 1);

	// find the end of the first array, which is the beginning of the second array
	while (xc_z > xc_a && *xc_z != NULL)
	{
		--xc_z;
	}

	PIFV* xi_a = reinterpret_cast<PIFV*>(xc_z);

	// call C global constructors
	if (initterm_e(xi_a, xi_z) != 0)
	{
		this->error = ERROR_GLOBAL_CONSTRUCTOR;
		return NULL;
	}

	// call C++ global constructors
	initterm(xc_a, xc_z);

	return exe;
}

const char* EXELoader::GetErrorName() const
{
	static const char* const NAMES[] = {
#define X(name) #name,
		EXE_LOADER_ERRORS
#undef X
	};

	return NAMES[this->error];
}
