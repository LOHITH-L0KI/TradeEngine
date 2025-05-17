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

	class Index {

	public:
		static constexpr size_t npos = _BITSET_UNIT;
	
	public:
		Index(size_t size)
			:_size(size)
		{
			size_t charCount = (_size / 8) + 1;

			_bits = (uint_fast8_t*)malloc(charCount);

			if(_bits != NULL && _size > 0)
				memset(_bits, 0xFF, charCount);
		}

		size_t GetFree() {

			//read byte by byte and check if it is less than 0xFF
			uint_fast8_t* check = _bits;
			int max = _size / 8;
			int cur = 0;

			while (cur < max && !(*check)) {
				++check;
				++cur;
			}

			if (cur != max)
			{
				//get high bit
				size_t bitPos = 0;
				char charVal = *check;
				while (!(charVal & 0x80)) {
					charVal = charVal << 1;
					++bitPos;
				}

				//make this bit as filled(0)
				*check &= ~(1 << (7 - bitPos));

				return (cur * 8) + bitPos;
			}

			return npos;
		}

		void MakeFree(size_t index) {

			size_t charPos = index / 8;
			size_t bitPos = index % 8;

			uint_fast8_t* charVal = _bits + charPos;

			//make this bit as free(1)
			*charVal |= (1 << (7 - bitPos));
		}

	private:
		const size_t _size;
		uint_fast8_t* _bits;
	};

	template<typename T, const std::size_t __count>
	class Pool {
	public:
		inline Pool() 
			:_idx(_count)
		{ }

		Pool(const Pool&) = delete;
		Pool& operator = (const Pool&) = delete;
		Pool(const Pool&&) = delete;
		Pool& operator = (const Pool&&) = delete;
		
		//Allocates memory 
		template<typename... A>
		inline T* allocate(A... args) noexcept{

			size_t freeIdx = _idx.GetFree();
			if (freeIdx == Index::npos) _UNLIKELY{
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
			_idx.MakeFree(idx);
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
		
		//mem block count
		const size_t _count = __count;

		//Index
		Index _idx;
	};
	
	
}

#endif // !POOL_H

