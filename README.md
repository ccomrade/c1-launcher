# c1-launcher

An open-source replacement of the original [Crysis](https://en.wikipedia.org/wiki/Crysis_(video_game)) executables.
It provides a better game launcher and dedicated server launcher with additional features.

## Features

- Full source code is available
- Supports all game versions in one executable
- Completely DRM-free launcher (no broken SecuROM crap inside)
- No compatibility mode is required to make the game work on modern systems
- Fixes all startup issues, including the well-known crash of 32-bit Crysis on modern AMD processors
- Smaller files, especially game launcher
- Annoying startup video ads are automatically skipped
- Very High settings are available in DX9 mode
- Game running in DX9 mode can connect to DX10 servers and everything works just like in the DX10 game
- Server doesn't kick players with the same CD key anymore
- It's possible to run multiple Crysis instances at once
- Advantages of pre-ordered version are available to everyone
- Details of any game or server crash are automatically written to the log file

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
| 1.0     | 6527  | Y      | N/A    |
| 1.1     | 6566  | Y      | Y      |
| 1.2     | 6586  | Y      | Y      |
| 1.3     | 6627  | Y      | Y      |
| 1.4     | 6670  | Y      | Y      |
| 1.5     | 6729  | Y      | Y      |

### Crysis Warhead (not supported yet)

| Version | Build | 32-bit | 64-bit |
| :-----: | :---: | :----: | :----: |
| 1.0     | 687   | -      | N/A    |
| 1.1     | 710   | -      | -      |
| 1.1.1   | 711   | -      | -      |

## Where to get the game

### Crysis

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 5767  | Y      | Y      | Y         |
| [GOG](https://www.gog.com/game/crysis)                            | 1.2.1   | 6156  | Y      | Y      | Y         |
| [Steam](https://store.steampowered.com/app/17300/Crysis/)         | 1.2.1   | 6156  | Y      | -      | Y         |
| Origin                                                            | 1.2.1   | 6156  | Y      | -      | Y         |

### Crysis Wars

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 6527  | Y      | -      | Y         |
| Trial version                                                     | 1.0     | 6527  | Y      | -      | Y         |
| [GOG (Warhead bundle)](https://www.gog.com/game/crysiswarhead)    | 1.5     | 6729  | Y      | Y      | Y         |
| Steam (Warhead bundle)                                            | 1.5     | 6729  | Y      | Y      | Y         |
| Origin (Warhead bundle)                                           | 1.5     | 6729  | Y      | Y      | Y         |

### Crysis Warhead

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :-------: |
| DVD                                                               | 1.0     | 687   | Y      | -      | -         |
| [GOG](https://www.gog.com/game/crysiswarhead)                     | 1.1.1   | 711   | -      | Y      | -         |
| [Steam](https://store.steampowered.com/app/17330/Crysis_Warhead/) | 1.1.1   | 711   | Y      | Y      | -         |
| Origin                                                            | 1.1.1   | 711   | Y      | Y      | -         |

## Official game patches

Patches marked with `*` require the previous patch to be installed.

### Crysis

| Patch                                                          | SHA-1 Checksum                           |
| :------------------------------------------------------------: | :--------------------------------------: |
| [1.1](https://crysis.nullptr.one/Crysis_Patch_1_1.exe)         | 1b7c7c48d38623bb55802f5e29d0c6780ba3e208 |
| [1.2](https://crysis.nullptr.one/Crysis_Patch_1_2.exe)         | 9ecb8947296a0356325bd73b28755fdabebfb67e |
| [1.2.1*](https://crysis.nullptr.one/Crysis_Patch_1_2_1.exe)    | e600606c1068f139ef71d77988bc8516913e0636 |

### Crysis Wars

| Patch                                                          | SHA-1 Checksum                           |
| :------------------------------------------------------------: | :--------------------------------------: |
| [1.1](https://crysis.nullptr.one/CrysisWars_patch1.exe)        | 8e41eec565487299a1c675b93467fbf9bcbfbaa3 |
| [1.2](https://crysis.nullptr.one/CrysisWars_patch2.exe)        | e258bbc3cb6f5d455eb35b3d3c9830297ee9b183 |
| [1.3](https://crysis.nullptr.one/CrysisWars_patch3.exe)        | dc6460d3cba7bb93820cc544881fe89eda2462b2 |
| [1.4](https://crysis.nullptr.one/CrysisWars_patch4.exe)        | 1bcff249e679b2bd666a2a5772783af3ebd4e349 |
| [1.5](https://crysis.nullptr.one/CrysisWars_patch5.exe)        | 4f368c09bbb551e2360d86d42cbd95f25b993e0f |

### Crysis Warhead

| Patch                                                          | SHA-1 Checksum                           |
| :------------------------------------------------------------: | :--------------------------------------: |
| [1.1](https://crysis.nullptr.one/CrysisWarhead_Patch1.exe)     | 859f8fb8e3798124c7c4afa650a30b35dbcc65cf |
| [1.1.1](https://crysis.nullptr.one/CrysisWarhead_Patch1_1.exe) | 926a9380bd7b911d9eb89ec8e080174f4ca3209c |

## FAQ

### How to use it?

Get the executables from [release section](../../releases) and just replace the original files. That's it.
You can also compile the source code yourself. See below for instructions.

You should always use a legal copy of Crysis. If you still don't have one, see the list above for where you can get it.

### I don't want to replace the original executables. Is there any way?

Yes, you can rename the executables of this launcher to whatever you want and use them next to the original executables.

### How can I play Crysis multiplayer?

Official multiplayer doesn't work anymore because of [GameSpy shutdown](https://en.wikipedia.org/wiki/GameSpy#Shutdown).
However, there are community projects that provide their own multiplayer services.
The most successful one is [CryMP Network](https://crymp.net).

### How can I run Crysis in DX9 mode instead of the default DX10 mode?

Just launch it with `-dx9` command line parameter.

### Does Crysis support screen resolutions higher than 1080p?

Yes, it does. There is a scrollbar in the resolution list.

### Why does the Bin64 folder contain 2 executables?

The original `Bin64/Crysis.exe` file is actually only a 32-bit SecuROM DRM launcher. It reads the `Bin64/Crysis.ini` config
file and runs `Bin64/crysis64.exe`, which is the 64-bit game launcher. However, running directly the game launcher usually
results in a crash during game startup. This is because the 64-bit CrySystem loads the `Bin64/b64.dll` file, which contains
additional SecuROM garbage. It checks whether the game was launched using the DRM launcher or not, and if not, the game
crashes in a sneaky way.

All of this nonsense is deactivated by the launcher provided here. That means you can safely delete all the mentioned files.

## Build instructions

### Requirements

- Microsoft Visual C++ compiler (MSVC) >= VS2005
- CMake >= 3.15

No other compilers are supported because Crysis was built using MSVC and the same ABI is required. You don't have to install
highly bloated Visual Studio to get the MSVC compiler. All you need is the Windows SDK. If you have modern Visual Studio
already installed, you probably don't need to install CMake because it's integrated there.

### Building

#### Using Windows SDK (recommended)

Open the Windows SDK command prompt and move to the directory with the source code. Then execute the following commands:

```
mkdir Build
cd Build
cmake -G "NMake Makefiles" -D CMAKE_BUILD_TYPE=Release ..
cmake --build .
```

If you want both 32-bit and 64-bit executables, use the appropriate Windows SDK command prompt and another build directory.

#### Using Visual Studio

With modern Visual Studio (VS2019) you can open directly the source code directory and compile the project as usual. Older
versions of Visual Studio require manually generated solution files. To do this, you can either open any command prompt and
use the same commands as above without the last command and with the appropriate generator matching your VS version, or use
the `cmake-gui` tool, which allows you to easily do the same without the command prompt. In any case, don't forget to change
the build type to `Release` and always use separate source and build directories.
