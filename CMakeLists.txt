cmake_minimum_required(VERSION 3.15)

################################################################################

project(C1-Launcher VERSION 3 LANGUAGES CXX RC)

################################################################################

if(NOT MSVC)
	message(FATAL_ERROR "MSVC is the only supported compiler!")
endif()

if(NOT CMAKE_MSVC_RUNTIME_LIBRARY)
	# detect VS2005 compiler
	if(MSVC_VERSION EQUAL 1400)
		# all original Crysis DLLs are dynamically linked to MSVC runtime library from VS2005
		# use dynamically linked MSVC runtime library to reduce size of the resulting executables
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	else()
		# use statically linked MSVC runtime library to avoid any unwanted DLL dependencies
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	endif()
endif()

################################################################################

math(EXPR BUILD_BITS "${CMAKE_SIZEOF_VOID_P} * 8")

message(STATUS "${PROJECT_NAME} v${PROJECT_VERSION_MAJOR} ${BUILD_BITS}-bit")

configure_file(Project.h.in ${PROJECT_BINARY_DIR}/Project.h)

################################################################################

add_library(LauncherBase STATIC
	Code/CryCommon/CryGame/IGameRef.h
	Code/CryCommon/CryGame/IGameStartup.h
	Code/CryCommon/CrySystem/IConsole.h
	Code/CryCommon/CrySystem/ILog.h
	Code/CryCommon/CrySystem/ISystem.cpp
	Code/CryCommon/CrySystem/ISystem.h
	Code/CryCommon/CrySystem/IValidator.h
	Code/Launcher/CPUInfo.cpp
	Code/Launcher/CPUInfo.h
	Code/Launcher/LauncherCommon.cpp
	Code/Launcher/LauncherCommon.h
	Code/Launcher/MemoryPatch.cpp
	Code/Launcher/MemoryPatch.h
	Code/Library/CPUID.cpp
	Code/Library/CPUID.h
	Code/Library/CrashLogger.cpp
	Code/Library/CrashLogger.h
	Code/Library/OS.cpp
	Code/Library/OS.h
	Code/Library/PathTools.cpp
	Code/Library/PathTools.h
	Code/Library/StdFile.h
	Code/Library/StringFormat.cpp
	Code/Library/StringFormat.h
	Code/Library/StringView.h
)

if(BUILD_BITS EQUAL 64)
	target_compile_definitions(LauncherBase PUBLIC BUILD_64BIT)
endif()

target_compile_definitions(LauncherBase PUBLIC _CRT_SECURE_NO_WARNINGS)
target_compile_options(LauncherBase PUBLIC /W3)

target_include_directories(LauncherBase PUBLIC Code ${PROJECT_BINARY_DIR})

# prevent modern MSVC from enabling ASLR, which breaks Crysis DLLs, and unlock memory above 2 GB
target_link_options(LauncherBase PUBLIC /DYNAMICBASE:NO /LARGEADDRESSAWARE)

target_link_libraries(LauncherBase PUBLIC dbghelp)

################################################################################

add_executable(Crysis WIN32
	Code/Launcher/Game/GameLauncher.cpp
	Code/Launcher/Game/GameLauncher.h
	Code/Launcher/Game/LanguageHook.cpp
	Code/Launcher/Game/LanguageHook.h
	Code/Launcher/Game/Main.cpp
	Code/Launcher/Game/Resources.h
	Resources/Launcher.manifest
	Resources/GameLauncher.rc
)

add_executable(CrysisDedicatedServer WIN32
	Code/Launcher/DedicatedServer/DedicatedServerLauncher.cpp
	Code/Launcher/DedicatedServer/DedicatedServerLauncher.h
	Code/Launcher/DedicatedServer/Main.cpp
	Code/Launcher/DedicatedServer/Resources.h
	Resources/Launcher.manifest
	Resources/DedicatedServerLauncher.rc
)

add_executable(CrysisHeadlessServer
	Code/Launcher/HeadlessServer/HeadlessServerLauncher.cpp
	Code/Launcher/HeadlessServer/HeadlessServerLauncher.h
	Code/Launcher/HeadlessServer/Logger.cpp
	Code/Launcher/HeadlessServer/Logger.h
	Code/Launcher/HeadlessServer/Main.cpp
	Code/Launcher/HeadlessServer/NullValidator.h
	Resources/HeadlessServerLauncher.rc
)

target_link_libraries(Crysis LauncherBase)
target_link_libraries(CrysisDedicatedServer LauncherBase)
target_link_libraries(CrysisHeadlessServer LauncherBase)

################################################################################

set(GAME_LAUNCHER_RESOURCES
	${PROJECT_SOURCE_DIR}/Resources/CursorAmber.cur
	${PROJECT_SOURCE_DIR}/Resources/CursorBlue.cur
	${PROJECT_SOURCE_DIR}/Resources/CursorGreen.cur
	${PROJECT_SOURCE_DIR}/Resources/CursorRed.cur
	${PROJECT_SOURCE_DIR}/Resources/CursorWhite.cur
	${PROJECT_SOURCE_DIR}/Resources/GameIcon.ico
)

set(DEDICATED_SERVER_LAUNCHER_RESOURCES
	${PROJECT_SOURCE_DIR}/Resources/DedicatedServerIcon.ico
)

set_source_files_properties(Resources/GameLauncher.rc
	PROPERTIES OBJECT_DEPENDS "${GAME_LAUNCHER_RESOURCES}"
)

set_source_files_properties(Resources/DedicatedServerLauncher.rc
	PROPERTIES OBJECT_DEPENDS "${DEDICATED_SERVER_LAUNCHER_RESOURCES}"
)

################################################################################
