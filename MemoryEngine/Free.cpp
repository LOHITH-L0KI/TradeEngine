#include "pch.h"
#include "Free.h"

namespace Mem {

	Free::Free(size_t size)
		:Block(size, Type::FREE)
	{
	}

}
