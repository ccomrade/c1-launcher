#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h>

#include "EXELoader.h"
#include "DLL.h"
#include "System.h"
#include "Format.h"
#include "Util.h"

using System::RVA;

namespace
{
	class MemoryProtectionGuard
	{
		void *m_address;
		size_t m_size;
		unsigned long m_oldProtection;

	public:
		MemoryProtectionGuard(void *address, size_t size, unsigned long protection)
		: m_address(address),
		  m_size(size),
		  m_oldProtection(0)
		{
			if (!VirtualProtect(m_address, m_size, protection, &m_oldProtection))
			{
				throw SystemError("Failed to change memory protection");
			}
		}

		~MemoryProtectionGuard()
		{
			if (!VirtualProtect(m_address, m_size, m_oldProtection, &m_oldProtection))
			{
				throw SystemError("Failed to restore memory protection");
			}
		}
	};

	// we have to redefine all these structures, because MSVC headers don't contain everything needed

	struct PEB_LDR_DATA
	{
		BYTE Reserved1[8];
		PVOID Reserved2[3];
		LIST_ENTRY InMemoryOrderModuleList;
	};

	struct LDR_DATA_TABLE_ENTRY
	{
		PVOID Reserved1[2];
		LIST_ENTRY InMemoryOrderLinks;
		PVOID Reserved2[2];
		PVOID DllBase;
		PVOID Reserved3[2];
		UNICODE_STRING FullDllName;
		// ...
	};

	// Process Environment Block
	struct PEB
	{
		BYTE Reserved1[4];
		PVOID Reserved2[2];
		PEB_LDR_DATA *LoaderData;
		// ...
	};

	// Thread Environment Block
	struct TEB
	{
		PVOID Reserved1[12];
		PEB *ProcessEnvironmentBlock;
		// ...
	};

	TEB *GetTEB()
	{
		// inline assembly in MSVC is broken, but NtCurrentTeb should work
		return reinterpret_cast<TEB*>(NtCurrentTeb());
	}

