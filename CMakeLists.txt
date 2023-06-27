cmake_minimum_required(VERSION 3.15)

################################################################################

project(C1-Launcher VERSION 3 LANGUAGES CXX RC)

################################################################################

if(NOT MSVC)
	message(FATAL_ERROR "MSVC is the only supported compiler!")
endif()

if(NOT CMAKE_MSVC_RUNTIME_LIBRARY)
	# detect VS2005 compiler and use dynamically linked MSVC runtime library, otherwise use statically linked one
	if(MSVC_VERSION EQUAL 1400)
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	else()
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

target_link_libraries(LauncherBase PUBLIC dbghelp)

if(BUILD_BITS EQUAL 64)
	target_compile_definitions(LauncherBase PUBLIC BUILD_64BIT)
endif()

# also include PROJECT_BINARY_DIR with configured Project.h
target_include_directories(LauncherBase PUBLIC Code ${PROJECT_BINARY_DIR})

# explicitly disable some stupid MSVC warnings
target_compile_options(LauncherBase PUBLIC /D_CRT_SECURE_NO_WARNINGS /wd4351)

# prevent modern MSVC from enabling ASLR, which breaks Crysis DLLs, and unlock memory above 2 GB
target_link_options(LauncherBase PUBLIC /DYNAMICBASE:NO /LARGEADDRESSAWARE)

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
