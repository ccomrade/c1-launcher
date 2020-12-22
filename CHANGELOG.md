# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

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
