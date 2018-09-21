cmake_minimum_required(VERSION 3.12)
project(channel_debug)

set(CMAKE_CXX_STANDARD 11)

add_executable(channel_debug main.cpp)

target_link_libraries(channel_debug
        pthread
        )
