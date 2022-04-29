#ifndef MEMORYREGION_H
#define MEMORYREGION_H

#include <algorithm>
#include <cstdint>
#include <malloc.h>
#include <list>
#include <memory>

void* AllocAligned(size_t size, int alignment = 64);
void FreeAligned(void* ptr);

// Allow for the allocation of arrays with AllocAligned<T>(n)
template <typename T>
T* AllocAligned(size_t count) {
	return (T*)AllocAligned(count * sizeof(T));
}

// Region-based memory management
class MemoryRegion
{
public:
	MemoryRegion(size_t size) :blockSize(size) {}

	void* Alloc(size_t n) {

		// Round up number of bytes to the nearest 16
		n = ((n + 15) & (~15));

		// Check if there is enough space in this block to store the data
		if (currentBlockPosition + n > currentAllocSize) {
			// Add the current (full) block to the  blocks list
			if (currentBlock) {
				usedBlocks.push_back(std::make_pair(currentAllocSize, currentBlock));
				currentBlock = nullptr;
			}

			// Check if there is a block of memory already available
			for (auto iterator = availableBlocks.begin(); iterator != availableBlocks.end(); iterator) {
				if (iterator->first >= n) {
					currentAllocSize = iterator->first;
					currentBlock = iterator->second;
					availableBlocks.erase(iterator);
					break;
				}
			}

			// Create a new block of memory
			if (!currentBlock) {
				currentAllocSize = std::max(n, blockSize);
				currentBlock = AllocAligned<uint8_t>(currentAllocSize);
			}
			currentBlockPosition = 0;
		}

		// Return a pointer to the next unused space in memory
		void* ptr = currentBlock + currentBlockPosition;
		currentBlockPosition += n;
		return ptr;
	}

	// Reset the region and set all blocks to available.
	void Reset() {
		availableBlocks.splice(availableBlocks.begin(), usedBlocks);
		currentBlockPosition = 0;
	}

private:
	const size_t blockSize;
	size_t currentBlockPosition = 0, currentAllocSize = 0;
	uint8_t* currentBlock = nullptr;
	std::list<std::pair<size_t, uint8_t*>> usedBlocks, availableBlocks;
};

#endif // !MEMORYREGION_H
