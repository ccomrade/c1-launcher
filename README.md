# C1-Launcher

An open-source replacement of [Crysis (2007)](https://en.wikipedia.org/wiki/Crysis_(video_game)) executables.
It provides a better game and dedicated server launcher with additional features.

## Features

- Supports all game versions in one executable.
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
- Fixes [two broken panels in Editor](https://github.com/ccomrade/c1-launcher/pull/38).
- New headless server launcher optimized for running Crysis servers on Linux.

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
- If you have DVD version, install the [1.2](https://comrade.one/Crysis_Patch_1_2.exe) and
[1.2.1](https://comrade.one/Crysis_Patch_1_2_1.exe) official patches to get the latest version.

### Crysis Warhead (2008)

| Platform                                                          | Version | Build | 32-bit | 64-bit | Supported  |
| :---------------------------------------------------------------: | :-----: | :---: | :----: | :----: | :--------: |
| DVD                                                               | 1.0     | 687   | Y      | -      | see below  |
| [GOG](https://www.gog.com/game/crysiswarhead)                     | 1.1.1   | 711   | -      | Y      | Y          |
| [Steam](https://store.steampowered.com/app/17330/Crysis_Warhead/) | 1.1.1   | 711   | Y      | Y      | Y (64-bit) |
| [EA](https://www.ea.com/games/crysis-warhead)                     | 1.1.1   | 711   | Y      | Y      | Y (64-bit) |

- The latest official version is 1.1.1 (711).
- If you have DVD version, install the [1.1.1](https://comrade.one/CrysisWarhead_Patch1_1.exe) official patch
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
- If you have DVD, Trial or EA version, install the [1.5](https://comrade.one/CrysisWars_patch5.exe)
official patch to get the latest version and 64-bit files.

## FAQ

### How to use it?

Get the latest [release](https://github.com/ccomrade/c1-launcher/releases) and just replace the original executables.

Note that Warhead and Editor launchers need the original executables, which is why the new ones have different names.

Remember that you should always use a legal copy of Crysis!

### Is there any way to avoid replacing the original executables?

Yes, you can rename C1-Launcher executables to whatever you want and use them next to the original ones.

### How can I play Crysis multiplayer?

The official multiplayer no longer works due to [GameSpy shutdown](https://en.wikipedia.org/wiki/GameSpy#Shutdown).
However, there is [CryMP](https://crymp.org) community providing their own multiplayer services.

For Crysis Wars take a look [here](https://crysiswarsmp.com) or [here](https://cryserv.de).

### Does Crysis support screen resolutions higher than 1080p?

Yes, it does. There is a scrollbar in the resolution list.

### Does C1-Launcher support Crysis Remastered?

No, it does not.

### Does C1-Launcher support Crysis Warhead?

Yes, it does, but only 64-bit version. The reason is that CryGame and CryAction DLLs are integrated into the EXE in
Crysis Warhead. This means the original EXEs are still needed. 32-bit one is encrypted by SecuROM, which makes it
essentially unusable. Luckily, 64-bit one is clean. See below why.

### Why does the Bin64 directory contain 2 executables?

The original `Bin64/Crysis.exe` file is actually only a 32-bit SecuROM DRM launcher. It reads the `Bin64/Crysis.ini`
config file and runs `Bin64/crysis64.exe`, which is the 64-bit game launcher. However, running the game launcher
directly results in a crash during game startup. This is because 64-bit CrySystem loads the `Bin64/b64.dll` library
with additional SecuROM garbage. It checks whether the game was launched using the DRM launcher, and if not, the game
crashes.

All this nonsense is skipped by C1-Launcher. That means you can safely delete all the mentioned files.

## Command line parameters

Parameter names are not case sensitive.

#### `-dx9` (vanilla, game only)

Runs the game with DX9 renderer. This is the default if DX10 is not available (WinXP).

#### `-dx10` (vanilla, game only)

Runs the game with DX10 renderer. This is the default if DX10 is available (WinVista+).

#### `-splash` (since v2.7, game only)

Disables automatic skipping of startup video ads.

#### `-language LANGUAGE` (since v4, game only)

Sets game language. Overrides `Game/Localized/Default.lng` config file.

Supported languages:

| Language  | Crysis | Crysis Warhead | Crysis Wars |
| :-------- | :----: | :------------: | :---------: |
| English   | Y      | Y              | Y           |
| Czech     | Y      | Y              | Y           |
| French    | Y      | Y              | Y           |
| German    | Y      | Y              | Y           |
| Hungarian | Y      | Y              | Y           |
| Italian   | Y      | Y              | Y           |
| Japanese  | Y      | Y              | ?           |
| Chinese   | ?      | Y              | ?           |
| Korean    | ?      | N              | N           |
| Polish    | Y      | Y              | Y           |
| Russian   | Y      | Y              | Y           |
| Spanish   | Y      | Y              | Y           |
| Turkish   | Y      | N              | N           |
| Thai      | ?      | ?              | ?           |

#### `-root PATH` (vanilla)

Sets root directory, where log and config files are stored. Defaults to main directory.

Used for running multiple instances within a single main directory. Mostly for servers.

#### `-mod NAME` (vanilla)

Loads a mod.

#### `-dedicated` (vanilla, game only)

Starts dedicated server instead of game.

#### `-devmode` (vanilla)

Enables developer features (DevMode).

#### `-lvlres` (vanilla)

Enables recording of startup and level resources (assets).

The resulting list is saved on quit to a file in the current level directory.

#### `-pakalias NAME,VALUE[,NAME,VALUE]...` (vanilla)

Adds CryPak aliases.

#### `-resetprofile` (vanilla)

Resets user profile, including `game.cfg` in the user directory. Triggers automatic detection of quality settings.

#### `-autodetect` (vanilla)

Forces automatic detection of quality settings (Low, Medium, High, Very High) based on computer hardware.

#### `-logfile NAME` (vanilla)

Sets name of the log file. Defaults to either `Game.log` or `Server.log` or `Editor.log`.

#### `-logprefix PREFIX` (since v3, headless server only)

Sets prefix of each log message. This is the default value of the `log_Prefix` cvar. Defaults to nothing.

The prefix consists of normal characters and the following special sequences:

| Sequence | Meaning                                                         |
| :------- | :-------------------------------------------------------------- |
| `%%`     | %                                                               |
| `%d`     | Day of the month (01..31)                                       |
| `%m`     | Month (01..12)                                                  |
| `%Y`     | Year (e.g. 2007)                                                |
| `%H`     | Hour (00..23)                                                   |
| `%M`     | Minute (00..59)                                                 |
| `%S`     | Second (00..60)                                                 |
| `%N`     | Millisecond (000..999)                                          |
| `%z`     | Offset from UTC (time zone) in the ISO 8601 format (e.g. +0100) |
| `%F`     | Equivalent to `%Y-%m-%d` (the ISO 8601 date format)             |
| `%T`     | Equivalent to `%H:%M:%S` (the ISO 8601 time format)             |
| `%t`     | Thread ID where the message was logged                          |

#### `-verbosity NUMBER` (since v3, headless server only)

Sets log verbosity. Defaults to `0` in headless server. In all other launchers, the default verbosity is always `1`.

The following verbosity values are supported:

| Verbosity | Meaning                             |
| :-------- | :---------------------------------- |
| `-1`      | Log disabled (headless server only) |
| `0`       | Only *always* messages              |
| `1`       | Additional errors                   |
| `2`       | Additional warnings                 |
| `3`       | Additional messages                 |
| `4`       | Additional comments                 |

#### `-userdirname NAME` (since v6)

Sets name of user directory in `Documents/My Games/`. Overrides `Game/Config/Folders.ini`.

| Example                     | User directory                        |
| :---------------------------| :------------------------------------ |
| `-userdirname Crysis`       | `Documents/My Games/Crysis` (default) |
| `-userdirname Test`         | `Documents/My Games/Test`             |
| `-userdirname "My New Mod"` | `Documents/My Games/My New Mod`       |

#### `-userpath PATH` (since v6)

Sets user directory path. Overrides `-userdirname` and `Game/Config/Folders.ini`. Both `\` and `/` are supported.

| Example                           | User directory                                               |
| :-------------------------------- | :----------------------------------------------------------- |
| `-userpath .`                     | Crysis main directory                                        |
| `-userpath Something\MyFolder`    | Crysis main directory + `Something\MyFolder` (relative path) |
| `-userpath C:\Something\MyFolder` | `C:\Something\MyFolder` (absolute path)                      |

#### `+CVAR VALUE` (vanilla)

Sets a console variable (cvar) value after startup.

This can be used to change certain settings from command line.
Here is an example that disables restricted console and enables showing basic engine info (the default in DevMode):

```
Crysis.exe +con_restricted 0 +r_DisplayInfo 1
```

#### `+CMD [ARG]...` (vanilla)

Executes a console command after startup.

This can be used to load a server config file.
Often in combination with loading a server-side mod (SSM) and changing root directory.
For example:

```
CrysisDedicatedServer.exe -mod MySSM -root C:\Crysis\ServerFolder +exec server.cfg
```

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
