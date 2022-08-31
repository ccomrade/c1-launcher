// Copyright (C) 2001-2004 Crytek GmbH

#pragma once

#include <cstddef>

struct IGame;

class IGameRef
{
	IGame** m_ptr;

public:
	IGameRef() : m_ptr(NULL)
	{
	}

	IGameRef(IGame** ptr) : m_ptr(ptr)
	{
	}

	IGameRef& operator=(IGame** ptr)
	{
		m_ptr = ptr;

		return *this;
	}

	IGame* operator->() const
	{
		return (m_ptr) ? *m_ptr : NULL;
	}

	operator IGame*() const
	{
		return (m_ptr) ? *m_ptr : NULL;
	}
};
