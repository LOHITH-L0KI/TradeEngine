#include "Order.h"

namespace Trader {
	
	Order::Order()
		:DLink(),
		id(0),
		prize(0),
		count(0),
		tp(),
		mode(ExchangeMode::Undefined),
		isDeleted(true)
	{
	}

	Order::Order(size_t _id, ExchangeMode _mode, double _prize, size_t _count)
		:DLink(),
		id(_id),
		prize(_prize),
		count(_count),
		tp(ch::system_clock::now()),
		mode(_mode),
		isDeleted(false)
	{

	}

	void Order::deleteOrder()
	{
		this->isDeleted = true;
	}

	bool Order::isValid()
	{
		return !this->isDeleted;
	}

}