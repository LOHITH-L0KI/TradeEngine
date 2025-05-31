#ifndef HEAP_TABLE_H
#define HEAP_TABLE_H
#include <cstdint>

namespace Mem {

	class Heap;
	constexpr size_t TABLE_SIZE = 15;
	
	class HeapTable
	{
	public:
		static constexpr size_t npos = 0;

		static size_t Add(Heap* addr) {

			size_t index = 0;
			while (index < TABLE_SIZE && table[index] != 0) {
				index++;
			}

			if (index < TABLE_SIZE) {
				table[index] = size_t(addr);
				return table[index];
			}

			return npos;
		}

		static size_t GetAddress(size_t index) {

			size_t addr = npos;

			if (index > 0 && index <= TABLE_SIZE) {
				addr = table[index];
			}

			return addr;
		}

	private:
		static size_t table[TABLE_SIZE];
	};
}


#endif // !HEAP_TABLE_H