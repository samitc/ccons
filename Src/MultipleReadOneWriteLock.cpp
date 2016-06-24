#include "MultipleReadOneWriteLock.h"

namespace Ccons
{
	MultipleReadOneWriteLock::MultipleReadOneWriteLock()
			: m(), writerCond(), readerCond(), numOfReader(0), isWriting(false)
	{
	}
	void MultipleReadOneWriteLock::startRead()
	{
		++numOfReader;
		if (isWriting)
		{
			--numOfReader;
			std::unique_lock<std::mutex> u(m);
			writerCond.notify_one();
			readerCond.wait(u, [=]() {
				return !this->isWriting;
			});
			++numOfReader;
		}
	}
	void MultipleReadOneWriteLock::endRead()
	{
		--numOfReader;
		writerCond.notify_one();
	}
	void MultipleReadOneWriteLock::startWrite()
	{
		isWriting = true;
		if (numOfReader > 0)
		{
			std::unique_lock<std::mutex> u(m);
			writerCond.wait_for(u, std::chrono::duration<int, std::ratio<1, 1>>(1), [=]() {
				return this->numOfReader > 0;
			});
		}
	}
	void MultipleReadOneWriteLock::endWrite()
	{
		isWriting = false;
		readerCond.notify_all();
	}
}