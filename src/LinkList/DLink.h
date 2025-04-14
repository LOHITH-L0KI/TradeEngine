#ifndef DLINK_H
#define DLINK_H

namespace Trader {

	//TODO:: Add template to the class which says the derived node type.
	class DLink
	{
	
	public:
		//Insert operation
		enum InsertAt {
			Before,
			After
		};

	public:
		DLink();
		DLink(DLink* _next, DLink* _prev);

		virtual ~DLink();

		//Swaps given two nodes.
		static void Swap(DLink* node1, DLink* node2);

		//Insert next to given node
		void Insert(DLink::InsertAt at, DLink& node);

	private:
		void privInsertBefore(DLink& before);
		void privInsertAfter(DLink& after);

	protected:
		DLink* getNext();
		DLink* getPrev();

	public:
		DLink* next;
		DLink* prev;
	};

}
#endif // !DLINK_H
