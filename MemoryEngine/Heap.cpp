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

	constexpr size_t USED_HEADERS_EXTRA_SPACE = 100 * sizeof(Used);

	struct HeapGaurd {
		Used rGlobalHead;
	};

	Heap::~Heap()
	{
		free(_allocator);
	}

	Heap::Heap(format fmt , size_t heapSize)
		:Heap(format::dynamic_blocks, heapSize, align::def)
	{
	}

	Heap::Heap(format fmt, size_t heapSize, align alignment)
		:_info(heapSize, alignment),
		_allocator(nullptr)
	{
		void* mem = malloc(heapSize + sizeof(DynamicBlockHeap) + sizeof(HeapGaurd));
		assert(mem);

		//update HeapTable.
		size_t index = HeapTable::Add(mem);
		if(index != HeapTable::npos)
		_allocator = new(mem)DynamicBlockHeap(this, heapSize, index);
		HeapTable::Update(index, _allocator);

		//set heap gaurd at the end of allocator
		void* allocEnd = (void*)((size_t)(_allocator + 1) + heapSize);
		HeapGaurd* gaurd = new(allocEnd) HeapGaurd();
		gaurd->rGlobalHead.SetGlobalPrev((Block*)(_allocator + 1));
	}

	Heap::Heap(format fmt, size_t blockSize, size_t blockCount)
		:Heap(format::fixed_blocks, blockSize, blockCount, align::def)
	{
	}

	Heap::Heap(format fmt, size_t blockSize, size_t blockCount, align alignment)
		:_info(blockSize * blockCount, alignment),
		_allocator(nullptr)
	{
		size_t heapSize = blockSize * blockCount;

		void* mem = malloc(heapSize + sizeof(FixedBlockHeap));
		assert(mem);

		//update HeapTable.
		size_t index = HeapTable::Add(mem);
		if (index != HeapTable::npos)
		_allocator = new(mem)FixedBlockHeap(this, blockSize, blockCount, index);
		HeapTable::Update(index, _allocator);

		//set heap gaurd at the end of allocator
		void* allocEnd = (void*)((size_t)(_allocator + 1) + heapSize);
		HeapGaurd* gaurd = new(allocEnd) HeapGaurd();
		gaurd->rGlobalHead.SetGlobalPrev((Block*)(_allocator + 1));
	}

	void* Heap::allocate(size_t size)
	{
		void* addr = nullptr;

		size_t unalignedBytes = size % _info.heapAlign;
		size_t bytesToAlign = unalignedBytes == 0 ? 0 : _info.heapAlign - unalignedBytes;
		size_t alignedSize = size + bytesToAlign;

		Used* usdBlk = _allocator->allocate(alignedSize);

		if (usdBlk) {
			
			_info.freeSize -= alignedSize;
			_info.usedSize += alignedSize;
			_info.currUsedBlocks++;
			if(_info.currUsedBlocks > _info.maxUsedBlocks)
				_info.maxUsedBlocks = _info.currUsedBlocks;

			//update object pointer to have only requested space by user.
			addr = (void*)((char*)usdBlk + sizeof(Used) + bytesToAlign);
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
		size_t heapEnd =  heapBeg + _info.totalSize + USED_HEADERS_EXTRA_SPACE;
		size_t addrVal = (size_t)addr;
		
		if (addrVal >= heapBeg && addrVal < heapEnd) {

			Used* used = ((Used*)addr) - 1;
			size_t allocSize = used->GetSize();

			_allocator->free(used);

			_info.freeSize += allocSize;
			_info.usedSize -= allocSize;
			_info.currUsedBlocks--;

		}
		else {

			//does not belong to this heap
		}

	}

	size_t Heap::size()
	{
		return _info.totalSize;
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
		std::cout << "\t\t" << "| Type |" << "\t" << "|	Size	|" << std::endl;
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
