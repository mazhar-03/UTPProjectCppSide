cmake_minimum_required(VERSION 3.27)
project(gameLogic)

set(CMAKE_CXX_STANDARD 20)

add_library(gameLogic SHARED library.cpp)

target_link_options(gameLogic PRIVATE "-static")

target_include_directories(gameLogic PRIVATE "C:\\Program Files\\Java\\jdk-22\\include")
target_include_directories(gameLogic PRIVATE "C:\\Program Files\\Java\\jdk-22\\include\\win32")