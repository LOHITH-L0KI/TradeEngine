#ifndef FREE_H
#define FREE_H

#include "Block.h"

namespace Mem {

	class Free final: public Block
	{
	
	public:
		Free() = delete;
		explicit Free(size_t size);
	};
}


#endif // !FREE_H