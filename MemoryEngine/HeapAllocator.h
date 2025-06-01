#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "Used.h"
#include "Free.h"

class Heap;
namespace Mem {

	class HeapAllocator
	{
		struct Header {

			Heap* heap;
			Used* usedHead;
			Free* freeHead;
			Free* nextFit;

			Header()
				:usedHead(nullptr),
				freeHead(nullptr),
				nextFit(nullptr)
			{}
		};

	public:
		HeapAllocator() = delete;
		HeapAllocator(const HeapAllocator&) = delete;
		HeapAllocator& operator = (const HeapAllocator&) = delete;

		HeapAllocator(size_t heapIndex);

		virtual ~HeapAllocator();

		virtual Used* allocate(size_t size) = 0;
		virtual void free(Used* addr) = 0;

	protected:
		Header _header;
	};
}

#endif // !HEAP_ALLOCATOR_H
