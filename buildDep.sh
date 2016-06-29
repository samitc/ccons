#!/bin/bash
mkdir Dependency
cd DependencyModules
cd googletest
g++ -isystem include -I. -pthread -c src/gtest-all.cc
ar -rv libgtest.a gtest-all.o
cd ..
cd ..
mv DependencyModules/googletest/libgtest.a Dependency/
mkdir Dependency/gtest
cp -rf DependencyModules/googletest/include Dependency/gtest
rm DependencyModules/googletest/gtest-all.o
