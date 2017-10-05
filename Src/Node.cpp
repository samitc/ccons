#ifndef _NODE_CPP_
#define _NODE_CPP_
#include "Node.h"

namespace Ccons
{
	template<typename T>
	Node<T>::Node() : Node(nullptr, nullptr)
	{
	}
	template<typename T>
	Node<T>::Node(T *v) : Node(v, nullptr)
	{
	}
	template<typename T>
	Node<T>::Node(T *v, Node *n) : val(v), next(n)
	{
	}
	template<typename T>
	Node<T> *Node<T>::getNext() const
	{
		return next;
	}
	template<typename T>
	T *Node<T>::getVal() const
	{
		return val;
	}
	template<typename T>
	void Node<T>::setNext(Node *n)
	{
		next = n;
	}
	template<typename T>
	void Node<T>::setVal(T *v)
	{
		val = v;
	}
}
#endif
