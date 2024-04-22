cmake_minimum_required(VERSION 3.10)  # Minimum version of CMake required

project(MyProject VERSION 1.0)  # Name and version of your project

set(CMAKE_CXX_STANDARD 20)  # C++ standard used by the project
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Boost library components
find_package(Boost 1.65 REQUIRED COMPONENTS date_time)  # Make sure the required date_time component is found

# Include directories for Boost and local include folder
include_directories(${Boost_INCLUDE_DIRS} include)

# Add an executable target named 'my_app' to be built from the source files listed

# add_executable(main src/main.cpp)

# add_executable(main2 src/main2.cpp)

# add_executable(main3 src/servectest.cpp)

add_executable(insampleconstructiontest src/insampleconstructiontest.cpp)



