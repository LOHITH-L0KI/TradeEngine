#include "pch.h"
#include "FixedBlockHeap.h"

namespace Mem {
    
    FixedBlockHeap::FixedBlockHeap(size_t blockSize, size_t blockCount, size_t heapIndex)
        :HeapAllocator(heapIndex)
    {

    }

    FixedBlockHeap::~FixedBlockHeap()
    {
    }

    Used* FixedBlockHeap::allocate(size_t size)
    {
        return nullptr;
    }

    void FixedBlockHeap::free(Used* addr)
    {
    }
}
