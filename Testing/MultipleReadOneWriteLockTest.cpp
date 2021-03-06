//
// Created by amit on 29/06/16.
//
#include <chrono>
#include <future>
#include "../Src/MultipleReadOneWriteLock.h"
#include "cconsTest.h"
#include "gtest/gtest.h"

using namespace Ccons;
using namespace std;
std::atomic_flag isRandFin = ATOMIC_FLAG_INIT;
volatile unsigned int preRand;
void MultipleReadOneWriteLockSetUp()
{
	srand((unsigned int)time(nullptr));
	preRand = rand();
}
int mRand()
{
	while (isRandFin.test_and_set());
	srand(preRand);
	preRand = rand();
	isRandFin.clear();
	return preRand;
}
using Time=std::chrono::high_resolution_clock;
constexpr double MAX_TIME_DIFF = 0.001;
TEST(MultipleReadOneWriteLockTest, GeneralTest)
{
	MultipleReadOneWriteLock lock;
	auto startTime = Time::now();
	lock.startRead();
	auto endTime = Time::now();
	double time = std::chrono::duration<double, ratio<1, 1>>(endTime - startTime).count();
	ASSERT_NEAR(0, time, MAX_TIME_DIFF);
	startTime = Time::now();
	lock.endRead();
	endTime = Time::now();
	time = std::chrono::duration<double, ratio<1, 1>>(endTime - startTime).count();
	ASSERT_NEAR(0, time, MAX_TIME_DIFF);
	startTime = Time::now();
	lock.startWrite();
	endTime = Time::now();
	time = std::chrono::duration<double, ratio<1, 1>>(endTime - startTime).count();
	ASSERT_NEAR(0, time, MAX_TIME_DIFF);
	startTime = Time::now();
	lock.endWrite();
	endTime = Time::now();
	time = std::chrono::duration<double, ratio<1, 1>>(endTime - startTime).count();
	ASSERT_NEAR(0, time, MAX_TIME_DIFF);
}
TEST(MultipleReadOneWriteLockTest, startReadThenWriteTest)
{
	MultipleReadOneWriteLock lock;
	lock.startRead();
	auto writeTask = createFuture<void>([&lock]() {
		lock.startWrite();
		lock.endWrite();
	});
	EXPECT_EQ(std::future_status::timeout, writeTask.wait_for(chrono::duration<int, ratio<1, 1>>(10)));
	lock.endRead();
}
TEST(MultipleReadOneWriteLockTest, testMultiReadWrite)
{
	MultipleReadOneWriteLock lock;
	std::promise<void> startSignal;
	std::shared_future<void> ready(startSignal.get_future());
	atomic<bool> startWrite;
	atomic<bool> endRead;
	atomic<bool> startRead;
	startWrite = false;
	endRead = false;
	startRead = false;
	constexpr int NUM_OF_READ = 10;
	constexpr int MIN_READ_TIME_SEC = 5;
	constexpr int MAX_WRITE_TIME_SEC = 25;
	constexpr int MAX_TIME_DIFF = 1;
	bool isReadBeforeWrite[NUM_OF_READ];
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		isReadBeforeWrite[i] = false;
	}
	auto readTask = createFutureArr<int>([&lock, &ready, &startWrite, &isReadBeforeWrite, &endRead, &startRead
#if defined(VS_SUP) && defined(_MSC_VER)
	, MIN_READ_TIME_SEC, MAX_WRITE_TIME_SEC
#endif
	](int index) {
		int randNum = mRand();
		randNum = MIN_READ_TIME_SEC + randNum % (MAX_WRITE_TIME_SEC - MIN_READ_TIME_SEC);
		if (index != 0)
		{
			ready.wait();
		}
		startRead = true;
		lock.startRead();
		startWrite = true;
		if (!endRead)
		{
			isReadBeforeWrite[index] = true;
			this_thread::sleep_for(chrono::seconds(randNum));
		}
		lock.endRead();
		return randNum;
	}, NUM_OF_READ);
	while (startRead == false);
	startSignal.set_value();
	auto startTime = Time::now();
	lock.startWrite();
	auto endTime = Time::now();
	endRead = true;
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		ASSERT_EQ(readTask[i].wait_for(1s),
			isReadBeforeWrite[i] ? future_status::ready : future_status::timeout);
	}
	lock.endWrite();
	int maxWait = MIN_READ_TIME_SEC;
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		int waitTime = readTask[i].get();
		if (isReadBeforeWrite[i] && waitTime > maxWait)
		{
			maxWait = waitTime;
		}
	}
	delete[] readTask;
	double time = chrono::duration<double, ratio<1, 1>>(endTime - startTime).count();
	ASSERT_NEAR(maxWait, time, MAX_TIME_DIFF);
	}
