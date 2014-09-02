cmake_minimum_required(VERSION 2.6)
project(afstuderen)

#There are lots of scripts with cmake
#for finding external libraries. 
#see /usr/local/share/cmake-2.6/Modules/Find*.cmake for more examples
find_package(GLUT)
find_package(OpenGL)

#Add C++ 11 
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x") 

add_executable(afstuderen main.cpp)