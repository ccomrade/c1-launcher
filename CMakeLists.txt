cmake_minimum_required(VERSION 3.15)

#################################################
project(C1-Launcher VERSION 2.3 LANGUAGES CXX RC)
#################################################

if(NOT MSVC)
	message(FATAL_ERROR "MSVC is the only supported compiler!")
endif()

if(NOT CMAKE_MSVC_RUNTIME_LIBRARY)
	# use statically linked MSVC runtime library
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# prevent modern MSVC from enabling ASLR
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DYNAMICBASE:NO")

# detect 64-bit compiler
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BUILD_64BIT TRUE)
	message(STATUS "64-bit build")
else()
	message(STATUS "32-bit build")
endif()

configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)

add_library(LauncherBase STATIC
  Code/CryCommon/IGameRef.h
  Code/CryCommon/IGameStartup.h
  Code/CryCommon/ILog.h
  Code/CryCommon/ISystem.h
  Code/Launcher/CmdLine.cpp
  Code/Launcher/CmdLine.h
  Code/Launcher/CPU.cpp
  Code/Launcher/CPU.h
  Code/Launcher/CrashLogger.cpp
  Code/Launcher/CrashLogger.h
  Code/Launcher/DLL.cpp
  Code/Launcher/DLL.h
  Code/Launcher/Launcher.cpp
  Code/Launcher/Launcher.h
  Code/Launcher/Patch.cpp
  Code/Launcher/Patch.h
  Code/Launcher/Util.cpp
  Code/Launcher/Util.h
)

target_include_directories(LauncherBase PUBLIC Code ${PROJECT_BINARY_DIR})

if(BUILD_64BIT)
	target_compile_definitions(LauncherBase PUBLIC BUILD_64BIT)
endif()

add_executable(Crysis WIN32
  Code/Launcher/Game/Main.cpp
  Code/Launcher/Game/Resources.h
  Resources/GameLauncher.rc
)

add_executable(CrysisDedicatedServer WIN32
  Code/Launcher/DedicatedServer/Main.cpp
  Code/Launcher/DedicatedServer/Resources.h
  Resources/DedicatedServerLauncher.rc
)

target_link_libraries(Crysis LauncherBase)
target_link_libraries(CrysisDedicatedServer LauncherBase)
