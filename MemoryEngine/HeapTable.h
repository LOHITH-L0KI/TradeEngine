#ifndef HEAP_TABLE_H
#define HEAP_TABLE_H

namespace Mem {

	constexpr size_t TABLE_SIZE = 15;
	
	class HeapTable
	{
	public:
		static constexpr size_t npos = 0;

		static size_t Add(void* addr) {

			size_t index = 0;
			while (index < TABLE_SIZE && table[index] != 0) {
				index++;
			}

			if (index < TABLE_SIZE) {
				table[index] = size_t(addr);
				return index + 1;
			}

			return npos;
		}

		static void Update(size_t index, void* addr) {

			if (index < TABLE_SIZE) {
				table[index] = size_t(addr);
			}
		}

		static size_t GetAddress(size_t index) {

			size_t addr = npos;

			if (index > 0 && index <= TABLE_SIZE) {
				addr = table[index - 1];
			}

			return addr;
		}

	private:
		static size_t table[TABLE_SIZE];
	};
}


#endif // !HEAP_TABLE_H