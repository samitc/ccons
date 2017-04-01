cmake_minimum_required(VERSION 3.5)
project(ccons)
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
LINK_DIRECTORIES(${ccons_SOURCE_DIR}/Dependency/)
ADD_LIBRARY(ccons STATIC ${COMMON_SOURCE})
add_executable(cconsTesting ${TEST_SOURCE})
find_package( Threads )
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions(-DVS_SUP) #use to remove code that visual studio product error
endif()
target_link_libraries(cconsTesting ccons)
TARGET_LINK_LIBRARIES(cconsTesting gtest)
TARGET_LINK_LIBRARIES(cconsTesting ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(cconsTesting ccons)
target_compile_options(cconsTesting PUBLIC -I${ccons_SOURCE_DIR}/Dependency/gtest/include)
