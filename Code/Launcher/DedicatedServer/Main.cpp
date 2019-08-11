/**
 * @file
 * @brief Dedicated server launcher.
 */

// CryEngine headers
#include "CryModuleDefs.h"
#include "platform_impl.h"
#include "platform.h"
#include "IGameStartup.h"

// Launcher headers
#include "Patch.h"
#include "Util.h"

static void ErrorBox( const char *msg )
{
	MessageBoxA( NULL, msg, "Error", MB_OK | MB_DEFAULT_DESKTOP_ONLY );
}

static int RunServer( HMODULE libCryGame )
{
	IGameStartup::TEntryFunction fCreateGameStartup;

	fCreateGameStartup = (IGameStartup::TEntryFunction) GetProcAddress( libCryGame, "CreateGameStartup" );
	if ( fCreateGameStartup == NULL )
	{
		ErrorBox( "The CryGame DLL is not valid!" );
		return 1;
	}

	IGameStartup *pGameStartup = fCreateGameStartup();
	if ( pGameStartup == NULL )
	{
		ErrorBox( "Unable to create the GameStartup interface!" );
		return 1;
	}

	const char *cmdLine = GetCommandLineA();
	const size_t cmdLineLength = strlen( cmdLine );

	SSystemInitParams params;
	memset( &params, 0, sizeof params );

	params.hInstance = GetModuleHandle( NULL );
	params.sLogFileName = "Server.log";
	params.bDedicatedServer = true;

	if ( cmdLineLength < sizeof params.szSystemCmdLine )
	{
		strcpy( params.szSystemCmdLine, cmdLine );
	}
	else
	{
		ErrorBox( "Command line is too long!" );
		return 1;
	}

	// init engine
	IGameRef gameRef = pGameStartup->Init( params );
	if ( gameRef == NULL )
	{
		ErrorBox( "Game initialization failed!" );
		pGameStartup->Shutdown();
		return 1;
	}

	// enter update loop
	pGameStartup->Run( NULL );

	pGameStartup->Shutdown();

	return 0;
}

static int InstallMemoryPatches( int version, void *libCryNetwork, void *libCrySystem )
{
	// CryNetwork

	if ( PatchDuplicateCDKey( libCryNetwork, version ) < 0 )
		return -1;

	// CrySystem

	if ( PatchUnhandledExceptions( libCrySystem, version ) < 0 )
		return -1;

	if ( HasAMDProcessor() && ! Is3DNowSupported() )
	{
		// dedicated server usually doesn't execute any code with 3DNow! instructions
		// but we should still make sure that ISystem::GetCPUFlags always returns correct flags

		if ( PatchDisable3DNow( libCrySystem, version ) < 0 )
			return -1;
	}

	return 0;
}

int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	HMODULE libCryGame = LoadLibraryA( "CryGame.dll" );
	if ( libCryGame == NULL )
	{
		ErrorBox( "Unable to load the CryGame DLL!" );
		return 1;
	}

	HMODULE libCryNetwork = LoadLibraryA( "CryNetwork.dll" );
	if ( libCryNetwork == NULL )
	{
		ErrorBox( "Unable to load the CryNetwork DLL!" );
		return 1;
	}

	HMODULE libCrySystem = LoadLibraryA( "CrySystem.dll" );
	if ( libCrySystem == NULL )
	{
		ErrorBox( "Unable to load the CrySystem DLL!" );
		return 1;
	}

	// obtain game build number from CrySystem DLL
	int gameVersion = GetCrysisGameVersion( libCrySystem );
	if ( gameVersion < 0 )
	{
		ErrorBox( "Unable to obtain game version from the CrySystem DLL!" );
		return 1;
	}

	// check vesion of the game and apply memory patches
	switch ( gameVersion )
	{
		case 5767:
		case 5879:
		case 6115:
		case 6156:
		{
			if ( InstallMemoryPatches( gameVersion, libCryNetwork, libCrySystem ) < 0 )
			{
				ErrorBox( "Unable to apply memory patch!" );
				return 1;
			}
			break;
		}
		default:
		{
			ErrorBox( "Unsupported version of the game!" );
			return 1;
		}
	}

	// launch the server
	int status = RunServer( libCryGame );

	FreeLibrary( libCrySystem );
	FreeLibrary( libCryNetwork );
	FreeLibrary( libCryGame );

	return status;
}

