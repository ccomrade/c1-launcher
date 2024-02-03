#pragma once

#include <cstddef>

struct CryRender_D3D9_AdapterInfo
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

struct CryRender_D3D10_AdapterInfo
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

struct CryRender_D3D10_SystemAPI
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
