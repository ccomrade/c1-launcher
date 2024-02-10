# C1-Launcher

An open-source replacement of [Crysis (2007)](https://en.wikipedia.org/wiki/Crysis_(video_game)) executables.
It provides a better game and dedicated server launcher with additional features.

## Features

- Supports all game versions in one executable.
- Provides additional headless server executable optimized for running Crysis servers on Linux.
- Completely DRM-free launcher.
- No compatibility mode is required to make the game work on modern systems.
- Fixes all startup issues, including the well-known crash of 32-bit Crysis on modern AMD processors.
- Fixes engine crash on systems with more than 32 logical processors.
- Fixes low refresh rate in fullscreen DX10 mode. Thanks to Guzz and Vladislav.
- Smaller files, especially game launcher.
- Annoying startup video ads are automatically skipped.
- Max settings (Very High) are available in DX9 mode as well.
- Game running in DX9 mode can connect to DX10 servers. Everything works just like in DX10 mode.
- Server does not kick players with the same CD key anymore.
- It is possible to run multiple Crysis instances at once.
- Advantages of pre-ordered version are available to everyone.
- Details of any game or server crash are automatically written to the log file.
- The `connect [IP] [PORT]` console command works without a GameSpy account.
- Language detection to workaround incomplete localization files in
[Crysis from Steam](https://store.steampowered.com/app/17300/Crysis/).

## Supported game versions

### Crysis (2007)

| Version | Build | 32-bit | 64-bit |
| :-----: | :---: | :----: | :----: |
| 1.0     | 5767  | Y      | Y      |
| 1.1     | 5879  | Y      | Y      |
| 1.2     | 6115  | Y      | Y      |
| 1.2.1   | 6156  | Y      | Y      |

### Crysis Warhead (2008)

| Version | Build | 32-bit | 64-bit |
| :-----: | :---: | :----: | :----: |
| 1.0     | 687   | N      | -      |
| 1.1     | 710   | N      | Y      |
| 1.1.1   | 711   | N      | Y      |

### Crysis Wars (2008)

| Version | Build | 32-bit | 64-bit |
| :-----: | :---: | :----: | :----: |
| 1.0     | 6527  | Y      | -      |
| 1.1     | 6566  | Y      | Y      |
| 1.2     | 6586  | Y      | Y      |
| 1.3     | 6627  | Y      | Y      |
| 1.4     | 6670  | Y      | Y      |
| 1.5     | 6729  | Y      | Y      |

### Crysis Editor (Sandbox 2)

| Version     | Build | 32-bit | 64-bit |
| :---------- | :---: | :----: | :----: |
| Crysis      | 5767  | Y      | Y      |
| Crysis Wars | 6670  | Y      | Y      |

## Where to get the game

### Crysis (2007)

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 5767  | Y      | Y      | Y         |
| [GOG](https://www.gog.com/game/crysis)                            | 1.2.1   | 6156  | Y      | Y      | Y         |
| [Steam](https://store.steampowered.com/app/17300/Crysis/)         | 1.2.1   | 6156  | Y      | Y      | Y         |
| [EA](https://www.ea.com/games/crysis/crysis)                      | 1.2.1   | 6156  | Y      | Y      | Y         |

- The latest official version is 1.2.1 (6156).
- If you have DVD version, install the [1.2](https://crysis.nullptr.one/Crysis_Patch_1_2.exe) and
[1.2.1](https://crysis.nullptr.one/Crysis_Patch_1_2_1.exe) official patches to get the latest version.

### Crysis Warhead (2008)

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported  |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :--------: |
| DVD                                                               | 1.0     | 687   | Y      | -      | see below  |
| [GOG](https://www.gog.com/game/crysiswarhead)                     | 1.1.1   | 711   | -      | Y      | Y          |
| [Steam](https://store.steampowered.com/app/17330/Crysis_Warhead/) | 1.1.1   | 711   | Y      | Y      | Y (64-bit) |
| [EA](https://www.ea.com/games/crysis-warhead)                     | 1.1.1   | 711   | Y      | Y      | Y (64-bit) |

- The latest official version is 1.1.1 (711).
- If you have DVD version, install the [1.1.1](https://crysis.nullptr.one/CrysisWarhead_Patch1_1.exe) official patch
to get the latest version and 64-bit files. C1-Launcher supports only 64-bit version of Crysis Warhead, so this step
is necessary.

### Crysis Wars (2008)

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 6527  | Y      | -      | Y         |
| Trial                                                             | 1.0     | 6527  | Y      | -      | Y         |
| [GOG](https://www.gog.com/game/crysiswarhead)                     | 1.5     | 6729  | -      | Y      | Y         |
| [Steam](https://store.steampowered.com/app/17330/Crysis_Warhead/) | 1.5     | 6729  | Y      | Y      | Y         |
| [EA](https://www.ea.com/games/crysis-warhead)                     | 1.0     | 6527  | Y      | -      | Y         |

- Bundled together with Crysis Warhead.
- The latest official version is 1.5 (6729).
- If you have DVD, Trial or EA version, install the [1.5](https://crysis.nullptr.one/CrysisWars_patch5.exe)
official patch to get the latest version and 64-bit files.

## FAQ

### How to use it?

Get the latest executables from [releases](../../releases) and just replace the original files.

Remember that you should always use a legal copy of Crysis!

### Is there any way to avoid replacing the original executables?

Yes, you can rename C1-Launcher executables to whatever you want and use them next to the original files.

### How can I play Crysis multiplayer?

The official multiplayer no longer works due to [GameSpy shutdown](https://en.wikipedia.org/wiki/GameSpy#Shutdown).
However, there is [CryMP Network](https://crymp.net) community providing their own multiplayer services.

For Crysis Wars take a look [here](https://crysiswarsmp.com) or [here](https://cryserv.de).

### How can I run Crysis in DX9 mode instead of the default DX10 mode?

Just launch the game with `-dx9` command line parameter.

### Is it possible to disable automatic skipping of startup video ads?

Yes, launch the game with `-splash` command line parameter.

### Does Crysis support screen resolutions higher than 1080p?

Yes, it does. There is a scrollbar in the resolution list.

### Does C1-Launcher support Crysis Remastered?

No, it does not.

### Does C1-Launcher support Crysis Warhead?

No, it does not. The reason is that CryGame and CryAction DLLs are integrated into the EXE in Crysis Warhead.

### Why does the Bin64 folder contain 2 executables?

The original `Bin64/Crysis.exe` file is actually only a 32-bit SecuROM DRM launcher. It reads the `Bin64/Crysis.ini`
config file and runs `Bin64/crysis64.exe`, which is the 64-bit game launcher. However, running the game launcher
directly results in a crash during game startup. This is because 64-bit CrySystem loads the `Bin64/b64.dll` library
with additional SecuROM garbage. It checks whether the game was launched using the DRM launcher, and if not, the game
crashes.

All this nonsense is skipped by C1-Launcher. That means you can safely delete all the mentioned files.

## Build instructions

### Requirements

- Microsoft Visual C++ compiler (MSVC) >= VS2005
- CMake >= 3.15

The source code is still fully compatible with the legacy VS2005 compiler that was used to build Crysis.
It is also used to build C1-Launcher releases to minimize their size and maximize compatibility.

MSVC is the only supported compiler due to required ABI compatibility with Crysis DLLs.

### Building

#### Using Native Tools Command Prompt

1. Download the source code.
2. Create `Build` directory inside the source code directory.
3. Create two empty directories named `Bin32` and `Bin64` inside the `Build` directory.
4. Open the x86 Native Tools Command Prompt in the `Build/Bin32` directory and run the following commands:

```
cmake -G "NMake Makefiles" -D CMAKE_BUILD_TYPE=Release ../..
cmake --build .
```

5. Repeat the previous step with the x64 Native Tools Command Prompt and the `Build/Bin64` directory.

#### Using Visual Studio

Modern Visual Studio (>= VS2019) and [Visual Studio Code](https://code.visualstudio.com/docs/cpp/config-msvc) allow you
to directly open the source code directory as a CMake project and build it as usual.

Older versions of Visual Studio require manually generated solution files.
Use the same steps as above with the following command:

```
cmake -G "Visual Studio 10 2010" -A Win32 ../..
```

Choose the appropriate version of Visual Studio. For 64-bit build replace `Win32` with `x64`.
