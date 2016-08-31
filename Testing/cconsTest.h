//
// Created by amit on 29/06/16.
//

#ifndef CCONSTESTING_CCONSTEST_H
#define CCONSTESTING_CCONSTEST_H
#include <future>

using namespace std;
void queueSetUp();
void MultipleReadOneWriteLockSetUp();
template<class T>
std::future<T> createFuture(const std::function<T(void)> &func)
{
	return (future<T>)(async(launch::async, func));
}
template<class T>
std::future<T> *createFutureArr(const std::function<T(int)> &func, int size)
{
	std::future<T> *arr = new std::future<T>[size];
	for (int i = 0; i < size; ++i)
	{
		arr[i] = async(launch::async, func, i);
	}
	return arr;
}
#endif //CCONSTESTING_CCONSTEST_H
