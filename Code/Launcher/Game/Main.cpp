/**
 * @file
 * @brief Game launcher.
 */

 // CryEngine headers
#include "CryModuleDefs.h"
#include "platform_impl.h"
#include "platform.h"
#include "IGameStartup.h"

// Launcher headers
#include "Patch.h"
#include "Util.h"

// tell to nVidia GPU driver that Crysis needs powerful graphics card and not the slow one (for multi-GPU laptops)
extern "C" __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// do the same as above for AMD GPU driver
// TODO: it seems this thing has no effect on modern AMD hardware, so user has to explicitly choose faster GPU
extern "C" __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

static void ErrorBox( const char *msg )
{
	MessageBoxA( NULL, msg, "Error", MB_OK | MB_DEFAULT_DESKTOP_ONLY );
}

static int RunGame( HMODULE libCryGame )
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
	params.sLogFileName = "Game.log";

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

static int InstallMemoryPatches( int version, void *libCryGame, void *libCryAction, void *libCryNetwork, void *libCrySystem )
{
	// CryAction

	if ( PatchDX9ImmersiveMultiplayer( libCryAction, version ) < 0 )
		return -1;

	// CryGame

	if ( PatchSkipIntros( libCryGame, version ) < 0 )
		return -1;

	if ( PatchCanJoinDX10Servers( libCryGame, version ) < 0 )
		return -1;

	if ( PatchFlashMenuDX10( libCryGame, version ) < 0 )
		return -1;

	// CryNetwork

	if ( PatchDuplicateCDKey( libCryNetwork, version ) < 0 )  // useful for non-dedicated servers
		return -1;

	// CrySystem

	if ( Patch64BitSecuROM( libCrySystem, version ) < 0 )
		return -1;

	if ( PatchDX9VeryHighSpec( libCrySystem, version ) < 0 )
		return -1;

	if ( PatchMultipleInstances( libCrySystem, version ) < 0 )
		return -1;

	if ( PatchUnhandledExceptions( libCrySystem, version ) < 0 )
		return -1;

	if ( HasAMDProcessor() && ! Is3DNowSupported() )
	{
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

	HMODULE libCryAction = LoadLibraryA( "CryAction.dll" );
	if ( libCryAction == NULL )
	{
		ErrorBox( "Unable to load the CryAction DLL!" );
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
			if ( InstallMemoryPatches( gameVersion, libCryGame, libCryAction, libCryNetwork, libCrySystem ) < 0 )
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

	// launch the game
	int status = RunGame( libCryGame );

	FreeLibrary( libCrySystem );
	FreeLibrary( libCryNetwork );
	FreeLibrary( libCryAction );
	FreeLibrary( libCryGame );

	return status;
}

