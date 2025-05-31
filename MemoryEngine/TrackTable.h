
#ifndef TRACK_TABLE_H
#define TRACK_TABLE_H

#include <unordered_map>

namespace Mem {

	class TrackTable
	{
		static size_t constexpr FILENAME_BUFF_SIZE = 64;

		struct Record {
			size_t line;
			char fileName[FILENAME_BUFF_SIZE];
		};

	private:
		std::unordered_map<void*, Record> _table;
	};
}

#endif // !TRACK_TABLE_H