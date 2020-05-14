cmake_minimum_required(VERSION 3.13)
project(SO2_bookstore)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-lncurses")

add_executable(SO2_bookstore main.cpp Room.cpp Customer.cpp Resources.cpp)
target_link_libraries(SO2_bookstore pthread curses)