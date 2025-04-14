#ifndef ORDER_H
#define ORDER_H
#include <chrono>
#include "ExchangeMode.h"
#include "DLink.h"

namespace ch = std::chrono;

namespace Trader {

	/// <summary>
	/// Contains data of current order. 
	/// </summary>

	/*
	*	Make sure there is always only one instance of order in entire application
	*	by disablting assignment and copy operators.
	*/

	class Order : public DLink
	{

	public:
		Order();
		Order(size_t _id, ExchangeMode _mode, double _prize, size_t _count);
		const Order operator = (const Order& _cpy) = delete;

		void deleteOrder();
		bool isValid();

	public:
		size_t id;
		double prize;
		size_t count;
		ch::time_point<ch::system_clock> tp;
		ExchangeMode mode;
		bool isDeleted;
	};

}

#endif // !ORDER_H
