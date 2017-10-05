#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <mutex>
#include <condition_variable>
#include "Node.h"

namespace Ccons
{
	template<typename T>
	class Queue
	{
	public:
		inline Queue();
		inline Queue(Queue &&);
		inline ~Queue();
		inline void enqueue(T *val);
		inline T *dequeue();
		inline T *dequeueW();
		inline bool isEmpty() const;
		inline T *top() const;
		inline T *topW() const;
	private:
		inline void revalidTail();
		inline void resetHead(const Node<T> *);
		mutable std::mutex m;
		mutable std::condition_variable dataAvi;
		Node<T> *head;
		Node<T> *tail;
#ifdef USE_CACHE_TOREUSE_NODES
		Node<T> *cStart;
#endif // USE_CACHE_TOREUSE_NODES
	};
}
#include "Queue.cpp"

#endif
