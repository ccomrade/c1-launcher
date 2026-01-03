#include "CryMallocHook.h"

// CryMemoryAllocator is used by CryEngine at various places. For example, as
// the default allocator of STLport, which is the C++ Standard Library within
// the engine, as Lua allocator in CryScriptSystem, and as FMOD allocator in
// CrySoundSystem. It's inlined all over the place in engine DLLs.

// CryMemoryAllocator contains a critical bug that causes 64-bit pointers to be
// truncated to 32-bit. It results in memory corruption and a crash when any
// instance of the allocator uses memory above 4 GB. This bug exists only in the
// 64-bit build because the 32-bit one cannot address more than 4 GB of memory.

// The following fix is based on the fact that CryMemoryAllocator allocates
// fixed size blocks from its upstream allocator, which is the global CryMalloc.
// The block size is 0x80000 (512 * 1024) and appears to be the same for all
// CryMemoryAllocator instances within the engine. Therefore, before starting
// the engine, we preallocate a large pool of these blocks below 4 GB, hook
// CryMalloc, and redirect requests for these blocks to the pool. This ensures
// that CryMemoryAllocator only gets safe blocks below 4 GB.

// This fix was originally created for the CryMP project.

#ifdef BUILD_64BIT

// std::malloc
#include <cstdlib>

// VirtualAlloc, _InterlockedIncrement64, etc.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Library/CrashLogger.h"
#include "Library/OS.h"

#define SAFE_BLOCK_SIZE 0x80000
#define SAFE_BLOCK_COUNT 2048  // 0x80000 * 2048 = 1 GiB should be enough for anyone

struct Stats : public CrashLogger::ExtraProvider
{
	volatile __int64 mallocCalls;
	volatile __int64 reallocCalls;
	volatile __int64 freeCalls;
	volatile __int64 sizeCalls;
	volatile __int64 crtMallocCalls;
	volatile __int64 crtFreeCalls;
	volatile __int64 crtSizeCalls;

	volatile __int64 safePoolBlocks;
	volatile __int64 safePoolFreeBlocks;
	volatile __int64 safePoolAllocs;
	volatile __int64 safePoolFailedAllocs;
	volatile __int64 safePoolDeallocs;

	Stats() : mallocCalls(0), reallocCalls(0), freeCalls(0), sizeCalls(0), crtMallocCalls(0), crtFreeCalls(0),
		crtSizeCalls(0), safePoolBlocks(0), safePoolFreeBlocks(0), safePoolAllocs(0), safePoolFailedAllocs(0),
		safePoolDeallocs(0) {}

	void OnCrash(std::FILE* file) override
	{
		std::fprintf(file, "CryMallocHook:\n");

		std::fprintf(file, "Calls:\n");
		std::fprintf(file, "Malloc = %I64d + %I64d\n", this->mallocCalls, this->crtMallocCalls);
		std::fprintf(file, "Realloc = %I64d\n", this->reallocCalls);
		std::fprintf(file, "Free = %I64d + %I64d\n", this->freeCalls, this->crtFreeCalls);
		std::fprintf(file, "Size = %I64d + %I64d\n", this->sizeCalls, this->crtSizeCalls);

		std::fprintf(file, "SafePool:\n");
		std::fprintf(file, "Blocks = %I64d (%I64d free)\n", this->safePoolBlocks, this->safePoolFreeBlocks);
		std::fprintf(file, "Allocs = %I64d (%I64d failed)\n", this->safePoolAllocs, this->safePoolFailedAllocs);
		std::fprintf(file, "Deallocs = %I64d\n", this->safePoolDeallocs);
	}
};

static Stats g_stats;

