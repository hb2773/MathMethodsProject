cmake_minimum_required(VERSION 3.10)  # Minimum version of CMake required

project(MyProject VERSION 1.0)  # Name and version of your project

set(CMAKE_CXX_STANDARD 17)  # C++ standard used by the project
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an executable target named 'my_app' to be built from the source files listed
add_executable(main src/main.cpp)
