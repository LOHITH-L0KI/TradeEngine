#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "Heap.h"
#include "AllocationStartegy.h"

namespace Mem {

	class Used;
	class Free;

	class HeapAllocator
	{
		struct Header {

			Heap* heap;
			Used* usedHead;
			Free* freeHead;
			AllocationStrategy* allocStrategy;

			Header()
				:usedHead(nullptr),
				freeHead(nullptr),
				allocStrategy(nullptr)
			{}
		};

	public:
		HeapAllocator() = delete;
		HeapAllocator(const HeapAllocator&) = delete;
		HeapAllocator& operator = (const HeapAllocator&) = delete;

		HeapAllocator(Heap* heap, size_t heapIndex);

		virtual ~HeapAllocator();

		virtual Used* allocate(size_t size) = 0;
		virtual void free(Used* addr) = 0;
		Heap* getHeap();

	protected:
		Header _header;
	};
}

#endif // !HEAP_ALLOCATOR_H