class SafePool
{
	void* m_pool;
	unsigned int m_blockCount;
	void* m_freeList;
	void* m_freeListLast;
	OS::Mutex m_mutex;

public:
	SafePool() : m_pool(NULL), m_blockCount(0), m_freeList(NULL), m_freeListLast(NULL), m_mutex()
	{
		// use 0x80000000 .. 0xc0000000 for the pool to avoid interfering with DLL placement
		void* hint = reinterpret_cast<void*>(0x80000000ULL);

		void* pool = VirtualAlloc(hint, SAFE_BLOCK_SIZE * SAFE_BLOCK_COUNT, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!pool)
		{
			return;
		}

		if (pool != hint)
		{
			VirtualFree(pool, 0, MEM_RELEASE);
			return;
		}

		m_pool = pool;

		int i = 0;
		for (; i < SAFE_BLOCK_COUNT; i++)
		{
			ULONG_PTR address = reinterpret_cast<ULONG_PTR>(pool) + (i * SAFE_BLOCK_SIZE);

			// make sure the whole block is below 4 GB
			if ((address + SAFE_BLOCK_SIZE) > 0x100000000ULL)
			{
				break;
			}

			void* block = reinterpret_cast<void*>(address);

			if (!m_freeList)
			{
				m_freeList = block;
				m_freeListLast = block;
			}
			else
			{
				*static_cast<void**>(m_freeListLast) = block;
				m_freeListLast = block;
			}
		}

		m_blockCount = i;
		g_stats.safePoolBlocks = i;
		g_stats.safePoolFreeBlocks = i;
	}

	void* Allocate()
	{
		_InterlockedIncrement64(&g_stats.safePoolAllocs);

		OS::LockGuard<OS::Mutex> lock(m_mutex);

		if (!m_freeList)
		{
			_InterlockedIncrement64(&g_stats.safePoolFailedAllocs);
			return NULL;
		}

		void* block = m_freeList;

		if (m_freeList == m_freeListLast)
		{
			m_freeList = NULL;
			m_freeListLast = NULL;
		}
		else
		{
			m_freeList = *static_cast<void**>(m_freeList);
		}

		_InterlockedDecrement64(&g_stats.safePoolFreeBlocks);

		return block;
	}

	void Deallocate(void* block)
	{
		_InterlockedIncrement64(&g_stats.safePoolDeallocs);

		OS::LockGuard<OS::Mutex> lock(m_mutex);

		if (!m_freeList)
		{
			m_freeList = block;
			m_freeListLast = block;
		}
		else
		{
			*static_cast<void**>(m_freeListLast) = block;
			m_freeListLast = block;
		}

		_InterlockedIncrement64(&g_stats.safePoolFreeBlocks);
	}

	bool Contains(void* ptr) const
	{
		const ULONG_PTR address = reinterpret_cast<ULONG_PTR>(ptr);
		const ULONG_PTR poolBegin = reinterpret_cast<ULONG_PTR>(m_pool);
		const ULONG_PTR poolEnd = poolBegin + (m_blockCount * SAFE_BLOCK_SIZE);

		return address >= poolBegin && address < poolEnd && !(address % SAFE_BLOCK_SIZE);
	}
};

static SafePool* g_safePool = NULL;

typedef void* (*TCryMalloc)(size_t, size_t&);
typedef void* (*TCryRealloc)(void*, size_t, size_t&);
typedef size_t (*TCryFree)(void*);
typedef size_t (*TCryGetMemSize)(void*, size_t);
typedef void* (*TCryCrtMalloc)(size_t);
typedef void (*TCryCrtFree)(void*);
typedef size_t (*TCryCrtSize)(void*);

// needed during loading of CrySystem.dll
static void* CryMallocStub(size_t size, size_t& allocated)
{
	allocated = size;
	return std::malloc(size);
}

static TCryMalloc g_pCryMalloc = &CryMallocStub;
static TCryRealloc g_pCryRealloc = NULL;
static TCryFree g_pCryFree = NULL;
static TCryGetMemSize g_pCryGetMemSize = NULL;
static TCryCrtMalloc g_pCryCrtMalloc = NULL;
static TCryCrtFree g_pCryCrtFree = NULL;
static TCryCrtSize g_pCryCrtSize = NULL;

