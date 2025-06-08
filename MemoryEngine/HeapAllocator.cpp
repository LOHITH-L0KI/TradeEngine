#include "pch.h"
#include "HeapAllocator.h"
#include "Heap.h"

namespace Mem {
	
	HeapAllocator::HeapAllocator(Heap* heap, size_t heapIndex)
		:_header()
	{
		_header.heap = heap;
	}

	HeapAllocator::~HeapAllocator()
	{
	}

	Heap* HeapAllocator::getHeap()
	{
		return _header.heap;
	}
}
