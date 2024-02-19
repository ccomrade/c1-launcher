#pragma once

#include <cstdarg>

struct CPUInfo;
struct CryRender_D3D9_AdapterInfo;
struct CryRender_D3D10_AdapterInfo;
struct CryRender_D3D10_SystemAPI;
struct ISystem;
struct ILocalizationManager;

namespace MemoryPatch
{
	namespace CryAction
	{
		void AllowDX9ImmersiveMultiplayer(void* pCryAction, int gameBuild);
		void DisableGameplayStats(void* pCryAction, int gameBuild);
	}

	namespace CryGame
	{
		void DisableIntros(void* pCryGame, int gameBuild);
		void CanJoinDX10Servers(void* pCryGame, int gameBuild);
		void EnableDX10Menu(void* pCryGame, int gameBuild);
	}

	namespace CryNetwork
	{
		void EnablePreordered(void* pCryNetwork, int gameBuild);
		void AllowSameCDKeys(void* pCryNetwork, int gameBuild);
		void FixInternetConnect(void* pCryNetwork, int gameBuild);
		void FixFileCheckCrash(void* pCryNetwork, int gameBuild);
		void DisableServerProfile(void* pCryNetwork, int gameBuild);
	}

	namespace CrySystem
	{
		void RemoveSecuROM(void* pCrySystem, int gameBuild);
		void AllowDX9VeryHighSpec(void* pCrySystem, int gameBuild);
		void AllowMultipleInstances(void* pCrySystem, int gameBuild);
		void DisableCrashHandler(void* pCrySystem, int gameBuild);
		void FixCPUInfoOverflow(void* pCrySystem, int gameBuild);
		void HookCPUDetect(void* pCrySystem, int gameBuild,
			void (*handler)(CPUInfo* info, ISystem* pSystem));
		void HookError(void* pCrySystem, int gameBuild,
			void (*handler)(const char* format, va_list args));
		void HookLanguageInit(void* pCrySystem, int gameBuild,
			void (*handler)(const char* defaultLanguage, ILocalizationManager* pLocalizationManager));
		void HookChangeUserPath(void* pCrySystem, int gameBuild,
			void (*handler)(ISystem* pSystem, const char* userPath));
	}

	namespace CryRenderD3D9
	{
		void HookAdapterInfo(void* pCryRenderD3D9, int gameBuild,
			void (*handler)(CryRender_D3D9_AdapterInfo* info));
	}

	namespace CryRenderD3D10
	{
		void FixLowRefreshRateBug(void* pCryRenderD3D10, int gameBuild);
		void HookAdapterInfo(void* pCryRenderD3D10, int gameBuild,
			void (*handler)(CryRender_D3D10_AdapterInfo* info));
		void HookInitAPI(void* pCryRenderD3D10, int gameBuild,
			bool (*handler)(CryRender_D3D10_SystemAPI* api));
	}

	namespace CryRenderNULL
	{
		void DisableDebugRenderer(void* pCryRenderNULL, int gameBuild);
	}

	namespace WarheadEXE
	{
		void FixHInstance(void* pEXE, int gameBuild);
	}

	namespace Editor
	{
		struct Version
		{
			int file_patch;
			int file_tweak;
			int file_minor;
			int file_major;

			int product_patch;
			int product_tweak;
			int product_minor;
			int product_major;
		};

		void FixBrokenPanels(void* pEditor, int editorBuild);
		void HookVersionInit(void* pEditor, int editorBuild,
			void (*handler)(Version* version));
	}
}
