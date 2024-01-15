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

namespace EX
{
	struct LDR_DATA_TABLE_ENTRY
	{
		PVOID Reserved1[2];
		LIST_ENTRY InMemoryOrderLinks;
		PVOID Reserved2[2];
		PVOID DllBase;
		PVOID EntryPoint;
		PVOID Reserved3;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		// ...
	};

	struct PEB_LDR_DATA
	{
		BYTE Reserved1[8];
		PVOID Reserved2[3];
		LIST_ENTRY InMemoryOrderModuleList;
	};

	// Process Environment Block
	struct PEB
	{
		BYTE Reserved1[2];
		BYTE BeingDebugged;
		BYTE Reserved2[1];
		PVOID Reserved3[2];
		PEB_LDR_DATA* Ldr;
		// ...
	};

	// Thread Environment Block
	struct TEB
	{
		PVOID Reserved1[12];
		PEB* ProcessEnvironmentBlock;
		// ...
	};
}

static EX::TEB* GetTEB()
{
	return reinterpret_cast<EX::TEB*>(NtCurrentTeb());
}

static LIST_ENTRY* GetLoadedModuleList()
{
	// InMemoryOrderModuleList -> InMemoryOrderLinks in GetModuleData
	return &GetTEB()->ProcessEnvironmentBlock->Ldr->InMemoryOrderModuleList;
}

static EX::LDR_DATA_TABLE_ENTRY* GetModuleData(LIST_ENTRY* entry)
{
	// InMemoryOrderLinks -> InMemoryOrderModuleList in GetLoadedModuleList
	return CONTAINING_RECORD(entry, EX::LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
}

static bool IsEqualNoCase(const UNICODE_STRING& a, const wchar_t* b)
{
	return _wcsnicmp(a.Buffer, b, a.Length / sizeof(wchar_t)) == 0;
}

static HMODULE FindLoadedDLL(const wchar_t* name)
{
	LIST_ENTRY* list = GetLoadedModuleList();

	for (LIST_ENTRY* entry = list->Flink; entry != list; entry = entry->Flink)
	{
		EX::LDR_DATA_TABLE_ENTRY* data = GetModuleData(entry);

		if (IsEqualNoCase(data->BaseDllName, name))
		{
			return static_cast<HMODULE>(data->DllBase);
		}
	}

	return NULL;
}

static HMODULE FindLoadedDLL(const char* name)
{
	enum { MAX_NAME_LENGTH = 256 };

	wchar_t wideName[MAX_NAME_LENGTH];
	if (MultiByteToWideChar(CP_UTF8, 0, name, -1, wideName, MAX_NAME_LENGTH) <= 0)
	{
		return NULL;
	}

	return FindLoadedDLL(wideName);
}

static void* FindThunkFunction(HMODULE exe, HMODULE dll, const IMAGE_THUNK_DATA* thunk)
{
	const char* name = NULL;

	if (IMAGE_SNAP_BY_ORDINAL(thunk->u1.Ordinal))
	{
		name = reinterpret_cast<const char*>(IMAGE_ORDINAL(thunk->u1.Ordinal));
	}
	else
	{
		IMAGE_IMPORT_BY_NAME* data = static_cast<IMAGE_IMPORT_BY_NAME*>(RVA(exe, thunk->u1.AddressOfData));

		name = reinterpret_cast<const char*>(data->Name);
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

EXELoader::Result EXELoader::Load(const char* name)
{
	HMODULE exe = LoadLibraryA(name);
	if (!exe)
	{
		return Result(ERROR_OPEN, GetLastError());
	}

	const IMAGE_OPTIONAL_HEADER* optionalHeader = GetOptionalHeader(exe);
	if (!optionalHeader)
	{
		FreeLibrary(exe);
		return Result(ERROR_OPTIONAL_HEADER);
	}

	const IMAGE_DATA_DIRECTORY* importData = GetDirectoryData(optionalHeader, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!importData)
	{
		FreeLibrary(exe);
		return Result(ERROR_IMPORT_TABLE_MISSING);
	}

	const IMAGE_DATA_DIRECTORY* iatData = GetDirectoryData(optionalHeader, IMAGE_DIRECTORY_ENTRY_IAT);
	if (!iatData)
	{
		FreeLibrary(exe);
		return Result(ERROR_IAT_MISSING);
	}

	void* iat = RVA(exe, iatData->VirtualAddress);

	// temporarily make IAT writable
	DWORD iatProtection;
	if (!VirtualProtect(iat, iatData->Size, PAGE_READWRITE, &iatProtection))
	{
		const DWORD sysError = GetLastError();
		FreeLibrary(exe);
		return Result(ERROR_IAT_VIRTUAL_PROTECT, sysError);
	}

	const IMAGE_IMPORT_DESCRIPTOR* importDescriptor =
		static_cast<const IMAGE_IMPORT_DESCRIPTOR*>(RVA(exe, importData->VirtualAddress));

	// fill IAT
	for (; importDescriptor->Name && importDescriptor->FirstThunk; ++importDescriptor)
	{
		const char* dllName = static_cast<const char*>(RVA(exe, importDescriptor->Name));

		// GetModuleHandleA cannot find certain DLLs
		HMODULE dll = FindLoadedDLL(dllName);
		if (!dll)
		{
			// TODO: unload previously loaded DLLs in case of error
			dll = LoadLibraryA(dllName);
			if (!dll)
			{
				const DWORD sysError = GetLastError();
				FreeLibrary(exe);
				return Result(ERROR_IMPORT_LOAD_LIBRARY, sysError);
			}
		}

		IMAGE_THUNK_DATA* thunk = static_cast<IMAGE_THUNK_DATA*>(RVA(exe, importDescriptor->FirstThunk));

		for (; thunk->u1.Ordinal; ++thunk)
		{
			void* func = FindThunkFunction(exe, dll, thunk);
			if (!func)
			{
				const DWORD sysError = GetLastError();
				FreeLibrary(exe);
				return Result(ERROR_IAT_GET_PROC_ADDRESS, sysError);
			}

			thunk->u1.Function = reinterpret_cast<DWORD_PTR>(func);
		}
	}

	// restore IAT protection
	if (!VirtualProtect(iat, iatData->Size, iatProtection, &iatProtection))
	{
		const DWORD sysError = GetLastError();
		FreeLibrary(exe);
		return Result(ERROR_IAT_VIRTUAL_PROTECT_RESTORE, sysError);
	}

	const IMAGE_SECTION_HEADER* textSection = GetSectionHeader(exe, ".text");
	if (!textSection)
	{
		FreeLibrary(exe);
		return Result(ERROR_TEXT_SECTION_MISSING);
	}

	void* textBegin = RVA(exe, textSection->VirtualAddress);
	void* textEnd = RVA(exe, textSection->VirtualAddress + textSection->Misc.VirtualSize);

	void* iatEnd = RVA(exe, iatData->VirtualAddress + iatData->Size);

	// make sure .rdata section exists, see below
	if (!GetSectionHeader(exe, ".rdata"))
	{
		FreeLibrary(exe);
		return Result(ERROR_RDATA_SECTION_MISSING);
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
		FreeLibrary(exe);
		return Result(ERROR_GLOBAL_CONSTRUCTOR);
	}

	// call C++ global constructors
	initterm(xc_a, xc_z);

	return Result(exe);
}

const char* const EXELoader::errorNames[] = {
#define X(name) #name,
	EXE_LOADER_ERRORS
#undef X
};
