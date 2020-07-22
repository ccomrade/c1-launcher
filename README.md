# cw-launcher

CW-Launcher is an open-source replacement of the original [Crysis Wars](https://en.wikipedia.org/wiki/Crysis_Warhead#Crysis_Wars)
executables. It provides better game launcher and dedicated server launcher with additional features.
Supports Crysis Wars patch 5 both 32-bit and 64-bit.

### Features

- Full source code is available
- Completely DRM-free launcher (no broken SecuROM crap inside)
- No compatibility mode is required to make the game work on modern systems
- Smaller files (especially game launcher)
- Very High settings are available in DX9 mode
- Game started in DX9 mode can connect to DX10 servers (everything works just like in DX10 game)
- Servers started via this launcher don't kick players with the same CD key
- It's possible to run multiple Crysis Wars instances at once
- Fixed crash of 32-bit Crysis Wars on modern AMD processors
- Added engine based FPS cap for minimum input latency
- Disabled server profiler for 32-bit dedicated servers'
- Disabled autoexec.cfg for the dedicated server (enables sharing an install with a client)

### FAQ

---

**Question:** How to use it?

**Answer:** Download the the EXE files from [release section](https://github.com/jedi95/c1-launcher/releases).
Then just replace original `Bin32\Crysis.exe` and `Bin64\Crysis.exe` with 32-bit and 64-bit version of this launcher.
Alternatively, you can compile the source code (see below for instructions)
If you do not have the game, you can download the Crysis Wars trial for free.
This is a full copy of the game, but it does not come with a CD key.
The replacement Gamespy combined with this launcher means that CD keys are not checked.

---

**Question:** I don't want to replace the original executables. Is there any way?

**Answer:** Yes, you can rename the EXE files of this launcher to whatever you want and use them next to untouched original
executables.

---

**Question:** How can I play Crysis Wars multiplayer?

**Answer:** Official multiplayer doesn't work anymore because of
[GameSpy shutdown](https://en.wikipedia.org/wiki/GameSpy#Shutdown). However, a Gamespy replacement is included!
You can host servers and play multiplayer just like before. The only difference is that you will need to make a new account,
which can be done within the game's multiplayer menu.

---

**Question:** How can I run Crysis Wars in DX9 mode instead of default DX10 mode?

**Answer:** Just launch it with `-dx9` command line parameter.

---

**Question:** Are there any files that are not needed if I use this launcher?

**Answer:** Yes, you can safely delete the following files if you don't plan to use the original executables anymore:
```
Bin32\Crysis.exe                      <-- 32-bit game launcher
Bin32\CrysisWarsDedicatedServer.exe   <-- 32-bit dedicated server launcher

Bin64\Crysis.exe                      <-- 64-bit game launcher
Bin64\CrysisWarsDedicatedServer.exe   <-- 64-bit dedicated server launcher
```

---

### Notes

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


This project uses the incomplete CryEngine headers (`Code/CryEngine`) from the **Crysis Mod SDK v1.2** released by Crytek. All
the header files have been converted to UTF-8 with LF line endings. In addition, there is only one modification - added content
of the `SSystemInitParams` structure in `CryCommon/ISystem.h` file. This structure is used to launch the game, but it's empty in
the original file, so its content has been reverse engineered. The following code is the new structure definition as you can see
it in the modified `CryCommon/ISystem.h` file. Names of its members are inspired by the source code of the latest
[CryEngine V](https://github.com/CRYTEK/CRYENGINE).

```c++
struct SSystemInitParams
{
	void *hInstance;                     // executable handle
	void *hWnd;                          // optional window handle
	ILog *pLog;                          // optional custom log
	ILogCallback *pLogCallback;          // optional log callback
	ISystemUserCallback *pUserCallback;  // optional engine callback
	const char *sLogFileName;            // usually "Game.log" or "Server.log"
	IValidator *pValidator;              // optional custom validator
	char szSystemCmdLine[2048];          // process command line obtained with GetCommandLineA
	char szUserPath[256];                // optional custom user folder in %USERPROFILE%\Documents
	bool bEditor;                        // editor mode
	bool bMinimal;                       // minimal mode - skip initialization of some subsystems
	bool bTesting;                       // test mode
	bool bDedicatedServer;               // launch dedicated server
	ISystem *pSystem;                    // initialized by IGameStartup::Init
	void *pCheckFunc;                    // not used
	void *pProtectedFunctions[10];       // probably not used
};
```

### Build instructions

#### Required tools

To build the source code you need **CMake**, which is lightweight build manager, and **Microsoft Visual C++** compiler (MSVC).
No other compilers are supported because Crysis Wars was built with MSVC, so the same ABI is required. There are 2 ways how to get
MSVC. The first way is Visual Studio (VS2005 or higher is required). You can use it if you already have it on your computer, but
it's not recommended way, because Visual Studio is highly bloated software and installing it just for building some project is
really bad idea. The second way is Windows SDK. It contains MSVC compiler and since Windows Vista it also fully supports C++ so
it's possible to use it here. Windows SDK is the recommended way to get MSVC.

CMake can be downloaded here:
* https://cmake.org/download/

It doesn't matter if you use 32-bit or 64-bit version. These build instructions expect CMake in PATH, so if you use the CMake
installer, you should say yes to add CMake *bin* directory to the PATH during installation. If you don't use the installer, you
should add the directory to the PATH manually.

Here are some download links for tested and well-working versions of Windows SDK:
* https://www.microsoft.com/en-us/download/details.aspx?id=23719 (latest WinVista SDK, MSVC from VS2005)
* https://www.microsoft.com/en-us/download/details.aspx?id=3138 (old Win7 SDK, MSVC from VS2008)
* https://www.microsoft.com/en-us/download/details.aspx?id=8279 (lastest Win7 SDK, MSVC from VS2010)

Note that you should use SDK version matching your version of Windows, so if you have some newer version of Windows, search for
appropriate version of the Windows SDK. Don't forget to read its description first. Windows SDK installer prompts you to choose
which parts of the SDK should be installed. For building projects like this one, you need just compilers, tools, headers, and
libraries for both x86 and x64. Everything else, like .NET things and IA-64 compilers and libraries, is not required. SDKs for
the newer versions of Windows also want to install some things for UWP and web development - you don't need these too.

#### Preparation

First of all, download the source code from this repository and unpack it somewhere. Then create 2 empty build directories. One
for 32-bit version and the second one for 64-bit version. If you want only one version, you need only one build directory.

#### Building

- Using Windows SDK (recommended):
    * Open Windows SDK x86 command prompt. If you want to build 64-bit version, use x64 command prompt instead.
    * Execute `cmake-gui` in the command prompt. It should open CMake window.
    * Select the directory with source code and the empty build directory in the CMake window.
    * Press `Configure` button.
    * Select **NMake Makefiles** generator.
    * You can now change build options in the CMake window. Press `Configure` button again when you're done.
    * Press `Generate` button.
    * Use `cd` command in the command prompt to move to the build directory.
    * Execute `nmake` in the command prompt.
    * The resulting EXE files can be found in the build directory.
    * If you want both 32-bit and 64-bit launchers, repeat all above steps with the second SDK command prompt.

- Using Visual Studio:
    * Open CMake (cmake-gui) and select the source code directory and the empty build directory.
    * Press `Configure` button.
    * Select generator matching your Visual Studio version. If you want to build 64-bit version, use Win64 generator.
    * You can now change build options in the CMake window. Press `Configure` button again when you're done.
    * Press `Generate` button.
    * Press `Open Project` button. It should open Visual Studio with the generated solution.
    * Select **Release** build in the Visual Studio.
    * Build the solution.
    * The resulting EXE files can be found in the build directory.
    * If you want both 32-bit and 64-bit launchers, repeat all above steps with the second generator.

