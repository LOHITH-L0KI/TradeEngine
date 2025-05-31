#ifndef USED_H
#define USED_H

#include "Block.h"

namespace Mem {

	class Used final: public Block
	{
	public:
		Used();
		explicit Used(size_t size);
	};
}

#endif USED_H
