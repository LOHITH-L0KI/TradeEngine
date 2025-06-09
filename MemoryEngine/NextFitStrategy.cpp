#include "pch.h"
#include "NextFitStrategy.h"
#include "Free.h"

namespace Mem {

    NextFitStrategy::NextFitStrategy(Free* initFree)
        :AllocationStrategy(),
        _nextFit(initFree)
    {

    }

    NextFitStrategy::~NextFitStrategy()
    {
        //do nothing, _nextFit is owned by heap object
    }

    Free* NextFitStrategy::FindNext(size_t reqSize)
    {
        Free* allocAddr = nullptr;

        if (_nextFit != nullptr) {

            //find free space using nextfit
            Block* nxtFit = _nextFit;
            //check if current nextfit is enough to allocate required size
            if (_nextFit->GetSize() < reqSize) {

                //go to next availabe free blocks till you get required size
                _nextFit = static_cast<Free*>(_nextFit->GetNext());

                while (nxtFit != _nextFit && _nextFit->GetSize() < reqSize) {
                    _nextFit = static_cast<Free*>(_nextFit->GetNext());
                }
            }

            //found nextfit. Check if it has enough space to accomidate requested size. 
            size_t availSize = _nextFit->GetSize();

            //split the _nextFit id availabe size > requested size
            if (availSize > reqSize)
                splitNextFitToAllocate(reqSize);

            allocAddr = _nextFit;

            //update nextfit
            _nextFit = (Free*)nxtFit->GetNext();
        }

        return allocAddr;
    }
    
    void NextFitStrategy::splitNextFitToAllocate(size_t reqSize)
    {
        Block* nxtFit = _nextFit;
        size_t extraSize = nxtFit->GetSize() - reqSize;

        if (extraSize > sizeof(Block)) {
            //split the block into 2 parts

            //update the free pointer
            Free* newFree = (Free*)((size_t)(nxtFit + 1) + reqSize);

            //Basic Stats
            newFree->SetToFree();                               //newFree::Set to Free
            newFree->SetSize(extraSize - sizeof(Block));        //newFree::Set Size
            newFree->SetAboveFree();                            //newFree::Set Above is free
            newFree->SetHeapIndex(nxtFit->GetHeapIndex());      //newFree::Set HeapIndex

            //navgaiton pointers                    
            /*
            * Make sure that you update gobale prev of 
            * newFree block and nextFree block of _nextFree
            */
            newFree->SetGlobalPrev(nxtFit);                     //newFree::Global Prev
            Block* gNext = nxtFit->GetGlobalNext();
            if (gNext)
                gNext->SetGlobalPrev(newFree);                  //nextFit->gNext:: Global Prev

            newFree->SetPrev(nxtFit);                           //newFree::Prev Free
            Block* next = nxtFit->GetNext();                    
            newFree->SetNext(next);                             //newFree::Next Free    
            if (next)
                next->SetPrev(newFree);                         //nextFit->next:: Prev Free
            
            nxtFit->SetNext(newFree);                           //nextFit::Next Free
                
            //update the next fit size      
            nxtFit->SetSize(reqSize);                           //nextFit::Size
        }
    }

    bool NextFitStrategy::HandleFree(Free* freeBlk)
    {
        bool handled = false;
        //check if valid pointer
        if (!freeBlk)
            return handled;



        return handled;
    }
}
