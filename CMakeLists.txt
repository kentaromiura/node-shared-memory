cmake_minimum_required(VERSION 3.6)
project(nan)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        node_shared_memory.cc)

add_executable(nan ${SOURCE_FILES})