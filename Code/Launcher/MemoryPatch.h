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
		void HookCPUDetect(void* pCrySystem, int gameBuild,
			void (*handler)(CPUInfo* info, ISystem* pSystem));
		void HookError(void* pCrySystem, int gameBuild,
			void (*handler)(const char* format, va_list args));
		void HookLanguageInit(void* pCrySystem, int gameBuild,
			void (*handler)(const char* defaultLanguage, ILocalizationManager* pLocalizationManager));
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

		void HookAdapterInfo(void* pCryRenderD3D9, int gameBuild,
			void (*handler)(AdapterInfo* info));
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

		void FixLowRefreshRateBug(void* pCryRenderD3D10, int gameBuild);
		void HookAdapterInfo(void* pCryRenderD3D10, int gameBuild,
			void (*handler)(AdapterInfo* info));
	}

	namespace CryRenderNULL
	{
		void DisableDebugRenderer(void* pCryRenderNULL, int gameBuild);
	}
}
