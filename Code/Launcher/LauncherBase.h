#pragma once

#include <cstring>
#include <stdexcept>

#include "CryCommon/CryGame/IGameStartup.h"
#include "CryCommon/CrySystem/ISystem.h"

#include "Library/DLL.h"
#include "Library/Format.h"

class LauncherBase
{
protected:
	IGameStartup* m_pGameStartup;
	SSystemInitParams m_params;
	int m_gameBuild;

public:
	LauncherBase() : m_pGameStartup(NULL), m_params(), m_gameBuild(0)
	{
	}

	~LauncherBase()
	{
		if (m_pGameStartup)
		{
			m_pGameStartup->Shutdown();
		}
	}

protected:
	void SetParamsCmdLine(const char* cmdLine)
	{
		const std::size_t length = std::strlen(cmdLine);

		if (length >= sizeof m_params.cmdLine)
		{
			throw std::runtime_error("Command line is too long!");
		}

		std::memcpy(m_params.cmdLine, cmdLine, length + 1);
	}

	void VerifyGameBuild()
	{
		switch (m_gameBuild)
		{
			case 5767:
			case 5879:
			case 6115:
			case 6156:
			{
				// Crysis
				break;
			}
#ifndef BUILD_64BIT
			// 64-bit binaries are missing in the first build of Crysis Wars
			case 6527:
#endif
			case 6566:
			case 6586:
			case 6627:
			case 6670:
			case 6729:
			{
				// Crysis Wars
				break;
			}
			case 687:
			case 710:
			case 711:
			{
				// Crysis Warhead
				throw std::runtime_error("Crysis Warhead is not supported!");
			}
			default:
			{
				throw std::runtime_error(Format("Unknown game build %d", m_gameBuild));
			}
		}
	}

	void StartEngine(const DLL& game)
	{
		IGameStartup::TEntryFunction entry = game.GetSymbol<IGameStartup::TEntryFunction>("CreateGameStartup");
		if (!entry)
		{
			throw std::runtime_error("The CryGame DLL is not valid!");
		}

		m_pGameStartup = entry();
		if (!m_pGameStartup)
		{
			throw std::runtime_error("Failed to create the GameStartup Interface!");
		}

		if (!m_pGameStartup->Init(m_params))
		{
			throw std::runtime_error("Game initialization failed!");
		}
	}

	int UpdateLoop()
	{
		return m_pGameStartup->Run(NULL);
	}
};
