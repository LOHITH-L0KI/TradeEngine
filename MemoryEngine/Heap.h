#ifndef HEAP_H
#define HEAP_H
#include <cstdint>

namespace Mem {

	class HeapAllocator;


	/*
	*			|---------------|
	*	Heap -->| HeapAllocator |
	*			|---------------|
	*			|	  Free		|
	*			|---------------|
	*			|				|
	*			|	  size		|
	*			|	   of		|
	*			|	  heap		|
	*			|				|
	*			|---------------|
	*			|   HeapGaurd	|
	*			|---------------|
	*/

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

		enum class format : uint8_t {
			dynamic_blocks,
			fixed_blocks,
			undefined
		};

		enum class allocPolocy : uint8_t {
			NextFit,
			BestFit,
			FirstFit,
			QuickFit,
			Default = NextFit
		};

	public:

		struct Info {

			size_t totalSize;
			size_t usedSize;
			size_t freeSize;
			size_t currFreeBlocks;
			size_t currUsedBlocks;
			size_t maxUsedBlocks;
			align  heapAlign;
			format heapFormat;

			Info()
				:totalSize(0),
				usedSize(0),
				freeSize(0),
				currFreeBlocks(0),
				currUsedBlocks(0),
				maxUsedBlocks(0),
				heapAlign(align::def),
				heapFormat(format::undefined)
			{
			}

			Info(size_t heapSize, align align, format format)
				:totalSize(heapSize),
				usedSize(0),
				freeSize(totalSize),
				currFreeBlocks(1),
				currUsedBlocks(0),
				maxUsedBlocks(0),
				heapAlign(align),
				heapFormat(format)
			{
			}
		};


		struct DYNAMIC_HEAP_DESC {


			size_t heapSize;
			align alignment;
			allocPolocy polocy;
		};

		struct FIXED_HEAP_DESC {

			size_t numberOfBlocks;
			size_t blockSize;
			align alignment;
		};


	public:
		Heap() = delete;
		Heap(const Heap&) = delete;
		Heap& operator =(const Heap&) = delete;

		~Heap();

		//Should provide heap size and heap type
		//Dynamic block heap
		Heap(DYNAMIC_HEAP_DESC& dynamicBuilder);

		//Fixed block heap
		Heap(FIXED_HEAP_DESC& fixedBuilder);

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