#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "DLL.h"

void DLL::release()
{
	FreeLibrary(static_cast<HMODULE>(m_handle));
}

bool DLL::load(const char *file, int flags)
{
	unload();

	m_flags = flags;
	m_handle = (flags & NO_LOAD) ? GetModuleHandleA(file) : LoadLibraryA(file);

	return isLoaded();
}

void *DLL::getSymbolAddress(const char *name) const
{
	if (!isLoaded())
	{
		return NULL;
	}

	return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(m_handle), name));
}
