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
		Queue();
		Queue(Queue &&);
		~Queue();
		void enqueue(T *val);
		T *dequeue();
		T *dequeueW();
		bool isEmpty() const;
		T *top() const;
		T *topW() const;
	private:
		struct Node
		{
			Node();
			Node(T *v);
			Node(T *v, Node *n);
			~Node() = default;
			Node *getNext() const;
			T *getVal() const;
			void setNext(Node *n);
			void setVal(T *v);
		private:
			Node *next;
			T *val;
		};
	private:
		void revalidTail();
		void resetHead(const Node *);
		Node *head;
		Node *tail;
		mutable std::mutex m;
		mutable std::condition_variable dataAvi;
	};
}
#include "Queue.cpp"

#endif