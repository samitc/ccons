cmake_minimum_required(VERSION 3.5)
project(ccons)
project(cconsTesting)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(COMMON_SOURCE
        Src/Queue.h
        Src/Queue.cpp
        Src/MultipleReadOneWriteLock.h
        Src/MultipleReadOneWriteLock.cpp
        )
set(TEST_SOURCE
        Testing/QueueTest.cpp
        Testing/MultipleReadOneWriteLockTest.cpp
        Testing/cconsTest.cpp
        )
LINK_DIRECTORIES(${cconsTesting_SOURCE_DIR}/Dependency/)
add_executable(ccons ${COMMON_SOURCE})
add_executable(cconsTesting ${COMMON_SOURCE} ${TEST_SOURCE})
TARGET_LINK_LIBRARIES(cconsTesting gtest.a)
TARGET_LINK_LIBRARIES(cconsTesting pthread)
target_compile_options(cconsTesting PUBLIC -I${cconsTesting_SOURCE_DIR}/Dependency/gtest/include)