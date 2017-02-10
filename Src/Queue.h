#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <mutex>
#include <condition_variable>

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
		struct Node
		{
			inline Node();
			inline explicit Node(T *v);
			inline Node(T *v, Node *n);
			inline ~Node() = default;
			inline Node *getNext() const;
			inline T *getVal() const;
			inline void setNext(Node *n);
			inline void setVal(T *v);
		private:
			Node *next;
			T *val;
		};
	private:
		inline void revalidTail();
		inline void resetHead(const Node *);
		mutable std::mutex m;
		mutable std::condition_variable dataAvi;
		Node *head;
		Node *tail;
#ifdef USE_CACHE_TOREUSE_NODES
		Node *cStart;
#endif // USE_CACHE_TOREUSE_NODES
	};
}
#include "Queue.cpp"

#endif