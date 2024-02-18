# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]
### Fixed
- Fix startup crash of Warhead launcher when Logitech G15 keyboard is connected
([#39](https://github.com/ccomrade/c1-launcher/issues/39)).

## [v6] - 2024-02-15
### Added
- Crysis Editor (Sandbox 2) support.
- Crysis Warhead support (64-bit only).
- Crysis Wars executable with correct icon. No other differences. All executables always support all game variants.
- DX10 renderer now supports [overriding d3d10.dll and dxgi.dll](https://github.com/ccomrade/c1-launcher/issues/34)
by placing them into the same directory.
- `-userpath` to change user directory path.
- `-userdirname` to change name of user directory in `Documents/My Games/`.
- Mods can now request their own user directory in `Documents/My Games/` by exporting `GetUserDirName` function:
```cpp
extern "C" __declspec(dllexport) const char* GetUserDirName()
{
    return "My Mod";  // -> "Documents/My Games/My Mod"
}
```
- Path information in the log:
```
Main directory: C:\Crysis
Root directory: C:\Crysis
User directory: C:\Users\User\Documents\My Games\Crysis
```
### Fixed
- Fixed [the two broken panels in Editor](https://github.com/ccomrade/c1-launcher/pull/38).
- Fixed incorrectly trimmed spaces from CPU model name in specific cases.
- Fixed rare crash in the engine due to out-of-bounds access of the `CPUInfo::cores` array.

### Changed
- Each game variant now has its own release ZIP with all available executables:
    - `c1-launcher-v6-build.zip` => Crysis => game + dedicated server + headless server + editor
    - `c1-launcher-v6-build-wars.zip` => Crysis Wars => game + dedicated server + headless server + editor
    - `c1-launcher-v6-build-warhead.zip` => Crysis Warhead => game (64-bit)
        - Both dedicated and headless server launchers also support Crysis Warhead, which is singleplayer-only and has
no server launcher by default. However, it ships without CryRenderNULL DLL, so both server launchers fail to start the
engine there.

## [v5] - 2023-11-03
### Added
- GPU information in the log:
```
Renderer initialization
D3D10 Adapter: AMD Radeon RX 480 Graphics (RADV POLARIS10)
D3D10 Adapter: PCI 1002:67df (rev 00)
D3D10 Adapter: Dedicated video memory = 8G
D3D10 Adapter: Dedicated system memory = 0
D3D10 Adapter: Shared system memory = 31G
```
### Fixed
- Crash of 64-bit DX10 renderer during startup with recent nVidia GPU drivers.

## [v4] - 2023-09-28
### Added
- Workaround for missing localization files in Steam version. See `LanguageHook`.
### Fixed
- Fixed engine crash on systems with more than 32 CPU cores/threads.
### Changed
- Improved crash logger.

## [v3] - 2022-11-17
### Added
- Headless server launcher.
### Fixed
- CryRenderD3D10 DLL is not being loaded in DX9 mode anymore.
- Static linking of MSVC runtime library is automatically disabled when VS2005 compiler is used.
- No more sporadic crashes when file check is enabled on server.
### Changed
- Improved crash logger.
- Version is now just one number.
- Released executables are now digitally signed.

## [2.7] - 2021-11-09
### Added
- Automatic skipping of startup video ads can now be disabled with the `-splash` command line parameter.
### Fixed
- The lowest available display refresh rate is not being used anymore. Thanks to Guzz and Vladislav.
- Code cleanup and refactoring.

## [2.6] - 2021-06-11
### Changed
- Connecting to internet servers without GameSpy account is now possible.

## [2.5] - 2020-12-22
### Fixed
- Unlocked memory above 2 GB in 32-bit version. Thanks to Vladislav.

## [2.4] - 2020-07-24
### Added
- Crysis Wars support.
- Unlocked advantages of pre-ordered version. Both game launcher and server launcher are required for this to work.
- Crash logger that writes crash info to the log file.
- Launcher version can now be found in the log after engine initialization.

### Fixed
- No more hangs after crash when running under Wine. Fixed by disabling broken unhandled exception handler in CrySystem.
- Message boxes now have correct modern visual style.
- Launcher is now fully DPI aware to prevent any blurring on high DPI screens.

## [2.3] - 2019-07-13
### Fixed
- Crash of 32-bit Crysis on modern AMD processors caused by CPU feature detection bug in CrySystem.
