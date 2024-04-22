#include <iostream>
#include <vector>

#include "utils.hpp"

int main () {

    std::vector<double> arr {1., 2., 3., -11., 25., 49., 590., 1., 1., 2., 3., 3., 1., 400., 0., 1.};
    auto hh1 = MinMaxSlidingWindow(arr, 3, true);
    auto hh2 = MinMaxSlidingWindow(arr, 2, true);
    auto hh3 = MinMaxSlidingWindow(arr, 3, false);
    auto hh4 = MinMaxSlidingWindow(arr, 2, false);

    std::cout << "Hello" << std::endl;

    double ChnLenMin = 0.015;
    double ChnLenStep = 0.001;
    double ChnLenMax = 0.018;
    int numThreads = 4;
    int numChnLen = static_cast<int>(std::ceil((ChnLenMax - ChnLenMin + ChnLenStep) / ChnLenStep));
    int workload_per_thread = numChnLen / numThreads;
    std::cout << "NUM CHANNELS " << numChnLen << std::endl;
    for (int i = 0; i < numThreads; ++i) {
        int startRange = i * workload_per_thread;
        int endRange = (i + 1 == numThreads) ? numChnLen : startRange + workload_per_thread; 
        double ChnLen = ChnLenMin + i * ChnLenStep;
        std::cout << ChnLen << std::endl;
        std::cout << "Start Range: " << startRange << " End Range: " << endRange << std::endl;
    }

}