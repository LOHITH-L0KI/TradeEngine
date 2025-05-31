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

		DynamicBlockHeap(size_t size);
		~DynamicBlockHeap();

	public:
		// Inherited via HeapAllocator
		void* allocate(size_t size) override;
		void free(Used* addr) override;

	private:

		void* getRequiredSizedBlock(Block* nxtFit, size_t reqSize);
		
	};
}

#endif // !DYNAMIC_BLOCK_HEAP_H
