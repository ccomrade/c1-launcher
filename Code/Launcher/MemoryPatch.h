#pragma once

#include <cstdarg>
#include <cstddef>

struct CPUInfo;
struct ISystem;
struct ILocalizationManager;

namespace MemoryPatch
{
	namespace CryAction
	{
		void AllowDX9ImmersiveMultiplayer(void* pCryAction, int gameBuild);
		void DisableGameplayStats(void* pCryAction, int gameBuild);
		void HookGameWarning(void* pCryAction, int gameBuild, void (*handler)(const char* format, ...));
		void HookCryWarning(void* pCryAction, int gameBuild,
			void (*handler)(int, int, const char* format, ...));
	}

	namespace CryGame
	{
		void DisableIntros(void* pCryGame, int gameBuild);
		void CanJoinDX10Servers(void* pCryGame, int gameBuild);
		void EnableDX10Menu(void* pCryGame, int gameBuild);
		void HookGameWarning(void* pCryGame, int gameBuild, void (*handler)(const char* format, ...));
		void HookCryWarning(void* pCryGame, int gameBuild,
			void (*handler)(int, int, const char* format, ...));
	}

	namespace CryNetwork
	{
		void EnablePreordered(void* pCryNetwork, int gameBuild);
		void AllowSameCDKeys(void* pCryNetwork, int gameBuild);
		void FixInternetConnect(void* pCryNetwork, int gameBuild);
		void FixFileCheckCrash(void* pCryNetwork, int gameBuild);
		void DisableServerProfile(void* pCryNetwork, int gameBuild);
		void HookCryWarning(void* pCryNetwork, int gameBuild,
			void (*handler)(int, int, const char* format, ...));
	}

	namespace CrySystem
	{
		void RemoveSecuROM(void* pCrySystem, int gameBuild);
		void AllowDX9VeryHighSpec(void* pCrySystem, int gameBuild);
		void AllowMultipleInstances(void* pCrySystem, int gameBuild);
		void DisableCrashHandler(void* pCrySystem, int gameBuild);
		void FixCPUInfoOverflow(void* pCrySystem, int gameBuild);
		void HookCPUDetect(void* pCrySystem, int gameBuild, void (*handler)(CPUInfo* info, ISystem* pSystem));
		void HookError(void* pCrySystem, int gameBuild, void (*handler)(const char* format, va_list args));
		void HookLanguageInit(void* pCrySystem, int gameBuild,
			void (*handler)(const char* defaultLanguage, ILocalizationManager* pLocalizationManager));
		void HookChangeUserPath(void* pCrySystem, int gameBuild,
			void (*handler)(ISystem* pSystem, const char* userPath));
		void HookCryWarning(void* pCrySystem, int gameBuild,
			void (*handler)(int, int, const char* format, ...));
	}

	namespace CryRenderD3D9
	{
		struct AdapterInfo
		{
			// D3DADAPTER_IDENTIFIER9
			char driver[512];
			char description[512];
			char device_name[32];
			unsigned long driver_version_lo;
			unsigned long driver_version_hi;
			unsigned long vendor_id;
			unsigned long device_id;
			unsigned long sub_sys_id;
			unsigned long revision;
			// ...
		};

		void HookAdapterInfo(void* pCryRenderD3D9, int gameBuild, void (*handler)(AdapterInfo* info));
		void HookLogWarning(void* pCryRenderD3D9, int gameBuild, void (*handler)(const char* format, ...));
	}

	namespace CryRenderD3D10
	{
		struct AdapterInfo
		{
			void* reserved;
			// DXGI_ADAPTER_DESC
			wchar_t description[128];
			unsigned int vendor_id;
			unsigned int device_id;
			unsigned int sub_sys_id;
			unsigned int revision;
			std::size_t dedicated_video_memory;
			std::size_t dedicated_system_memory;
			std::size_t shared_system_memory;
			// ...
		};

		struct SystemAPI
		{
			void* pDXGI;
			void* pCreateDXGIFactory;
			void* pD3D10;
			void* pD3D10CreateStateBlock;              // unused
			void* pD3D10CreateDevice;
			void* pD3D10StateBlockMaskUnion;           // unused
			void* pD3D10StateBlockMaskIntersect;       // unused
			void* pD3D10StateBlockMaskDifference;      // unused
			void* pD3D10StateBlockMaskEnableCapture;   // unused
			void* pD3D10StateBlockMaskDisableCapture;  // unused
			void* pD3D10StateBlockMaskEnableAll;       // unused
			void* pD3D10StateBlockMaskDisableAll;      // unused
			void* pD3D10StateBlockMaskGetSetting;      // unused
		};

		void FixLowRefreshRateBug(void* pCryRenderD3D10, int gameBuild);
		void HookAdapterInfo(void* pCryRenderD3D10, int gameBuild, void (*handler)(AdapterInfo* info));
		void HookInitAPI(void* pCryRenderD3D10, int gameBuild, bool (*handler)(SystemAPI* api));
		void HookLogWarning(void* pCryRenderD3D10, int gameBuild, void (*handler)(const char* format, ...));
	}

	namespace CryRenderNULL
	{
		void DisableDebugRenderer(void* pCryRenderNULL, int gameBuild);
	}

	namespace WarheadEXE
	{
		using CryAction::AllowDX9ImmersiveMultiplayer;
		using CryAction::DisableGameplayStats;
		using CryAction::HookCryWarning;
		using CryAction::HookGameWarning;
		using CryGame::DisableIntros;

		void FixHInstance(void* pEXE, int gameBuild);
	}

	namespace Editor
	{
		struct Version
		{
			int file_tweak;
			int file_patch;
			int file_minor;
			int file_major;

			int product_tweak;
			int product_patch;
			int product_minor;
			int product_major;
		};

		void FixBrokenPanels(void* pEditor, int editorBuild);
		void HookVersionInit(void* pEditor, int editorBuild, void (*handler)(Version* version));
	}

	namespace FMODEx
	{
		void Fix64BitHeapAddressTruncation(void* pFMODEx, int gameBuild);
	}
}
