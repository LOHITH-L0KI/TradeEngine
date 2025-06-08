#include "pch.h"
#include "Block.h"
#include "HeapTable.h"
#include "HeapAllocator.h"

namespace Mem {

// _magic - stores black size, type and if above is free.
//-------------------------------- 
//	data				|	bits
//-------------------------------- 
// 	globalPrevOffset	|	63 - 34
//	currSize			|	33 - 4
// 	reserve				|	3 - 2
//	Type				|	1 (true - Used, false - Free)
//	Is_Above_Free		|	0 (true | false)

#define GET_GLOBAL_PREV_OFFSET(x) (x >> 34)
#define SET_GLOBAL_PREV_OFFSET(x, off) x = ((x & ((1ull << 34) - 1)) | (off << 34))

#define GET_SIZE(x) ((x >> 4) & ((1ull << 30) - 1))
//#define SET_SIZE(x, size) x = ( (x & ((~((1ull << 34) - 1)) | (~((1ull << 4) - 1)))) | (size << 4))
#define SET_SIZE(x, size) x = ((x & (~(((1ull << 30) - 1) << 4))) | (size << 4))

#define GET_TYPE(x) ((x >> 1) & 0x1ull)
#define SET_TO_USED(x) x |= 0x2ull
#define SET_TO_FREE(x) x &= ~(0x2ull)

#define IS_ABOVE_FREE(x) (bool)(x & 0x1ull)
#define SET_ABOVE_FREE(x) x |= 0x1ull
#define SET_ABOVE_USED(x) x &= ~(0x1ull)

// _nav - offset to type specific prev and next. 
//-------------------------------- 
//	data				|	bits 
//--------------------------------
//	heapAddrIndx		|	63 - 60
//	prev Offset			|	59 - 30 
//	next Offset			|	29 - 0 

#define GET_HEAP_INDEX(x) (x >> 60);
#define SET_HEAP_INDEX(x, index) x = ((x & ((1ull << 60) - 1)) | (index << 60))

#define GET_NEXT_OFFSET(x) (x & ((1ull << 30) - 1))
#define SET_NEXT_OFFSET(x, offset) x = ((x & (~((1ull << 30) - 1))) | offset)

#define GET_PREV_OFFSET(x) ((x >> 30) &  ((1ull << 30) - 1))
#define SET_PREV_OFFSET(x, offset) x = ((x & (~(((1ull << 30) - 1) << 30))) | (offset << 30))

	Block::Block()
		:_magic(0),
		_nav(0)
	{
	}

	Block::Block(size_t size, Type type)
		:_magic(0),
		_nav(0)
	{
		SET_SIZE(_magic, size);

		if (type == Type::USED)
			SET_TO_USED(_magic);
		else
			SET_TO_FREE(_magic);

	}

	Block* Block::GetNext()
	{
		Block* next = nullptr;
		size_t nextOffSet = GET_NEXT_OFFSET(_nav);

		if (nextOffSet > 0) {

			size_t heapIndex = GET_HEAP_INDEX(_nav)
			size_t addr = HeapTable::GetAddress(heapIndex) + nextOffSet;

			if (addr != nextOffSet)
				next = (Block*)(addr);
		}

		return next;
	}

	void Block::SetNext(Block* blk)
	{
		if (blk) {
			size_t heapIndex = GET_HEAP_INDEX(_nav)
			size_t heapaddr = HeapTable::GetAddress(heapIndex);

			if (heapaddr != HeapTable::npos) {
				SET_NEXT_OFFSET(_nav, (size_t)blk - heapaddr);
			}
		}else
			SET_NEXT_OFFSET(_nav, 0ul);
	}

	Block* Block::GetPrev()
	{
		Block* prev = nullptr;
		size_t prevOffSet = GET_PREV_OFFSET(_nav);

		if (prevOffSet > 0) {

			size_t heapIndex = GET_HEAP_INDEX(_nav)
			size_t addr = HeapTable::GetAddress(heapIndex) + prevOffSet;

			if (addr != prevOffSet)
				prev = (Block*)(addr);
		}

		return prev;
	}

	void Block::SetPrev(Block* blk)
	{
		if (blk) {
			size_t heapIndex = GET_HEAP_INDEX(_nav)
			size_t heapaddr = HeapTable::GetAddress(heapIndex);

			if (heapaddr != HeapTable::npos) {
				SET_PREV_OFFSET(_nav, ((size_t)blk) - heapaddr);
			}
		}
		else
			SET_PREV_OFFSET(_nav, 0ul);
	}

	Block* Block::GetGlobalPrev()
	{
		Block* prev = nullptr;
		size_t prevOffSet = GET_GLOBAL_PREV_OFFSET(_magic);

		if (prevOffSet > 0) {

			size_t addr = (size_t)this - prevOffSet;

			if (addr != prevOffSet)
				prev = (Block*)(addr);
		}

		return prev;
	}

	void Block::SetGlobalPrev(Block* blk)
	{
		if(blk)
			SET_GLOBAL_PREV_OFFSET(_magic, ((size_t)this) - (size_t)blk);
		else
			SET_GLOBAL_PREV_OFFSET(_magic, 0ull);
	}

	Block* Block::GetGlobalNext()
	{
		Block* gNext = nullptr;
		size_t heapIndex = GET_HEAP_INDEX(_nav)
		size_t heapAddr = HeapTable::GetAddress(heapIndex);

		if (heapAddr != HeapTable::npos) {
			if (HeapAllocator* heapAlloc = (HeapAllocator*)(heapAddr)) {
				Heap* heap = heapAlloc->getHeap();
				size_t gNextAddr = (size_t)(this + 1) + GET_SIZE(_magic);

				if (heap && gNextAddr < heap->end())
					gNext = (Block*)(gNextAddr);
			}
		}

		return gNext;
	}

	void Block::SetHeapIndex(size_t idx)
	{
		SET_HEAP_INDEX(_nav, idx);
	}

	size_t Block::GetHeapIndex()
	{
		return GET_HEAP_INDEX(_nav);
	}

	size_t Block::GetSize() const
	{
		return GET_SIZE(_magic);
	}

	void Block::SetSize(size_t size)
	{
		SET_SIZE(_magic, size);
	}

	Block::Type Block::GetType() const
	{
		Block::Type type = static_cast<Block::Type>(GET_TYPE(_magic));

		return type;
	}

	void Block::SetToUsed()
	{
		SET_TO_USED(_magic);
	}

	void Block::SetToFree()
	{
		SET_TO_FREE(_magic);
	}

	bool Block::IsAboveFree() const
	{
		return IS_ABOVE_FREE(_magic);
	}

	void Block::SetAboveFree()
	{
		SET_ABOVE_FREE(_magic);
	}

	void Block::SetAboveUsed()
	{
		SET_ABOVE_USED(_magic);
	}
}
