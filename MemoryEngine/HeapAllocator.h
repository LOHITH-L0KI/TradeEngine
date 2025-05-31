#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include "Used.h"
#include "Free.h"

namespace Mem {

	class HeapAllocator
	{
		struct Header {

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
		HeapAllocator();
		HeapAllocator(const HeapAllocator&) = delete;
		HeapAllocator& operator = (const HeapAllocator&) = delete;

		virtual ~HeapAllocator();

		virtual void* allocate(size_t size) = 0;
		virtual void free(Used* addr) = 0;

	protected:
		Header _header;
	};
}

#endif // !HEAP_ALLOCATOR_H
