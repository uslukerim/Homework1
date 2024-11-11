cmake_minimum_required(VERSION 3.10)

# define to Project Name
project(Homework1)

# C++ standart setting 
set(CMAKE_CXX_STANDARD 11)

# include dizinini belirt
include_directories(include)

#set(SOURCES src/AnsiTerminal.cpp src/main.cpp)

# Projedeki kaynak dosyaları listele
file(GLOB SOURCES "src/*.cpp")

# Çıktı dosyasının ismini ayarla

add_executable(Homework1 ${SOURCES})
