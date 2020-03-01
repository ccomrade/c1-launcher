#pragma once

#include <cstddef>

struct DLL
{
	enum ELoadFlags
	{
		NO_RELEASE = (1 << 0),  //!< Do not unload the library.
		NO_LOAD    = (1 << 1)   //!< Do not load and unload the library. Only obtain handle to it.
	};

private:
	void *m_handle;
	int m_flags;

	// disable implicit copy constructor and copy assignment operator
	DLL(const DLL &);
	DLL & operator=(const DLL &);

public:
	DLL()
	: m_handle(NULL),
	  m_flags(0)
	{
	}

	~DLL()
	{
		release();
	}

	void release();

	bool load(const char *file, int flags = 0);

	bool isLoaded() const
	{
		return m_handle != NULL;
	}

	void *getHandle() const
	{
		return m_handle;
	}

	int getFlags() const
	{
		return m_flags;
	}

	void *getSymbolAddress(const char *name) const;

	template<class T>
	T getSymbol(const char *name) const
	{
		return reinterpret_cast<T>(getSymbolAddress(name));
	}
};
