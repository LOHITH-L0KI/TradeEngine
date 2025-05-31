#ifndef FIXED_BLOCK_HEAP_H
#define FIXED_BLOCK_HEAP_H

#include "HeapAllocator.h"

namespace Mem {

	class FixedBlockHeap final : public HeapAllocator
	{
	public:
		FixedBlockHeap() = delete;
		FixedBlockHeap(const FixedBlockHeap&) = delete;
		FixedBlockHeap& operator=(const FixedBlockHeap&) = delete;

		FixedBlockHeap(size_t blockSize, size_t blockCount);
		~FixedBlockHeap();

	public:
		// Inherited via HeapAllocator
		void* allocate(size_t size) override;
		void free(Used* addr) override;
	};
}

#endif // !FIXED_BLOCK_HEAP_H