// CryMalloc functions exported by the EXE are automatically used instead of CrySystem.dll ones
#define HOOKED extern "C" __declspec(dllexport)

HOOKED void* CryMalloc(size_t size, size_t& allocated)
{
	_InterlockedIncrement64(&g_stats.mallocCalls);

	if (g_safePool && size == SAFE_BLOCK_SIZE)
	{
		void* block = g_safePool->Allocate();
		if (block)
		{
			allocated = SAFE_BLOCK_SIZE;
			return block;
		}
	}

	return g_pCryMalloc(size, allocated);
}

HOOKED void* CryRealloc(void* memblock, size_t size, size_t& allocated)
{
	_InterlockedIncrement64(&g_stats.reallocCalls);

	return g_pCryRealloc(memblock, size, allocated);
}

HOOKED size_t CryFree(void* p)
{
	_InterlockedIncrement64(&g_stats.freeCalls);

	if (g_safePool && g_safePool->Contains(p))
	{
		g_safePool->Deallocate(p);
		return SAFE_BLOCK_SIZE;
	}

	return g_pCryFree(p);
}

HOOKED size_t CryGetMemSize(void* p, size_t size)
{
	_InterlockedIncrement64(&g_stats.sizeCalls);

	if (g_safePool && g_safePool->Contains(p))
	{
		return SAFE_BLOCK_SIZE;
	}

	return g_pCryGetMemSize(p, size);
}

HOOKED void* CrySystemCrtMalloc(size_t size)
{
	_InterlockedIncrement64(&g_stats.crtMallocCalls);

	if (g_safePool && size == SAFE_BLOCK_SIZE)
	{
		void* block = g_safePool->Allocate();
		if (block)
		{
			return block;
		}
	}

	return g_pCryCrtMalloc(size);
}

HOOKED void CrySystemCrtFree(void* p)
{
	_InterlockedIncrement64(&g_stats.crtFreeCalls);

	if (g_safePool && g_safePool->Contains(p))
	{
		g_safePool->Deallocate(p);
		return;
	}

	g_pCryCrtFree(p);
}

HOOKED size_t CrySystemCrtSize(void* p)
{
	_InterlockedIncrement64(&g_stats.crtSizeCalls);

	if (g_safePool && g_safePool->Contains(p))
	{
		return SAFE_BLOCK_SIZE;
	}

	return g_pCryCrtSize(p);
}

#endif  // BUILD_64BIT

void CryMallocHook::Init(void* pCrySystem)
{
#ifdef BUILD_64BIT
	CrashLogger::AddExtraProvider(&g_stats);

	if (!OS::CmdLine::HasArg("-nosafepool"))
	{
		g_safePool = new SafePool;
	}

	g_pCryMalloc = static_cast<TCryMalloc>(OS::DLL::FindSymbol(pCrySystem, "CryMalloc"));
	g_pCryRealloc = static_cast<TCryRealloc>(OS::DLL::FindSymbol(pCrySystem, "CryRealloc"));
	g_pCryFree = static_cast<TCryFree>(OS::DLL::FindSymbol(pCrySystem, "CryFree"));
	g_pCryGetMemSize = static_cast<TCryGetMemSize>(OS::DLL::FindSymbol(pCrySystem, "CryGetMemSize"));
	g_pCryCrtMalloc = static_cast<TCryCrtMalloc>(OS::DLL::FindSymbol(pCrySystem, "CrySystemCrtMalloc"));
	g_pCryCrtFree = static_cast<TCryCrtFree>(OS::DLL::FindSymbol(pCrySystem, "CrySystemCrtFree"));
	g_pCryCrtSize = static_cast<TCryCrtSize>(OS::DLL::FindSymbol(pCrySystem, "CrySystemCrtSize"));
#endif
}
