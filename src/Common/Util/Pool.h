#ifndef POOL_H
#define POOL_H

namespace Common {

#if defined(_WIN64) || defined(__x86_64__)
#define _BITSET_UNIT UINT64_MAX
#else
#define _BITSET_UNIT UINT32_MAX
#endif

	template<typename T, const std::size_t _count, const std::size_t _bitCount = sizeof(size_t)*8>
	class Pool {
	public:
		Pool() {
			std::memset(&(this->_freeIdx), _BITSET_UNIT, this->_freeCount * sizeof(size_t));

			std::cout << "Array Addr:: " << &this->_memBlock << "\n";
		}
		
		template<typename... A>
		T* allocate(A&&... args) {

			int freeIdx = getFreeIndex();

			if (freeIdx == -1)
				return nullptr;

			return new(&(this->_memBlock[0]) + freeIdx) T((std::forward<A>(args))...);
		}

		void dealloc(T* t) {

			size_t idx = (t - &this->_memBlock[0]);
			size_t uiIdx = idx/(sizeof(size_t) * 8);

			size_t toggleBit = idx % (sizeof(size_t) * 8);

			this->_freeIdx[uiIdx] = this->_freeIdx[uiIdx] | ((size_t)1 << (sizeof(size_t) * 8 - 1 - toggleBit));
		}

	private:

		int getFreeIndex() noexcept {

			int idx = -1;
			size_t uiIdx = 0;

			while ((uiIdx < this->_freeCount) && !this->_freeIdx[uiIdx]) {
				++uiIdx;
			}

			if (uiIdx < this->_freeCount)
				idx = (sizeof(size_t) * uiIdx) + toggleHBit(uiIdx);

			return idx;
		}

		size_t toggleHBit(size_t uiIdx) {
			
			size_t pos = 0;
			size_t hBitVal = (size_t)1 << (sizeof(size_t) * 8 - 1);

			while (!(this->_freeIdx[uiIdx] & hBitVal)) {
				hBitVal = hBitVal >> 1;
				++pos;
			}
			this->_freeIdx[uiIdx] = (this->_freeIdx[uiIdx] & (~hBitVal));

			return pos;
		}

	private:
		T _memBlock[_count];
		size_t _freeIdx[(_count / _bitCount) + ((_count % _bitCount) > 0 ? 1 : 0)];
		const size_t _freeCount = (_count / _bitCount) + ((_count % _bitCount) > 0 ? 1 : 0);
	};
}

#endif // !POOL_H

