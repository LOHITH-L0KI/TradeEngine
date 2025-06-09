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
        //First update the size to include alignment
        size_t padding = 0;
        size_t heapAlign = _header.heap->alignement();

        if (size_t predictedNextUserAddr = (size + sizeof(Block)) % heapAlign) {
            padding = heapAlign - predictedNextUserAddr;
        }

        size_t alignedSize = size + padding;

        //allocate block with alignedsize.
        Free* freeBlk = _header.allocStrategy->FindNext(alignedSize);
        
        //update if freeHead
        if (freeBlk == _header.freeHead)
            _header.freeHead = static_cast<Free*>(freeBlk->GetNext());
         
        Used* usedBlk = makeFreeToUsedBlock(freeBlk);
        return usedBlk;
    }

    void DynamicBlockHeap::free(Used* usedBlk)
    {
        //update usedblock pointers and make it free
        
        //update neighbours
        Block* next = usedBlk->GetNext();
        Block* prev = usedBlk->GetPrev();

        if (next) {
            next->SetPrev(prev);
            usedBlk->SetNext(nullptr);
        }

        if (prev) {
            prev->SetNext(next);
            usedBlk->SetPrev(nullptr);
        }

        //since neighbours are update and this block is excluded from used list,
        //we can change this block to free. But remenber, it is not yet added to free list
        Free* freeBlk = (Free*)usedBlk;
        freeBlk->SetToFree();
        
        //send this free block to allocStrategy to handle it.
        _header.allocStrategy->HandleFree(freeBlk);
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
