//
// Created by amit on 20/06/16.
//

#include <future>
#include "gtest/gtest.h"
#include "../Src/Queue.h"

using namespace Ccons;
class IntSafe
{
private:
	int *num;
public:
	IntSafe(int num)
	{
		this->num = new int(num);
	}
	~IntSafe()
	{
		delete num;
	}
	int *get()
	{
		return num;
	}
	void set(int num)
	{
		*this->num = num;
	}
};
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
	EXPECT_EQ(false, q.isEmpty());
	EXPECT_EQ(num.get(), q.top());
	EXPECT_EQ(false, q.isEmpty());
	EXPECT_EQ(num.get(), q.dequeue());
	EXPECT_EQ(true, q.isEmpty());
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
		push_done = std::async(std::launch::async,
							   [&q, ready, &push_ready, &num]() {
								   push_ready.set_value();
								   ready.wait();
								   std::this_thread::sleep_for(std::chrono::duration<int, std::ratio<1, 1>>(3));
								   q.enqueue(num.get());
							   }
							  );
		pop_done = std::async(std::launch::async,
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
		EXPECT_NEAR(3.0, time, 0.001);
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
	IntSafe num(6);
	std::promise<void> go, push_ready, pop_ready;
	std::shared_future<void> ready(go.get_future());
	std::future<void> push_done;
	std::future<int *> pop_done;
	try
	{
		push_done = std::async(std::launch::async,
							   [&q, ready, &push_ready, &num]() {
								   push_ready.set_value();
								   ready.wait();
								   std::this_thread::sleep_for(std::chrono::duration<int, std::ratio<1, 1>>(3));
								   q.enqueue(num.get());
							   }
							  );
		pop_done = std::async(std::launch::async,
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
		ASSERT_EQ(false, q.isEmpty());
		EXPECT_EQ(num.get(), retVal);
		double time = std::chrono::duration<double, std::ratio<1, 1>>(e - s).count();
		EXPECT_NEAR(3.0, time, 0.001);
		ASSERT_EQ(num.get(),q.dequeue());
		push_done.get();
	}
	catch (...)
	{
		go.set_value();
		throw;
	}
}
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int returnValue;
	returnValue = RUN_ALL_TESTS();
	return returnValue;
}