#include <iostream>
#include <vector>

#include "DataReader.hpp"
#include "utils.hpp"

int main () {

    std::vector<float> arr {1.f, 2.f, 3.f, -11.f, 25.f, 49.f, 590.f, 1.f, 1.f, 2.f, 3.f, 3.f, 1.f, 400.f, 0.f, 1.f};
    auto hh1 = MinMaxSlidingWindow(arr, 3, true);
    auto hh2 = MinMaxSlidingWindow(arr, 2, true);
    auto hh3 = MinMaxSlidingWindow(arr, 3, false);
    auto hh4 = MinMaxSlidingWindow(arr, 2, false);

    std::cout << "Hello" << std::endl;

    float ChnLenMin = 0.015f;
    float ChnLenStep = 0.001f;
    float ChnLenMax = 0.018f;
    int numThreads = 4;
    int numChnLen = static_cast<int>(std::ceil((ChnLenMax - ChnLenMin + ChnLenStep) / ChnLenStep));
    int workload_per_thread = numChnLen / numThreads;

    std::cout << "NUM CHANNELS " << numChnLen << std::endl;

    for (int i = 0; i < numThreads; ++i) {
        int startRange = i * workload_per_thread;
        int endRange = (i + 1 == numThreads) ? numChnLen : startRange + workload_per_thread; 
        float ChnLen = ChnLenMin + i * ChnLenStep;
        std::cout << ChnLen << std::endl;
        std::cout << "Start Range: " << startRange << " End Range: " << endRange << std::endl;
    }

    std::string fileName = "HHFilename_.dat";
    const char* fileName_ = fileName.data();

    const int CHN_LEN_MIN = 2;
    const int CHN_LEN_MAX = 10;
    const int CHN_LEN_STEP = 1;
    const int NUM_CHN_LEN = (CHN_LEN_MAX - CHN_LEN_MIN + CHN_LEN_STEP) / CHN_LEN_STEP;

    size_t fileSize = 2 * NUM_CHN_LEN * arr.size() * sizeof(float);

    create_HH_LL_Vector(
        fileName_, fileSize,
        CHN_LEN_MIN, CHN_LEN_MAX, CHN_LEN_STEP,
        arr,
        true);

    for (int k = 0; k < NUM_CHN_LEN; k++) {
        auto vec = read_kth_vector(fileName_, arr.size(), k);
        for (const auto& e: vec) {
            std::cout << e << ", ";
        }
        std::cout << std::endl;
    }

}