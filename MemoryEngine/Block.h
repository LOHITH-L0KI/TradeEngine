#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

namespace Mem {

	class Block
	{
	public:
		enum Type {
			FREE,
			USED
		};

	public:
		Block();
		Block(size_t size, Type type);

		Block* GetNext() const;
		void SetNext(Block* blk);

		Block* GetPrev() const;
		void SetPrev(Block* blk);

		Block* GetGlobalPrev() const;
		void SetGlobalPrev(Block* blk);

		Block* GetGlobalNext() const;

		void SetHeapIndex(size_t idx);
		size_t GetHeapIndex();

		size_t GetSize() const;
		void SetSize(size_t size);

		Type GetType() const;
		bool IsFree() const;
		bool IsUsed() const;
		void SetToUsed();
		void SetToFree();

		bool IsAboveFree() const;
		void SetAboveFree();
		void SetAboveUsed();

	protected:
		uint64_t _magic;
		uint64_t _nav;
	};

}

#endif // !BLOCK_H