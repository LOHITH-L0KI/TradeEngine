#ifndef LFC_QUEUE_H
#define LFC_QUEUE_H
#include <atomic>
#include <vector>
#include <bit>
namespace Util {

	template<typename T, size_t __capacity>
	requires(std::has_single_bit(__capacity)) // Alowes only power of two.
	class LFC_Queue {

		//Queue Node
		struct LFC_Node {

		//CREATERS
		public:
			LFC_Node()
				:_data(),
				_filled(false)
			{ }

			LFC_Node(const LFC_Node& cpy)
				:_data(cpy._data),
				_filled(!cpy.isFree())
			{

			}

		//METHODS
		public:

			/// <summary>
			/// Writes data to data slot.
			/// Waits till the slot is freed by other threads.
			/// </summary>
			/// <param name="data">: data to be stored</param>
			void set(const T& data) {

				_data = data;
				_filled.store(true, std::memory_order_release);
			}

			/// <summary>
			/// Frees the data slot and releases it to other threads
			/// </summary>
			void free() {
				_filled.store(false, std::memory_order_release);
			}

			/// <summary>
			/// Checks if data slot is free.
			/// </summary>
			/// <returns>True if slot is free</returns>
			bool isFree() const {
				return !_filled.load(std::memory_order_acquire);
			}

			/// <summary>
			/// Returns refernce to data.
			/// </summary>
			/// <returns>Refernce to data</returns>
			T& get() {
				return _data;
			}

		//DATA
		public:
			T _data;
			std::atomic_bool _filled;

		};
	
	//CREATORS
	public:
		LFC_Queue(const LFC_Queue&) = delete;
		LFC_Queue& operator= (const LFC_Queue&) = delete;
		
		LFC_Queue()
			:_queue(__capacity, LFC_Node()),
			_read_index(0),
			_write_index(0),
			_size(0),
			_indexMask(__capacity - 1) {

		}

	//METHODS
	public:

		/// <summary>
		/// Removes first element to queue.
		/// </summary>
		/// <param name="data">: Memory location to copy the element from queue</param>
		/// <returns>True if element is successfully removed from queue</returns>
		inline bool pop(T& data) {

			size_t curr_r_idx;

			curr_r_idx = _read_index.load(std::memory_order_acquire);

			//update the index if curr_r_idx is still valid, i.e if other thread have not consumed data from this index.
			if (_queue[curr_r_idx].isFree() || !_read_index.compare_exchange_strong(curr_r_idx, (curr_r_idx + 1) & _indexMask))
				return false;

			data = _queue[curr_r_idx].get();

			//make the data slot available for next write
			_queue[curr_r_idx].free();

			//decrement the counter
			/** std::memory_order_release: Since this should happen only if there is valid data in inner queue **/
			_size.fetch_sub(1, std::memory_order_release);

			return true;
		}

		/// <summary>
		/// Inserts element at the end
		/// </summary>
		/// <param name="data">: data that should be inserted into the queue</param>
		/// <returns>True if data is successfully inserted to queue</returns>
		inline bool push(const T& data) {

			size_t curr_w_idx = _write_index.load(std::memory_order_relaxed);

			//wait till you get a sloat to fill the data or return if the queue is full
			//TODO:: write this data to aux queue or increase the queue size.
			do {

				if (!_queue[curr_w_idx].isFree())
					return false;

			} while (!_write_index.compare_exchange_strong(curr_w_idx, (curr_w_idx + 1) & _indexMask, std::memory_order_release));

			//set data into queue node.
			//node waits till the data block is set to free by reading node.
			_queue[curr_w_idx].set(data);

			//increase the counter.
			/** std::memory_order_release: Since this should happen only after perfect push into inner queue **/
			_size.fetch_add(1, std::memory_order_release);

			return true;
		}
				
		/// <summary>
		/// Returns the size of queue.
		/// </summary>
		/// <returns>element count</returns>
		inline size_t const size() { return _size.load(); }
	
	//DATA
	private:

		//ATOMICS
		std::atomic_size_t _read_index;
		std::atomic_size_t _write_index;
		std::atomic_size_t _size;

		//MASK
		const std::size_t _indexMask;
		
		//DATA STORAGE
		std::vector<LFC_Node> _queue;
	};
}

#endif // !LFC_QUEUE_H