#pragma once

#include "memory.h"
#include <stdlib.h>

void* AllocAligned(size_t size) {
#if __APPLE__
	void* ptr;
	if (posix_memalign(&ptr, 64, size) != 0)
		ptr = nullptr;
	return ptr;
#else
	return _aligned_malloc(size, 64);
#endif
}

void* ReallocAligned(void* block, int size) {
#if __APPLE__
	// TODO
#else
	return _aligned_realloc(block, size, 64);
#endif
}

// Free cache-aligned memory blocks
void FreeAligned(void* ptr) {
	if (!ptr) return;
#if __APPLE__
	free(ptr);
#else
	_aligned_free(ptr);
#endif
}