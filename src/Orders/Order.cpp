#include "Order.h"
#include "OrderBook.h"

namespace Trader {
	
	Order::Order()
		:DLink(),
		id(0),
		prize(0),
		count(0),
		mode(ExchangeMode::Undefined),
		book(nullptr),
		tp(),
		hint(0)
	{
		//default set as deleted
		SET_DELETED(this->hint);
	}

	Order::Order(ID _id, OrderType _type, ExchangeMode _mode, double _prize, size_t _count, OrderBook* _book)
		:DLink(),
		id(_id),
		prize(_prize),
		count(_count),
		mode(_mode),
		book(_book),
		tp(ch::system_clock::now()),
		hint(0)
	{
		//set order type
		if (_type == OrderType::Buy)
			SET_SELL(hint);
		else
			SET_BUY(hint);
	}

	void Order::deleteOrder()
	{
		SET_DELETED(this->hint);
	}

	bool Order::isValid()
	{
		return !IS_DELETED(this->hint);
	}

	OrderType Order::getType()
	{
		return IS_BUY(this->hint) ? OrderType::Buy : OrderType::Sell;
	}

	void Order::update(OrderUpdateData& data)
	{
		this->book->update(this->id, data);
	}

}