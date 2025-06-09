#ifndef HEAP_H
#define HEAP_H
#include <cstdint>

namespace Mem {

class HeapAllocator;


	/// <summary>
	/// Heap memory used for fast allocation and reduce averhead on OS to search for heap memory at run time
	/// Points to list of used and free blocks of memory.
	/// </summary>
	class Heap
	{
	public:
		enum align : uint32_t {
			byte1 = 1,
			byte4 = 4,
			byte8 = 8,
			byte16 = 16,
			byte32 = 32,
			byte64 = 64,
			byte128 = 128,
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

			Info()
				:totalSize(0),
				usedSize(0),
				freeSize(0),
				currUsedBlocks(0),
				maxUsedBlocks(0),
				heapAlign(align::def)
			{}

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

		~Heap();

		//Should provide heap size and heap type
		//Dynamic block heap
		Heap(format fmt, size_t heapSize);
		Heap(format fmt, size_t heapSize, align alignment);

		//Fixed block heap
		Heap(format fmt, size_t blockSize, size_t blockCount);
		Heap(format fmt, size_t blockSize, size_t blockCount, align alignment);

	public:

		void* allocate(size_t size);
		void release(void* addr);
		size_t size();
		align alignement();
		size_t end();
		void status(Info& status);

		void printHeapFragmentation();

	private:
		Info _info;
		HeapAllocator* _allocator;
	};

}

#endif HEAP_H