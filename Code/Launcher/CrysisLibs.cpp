/**
 * @file
 * @brief Implementation of Crysis DLL loader.
 */

#include "CrysisLibs.h"
#include "System.h"
#include "Format.h"

void CrysisLibs::load()
{
	// Some Crysis DLLs may cause crash when being unloaded. That's why DLL::NO_RELEASE flag is used here.

	if (!m_CrySystem.load("CrySystem.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CrySystem DLL!");
	}

	m_gameVersion = System::GetCrysisGameBuild(m_CrySystem);
	if (!m_gameVersion)
	{
		throw SystemError("Failed to get the game version!");
	}

	if (!isCrysis() && !isCrysisWars() && !isCrysisWarhead())
	{
		throw Error(Format("Unknown game version %u!", m_gameVersion));
	}

	if (isCrysisWarhead())
	{
		throw Error("Crysis Warhead is not supported!");
	}

	if (!m_CryGame.load("CryGame.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CryGame DLL!");
	}

	if (!m_CryNetwork.load("CryNetwork.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CryNetwork DLL!");
	}

	if (m_type == GAME && !isCrysisWarhead())  // Crysis Warhead doesn't have the CryAction DLL
	{
		if (!m_CryAction.load("CryAction.dll", DLL::NO_RELEASE))
		{
			throw SystemError("Failed to load the CryAction DLL!");
		}
	}

	if (!m_CryRenderD3D10.load("CryRenderD3D10.dll", DLL::NO_RELEASE))
	{
		throw SystemError("Failed to load the CryRenderD3D10 DLL!");
	}
}

bool CrysisLibs::isCrysis() const
{
	switch (m_gameVersion)
	{
		case 5767:
		case 5879:
		case 6115:
		case 6156:
		{
			return true;
		}
	}

	return false;
}

bool CrysisLibs::isCrysisWars() const
{
	switch (m_gameVersion)
	{
		case 6527:
		case 6566:
		case 6586:
		case 6627:
		case 6670:
		case 6729:
		{
			return true;
		}
	}

	return false;
}

bool CrysisLibs::isCrysisWarhead() const
{
	switch (m_gameVersion)
	{
		case 687:
		case 710:
		case 711:
		{
			return true;
		}
	}

	return false;
}
