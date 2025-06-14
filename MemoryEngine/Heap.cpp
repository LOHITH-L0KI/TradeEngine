#include "pch.h"
#include "Heap.h"
#include "Used.h"
#include "DynamicBlockHeap.h"
#include "FixedBlockHeap.h"
#include "HeapTable.h"
#include <new>
#include <cassert>
#include <iostream>

namespace Mem {

	struct HeapGaurd {
		Used rGlobalHead;
	};

	Heap::~Heap()
	{
		free(_allocator);
	}

	Heap::Heap(DYNAMIC_HEAP_DESC& dynamicBuilder)
		:_info(dynamicBuilder.heapSize, dynamicBuilder.alignment, format::dynamic_blocks),
		_allocator(nullptr)
	{
		void* mem = malloc(_info.totalSize + sizeof(DynamicBlockHeap) + sizeof(HeapGaurd) + _info.heapAlign);
		assert(mem);

		//align heapallocator.
		size_t alignedMem = ((size_t)mem + sizeof(DynamicBlockHeap) + sizeof(Block) + _info.heapAlign - 1) & (~(_info.heapAlign - 1));

		//update HeapTable.
		size_t index = HeapTable::Add((void*)alignedMem);
		if (index != HeapTable::npos)
			_allocator = new((void*)alignedMem)DynamicBlockHeap(this, _info.totalSize, index);

		//set heap gaurd at the end of allocator
		void* allocEnd = (void*)((size_t)(_allocator + 1) + _info.totalSize);
		HeapGaurd* gaurd = new(allocEnd) HeapGaurd();
		gaurd->rGlobalHead.SetGlobalPrev((Block*)(_allocator + 1));
	}

	Heap::Heap(FIXED_HEAP_DESC& fixedBuilder)
		:_info(fixedBuilder.blockSize * fixedBuilder.numberOfBlocks, fixedBuilder.alignment, format::fixed_blocks),
		_allocator(nullptr)
	{
		size_t padding = 0;
		if (size_t predictedNextUserAddr = (fixedBuilder.blockSize + sizeof(Block)) % fixedBuilder.alignment) {
			padding = fixedBuilder.alignment - predictedNextUserAddr;
		}

		size_t alignedBlockSize = fixedBuilder.blockSize + padding;
		size_t heapSize = alignedBlockSize * fixedBuilder.numberOfBlocks;

		void* mem = malloc(heapSize + sizeof(FixedBlockHeap) + sizeof(HeapGaurd) + fixedBuilder.alignment);
		assert(mem);

		//align heapallocator.
		size_t alignedMem = ((size_t)mem + sizeof(FixedBlockHeap) + sizeof(Block) + fixedBuilder.alignment - 1) & (~(fixedBuilder.alignment - 1));

		//update HeapTable.
		size_t index = HeapTable::Add(mem);
		if (index != HeapTable::npos)
			_allocator = new(mem)FixedBlockHeap(this, fixedBuilder.blockSize, fixedBuilder.numberOfBlocks, index);

		//set heap gaurd at the end of allocator
		void* allocEnd = (void*)((size_t)(_allocator + 1) + heapSize);
		HeapGaurd* gaurd = new(allocEnd) HeapGaurd();
		gaurd->rGlobalHead.SetGlobalPrev((Block*)(_allocator + 1));
	}

#ifdef _MEMORY_TRACK
	void* Heap::allocate(size_t size, char* fileName, uint32_t lineNum)
	{
		void* allocAddr = allocate(size);

		if(allocAddr)
			_memTracker.insert(allocAddr, fileName, lineNum);

		return allocAddr;
	}

