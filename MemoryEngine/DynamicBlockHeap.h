#ifndef DYNAMIC_BLOCK_HEAP_H
#define DYNAMIC_BLOCK_HEAP_H

#include "HeapAllocator.h"

namespace Mem {

	class DynamicBlockHeap final: public HeapAllocator
	{
	public:
		DynamicBlockHeap() = delete;
		DynamicBlockHeap(const DynamicBlockHeap&) = delete;
		DynamicBlockHeap& operator= (const DynamicBlockHeap&) = delete;

		DynamicBlockHeap(Heap* heap, size_t size, size_t heapIndex);
		~DynamicBlockHeap();

	public:
		// Inherited via HeapAllocator
		Used* allocate(size_t size) override;
		void free(Used* addr) override;

	private:

		Used* makeFreeToUsedBlock(Free* freeBlk);
	};
}

#endif // !DYNAMIC_BLOCK_HEAP_H
