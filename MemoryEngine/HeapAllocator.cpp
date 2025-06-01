#include "pch.h"
#include "HeapAllocator.h"
#include "Free.h"

namespace Mem {
	
	HeapAllocator::HeapAllocator(size_t heapIndex)
		:_header()
	{
	}

	HeapAllocator::~HeapAllocator()
	{
	}
}
