#ifndef _NODE_H_
#define _NODE_H_

namespace Ccons
{
	template<typename T>
	struct Node
	{
    public:
		inline Node();
		inline explicit Node(T *v);
		inline Node(T *v, Node *n);
		inline ~Node() = default;
		inline Node *getNext() const;
		inline T *getVal() const;
		inline void setNext(Node *n);
		inline void setVal(T *v);
	private:
		Node<T> *next;
		T *val;
	};
}
#include "Node.cpp"

#endif
