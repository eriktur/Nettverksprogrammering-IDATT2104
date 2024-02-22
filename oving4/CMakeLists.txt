cmake_minimum_required(VERSION 3.27)
project(oving_4)

set(CMAKE_CXX_STANDARD 17)

# Create an executable for the client
add_executable(Client Client.cpp)

# Create an executable for the server
add_executable(Server Server.cpp)