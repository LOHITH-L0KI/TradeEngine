#include "pch.h"
#include "TrackTable.h"

namespace Mem {
	
	TrackTable::TrackTable()
		:_table()
	{
	}

	void TrackTable::insert(void* object, char* fileName, uint32_t lineNum)
	{
		Record rec{ *fileName, lineNum };
		_table.insert({ object, rec });
	}

	void TrackTable::remove(void* object)
	{
		_table.erase(object);
	}
}
