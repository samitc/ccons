//
// Created by amit on 29/06/16.
//
#include "gtest/gtest.h"
#include "cconsTest.h"
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	queueSetUp();
	MultipleReadOneWriteLockSetUp();
	int returnValue;
	returnValue = RUN_ALL_TESTS();
	return returnValue;
}