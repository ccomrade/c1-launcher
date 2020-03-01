#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "DLL.h"

void DLL::release()
{
	if (m_handle && !(m_flags & NO_LOAD) && !(m_flags & NO_RELEASE))
	{
		FreeLibrary(static_cast<HMODULE>(m_handle));
	}

	m_handle = NULL;
	m_flags = 0;
}

bool DLL::load(const char *file, int flags)
{
	release();

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
