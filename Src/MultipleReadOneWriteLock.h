#ifndef _MULTIPLEREADONEWRITELOCK_H_
#define _MULTIPLEREADONEWRITELOCK_H_
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace Ccons
{
	class MultipleReadOneWriteLock
	{
	public:
		MultipleReadOneWriteLock();
		~MultipleReadOneWriteLock() = default;
		void startRead();
		void endRead();
		void startWrite();
		void endWrite();
	private:
		std::mutex m;
		std::condition_variable writerCond;
		std::condition_variable readerCond;
		std::atomic<unsigned int> numOfReader;
		std::atomic<bool> isWriting;
		std::atomic<unsigned int> numOfWriter;
		std::atomic_flag isWriterCanRun = ATOMIC_FLAG_INIT;
	};
}
#endif