	void Heap::printLeaks()
	{
		std::cout << "****** Memory Tracker Start ******" << std::endl;
		if (_memTracker._table.empty()) {
			std::cout << "***** No Memory Leaks *****" << std::endl;
			return;
		}else
			std::cout << "***** Found Memory Leaks *****" << std::endl;

		auto itr = _memTracker._table.begin();
		auto end = _memTracker._table.end();

		while (itr != end) {
			std::cout << "Source:: " << itr->second.fileName
				<< ", at Line:: " << itr->second.line
				<< std::endl;

			itr = itr++;
		}

		std::cout << "****** Memory Tracker End ******" << std::endl;
	}
#endif

	void* Heap::allocate(size_t size)
	{
		void* addr = nullptr;

		Used* usdBlk = _allocator->allocate(size);

		if (usdBlk) {
			
			size_t usedSize = usdBlk->GetSize();
			_info.freeSize -= usedSize;
			_info.usedSize += usedSize;

			_info.currUsedBlocks++;
			if(_info.currUsedBlocks > _info.maxUsedBlocks)
				_info.maxUsedBlocks = _info.currUsedBlocks;

			//get pointer to user space.
			addr = (void*)((char*)usdBlk + sizeof(Used));
		}
		else {

			//no block of size >= 'size' available 
		}

		return addr;
	}

	void Heap::release(void* addr)
	{
		//check if this is a valid address
		size_t heapBeg = (size_t)((void*)_allocator);
		size_t heapEnd =  heapBeg + _info.totalSize;
		size_t addrVal = (size_t)addr;
		
		if (addrVal >= heapBeg && addrVal < heapEnd) {

			Used* used = ((Used*)addr) - 1;
			size_t allocSize = used->GetSize();

			_allocator->free(used);

			_info.freeSize += allocSize;
			_info.usedSize -= allocSize;
			_info.currUsedBlocks--;

#ifdef _MEMORY_TRACK
			_memTracker.remove(addr);
#endif // _MEMORY_TRACK


		}
		else {

			//does not belong to this heap
		}

	}

	size_t Heap::size()
	{
		return _info.totalSize;
	}

	Heap::align Heap::alignement()
	{
		return _info.heapAlign;
	}

	size_t Heap::end()
	{
		size_t end = (size_t)(this->_allocator + 1) + _info.totalSize;
		return end;
	}

	void Heap::status(Info& status)
	{
		status = _info;
	}

	void Heap::printHeapFragmentation()
	{
		std::cout << "-----------Heap Dump :: Begin-----------\n";
		//Print Heap Stats
		std::cout << "\tHeap Stats::\n";
		std::cout << "\t\tToatalSize - " << _info.totalSize << std::endl;
		std::cout << "\t\tUsedSize - " << _info.usedSize << std::endl;
		std::cout << "\t\tFreeSize - " << _info.freeSize << std::endl;
		std::cout << "\t\tCurrUsedBlocks - " << _info.currUsedBlocks << std::endl;
		std::cout << "\t\tMaxUsedBlocks - " << _info.maxUsedBlocks << std::endl;
		std::cout << "\t\tALignment - " << _info.heapAlign << "bytes" << std::endl;

		//Print heap fragments (Blocks)
		Block* heapBegin = (Block*)((size_t)(_allocator + 1));
		
		void* allocEnd = (void*)((size_t)(_allocator + 1) + _info.totalSize);
		HeapGaurd* gaurd = (HeapGaurd*)allocEnd;
		Block* heapEnd = &gaurd->rGlobalHead;
		
		Block* currBlock = heapBegin;

		std::cout << "\tHeap Blocks::\n";
		std::cout << "\t\t" << "| Type |" << "\t" << "|	Size |" << std::endl;
		std::cout << "\t\t" << "------------------------------------" << std::endl;

		while (currBlock && currBlock != heapEnd) {

			std::cout << "\t\t"
				<< "| " << (currBlock->GetType() == Block::FREE ?"FREE" : "USED") << " |"
				<< "\t"
				<< "| " << currBlock->GetSize() << " |"
				<< std::endl;

			currBlock = currBlock->GetGlobalNext();
		}

		std::cout << "-----------Heap Dump :: End-----------\n";
	}
}
