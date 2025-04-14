#ifndef ORDER_UTIL_H
#define ORDER_UTIL_H

namespace Trader{

	typedef size_t ID;

	struct OrderUpdateData {
		double prize;
		size_t count;
	};

#define SET_BUY(hint) hint |= 0x1
#define IS_BUY(hint) !(hint & 0x1)

#define SET_SELL(hint) hint &= 0xFFFFFFFE
#define IS_SELL(hint) hint & 0x1

#define SET_DELETED(hint) hint |= 0x2
#define IS_DELETED(hint) hint & 0x2

}

#endif // !ORDER_UTIL_H

