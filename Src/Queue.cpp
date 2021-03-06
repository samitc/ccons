#ifndef _QUEUE_CPP_
#define _QUEUE_CPP_
#include "Queue.h"

namespace Ccons
{
	template<typename T>
	inline Queue<T>::Queue() : m(), dataAvi(), head(new Node<T>(nullptr, nullptr)), tail(head)
#ifdef USE_CACHE_TOREUSE_NODES
		, cStart(nullptr)
#endif // USE_CACHE_TOREUSE_NODES
	{
	}
	template<typename T>
	Queue<T>::Queue(Queue &&queue) : m(), dataAvi(), head(queue.head), tail(queue.tail)
#ifdef USE_CACHE_TOREUSE_NODES
		, cStart(nullptr)
#endif // USE_CACHE_TOREUSE_NODES
	{
		std::unique_lock<std::mutex> u(queue.m);
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
	void Queue<T>::enqueue(T *val)
	{
#ifdef USE_CACHE_TOREUSE_NODES
		Node<T> *add;
#else
		Node<T> *add = new Node<T>(val);
#endif // USE_CACHE_TOREUSE_NODES
		{
			std::unique_lock<std::mutex> u(m);
#ifdef USE_CACHE_TOREUSE_NODES
			if (cStart == nullptr)
			{
				add = new Node<T>(val);
			}
			else
			{
				add = cStart;
				cStart = cStart->getNext();
				add->setNext(nullptr);
				add->setVal(val);
			}
#endif // USE_CACHE_TOREUSE_NODES
				tail->setNext(add);
				tail = add;
		}
		dataAvi.notify_one();
	}
	template<typename T>
	T *Queue<T>::dequeue()
	{
		Node<T> *n;
		T *ret;
		{
			std::unique_lock<std::mutex> u(m);
			n = head->getNext();
			if (n == nullptr)
			{
				return nullptr;
			}
			resetHead(n);
#ifdef USE_CACHE_TOREUSE_NODES
			ret = n->getVal();
			n->setNext(cStart);
			cStart = n;
#endif // USE_CACHE_TOREUSE_NODES
		}
#ifndef USE_CACHE_TOREUSE_NODES
		ret = n->getVal();
		delete n;
#endif // !USE_CACHE_TOREUSE_NODES
		return ret;
	}
	template<typename T>
	T *Queue<T>::dequeueW()
	{
		Node<T> *n;
		T *ret;
		{
			std::unique_lock<std::mutex> u(m);
			Node<T> **th = &head;
			n = head->getNext();
			if (n == nullptr)
			{
				dataAvi.wait(u, [th, &n]() {
					n = (*th)->getNext();
					return n != nullptr;
				});
			}
			resetHead(n);
#ifdef USE_CACHE_TOREUSE_NODES
			ret = n->getVal();
			n->setNext(cStart);
			cStart = n;
#endif // USE_CACHE_TOREUSE_NODES
		}
#ifndef USE_CACHE_TOREUSE_NODES
		ret = n->getVal();
		delete n;
#endif // !USE_CACHE_TOREUSE_NODES
		return ret;
	}
	template<typename T>
	bool Queue<T>::isEmpty() const
	{
		std::unique_lock<std::mutex> u(m);
		return head->getNext() == nullptr;
	}
	template<typename T>
	T *Queue<T>::top() const
	{
		std::unique_lock<std::mutex> u(m);
		Node<T> *n = head->getNext();
		if (n == nullptr)
		{
			return nullptr;
		}
		return n->getVal();
	}
	template<typename T>
	T *Queue<T>::topW() const
	{
		std::unique_lock<std::mutex> u(m);
		Node<T> *n = head->getNext();
		Node<T> *const *th = &head;
		if (n == nullptr)
		{
			dataAvi.wait(u, [th, &n]() {
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
	void Queue<T>::resetHead(const Node<T> *n)
	{
		head->setNext(n->getNext());
		revalidTail();
	}
}
#endif
