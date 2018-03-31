cmake_minimum_required(VERSION 3.9)
project(MemoryAllocator)

set(CMAKE_CXX_STANDARD 17)

add_executable(MemoryAllocator main.cpp MemoryPartDescriptor.cpp MemoryPartDescriptor.h Selector.h Selector.cpp MemoryManager.cpp MemoryManager.h)