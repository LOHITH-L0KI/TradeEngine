#ifndef POOL_H
#define POOL_H

#include <assert.h>
#include <iostream>

namespace Util {

//define bitset unit to eaither 64 or 32 bit unsigned int
#if defined(_WIN64) || defined(__x86_64__)
	#define _BITSET_UNIT UINT64_MAX
	#define _BIT_COUNT 64
#else
	#define _BITSET_UNIT UINT32_MAX
	#define _BIT_COUNT 32
#endif

#define _FREE_IDX(bitIdx) (bitIdx / _BIT_COUNT) + ((bitIdx % _BIT_COUNT) > 0 ? 1 : 0)
#define _SIZE_T_BY_HBIT(x) (size_t)1 << (x)

	template<typename T, const std::size_t __count>
	class Pool {
	public:
		inline Pool() {
			//set all blocks of memory to free.
			std::cout << "Mem Size Idx Cnt:: " << this->_freeCount;
			std::memset(&(this->_freeIdx), _BITSET_UNIT, this->_freeCount * sizeof(size_t));
		}

		Pool(const Pool&) = delete;
		Pool& operator = (const Pool&) = delete;
		Pool(const Pool&&) = delete;
		Pool& operator = (const Pool&&) = delete;
		
		//Allocates memory 
		template<typename... A>
		inline T* allocate(A... args) noexcept{

			int freeIdx = getFreeIndex();

			if(freeIdx < 0) _UNLIKELY{
				std::cerr << "Memory Full, no free bloack available to allocate memory.\n";
				return nullptr;
			}
			
			//TODO :: check if operator()(...) can be used.
			//return &((this->_memBlock[freeIdx])(args...));
			
			// placement new
			return new(&(this->_memBlock[0]) + freeIdx) T(args...);
		}

		//free memory
		inline void dealloc(T* t) noexcept{

			const size_t idx = (t - &this->_memBlock[0]);

			if (idx < 0 || idx > _count) _UNLIKELY{
				std::cerr << "This object does not belong to this pool.\n";
				return;
			}

			//clear the object
			memset(t, 0x0, sizeof(T));

			const size_t uiIdx = idx / _BIT_COUNT;
			const size_t bitToToggle = idx % _BIT_COUNT;

			this->_freeIdx[uiIdx] = this->_freeIdx[uiIdx] | (_SIZE_T_BY_HBIT(_BIT_COUNT - 1 - bitToToggle));
		}

	private:

		inline int getFreeIndex() noexcept {

			int idx = -1;
			size_t uiIdx = 0;

			while ((uiIdx < this->_freeCount) && (this->_freeIdx[uiIdx] == 0)) {
				++uiIdx;
			}

			if (uiIdx < this->_freeCount)
				idx = (_BIT_COUNT * uiIdx) + toggleHBit(uiIdx);

			return idx;
		}

		inline size_t toggleHBit(size_t uiIdx) noexcept{
			
			size_t pos = 0;
			size_t hBitVal = _SIZE_T_BY_HBIT(_BIT_COUNT - 1);

			while (!(this->_freeIdx[uiIdx] & hBitVal)) {
				hBitVal = hBitVal >> 1;
				++pos;
			}

			this->_freeIdx[uiIdx] = (this->_freeIdx[uiIdx] & (~hBitVal));

			return pos;
		}

	private:
		//Array of objects of type 'T'.
		T _memBlock[__count];
		
		//Bit map to check if element at an index is free.
		size_t _freeIdx[_FREE_IDX(__count)];

		//Bit map count
		const size_t _freeCount = _FREE_IDX(__count);

		//mem block count
		const size_t _count = __count;
	};
}

#endif // !POOL_H

