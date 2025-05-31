#ifndef HEAP_H
#define HEAP_H

namespace Mem {

class HeapAllocator;


	/// <summary>
	/// Heap memory used for fast allocation and reduce averhead on OS to search for heap memory at run time
	/// Points to list of used and free blocks of memory.
	/// </summary>
	class Heap
	{
		enum align : uint8_t {
			byte4,
			byte8,
			byte16,
			byte32,
			byte64,
			byte128,
			def = byte4
		};

		enum format : uint8_t {
			dynamic_blocks,
			fixed_blocks
		};

	public:

		struct Info {

			size_t totalSize;
			size_t usedSize;
			size_t freeSize;
			size_t currUsedBlocks;
			size_t maxUsedBlocks;
			align  heapAlign;

			Info() = delete;
			Info(size_t heapSize, align align)
				:totalSize(heapSize),
				usedSize(0),
				freeSize(totalSize),
				currUsedBlocks(0),
				maxUsedBlocks(0),
				heapAlign(align)
			{}
		};


	public:
		Heap() = delete;
		Heap(const Heap&) = delete;
		Heap& operator =(const Heap&) = delete;

		//Should provide heap size and heap type
		//Dynamic block heap
		Heap(format fmt, size_t heapSize);
		Heap(format fmt, size_t heapSize, align alignment);

		//Fixed block heap
		Heap(format fmt, size_t blockSize, size_t blockCount);
		Heap(format fmt, size_t blockSize, size_t blockCount, align alignment);

	public:

		void* allocate(size_t size);
		void free(void* addr);
		size_t size();
		size_t end();
		void status(Info& status);

	private:
		Info _info;
		HeapAllocator* _allocator;
	};

}

#endif HEAP_H