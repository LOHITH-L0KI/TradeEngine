#include <OrderType.h>
#include <vector>
#include <stack>
#include <ExchangeMode.h>
#include <OrderUtil.h>

#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

namespace Trader {

	class Order;
	class OrderBook
	{
	public:

		OrderBook();
		OrderBook(size_t intiSize);

		~OrderBook();

		//add order
		Order* add(OrderType type, double prize, size_t count, ExchangeMode mode);

		//update order
		void update(ID orderId, OrderUpdateData& data);

		//delete order

	private:
		// Get new order Id. Id is the index of order object in memOrders vector.
		size_t privGetNewId();

		//Adds order to Buy list, ensureing max order with price and time.
		void privAddToBuy(Order& order);

		//Adds order to Buy list, ensureing min order with price and time.
		void privAddToSell(Order& order);
	
	//DATA
	private:
		//static default capacity
		static const size_t DEFAULT_ORDER_CAPACITY = 1024 * 512;

		size_t orderId;

		//memSize
		size_t allocSize;

		//sell list -- min order
		Order* minSell;
		
		//buy list -- max order
		Order* maxBuy;

		// Accquire memory to have contenious Order objects in an array.
		// This improves cashing of more order objects, improving performance.
		std::vector<Order> memOrders;

		//Container to store free slots.
		//this takes priority than global ORDER_ID
		std::vector<ID> freedSlots;
	};

}
#endif // !ORDER_BOOK_H
