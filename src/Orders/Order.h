#ifndef ORDER_H
#define ORDER_H
#include <chrono>
#include "ExchangeMode.h"
#include "OrderType.h"
#include "DLink.h"
#include "OrderUtil.h"

using namespace Util;

namespace ch = std::chrono;
namespace Trader {

	/// <summary>
	/// Contains data of current order. 
	/// </summary>

	/*
	*	Make sure there is always only one instance of order in entire application
	*	by disablting assignment and copy operators.
	*/

	class OrderBook;
	class Order : public DLink
	{
	public:
		Order();
		Order(ID _id, OrderType _type, ExchangeMode _mode, double _prize, size_t _count, OrderBook* _book);
		const Order operator = (const Order& _cpy) = delete;

		void deleteOrder();
		bool isValid();
		OrderType getType();

		void update(OrderUpdateData& data);

	private:
		ID id;
		size_t hint;
		OrderBook* book;

	public:
		double prize;
		size_t count;
		ExchangeMode mode;
		ch::time_point<ch::system_clock> tp;
	};

}

#endif // !ORDER_H
