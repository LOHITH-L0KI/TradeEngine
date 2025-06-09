#ifndef NEXT_FIT_STRATEGY_H
#define NEXT_FIT_STRATEGY_H

#include "AllocationStartegy.h"

namespace Mem {
    class NextFitStrategy final:  public AllocationStrategy
    {
    public:
        NextFitStrategy() = delete;
        NextFitStrategy(Free* initFree);

        ~NextFitStrategy();

        // Inherited via AllocationStrategy
        Free* FindNext(size_t reqSize) override;
        bool HandleFree(Free* freeBlk) override;

    private:
        void splitNextFitToAllocate(size_t reqSize);

    public:
        Free* _nextFit;

    };
}

#endif // !NEXT_FIT_STRATEGY_H
