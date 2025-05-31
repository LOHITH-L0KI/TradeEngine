#include "pch.h"
#include "Used.h"

namespace Mem {
	Used::Used()
		:Used(0)
	{
	}

	Used::Used(size_t size)
		:Block(size, Type::USED)
	{
	}
}