TEST(MultipleReadOneWriteLockTest, testMultiWriteRead)
{
	MultipleReadOneWriteLock lock;
	std::promise<void> startSignal;
	std::shared_future<void> ready(startSignal.get_future());
	atomic<bool> isRead(false);
	constexpr int NUM_OF_READ = 10;
	constexpr int MIN_READ_TIME_SEC = 5;
	constexpr int MAX_WRITE_TIME_SEC = 15;
	constexpr int MAX_TIME_DIFF = 1;
	auto readTask = createFutureArr<int>([&lock, &ready, &isRead
#if defined(VS_SUP) && defined(_MSC_VER)
	, MIN_READ_TIME_SEC, MAX_WRITE_TIME_SEC
#endif
	](int index) {
		int randNum = mRand();
		int retVal = 0;
		randNum = MIN_READ_TIME_SEC + randNum % (MAX_WRITE_TIME_SEC - MIN_READ_TIME_SEC);
		ready.wait();
		lock.startWrite();
		this_thread::sleep_for(chrono::seconds(randNum));
		if (!isRead)
		{
			retVal = randNum;
		}
		lock.endWrite();
		return retVal;
	}, NUM_OF_READ);
	startSignal.set_value();
	auto startTime = Time::now();
	lock.startRead();
	auto endTime = Time::now();
	isRead = true;
	lock.endRead();
	int wait = 0;
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		wait += readTask[i].get();
	}
	delete[] readTask;
	double time = chrono::duration<double, ratio<1, 1>>(endTime - startTime).count();
	ASSERT_NEAR(wait, time, MAX_TIME_DIFF);
}
TEST(MultipleReadOneWriteLockTest, testMultiWriteMiddelRead)
{
	MultipleReadOneWriteLock lock;
	std::promise<void> startSignal;
	std::shared_future<void> ready(startSignal.get_future());
	atomic<bool> isRead(false);
	constexpr int NUM_OF_READ = 10;
	constexpr int MIN_READ_TIME_SEC = 5;
	constexpr int MAX_WRITE_TIME_SEC = 15;
	constexpr int MAX_TIME_DIFF = 1;
	auto readTask = createFutureArr<int>([&lock, &ready, &isRead
#if defined(VS_SUP) && defined(_MSC_VER)
	, MIN_READ_TIME_SEC, MAX_WRITE_TIME_SEC
#endif
	](int index) {
		int randNum = mRand();
		int retVal = 0;
		randNum = MIN_READ_TIME_SEC + randNum % (MAX_WRITE_TIME_SEC - MIN_READ_TIME_SEC);
		ready.wait();
		if (randNum > 10)
		{
			while (!isRead)
			{
				this_thread::sleep_for(chrono::milliseconds(100));
			}
		}
		lock.startWrite();
		this_thread::sleep_for(chrono::seconds(randNum));
		if (!isRead)
		{
			retVal = randNum;
		}
		lock.endWrite();
		return retVal;
	}, NUM_OF_READ);
	startSignal.set_value();
	auto startTime = Time::now();
	lock.startRead();
	auto endTime = Time::now();
	isRead = true;
	lock.endRead();
	int wait = 0;
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		wait += readTask[i].get();
	}
	delete[] readTask;
	double time = chrono::duration<double, ratio<1, 1>>(endTime - startTime).count();
	ASSERT_NEAR(wait, time, MAX_TIME_DIFF);
	}
TEST(MultipleReadOneWriteLockTest, testInterface)
{
	MultipleReadOneWriteLock lock;
	std::promise<void> startSignal;
	std::shared_future<void> ready(startSignal.get_future());
	constexpr int NUM_OF_READ = 40;
	constexpr int NUM_OF_WRITE = 40;
	constexpr int MIN_READ_TIME_SEC = 5;
	constexpr int MAX_WRITE_TIME_SEC = 15;
	std::atomic<int> numOfReaders(0);
	std::atomic<int> numOfWriters(0);
	auto readTask = createFutureArr<void>([&lock, &ready, &numOfReaders, &numOfWriters
#if defined(VS_SUP) && defined(_MSC_VER)
		, MIN_READ_TIME_SEC, MAX_WRITE_TIME_SEC
#endif
	](int index) {
		int randNum = mRand();
		randNum = MIN_READ_TIME_SEC + randNum % (MAX_WRITE_TIME_SEC - MIN_READ_TIME_SEC);
		ready.wait();
		this_thread::sleep_for(chrono::seconds(randNum) * 2);
		lock.startRead();
		++numOfReaders;
		ASSERT_EQ(0, numOfWriters);
		this_thread::sleep_for(chrono::seconds(randNum));
		--numOfReaders;
		lock.endRead();
	}, NUM_OF_READ);
	auto writeTask = createFutureArr<void>([&lock, &ready, &numOfWriters, &numOfReaders
#if defined(VS_SUP) && defined(_MSC_VER)
		, MIN_READ_TIME_SEC, MAX_WRITE_TIME_SEC
#endif
	](int index) {
		int randNum = mRand();
		randNum = MIN_READ_TIME_SEC + randNum % (MAX_WRITE_TIME_SEC - MIN_READ_TIME_SEC);
		ready.wait();
		this_thread::sleep_for(chrono::seconds(randNum) * 2);
		lock.startWrite();
		++numOfWriters;
		ASSERT_EQ(1, numOfWriters);
		ASSERT_EQ(0, numOfReaders);
		this_thread::sleep_for(chrono::seconds(randNum));
		--numOfWriters;
		lock.endWrite();
	}, NUM_OF_WRITE);
	this_thread::sleep_for(chrono::seconds(1));
	startSignal.set_value();
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		readTask[i].get();
	}
	for (int i = 0; i < NUM_OF_WRITE; ++i)
	{
		writeTask[i].get();
	}
	delete[] readTask;
	delete[] writeTask;
}