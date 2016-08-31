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
void MultipleReadOneWriteLockSetUp()
{
	srand(time(nullptr));
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
	startWrite = false;
	endRead = false;
	const int NUM_OF_READ = 10;
	const int MIN_READ_TIME_SEC = 5;
	const int MAX_WRITE_TIME_SEC = 25;
	const int MAX_TIME_DIFF = 1;
	bool isReadBeforeWrite[NUM_OF_READ];
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		isReadBeforeWrite[i] = false;
	}
	auto readTask = createFutureArr<int>([&lock, &ready, &startWrite, &isReadBeforeWrite, &endRead](int index) {
		int randNum = rand();
		randNum = MIN_READ_TIME_SEC + randNum % (MAX_WRITE_TIME_SEC - MIN_READ_TIME_SEC);
		ready.wait();
		lock.startRead();
		startWrite = true;
		if (!endRead)
		{
			isReadBeforeWrite[index] = true;
		}
		this_thread::sleep_for(chrono::seconds(randNum));
		lock.endRead();
		return randNum;
	}, NUM_OF_READ);
	startSignal.set_value();
	while (!startWrite)
	{ }
	auto startTime = Time::now();
	lock.startWrite();
	endRead = true;
	auto endTime = Time::now();
	for (int i = 0; i < NUM_OF_READ; ++i)
	{
		ASSERT_EQ(readTask[i].wait_for(std::chrono::microseconds(0)),
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
	atomic<bool> isRead;
	const int NUM_OF_READ = 10;
	const int MIN_READ_TIME_SEC = 5;
	const int MAX_WRITE_TIME_SEC = 15;
	const int MAX_TIME_DIFF = 1;
	auto readTask = createFutureArr<int>([&lock, &ready, &isRead](int index) {
		int randNum = rand();
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