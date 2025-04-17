#include "OrderBook.h"
#include "Order.h"
#include <cassert>

namespace Trader {

    OrderBook::OrderBook()
        :orderId(0),
        allocSize(OrderBook::DEFAULT_ORDER_CAPACITY),
        minSell(nullptr),
        maxBuy(nullptr)
    {
        //allocate contenious memory to orders
        memOrders.resize(allocSize);

        //clear freeSlot data
        freedSlots.reserve(allocSize);
        freedSlots.clear();
    }

    OrderBook::OrderBook(size_t initSize)
        :orderId(0),
        allocSize(initSize),
        minSell(nullptr),
        maxBuy(nullptr)

    {
        if (!initSize)
            allocSize = OrderBook::DEFAULT_ORDER_CAPACITY;
        
        //allocate contenious memory to orders
        memOrders.resize(allocSize);

        //clear freeSlot data
        freedSlots.reserve(allocSize);
        freedSlots.clear();

    }

    OrderBook::~OrderBook()
    {
        //delete any heap allocated memory.
    }

    Order* OrderBook::add(OrderType type, double prize, size_t count, ExchangeMode mode)
    {
        //get available slot
        size_t slotId = privGetNewId();

        //placement new
        Order* order = new((&memOrders[0]) + slotId) Order(slotId, type, mode, prize, count, this);

        if (type == OrderType::Buy)
            privAddToBuy(*order);
        else
            privAddToSell(*order);
        
        return order;
    }

    void OrderBook::update(ID orderId, OrderUpdateData& data)
    {
        Order* order = &memOrders[orderId];
        order->count = data.count;

        //perform operatoins according to the order type.
        if (order->getType() == OrderType::Buy) {


        }
        else {

           
        }
    }

    size_t OrderBook::privGetNewId()
    {
        size_t slotId = 0;
        
        //freedSlot List takes priority
        if (freedSlots.empty()) {
            if (++orderId == memOrders.size())
                memOrders.push_back(Order());

            slotId = orderId;
        }
        else {

            slotId = freedSlots.back();
            freedSlots.pop_back();
        }

        return slotId;
    }

    void OrderBook::privAddToBuy(Order& order)
    {
        //add order to buy list in max order
        if (!maxBuy)
            maxBuy = &order;
        else _LIKELY {
            Order* curr = maxBuy;
            Order* prev = nullptr;

            while (curr && curr->prize >= order.prize) {
                prev = curr;
                curr = (Order*)curr->next;
            }

            //insert after prevnode or before currnode
            if (prev)
                order.Insert(DLink::InsertAt::After, *prev);
            else {
                order.Insert(DLink::InsertAt::Before, *curr);
                maxBuy = &order;
            }
                
        }
    }

    void OrderBook::privAddToSell(Order& order)
    {
        //add order to buy list in max order
        if (!minSell)
            minSell = &order;
        else {

            Order* curr = minSell;
            Order* prev = nullptr;

            while (curr && curr->prize <= order.prize) {
                prev = curr;
                curr = (Order*)curr->next;
            }

            //insert after prevnode or before currnode
            if (prev)
                order.Insert(DLink::InsertAt::After, *prev);
            else {
                order.Insert(DLink::InsertAt::Before, *curr);
                minSell = &order;
            }

        }
    }
}



