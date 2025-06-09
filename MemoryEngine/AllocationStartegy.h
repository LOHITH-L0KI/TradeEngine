#ifndef ALLOCATION_STRATEGY_H
#define ALLOCATION_STRATEGY_H

namespace Mem {

	class Free;
	class AllocationStrategy {

	public:
		AllocationStrategy() {};
		virtual ~AllocationStrategy() {};
		virtual Free* FindNext(size_t reqSize) = 0;
		virtual bool HandleFree(Free* freeBlk) = 0;

	};
}
#endif // !ALLOCATION_STRATEGY_H
