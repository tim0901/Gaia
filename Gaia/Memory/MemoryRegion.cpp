#include "MemoryRegion.h"

// Allocate cache-aligned memory blocks
void* AllocAligned(size_t size, int alignment) {
#if defined(WIN32) || defined(_WIN32) || defined (__WIN32)
	return _aligned_malloc(size, alignment);
#elif __APPLE__
	void* ptr;
	if (posix_memalign(&ptr, alignment, size) != 0)
		ptr = nullptr;
	return ptr;
#else
	return memalign(size, alignment);
#endif
}

// Free cache-aligned memory blocks
void FreeAligned(void* ptr) {
	if (!ptr) return;
#if defined(WIN32) || defined(_WIN32) || defined (__WIN32)
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}