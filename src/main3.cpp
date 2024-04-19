#include <iostream>
#include <utils.hpp>
#include <vector>

int main () {
    std::vector<double> arr {1., 2., 3., -11., 25., 49., 590., 1., 1., 2., 3., 3., 1., 400., 0., 1.};
    auto hh1 = MinMaxSlidingWindow(arr, 3, true);
    auto hh2 = MinMaxSlidingWindow(arr, 2, true);
    auto hh3 = MinMaxSlidingWindow(arr, 3, false);
    auto hh4 = MinMaxSlidingWindow(arr, 2, false);

    std::cout << "Hello";

}