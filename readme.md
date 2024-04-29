# MathMethodsProject

Welcome to the Math Methods Project repository. This project aims to implement a grid search on two parameters that define a Channel Breakout Trend-Following Strategy. The first parameter that is studied is the Channel Length, and the second one is the Stop Percentage. The grid covers a large range of those parameters. This project includes implementations for constructing Back Testing data for HO using `insampleconstructiontest` and XB data using `insampleconstructiontest2`.

## Prerequisites

Before you begin, ensure you have the following installed:
- **C++20**: Your C++ compiler must support C++20 standards.
- **Boost Library**: The Boost library must be installed and its location added to your system's PATH.
- **CMake**: The C++ build system must be installed and its location added to your system's PATH.

## Building and Compiling

To build and compile the project, execute the following commands in the root directory:

```bash
mkdir build
cd build
cmake ..
make
