cmake_minimum_required(VERSION 3.10)

# define to Project Name
project(Homework1)

# C++ standart setting 
set(CMAKE_CXX_STANDARD 11)

#declerate include path 
include_directories(include)

#set(SOURCES src/AnsiTerminal.cpp src/main.cpp)

# list of project source codes
file(GLOB SOURCES "src/*.cpp")

# Execautable file name

add_executable(Homework1 ${SOURCES})
