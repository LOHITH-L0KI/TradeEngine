#include "pch.h"
#include "DynamicBlockHeap.h"
#include "NextFitStrategy.h"
#include "Free.h"
#include "Used.h"
#include <malloc.h>
#include <new>


namespace Mem {

    DynamicBlockHeap::DynamicBlockHeap(Heap* heap, size_t size, size_t heapIndex)
        :HeapAllocator(heap, heapIndex)
    {
        Free* mem = (Free*)(this + 1);
        if (size > sizeof(Free)) {
            _header.freeHead = new(mem) Free(size - sizeof(Free));
            _header.freeHead->SetHeapIndex(heapIndex);
            _header.freeHead->SetAboveUsed();
        }

        _header.allocStrategy = new NextFitStrategy(_header.freeHead);
    }

    DynamicBlockHeap::~DynamicBlockHeap()
    {
    }

    Used* DynamicBlockHeap::allocate(size_t size)
    {
        Free* freeBlk = _header.allocStrategy ? _header.allocStrategy->FindNext(size) : nullptr;
        Used* usedBlk = makeFreeToUsedBlock(freeBlk);

        return usedBlk;
    }

    void DynamicBlockHeap::free(Used* addr)
    {

    }

    Used* DynamicBlockHeap::makeFreeToUsedBlock(Free* freeBlk)
    {
        Used* usedBlk = nullptr;

        if (freeBlk != nullptr) {

            /*
            * Remove this free block from free list by updating the next and previous pointers
            */
            Block* prev = freeBlk->GetPrev();
            Block* next = freeBlk->GetNext();

            if (prev)
                prev->SetNext(next);

            if (next) {
                next->SetPrev(prev);
                next->SetAboveUsed();
            }

            /*
            * Update freeblock to used.
            * No need to update global navigation as no new blocks are created.
            */
            usedBlk = (Used*)freeBlk;
            usedBlk->SetToUsed();

            //add this used block to head of used list.
            usedBlk->SetPrev(nullptr);
            if (_header.usedHead) {
                usedBlk->SetNext(_header.usedHead);
                _header.usedHead->SetPrev(usedBlk);
            }

            _header.usedHead = usedBlk;
        }

        return usedBlk;
    }
}
