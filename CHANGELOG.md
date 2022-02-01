# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]
### Fixed
- CryRenderD3D10 DLL is not being loaded anymore in DX9 mode.

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
