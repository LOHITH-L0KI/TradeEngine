#include "DLink.h"

namespace Trader {

	DLink::DLink()
		:next(nullptr),
		prev(nullptr)
	{
	}

	DLink::DLink(DLink* _next, DLink* _prev)
		:next(_next),
		prev(_prev)
	{
	}

	DLink::~DLink()
	{
		//deligate to derived class
	}

	void DLink::Swap(DLink* node1, DLink* node2)
	{
		if (node1 && node2) {

			DLink* n1Prev = node1->prev;
			DLink* n1Next = node1->next;

			DLink* n2Prev = node2->prev;
			DLink* n2Next = node2->next;

			//change pointers of node1
			node1->prev = n2Prev;
			node1->next = n2Next;

			//change pointers of node2
			node2->prev = n1Prev;
			node2->next = n1Next;

			//change pointers of node1's Previous Node
			if (n1Prev)
				n1Prev->next = node2;

			//change pointers of node1's Next Node
			if (n1Next)
				n1Next->prev = node2;

			//change pointers of node2's Previous Node
			if (n2Prev)
				n2Prev->next = node1;

			//change pointers of node2's Next Node
			if (n2Next)
				n2Next->prev = node1;

			
		}
	}

	void DLink::Insert(DLink::InsertAt at, DLink& node)
	{
		if (at == DLink::InsertAt::After)
			privInsertAfter(node);
		else
			privInsertBefore(node);

	}

	void DLink::privInsertBefore(DLink& before)
	{
		this->next = &before;
		this->prev = before.prev;

		if (before.prev)
			before.prev->next = this;

		before.prev = this;

	}

	void DLink::privInsertAfter(DLink& after)
	{
		this->next = after.next;
		this->prev = &after;

		if (after.next)
			after.next->prev = this;

		after.next = this;
	}

	DLink* DLink::getNext()
	{
		return this->next;
	}

	DLink* DLink::getPrev()
	{
		return this->prev;
	}

}