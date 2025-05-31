#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

namespace Mem {

	class Block
	{
	public:
		enum Type {
			USED,
			FREE
		};

	public:
		Block();
		Block(size_t size, Type type);

		Block* GetNext();
		void SetNext(Block* blk);

		Block* GetPrev();
		void SetPrev(Block* blk);

		Block* GetGlobalPrev();
		void SetGlobalPrev(Block* blk);

		Block* GetGlobalNext();

		size_t GetSize() const;
		void SetSize(size_t size);

		Type GetType() const;
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