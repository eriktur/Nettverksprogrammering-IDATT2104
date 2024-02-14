cmake_minimum_required(VERSION 3.26)
project(oving3)

set(CMAKE_CXX_STANDARD 17)

add_executable(oving3 client.cpp
        server.cpp
        server.h
        webserver.cpp
        webserver.h)
