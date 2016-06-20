#ifndef _QUEUE_CPP_
#define _QUEUE_CPP_
#include "Queue.h"
namespace Sys
{
	namespace Logging
	{
		namespace concurrency
		{
			template<typename T>
			inline Queue<T>::Queue() :m(), head(new Node(nullptr, nullptr)), tail(head), dataAvi()
			{
			}
			template<typename T>
			Queue<T>::Queue(Queue && queue) : m(), dataAvi()
			{
				std::unique_lock<std::mutex> u(queue.m);
				head = queue.head;
				tail = queue.tail;
				queue.head = nullptr;
				queue.tail = nullptr;
			}
			template<typename T>
			Queue<T>::~Queue()
			{
				while (!isEmpty())
				{
					dequeue();
				}
				delete head;
			}
			template<typename T>
			void Queue<T>::enqueue(T * val)
			{
				Node* add = new Node(val);
				{
					std::unique_lock<std::mutex> u(m);
					tail->setNext(add);
					tail = add;
				}
				dataAvi.notify_one();
			}
			template<typename T>
			T * Queue<T>::dequeue()
			{
				Node* n;
				{
					std::unique_lock<std::mutex> u(m);
					n = head->getNext();
					if (n == nullptr)
					{
						return nullptr;
					}
					resetHead(n);
				}
				T* ret = n->getVal();
				delete n;
				return ret;
			}
			template<typename T>
			T * Queue<T>::dequeueW()
			{
				Node* n;
				{
					std::unique_lock<std::mutex> u(m);
					Node** th = &head;
					n = head->getNext();
					if (n == nullptr)
					{
						dataAvi.wait(u, [th, &n]()
						{
							n = (*th)->getNext();
							return n != nullptr;
						});
					}
					resetHead(n);
				}
				T* ret = n->getVal();
				delete n;
				return ret;
			}
			template<typename T>
			bool Queue<T>::isEmpty() const
			{
				std::unique_lock<std::mutex> u(m);
				return head->getNext() == nullptr;
			}
			template<typename T>
			T * Queue<T>::top() const
			{
				std::unique_lock<std::mutex> u(m);
				Node* n = head->getNext();
				if (n == nullptr)
				{
					return nullptr;
				}
				return n->getVal();
			}
			template<typename T>
			T * Queue<T>::topW() const
			{
				std::unique_lock<std::mutex> u(m);
				Node* n = head->getNext();
				Node** th=&head;
				if (n == nullptr)
				{
					dataAvi.wait(u, [th, n]()
					{
						n = (*th)->getNext();
						return n != nullptr;
					});
					return n->getVal();
				}
				return n->getVal();
			}
			template<typename T>
			void Queue<T>::revalidTail()
			{
				if (head->getNext() == nullptr)
				{
					tail = head;
				}
			}
			template<typename T>
			void Queue<T>::resetHead(const Node *n)
			{
				head->setNext(n->getNext());
				revalidTail();
			}
			template<typename T>
			Queue<T>::Node::Node() :Node(nullptr, nullptr)
			{
			}
			template<typename T>
			Queue<T>::Node::Node(T * v) : Node(v, nullptr)
			{
			}
			template<typename T>
			Queue<T>::Node::Node(T * v, Node * n) : val(v), next(n)
			{
			}
			template<typename T>
			typename Queue<T>::Node * Queue<T>::Node::getNext() const
			{
				return next;
			}
			template<typename T>
			T * Queue<T>::Node::getVal() const
			{
				return val;
			}
			template<typename T>
			void Queue<T>::Node::setNext(Node * n)
			{
				next = n;
			}
			template<typename T>
			void Queue<T>::Node::setVal(T * v)
			{
				val = v;
			}
		}
	}
}
#endif