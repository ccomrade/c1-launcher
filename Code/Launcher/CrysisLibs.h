/**
 * @file
 * @brief Crysis DLL loader.
 */

#pragma once

#include "DLL.h"

struct CrysisLibs
{
	enum EType
	{
		GAME, DEDICATED_SERVER
	};

private:
	EType m_type;
	unsigned int m_gameVersion;
	DLL m_CryAction;
	DLL m_CryGame;
	DLL m_CryNetwork;
	DLL m_CrySystem;

	void load();

public:
	CrysisLibs(EType type)
	: m_type(type),
	  m_gameVersion(0),
	  m_CryAction(),
	  m_CryGame(),
	  m_CryNetwork(),
	  m_CrySystem()
	{
		load();
	}

	EType getType() const
	{
		return m_type;
	}

	unsigned int getGameVersion() const
	{
		return m_gameVersion;
	}

	const DLL & getCryAction() const
	{
		return m_CryAction;
	}

	const DLL & getCryGame() const
	{
		return m_CryGame;
	}

	const DLL & getCryNetwork() const
	{
		return m_CryNetwork;
	}

	const DLL & getCrySystem() const
	{
		return m_CrySystem;
	}

	bool isCrysis() const;
	bool isCrysisWars() const;
	bool isCrysisWarhead() const;
};
