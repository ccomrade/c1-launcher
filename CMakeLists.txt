cmake_minimum_required(VERSION 3.0)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build.")
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}" CACHE INTERNAL "Install target is not defined.")

#################################################
project(C1-Launcher VERSION 2.3 LANGUAGES CXX RC)
#################################################

if(NOT MSVC)
	message(FATAL_ERROR "MSVC is the only supported compiler!")
endif()

# detect 64-bit compiler
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(BUILD_64BIT TRUE)
	message(STATUS "64-bit build")
else()
	message(STATUS "32-bit build")
endif()

# prevent modern MSVC from enabling ASLR
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DYNAMICBASE:NO")

option(C1LAUNCHER_STATIC_MSVC_RUNTIME "Enable static link of MSVC runtime library." ON)
if(C1LAUNCHER_STATIC_MSVC_RUNTIME)
	foreach(flag
	  CMAKE_CXX_FLAGS
	  CMAKE_CXX_FLAGS_DEBUG
	  CMAKE_CXX_FLAGS_RELEASE
	  CMAKE_CXX_FLAGS_RELWITHDEBINFO
	  CMAKE_CXX_FLAGS_MINSIZEREL)
		if(${flag} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
		endif()
	endforeach()
endif()

configure_file(config.h.in ${PROJECT_BINARY_DIR}/config.h)

add_library(LauncherBase STATIC
  Code/CryCommon/IGameRef.h
  Code/CryCommon/IGameStartup.h
  Code/CryCommon/ISystem.h
  Code/Launcher/CPU.cpp
  Code/Launcher/CPU.h
  Code/Launcher/DLL.cpp
  Code/Launcher/DLL.h
  Code/Launcher/Launcher.cpp
  Code/Launcher/Launcher.h
  Code/Launcher/Patch.cpp
  Code/Launcher/Patch.h
  Code/Launcher/StringBuffer.h
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

target_link_libraries(Crysis                LauncherBase)
target_link_libraries(CrysisDedicatedServer LauncherBase)
