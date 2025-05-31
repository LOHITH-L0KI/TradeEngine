#include "Heap.h"
#include "Used.h"
#include "DynamicBlockHeap.h"
#include "FixedBlockHeap.h"
#include <new>
#include <cassert>

namespace Mem {

	constexpr size_t USED_HEADERS_EXTRA_SPACE = 100 * sizeof(Used);

	Heap::Heap(format fmt , size_t heapSize)
		:Heap(format::dynamic_blocks, heapSize, align::def)
	{
	}

	Heap::Heap(format fmt, size_t heapSize, align alignment)
		:_info(heapSize, alignment)
	{
		void* mem = malloc(heapSize + sizeof(DynamicBlockHeap));
		assert(mem);

		_allocator = new(mem)DynamicBlockHeap(heapSize);
	}

	Heap::Heap(format fmt, size_t blockSize, size_t blockCount)
		:Heap(format::fixed_blocks, blockSize, blockCount, align::def)
	{
	}

	Heap::Heap(format fmt, size_t blockSize, size_t blockCount, align alignment)
		:_info(blockSize * blockCount, alignment)
	{
		size_t heapSize = blockSize * blockCount;

		void* mem = malloc(heapSize + sizeof(FixedBlockHeap));
		assert(mem);

		_allocator = new(mem)FixedBlockHeap(blockSize, blockCount);
	}

	void* Heap::allocate(size_t size)
	{
		void* addr = nullptr;
		addr = _allocator->allocate(size);

		if (addr) {
			
			_info.freeSize -= size;
			_info.usedSize += size;
			_info.currUsedBlocks++;
			if(_info.currUsedBlocks > _info.maxUsedBlocks)
				_info.maxUsedBlocks = _info.currUsedBlocks;
		}
		else {

			//no block of size >= 'size' available 
		}

		return addr;
	}

	void Heap::free(void* addr)
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
}