	const IMAGE_DATA_DIRECTORY *GetDirectoryData(const void *pDLL, unsigned int directory)
	{
		const IMAGE_DOS_HEADER *pDOSHeader = static_cast<const IMAGE_DOS_HEADER*>(pDLL);

		if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return NULL;

		const IMAGE_NT_HEADERS *pPEHeader = static_cast<const IMAGE_NT_HEADERS*>(RVA(pDLL, pDOSHeader->e_lfanew));

		if (pPEHeader->Signature != IMAGE_NT_SIGNATURE)
			return NULL;

	#ifdef BUILD_64BIT
		if (pPEHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
			return NULL;
	#else
		if (pPEHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
			return NULL;
	#endif

		if (pPEHeader->OptionalHeader.NumberOfRvaAndSizes <= directory)
			return NULL;

		const IMAGE_DATA_DIRECTORY *pData = &pPEHeader->OptionalHeader.DataDirectory[directory];

		if (pData->VirtualAddress == 0 || pData->Size == 0)
			return NULL;

		return pData;
	}

	bool IsModuleNameEqual(const UNICODE_STRING & moduleName, const std::wstring & expectedName)
	{
		const size_t totalLength = moduleName.Length / sizeof (wchar_t);

		const wchar_t *name = moduleName.Buffer;
		size_t length = totalLength;

		// skip file path
		for (size_t i = 0; i < totalLength; i++)
		{
			const wchar_t ch = moduleName.Buffer[i];

			if (ch == L'/' || ch == L'\\')
			{
				const size_t pos = i + 1;

				name = moduleName.Buffer + pos;
				length = totalLength - pos;
			}
		}

		if (length != expectedName.length())
		{
			return false;
		}

		// compare names
		for (size_t i = 0; i < length; i++)
		{
			if (Util::ToLower(name[i]) != Util::ToLower(expectedName[i]))
			{
				return false;
			}
		}

		return true;
	}

	HMODULE FindLoadedModule(const char *name)
	{
		const std::wstring expectedName = Util::ConvertUTF8To16(name);

		LIST_ENTRY *list = &GetTEB()->ProcessEnvironmentBlock->LoaderData->InMemoryOrderModuleList;

		for (LIST_ENTRY *entry = list->Flink; entry != list; entry = entry->Flink)
		{
			LDR_DATA_TABLE_ENTRY *data = CONTAINING_RECORD(entry, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

			if (IsModuleNameEqual(data->FullDllName, expectedName))
			{
				return static_cast<HMODULE>(data->DllBase);
			}
		}

		return NULL;
	}
}

void EXELoader::FillIAT(const DLL & exe)
{
	void *handle = exe.getHandle();

	const IMAGE_DATA_DIRECTORY *pImportData = GetDirectoryData(handle, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!pImportData)
	{
		throw Error("Failed to locate Import directory");
	}

	const IMAGE_DATA_DIRECTORY *pIATData = GetDirectoryData(handle, IMAGE_DIRECTORY_ENTRY_IAT);
	if (!pIATData)
	{
		throw Error("Failed to locate IAT");
	}

	void *pIAT = RVA(handle, pIATData->VirtualAddress);

	// make IAT writable
	MemoryProtectionGuard writeGuard(pIAT, pIATData->Size, PAGE_READWRITE);

	IMAGE_IMPORT_DESCRIPTOR *imports = static_cast<IMAGE_IMPORT_DESCRIPTOR*>(RVA(handle, pImportData->VirtualAddress));

	// fill IAT with function pointers
	for (IMAGE_IMPORT_DESCRIPTOR *pImport = imports; pImport->Name && pImport->FirstThunk; pImport++)
	{
		const char *dllName = static_cast<const char*>(RVA(handle, pImport->Name));

		HMODULE dll = FindLoadedModule(dllName);
		if (!dll)
		{
			dll = LoadLibraryA(dllName);
			if (!dll)
			{
				throw SystemError(Format("Failed to load %s", dllName));
			}
		}

		IMAGE_THUNK_DATA *thunks = static_cast<IMAGE_THUNK_DATA*>(RVA(handle, pImport->FirstThunk));

		for (IMAGE_THUNK_DATA *pThunk = thunks; pThunk->u1.Ordinal; pThunk++)
		{
			void *pFunc = NULL;

			if (IMAGE_SNAP_BY_ORDINAL(pThunk->u1.Ordinal))
			{
				const unsigned int id = IMAGE_ORDINAL(pThunk->u1.Ordinal);

				pFunc = reinterpret_cast<void*>(GetProcAddress(dll, reinterpret_cast<const char*>(id)));
				if (!pFunc)
				{
					throw SystemError(Format("Failed to get function %d from %s", id, dllName));
				}
			}
			else
			{
				const size_t dataOffset = pThunk->u1.AddressOfData;

				IMAGE_IMPORT_BY_NAME *pData = static_cast<IMAGE_IMPORT_BY_NAME*>(RVA(handle, dataOffset));

				const char *name = reinterpret_cast<const char*>(pData->Name);

				pFunc = reinterpret_cast<void*>(GetProcAddress(dll, name));
				if (!pFunc)
				{
					throw SystemError(Format("Failed to get function %s from %s", name, dllName));
				}
			}

			pThunk->u1.Function = reinterpret_cast<uintptr_t>(pFunc);
		}
	}
}

namespace
{
	typedef int  (__cdecl *TGlobalInitializerC)();
	typedef void (__cdecl *TGlobalInitializerCPP)();

	struct GlobalInitializerList
	{
		TGlobalInitializerC *c;
		size_t c_count;
		TGlobalInitializerCPP *cpp;
		size_t cpp_count;

		GlobalInitializerList()
		: c(NULL),
		  c_count(0),
		  cpp(NULL),
		  cpp_count(0)
		{
		}
	};

	GlobalInitializerList FindGlobalInitializers(void *pDLL)
	{
		// there's no reliable way to locate list of global initializers in compiled binary
		// but we can exploit the fact that MSVC places global initializers right after the IAT in rdata section
		// so let's get IAT location first
		const IMAGE_DATA_DIRECTORY *pIATData = GetDirectoryData(pDLL, IMAGE_DIRECTORY_ENTRY_IAT);
		if (!pIATData)
		{
			throw Error("Failed to locate IAT");
		}

		void **cpp_begin = static_cast<void**>(RVA(pDLL, pIATData->VirtualAddress + pIATData->Size));
		void **cpp_end = cpp_begin;

		void *pIAT = RVA(pDLL, pIATData->VirtualAddress);

		while (*cpp_end < pIAT)
		{
			cpp_end++;
		}

		void **c_end = cpp_end;

		if (cpp_end > cpp_begin)
		{
			cpp_end--;
		}

		while (*cpp_end == NULL && cpp_end > cpp_begin)
		{
			cpp_end--;
		}

		while (*cpp_end != NULL && cpp_end > cpp_begin)
		{
			cpp_end--;
		}

		void **c_begin = cpp_end;

		GlobalInitializerList result;
		result.c = reinterpret_cast<TGlobalInitializerC*>(c_begin);
		result.c_count = c_end - c_begin;
		result.cpp = reinterpret_cast<TGlobalInitializerCPP*>(cpp_begin);
		result.cpp_count = cpp_end - cpp_begin;

		return result;
	}
}

void EXELoader::CallGlobalInitializers(const DLL & exe)
{
	const GlobalInitializerList list = FindGlobalInitializers(exe.getHandle());

	// C initializers
	for (size_t i = 0; i < list.c_count; i++)
	{
		TGlobalInitializerC initializer = list.c[i];

		if (initializer)
		{
			int status = initializer();

			if (status != 0)
			{
				throw Error(Format("Global initializer failed with error code %d", status));
			}
		}
	}

	// C++ initializers
	for (size_t i = 0; i < list.cpp_count; i++)
	{
		TGlobalInitializerCPP initializer = list.cpp[i];

		if (initializer)
		{
			initializer();
		}
	}
}
