#include "pch.h"
#include "DynamicBlockHeap.h"
#include <malloc.h>
#include <new>

namespace Mem {

    DynamicBlockHeap::DynamicBlockHeap(size_t size, size_t heapIndex)
        :HeapAllocator(heapIndex)
    {
        Free* mem = (Free*)(this + 1);
        if (size > sizeof(Free)) {
            _header.freeHead = new(mem) Free(size - sizeof(Free));
            _header.freeHead->SetHeapIndex(heapIndex);
            _header.freeHead->SetAboveUsed();
        }

        _header.nextFit = _header.freeHead;
    }

    DynamicBlockHeap::~DynamicBlockHeap()
    {
    }

    Used* DynamicBlockHeap::allocate(size_t size)
    {
        Used* addr = allocateFromNextFit(size);

        return addr;
    }

    void DynamicBlockHeap::free(Used* addr)
    {

    }

    Used* DynamicBlockHeap::allocateFromNextFit(size_t reqSize)
    {
        Used* allocAddr = nullptr;
        //find free space using nextfit
        Block* nxtFit = _header.nextFit;

        if (nxtFit->GetSize() < reqSize) {

            nxtFit = nxtFit->GetNext();

            while (nxtFit != _header.nextFit && nxtFit->GetSize() < reqSize) {
                nxtFit = nxtFit->GetNext();
            }

        }

        //found nextfit. Check if it has enough space to accomidate requested size. 
        size_t availSize = nxtFit->GetSize();
        if ((nxtFit->GetType() == Block::Type::FREE) && (availSize >= reqSize)) {
            
            _header.nextFit = (Free*)nxtFit;
            
            adjustNextFitToAllocate(reqSize);
            allocAddr = updateNextFitToUsedBlock();
        }

        return allocAddr;
    }

    void DynamicBlockHeap::adjustNextFitToAllocate(size_t reqSize)
    {
        Block* nxtFit = _header.nextFit;

        size_t extraSize = nxtFit->GetSize() - reqSize;
        if (extraSize > sizeof(Block)) {
            //split the block into 2 parts

            //update the free pointer
            Free* newFree = (Free*)((size_t)(nxtFit + 1) + reqSize);
            //Basic Stats
            newFree->SetSize(extraSize - sizeof(Block));
            newFree->SetToFree();
            newFree->SetAboveFree();
            newFree->SetHeapIndex(nxtFit->GetHeapIndex());

            //Global Pointers
            newFree->SetGlobalPrev(nxtFit);
            Block* gNext = nxtFit->GetGlobalNext();
            if (gNext) {
                gNext->SetGlobalPrev(newFree);
            }

            //Type Specific Pointers
            newFree->SetPrev(nxtFit);

            Block* next = nxtFit->GetNext();
            if (next) {
                newFree->SetNext(next);
                next->SetPrev(newFree);
            }

            nxtFit->SetNext(newFree);

            //update the next fit size
            nxtFit->SetSize(reqSize);
        }
    }

    Used* DynamicBlockHeap::updateNextFitToUsedBlock()
    {
        Block* nxtFit = _header.nextFit;

        if (nxtFit != nullptr) {
            
            //point nextfit to next available free block
            _header.nextFit = (Free*)nxtFit->GetNext();

            //update freeblock to used
            nxtFit->SetToUsed();

            //update next and prev offset
            Block* prev = nxtFit->GetPrev();
            Block* next = nxtFit->GetNext();

            if (prev)
                prev->SetNext(next);

            if (next)
                next->SetPrev(prev);

            //used block next and prev offset.
            nxtFit->SetPrev(nullptr);
            if (_header.usedHead) {
                nxtFit->SetNext(_header.usedHead);
                _header.usedHead->SetPrev(nxtFit);
            }

            _header.usedHead = (Used*)nxtFit;
        }

        return (Used*)nxtFit;
    }

}
