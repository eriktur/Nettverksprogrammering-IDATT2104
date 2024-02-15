cmake_minimum_required(VERSION 3.26)
project(oving4)

set(CMAKE_CXX_STANDARD 17)

add_executable(oving4 client.cpp
        server.cpp
        server.h)
