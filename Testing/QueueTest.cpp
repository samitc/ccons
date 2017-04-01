//
// Created by amit on 20/06/16.
//

#include <future>
#include "../Src/Queue.h"
#include "cconsTest.h"
#include "gtest/gtest.h"

using namespace Ccons;
using namespace std;
class IntSafe
{
private:
	int num;
public:
	constexpr IntSafe() : num(0) { }
	explicit constexpr IntSafe(int num) : num(num)
	{
	}
#if !defined(VS_SUP) || !defined(_MSC_VER)
	constexpr
#endif
	int *get()
	{
		return &num;
	}
#if !defined(VS_SUP) || !defined(_MSC_VER)
	constexpr
#endif
		void set(int num)
	{
		this->num = num;
	}
#if !defined(VS_SUP) || !defined(_MSC_VER)
	constexpr
#endif
		IntSafe &operator=(const IntSafe &copy)
	{
		if (this != &copy)
		{
			num = copy.num;
		}
		return *this;
	}
};
const int NUM_OF_NUMBERS_PER_THREAD = 1000;
const int NUM_OF_THREAD = 6;
#if !defined(VS_SUP) || !defined(_MSC_VER)
constexpr
#endif
unsigned long long calculateSum()
{
	unsigned long long sum = 0;
	for (int i = 0; i < NUM_OF_NUMBERS_PER_THREAD * NUM_OF_THREAD; ++i)
	{
		sum += i;
	}
	return sum;
}
IntSafe numbers[NUM_OF_NUMBERS_PER_THREAD * NUM_OF_THREAD];
const unsigned long long gSum = calculateSum();
TEST(QueueTest, generalTest)
{
	Queue<int> q;
	EXPECT_EQ(true, q.isEmpty());
	EXPECT_EQ(nullptr, q.dequeue());
	EXPECT_EQ(nullptr, q.top());
}
TEST(QueueTest, putGetTest)
{
	Queue<int> q;
	IntSafe num(5);
	q.enqueue(num.get());
	EXPECT_FALSE(q.isEmpty());
	EXPECT_EQ(num.get(), q.top());
	EXPECT_FALSE(q.isEmpty());
	EXPECT_EQ(num.get(), q.dequeue());
	EXPECT_TRUE(q.isEmpty());
}
TEST(QueueTest, putWaitTest)
{
	Queue<int> q;
	IntSafe num(6);
	std::promise<void> go, push_ready, pop_ready;
	std::shared_future<void> ready(go.get_future());
	std::future<void> push_done;
	std::future<int *> pop_done;
	try
	{
		push_done = createFuture<void>([&q, ready, &push_ready, &num]() {
			push_ready.set_value();
			ready.wait();
			std::this_thread::sleep_for(std::chrono::duration<int, std::ratio<1, 1>>(3));
			q.enqueue(num.get());
		});
		pop_done = createFuture<int *>(
				[&q, ready, &pop_ready, &num]() {
					pop_ready.set_value();
					ready.wait();
					return q.dequeueW();
				}
									  );
		push_ready.get_future().wait();
		pop_ready.get_future().wait();
		go.set_value();
		auto s = std::chrono::high_resolution_clock::now();
		int *retVal = pop_done.get();
		auto e = std::chrono::high_resolution_clock::now();
		ASSERT_EQ(true, q.isEmpty());
		EXPECT_EQ(num.get(), retVal);
		double time = std::chrono::duration<double, std::ratio<1, 1>>(e - s).count();
		EXPECT_NEAR(3.0, time, 0.01);
		push_done.get();
	}
	catch (...)
	{
		go.set_value();
		throw;
	}
}
TEST(QueueTest, putWaitTopTest)
{
	Queue<int> q;
	IntSafe num(7);
	std::promise<void> go, push_ready, pop_ready;
	std::shared_future<void> ready(go.get_future());
	std::future<void> push_done;
	std::future<int *> pop_done;
	try
	{
		push_done = createFuture<void>(
				[&q, ready, &push_ready, &num]() {
					push_ready.set_value();
					ready.wait();
					std::this_thread::sleep_for(std::chrono::duration<int, std::ratio<1, 1>>(3));
					q.enqueue(num.get());
				}
									  );
		pop_done = createFuture<int *>(
				[&q, ready, &pop_ready, &num]() {
					pop_ready.set_value();
					ready.wait();
					return q.topW();
				}
									  );
		push_ready.get_future().wait();
		pop_ready.get_future().wait();
		go.set_value();
		auto s = std::chrono::high_resolution_clock::now();
		int *retVal = pop_done.get();
		auto e = std::chrono::high_resolution_clock::now();
		ASSERT_FALSE(q.isEmpty());
		EXPECT_EQ(num.get(), retVal);
		double time = std::chrono::duration<double, std::ratio<1, 1>>(e - s).count();
		EXPECT_NEAR(3.0, time, 0.1);
		ASSERT_EQ(num.get(), q.dequeue());
		push_done.get();
	}
	catch (...)
	{
		go.set_value();
		throw;
	}
}
TEST(QueueTest, MultiPutOneGet)
{
	Queue<int> q;
	const int TIME_TO_WAIT_IN_SEC = 60;
	std::promise<void> startSignal;
	std::promise<void> pushReady[NUM_OF_THREAD];
	std::promise<void> popReady;
	std::shared_future<void> ready(startSignal.get_future());
	std::future<void> pushDone[NUM_OF_THREAD];
	std::future<void> popDone;
	unsigned long long sum = gSum;
	try
	{
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			std::promise<void> *temp = &pushReady[i];
			int start = i;
			int jump = NUM_OF_THREAD;
			pushDone[i] = createFuture<void>(
					[&q, ready, temp, start, jump]() {
						int max = NUM_OF_THREAD * NUM_OF_NUMBERS_PER_THREAD;
						temp->set_value();
						ready.wait();
						for (int i = start; i < max; i += jump)
						{
							q.enqueue(numbers[i].get());
						}
					});
		}
		popDone = createFuture<void>(
				[&q, ready, &popReady, &sum]() {
					int max = NUM_OF_THREAD * NUM_OF_NUMBERS_PER_THREAD;
					popReady.set_value();
					ready.wait();
					for (int i = 0; i < max; ++i)
					{
						sum -= *q.dequeueW();
					}
				}
									);
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			pushReady[i].get_future().wait();
		}
		popReady.get_future().wait();
		startSignal.set_value();
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			pushDone[i].get();
		}
		ASSERT_EQ(std::future_status::ready,
				  popDone.wait_for(std::chrono::duration<int, std::ratio<1, 1>>(TIME_TO_WAIT_IN_SEC)));
		EXPECT_EQ(true, q.isEmpty());
		ASSERT_EQ(0, sum);
	}
	catch (...)
	{
		startSignal.set_value();
		throw;
	}
}
TEST(QueueTest, MultiGetOnePut)
{
	Queue<int> q;
	const int TIME_TO_WAIT_IN_SEC = 60;
	std::promise<void> startSignal;
	std::promise<void> popReady[NUM_OF_THREAD];
	std::promise<void> pushReady;
	std::shared_future<void> ready(startSignal.get_future());
	std::future<unsigned long long> popDone[NUM_OF_THREAD];
	std::future<void> pushDone;
	unsigned long long sum = gSum;
	try
	{
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			std::promise<void> *temp = &popReady[i];
			int start = i;
			int jump = NUM_OF_THREAD;
			popDone[i] = createFuture<unsigned long long>(
					[&q, ready, temp, start, jump]() {
						int max = NUM_OF_THREAD * NUM_OF_NUMBERS_PER_THREAD;
						unsigned long long sum = 0;
						temp->set_value();
						ready.wait();
						for (int i = start; i < max; i += jump)
						{
							sum += *q.dequeueW();
						}
						return sum;
					}
														 );
		}
		pushDone = createFuture<void>(
				[&q, ready, &pushReady]() {
					int max = NUM_OF_THREAD * NUM_OF_NUMBERS_PER_THREAD;
					pushReady.set_value();
					ready.wait();
					for (int i = 0; i < max; ++i)
					{
						q.enqueue(numbers[i].get());
					}
				}
									 );
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			popReady[i].get_future().wait();
		}
		pushReady.get_future().wait();
		startSignal.set_value();
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			EXPECT_EQ(std::future_status::ready,
					  popDone[i].wait_for(std::chrono::duration<int, std::ratio<1, 1>>(TIME_TO_WAIT_IN_SEC)));
		}
		pushDone.get();
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			sum -= popDone[i].get();
		}
		EXPECT_EQ(true, q.isEmpty());
		ASSERT_EQ(0, sum);
	}
	catch (...)
	{
		startSignal.set_value();
		throw;
	}
}
TEST(QueueTest, MultiGetMultiPut)
{
	Queue<int> q;
	std::promise<void> startSignal;
	std::promise<void> popReady[NUM_OF_THREAD * 2];
	std::promise<void> pushReady[NUM_OF_THREAD];
	std::shared_future<void> ready(startSignal.get_future());
	std::future<unsigned long long> popDone[NUM_OF_THREAD * 2];
	std::future<void> pushDone[NUM_OF_THREAD];
	unsigned long long sum = gSum;
	try
	{
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			std::promise<void> *temp = &popReady[i];
			std::promise<void> *temp2 = &popReady[i + NUM_OF_THREAD];
			std::promise<void> *temp1 = &pushReady[i];
			int start = i;
			int jump = NUM_OF_THREAD;
			popDone[i] = createFuture<unsigned long long>(
					[&q, ready, temp, start, jump]() {
						int max = NUM_OF_THREAD * NUM_OF_NUMBERS_PER_THREAD;
						unsigned long long sum = 0;
						temp->set_value();
						ready.wait();
						for (int i = start; i < max - 1; i += jump)
						{
							sum += *q.dequeueW();
						}
						return sum;
					}
														 );
			popDone[i + NUM_OF_THREAD] = createFuture<unsigned long long>(
					[&q, temp2]() {
						const int TOP_WAIT = 5000;
						temp2->set_value();
						int *num;
						for (int i = 0; i < TOP_WAIT; ++i)
						{
							num = q.topW();
							num = q.top();
						}
						for (int i = 0; i < TOP_WAIT; ++i)
						{
							num = q.top();
						}
						return (unsigned long long)0;
					}
																		 );
			pushDone[i] = createFuture<void>(
					[&q, ready, temp1, start, jump]() {
						int max = NUM_OF_THREAD * NUM_OF_NUMBERS_PER_THREAD;
						temp1->set_value();
						ready.wait();
						for (int i = start; i < max; i += jump)
						{
							q.enqueue(numbers[i].get());
						}
					}
											);
		}
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			popReady[i + NUM_OF_THREAD].get_future().wait();
			popReady[i].get_future().wait();
			pushReady[i].get_future().wait();
		}
		startSignal.set_value();
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			pushDone[i].get();
		}
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			sum -= popDone[i].get();
		}
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			popDone[i + NUM_OF_THREAD].get();
		}
		EXPECT_FALSE(q.isEmpty());
		while (!q.isEmpty())
		{
			sum -= *q.dequeue();
		}
		ASSERT_EQ(0, sum);
	}
	catch (...)
	{
		startSignal.set_value();
		throw;
	}
}
TEST(QueueTest, MultiTopMultiPut)
{
	Queue<int> q;
	std::promise<void> startSignal;
	std::promise<void> popReady[NUM_OF_THREAD];
	std::promise<void> pushReady[NUM_OF_THREAD];
	std::shared_future<void> ready(startSignal.get_future());
	std::future<void> popDone[NUM_OF_THREAD];
	std::future<void> pushDone[NUM_OF_THREAD];
	unsigned long long sum = gSum;
	std::atomic<bool> isPushDone;
	isPushDone = false;
	try
	{
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			std::promise<void> *temp = &popReady[i];
			std::promise<void> *temp1 = &pushReady[i];
			int start = i;
			int jump = NUM_OF_THREAD;
			popDone[i] = createFuture<void>(
					[&q, ready, temp, start, jump, &isPushDone]() {
						unsigned long long sum = 0;
						temp->set_value();
						while (!isPushDone)
						{
							sum += *q.topW();
							sum += *q.top();
						}
					}
										   );
			pushDone[i] = createFuture<void>(
					[&q, ready, temp1, start, jump]() {
						int max = NUM_OF_THREAD * NUM_OF_NUMBERS_PER_THREAD;
						temp1->set_value();
						ready.wait();
						for (int i = start; i < max; i += jump)
						{
							q.enqueue(numbers[i].get());
						}
					}
											);
		}
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			popReady[i].get_future().wait();
			pushReady[i].get_future().wait();
		}
		startSignal.set_value();
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			pushDone[i].get();
		}
		isPushDone = true;
		for (int i = 0; i < NUM_OF_THREAD; ++i)
		{
			popDone[i].get();
		}
		EXPECT_FALSE(q.isEmpty());
		int *num = q.dequeue();
		while (num != nullptr)
		{
			sum -= *num;
			num = q.dequeue();
		}
		EXPECT_EQ(0, sum);
	}
	catch (...)
	{
		startSignal.set_value();
		throw;
	}
}
void queueSetUp()
{
	for (int i = 0; i < NUM_OF_NUMBERS_PER_THREAD * NUM_OF_THREAD; i++)
	{
		numbers[i] = IntSafe(i);
	}
}
