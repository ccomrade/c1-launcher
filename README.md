# cw-launcher

An open-source replacement of the original [Crysis Wars](https://en.wikipedia.org/wiki/Crysis_Warhead#Crysis_Wars) executables.
It provides a better game launcher and dedicated server launcher with additional features.

## Features

- Full source code is available.
- Supports all game versions in one executable.
- Completely DRM-free launcher.
- No compatibility mode is required to make the game work on modern systems.
- Fixes all startup issues, including the well-known crash of 32-bit Crysis Wars on modern AMD processors.
- Smaller files, especially game launcher.
- Annoying startup video ads are automatically skipped.
- Very High settings are available in DX9 mode.
- Game running in DX9 mode can connect to DX10 servers. Everything works just like in the DX10 mode.
- Server does not kick players with the same CD key anymore.
- Added engine based FPS cap for minimum input latency
- It is possible to run multiple Crysis Wars instances at once.
- Advantages of pre-ordered version are available to everyone.
- Details of any game or server crash are automatically written to the log file.
- The `connect [IP] [PORT]` console command now works without a GameSpy account.
- Fixes the low refresh rate bug in DX10 mode.
- Multiplayer works via a Gamespy replacement (crysiswarsmp.com)
- Disabled server profiler for 32-bit dedicated servers
- Disabled autoexec.cfg for the dedicated server (enables sharing an install with a client)
- Disabled some spam log messages to make Server.log more readable
- Fixed warping lag issue for high ping players! https://www.youtube.com/watch?v=BUrUXef3n4Q

## Supported game versions

### Crysis

| Version | Build | 32-bit | 64-bit |
| :-----: | :---: | :----: | :----: |
| 1.0     | 5767  | Y      | Y      |
| 1.1     | 5879  | Y      | Y      |
| 1.2     | 6115  | Y      | Y      |
| 1.2.1   | 6156  | Y      | Y      |

### Crysis Wars

| Version | Build | 32-bit | 64-bit |
| :-----: | :---: | :----: | :----: |
| 1.0     | 6527  | Y      | -      |
| 1.1     | 6566  | Y      | Y      |
| 1.2     | 6586  | Y      | Y      |
| 1.3     | 6627  | Y      | Y      |
| 1.4     | 6670  | Y      | Y      |
| 1.5     | 6729  | Y      | Y      |

Some patches are only supported on Crysis Wars patch 1.5

## Where to get the game

### Crysis

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 5767  | Y      | Y      | Y         |
| [GOG](https://www.gog.com/game/crysis)                            | 1.2.1   | 6156  | Y      | Y      | Y         |
| [Steam](https://store.steampowered.com/app/17300/Crysis/)         | 1.2.1   | 6156  | Y      | -      | Y         |
| Origin                                                            | 1.2.1   | 6156  | Y      | Y      | Y         |

- The latest official version is 1.2.1 (6156).
- If you have DVD version, install the
[1.2](https://crysis.nullptr.one/Crysis_Patch_1_2.exe) and
[1.2.1](https://crysis.nullptr.one/Crysis_Patch_1_2_1.exe) official patches to get the latest version.
- If you have Steam version, missing 64-bit files can be downloaded
[here](https://crysis.nullptr.one/Crysis_6156_Bin64.zip).

### Crysis Wars

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 6527  | Y      | -      | Y         |
| Trial                                                             | 1.0     | 6527  | Y      | -      | Y         |
| [GOG (Warhead bundle)](https://www.gog.com/game/crysiswarhead)    | 1.5     | 6729  | -      | Y      | Y         |
| Steam (Warhead bundle)                                            | 1.5     | 6729  | Y      | Y      | Y         |
| Origin (Warhead bundle)                                           | 1.5     | 6729  | Y      | -      | Y         |

- The latest official version is 1.5 (6729).
- If you have DVD or Trial version, install the
[1.5](https://crysis.nullptr.one/CrysisWars_patch5.exe) official patch to get the latest version and 64-bit files.

### FPS cap

The launcher implents an FPS limit in the game itself. This provides lower input latency compared to 3rd party FPS limiters
such as RTSS or the options in the Nvidia/AMD drivers. This feature is disabled by default unless a mod enables it via the
provided interface in ILauncher.h. The Falcon mod implements this interface in the on change callback of sys_MaxFPS.

Sample code to add support to a 3rd party mod:
```c++
ILauncher *pLauncher = NULL;

HMODULE hExe = GetModuleHandleA( NULL );
ILauncher::TGetFunc pGetILauncher = (ILauncher::TGetFunc) GetProcAddress( hExe, "GetILauncher" );
if ( pGetILauncher )
{
	pLauncher = pGetILauncher();
}

if ( pLauncher )
{
	// Launcher API is available
	pLauncher->SetFPSCap(120);
}
else
{
	// Launcher API is not available
}
```

## FAQ

### How to use it?

Get the latest executables from the [release section](../../releases) and just replace the original files. That's it.

You can also compile the source code yourself. See below for [instructions](#build-instructions).

### I don't want to replace the original executables. Is there any way?

Yes, you can rename the executables of this launcher to whatever you want and use them next to the original files.

### How can I play Crysis Wars multiplayer?

The official multiplayer no longer works due to [GameSpy shutdown](https://en.wikipedia.org/wiki/GameSpy#Shutdown).
However, there is a community project that provide a replacement multiplayer service [here](https://crysiswarsmp.com).

### How can I run Crysis Wars in DX9 mode instead of the default DX10 mode?

Just launch the game with `-dx9` command line parameter.

### Is it possible to disable automatic skipping of startup video ads?

Yes, launch the game with `-splash` command line parameter.

### Does Crysis Wars support screen resolutions higher than 1080p?

Yes, it does. There is a scrollbar in the resolution list.

### Does this launcher support Crysis Remastered?

No, it doesn't.

### Does this launcher support Crysis Warhead?

No, it doesn't. The reason is that CryGame and CryAction DLLs have been moved into the EXE in Crysis Warhead.

### Why does the Crysis Bin64 folder contain 2 executables?

The original `Bin64/Crysis.exe` file is actually only a 32-bit SecuROM DRM launcher. It reads the `Bin64/Crysis.ini`
config file and runs `Bin64/crysis64.exe`, which is the 64-bit game launcher. However, running the game launcher
directly results in a crash during game startup. This is because the 64-bit CrySystem loads the `Bin64/b64.dll`
library with additional SecuROM garbage. It checks whether the game was launched using the DRM launcher or not,
and if not, the game crashes in a sneaky way.

All this nonsense is skipped by this launcher. That means you can safely delete all the mentioned files.

## Build instructions

### Requirements

- Microsoft Visual C++ compiler (MSVC) >= VS2005
- CMake >= 3.15

No other compilers are supported because Crytek used the MSVC compiler to build Crysis, and the same ABI is required.
You don't have to install the highly bloated Visual Studio to get the MSVC compiler. All you need is the Windows SDK.

### Building

#### Using Windows SDK (recommended)

1. Download the source code.
2. Create two empty build directories named `Build32` and `Build64` inside the source code directory.
3. Open the 32-bit Windows SDK command prompt in the newly created `Build32` directory and run the following commands:

```
cmake -G "NMake Makefiles" -D CMAKE_BUILD_TYPE=Release ..
cmake --build .
```

4. Repeat the previous step using the 64-bit Windows SDK command prompt and the `Build64` directory.

#### Using Visual Studio

Modern Visual Studio (VS2019) allows you to directly open the source code directory and compile the project as usual.

Older versions of Visual Studio require manually generated solution files.
You can use either the `cmake-gui` tool or the same steps as above with the following command:

```
cmake -G "Visual Studio 10 2010" -A Win32 ..
```

Use the appropriate version of Visual Studio. For 64-bit build replace `Win32` with `x64`.
