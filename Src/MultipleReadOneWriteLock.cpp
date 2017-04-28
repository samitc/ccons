#include "MultipleReadOneWriteLock.h"

namespace Ccons
{
	MultipleReadOneWriteLock::MultipleReadOneWriteLock()
		: m(), writerCond(), readerCond(), numOfReader(0), isWriting(false), numOfWriter(0)
	{
	}
	void MultipleReadOneWriteLock::startRead()
	{
		++numOfReader;
		if (isWriting)
		{
			endRead();
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
		if (numOfReader.fetch_sub(1, std::memory_order_acq_rel) == 1)
		{
			writerCond.notify_one();
		}
	}
	void MultipleReadOneWriteLock::startWrite()
	{
		isWriting = true;
		if (numOfWriter.fetch_add(1, std::memory_order_acq_rel) > 0 || numOfReader > 0 || this->isWriterCanRun.test_and_set(std::memory_order_acq_rel))
		{
			std::unique_lock<std::mutex> u(m);
			writerCond.wait(u, [=]() {
				return this->numOfReader == 0 && !this->isWriterCanRun.test_and_set(std::memory_order_acq_rel);
			});
		}
	}
	void MultipleReadOneWriteLock::endWrite()
	{
		if (numOfWriter.fetch_sub(1, std::memory_order_acq_rel) > 1)
		{
			isWriterCanRun.clear();
			writerCond.notify_one();
		}
		else
		{
			isWriterCanRun.clear();
			isWriting = false;
			readerCond.notify_all();
		}
	}
}