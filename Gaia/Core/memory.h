#pragma once

#ifndef MEMORY_H
#define MEMORY_H

// Allocate cache-aligned memory blocks

void* AllocAligned(size_t size);
void* ReallocAligned(void* block, int size);
void FreeAligned(void* ptr);

template <typename T> T* AllocAligned(size_t count) {
	return (T*)AllocAligned(count * sizeof(T));
}
template <typename T> T* ReallocAligned(void* block, int count) {
	return (T*)ReallocAligned(block, count * sizeof(T));
}

#endif // !MEMORY_H