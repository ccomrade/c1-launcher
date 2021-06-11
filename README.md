# c1-launcher

An open-source replacement of the original [Crysis](https://en.wikipedia.org/wiki/Crysis_(video_game)) executables.
It provides a better game launcher and dedicated server launcher with additional features.

## Features

- Full source code is available.
- Supports all game versions in one executable.
- Completely DRM-free launcher without any broken SecuROM crap inside.
- No compatibility mode is required to make the game work on modern systems.
- Fixes all startup issues, including the well-known crash of 32-bit Crysis on modern AMD processors.
- Smaller files, especially game launcher.
- Annoying startup video ads are automatically skipped.
- Very High settings are available in DX9 mode.
- Game running in DX9 mode can connect to DX10 servers. Everything works just like in the DX10 mode.
- Server doesn't kick players with the same CD key anymore.
- It's possible to run multiple Crysis instances at once.
- Advantages of pre-ordered version are available to everyone.
- Details of any game or server crash are automatically written to the log file.
- The `connect [IP] [PORT]` console command now works even without GameSpy account.

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

## Where to get the game

### Crysis

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 5767  | Y      | Y      | Y         |
| [GOG](https://www.gog.com/game/crysis)                            | 1.2.1   | 6156  | Y      | Y      | Y         |
| [Steam](https://store.steampowered.com/app/17300/Crysis/)         | 1.2.1   | 6156  | Y      | -      | Y         |
| Origin                                                            | 1.2.1   | 6156  | Y      | Y      | Y         |

- The latest official version is 1.2.1 (Build 6156).
- DVD version can be updated by installing the
[1.2](https://crysis.nullptr.one/Crysis_Patch_1_2.exe) and
[1.2.1](https://crysis.nullptr.one/Crysis_Patch_1_2_1.exe) official patches.
- The missing 64-bit files in Steam version can be downloaded
[here](https://crysis.nullptr.one/Crysis_6156_Bin64.zip).

### Crysis Wars

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 6527  | Y      | -      | Y         |
| Trial version                                                     | 1.0     | 6527  | Y      | -      | Y         |
| [GOG (Warhead bundle)](https://www.gog.com/game/crysiswarhead)    | 1.5     | 6729  | -      | Y      | Y         |
| Steam (Warhead bundle)                                            | 1.5     | 6729  | Y      | Y      | Y         |
| Origin (Warhead bundle)                                           | 1.5     | 6729  | Y      | Y      | Y         |

- The latest official version is 1.5 (Build 6729).
- DVD and Trial versions can be updated by installing the
[1.5](https://crysis.nullptr.one/CrysisWars_patch5.exe) official patch.
It also adds the missing 64-bit files.

## FAQ

### How to use it?

Get the latest executables from the [release section](../../releases) and just replace the original files. That's it.

You can also compile the source code yourself. See below for [instructions](#build-instructions).

Remember that you should always use a legal copy of Crysis.
If you still don't have one, see the [list](#where-to-get-the-game) above for where you can get it.

### I don't want to replace the original executables. Is there any way?

Yes, you can rename the executables of this launcher to whatever you want and use them next to the original files.

### How can I play Crysis multiplayer?

Official multiplayer has been deactivated by [GameSpy shutdown](https://en.wikipedia.org/wiki/GameSpy#Shutdown).
However, there are community projects that provide their own multiplayer services.
The most successful one is [CryMP Network](https://crymp.net).

For Crysis Wars take a look [here](https://crysiswarsmp.com) or [here](https://cryserv.eu).

### How can I run Crysis in DX9 mode instead of the default DX10 mode?

Just launch the game with `-dx9` command line parameter.

### Does Crysis support screen resolutions higher than 1080p?

Yes, it does. There is a scrollbar in the resolution list.

### Does this launcher support Crysis Remastered?

No, it doesn't.

### Does this launcher support Crysis Warhead?

No, it doesn't.

### Why does the Bin64 folder contain 2 executables?

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
