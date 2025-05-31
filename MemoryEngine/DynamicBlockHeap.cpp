#include "DynamicBlockHeap.h"
#include <malloc.h>
#include <new>

namespace Mem {

    DynamicBlockHeap::DynamicBlockHeap(size_t size)
    {
        Free* mem = (Free*)(this + 1);
        if (size > sizeof(Free)) {
            _header.freeHead = new(mem) Free(size - sizeof(Free));
            _header.freeHead->SetAboveUsed();
        }

        _header.nextFit = _header.freeHead;
    }

    DynamicBlockHeap::~DynamicBlockHeap()
    {
    }

    void* DynamicBlockHeap::allocate(size_t size)
    {
        void* addr = nullptr;

        //find free space using nextfit
        Block* nxtFit = _header.nextFit;

        if (nxtFit->GetSize() < size) {

            nxtFit = nxtFit->GetNext();
            
            while (nxtFit != _header.nextFit && nxtFit->GetSize() < size) {
                nxtFit = nxtFit->GetNext();
            }

        }

        if (!(nxtFit->GetSize() < size) && (nxtFit->GetType() == Block::Type::FREE))
            addr = getRequiredSizedBlock(nxtFit, size);

        return addr;
    }

    void DynamicBlockHeap::free(Used* addr)
    {

    }

    void* DynamicBlockHeap::getRequiredSizedBlock(Block* nxtFit, size_t reqSize)
    {
        void* blk = nullptr;

        size_t nxtSize = nxtFit->GetSize();

        //allocate only required size
        size_t extraSize = nxtSize - reqSize;

        //no use of just having a headder with no space for data
        if (extraSize > sizeof(Block)) {
            //split the block into 2 parts
            //update the free pointer

            Free* newFree = (Free*)((size_t)(nxtFit + 1) + reqSize);
            //Basic Stats
            newFree->SetSize(extraSize - sizeof(Block));
            newFree->SetToFree();
            newFree->SetAboveUsed();

            //Global Pointers
            newFree->SetGlobalPrev(nxtFit);
            Block* gNext = nxtFit->GetGlobalNext();
            if (gNext) {
                gNext->SetGlobalPrev(newFree);
            }

            //Type Specific Pointers
            Block* prev = nxtFit->GetPrev();
            Block* next = nxtFit->GetNext();

            if (prev) {
                newFree->SetPrev(prev);
                prev->SetNext(newFree);
            }

            if (next) {
                newFree->SetNext(next);
                next->SetPrev(newFree);
            }

            //make a Used Block
            nxtFit->SetSize(reqSize);
        }

        nxtFit->SetToUsed();

        if (_header.usedHead) {
            nxtFit->SetNext(_header.usedHead);
            _header.usedHead->SetPrev(nxtFit);
        }

        _header.usedHead = (Used*)nxtFit;

        return _header.usedHead;
    }
}
