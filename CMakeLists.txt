cmake_minimum_required(VERSION 3.5)
project(ccons)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
        MultipleReadOneWriteLock.h
        MultipleReadOneWriteLock.cpp
        Queue.h
        Queue.cpp
    )
add_executable(ccons ${SOURCE_FILES})