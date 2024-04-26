#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <climits>
#include <map>
#include <span>

#include "TimeWindowEngine.hpp"
#include "BackTestEngine.hpp"
#include "DataReader.hpp"
#include "Bar.hpp"
#include "Constants.hpp"
#include "Strategy.hpp"
#include "utils.hpp"


int main () {

    // ASSET /////////////////////////////////////////////////////
    const std::string ASSET = "HO-5minHLV";
    // SIZE 
    const int SIZE = 611'839;

    const float NUM_CONTRACTS = 1.f; 
    const float POINT_VALUE = 64'000.f;
    const float SLPG = 65.f;


    std::string filename = "../data/" + ASSET + ".csv"; // Replace with your actual file name

    std::string HHFilename = "../data/" + ASSET + "_HH_data.dat";
    std::string LLFilename = "../data/" + ASSET + "_LL_data.dat";

    const char* HHFilename_ = HHFilename.data();
    const char* LLFilename_ = LLFilename.data(); 

    const int CHN_LEN_MIN = 500; // 500
    const int CHN_LEN_MAX = 10000; // 10000
    const int CHN_LEN_STEP = 10; // 10

    const float STP_PCT_MIN = 0.005f; // 0.005
    const float STP_PCT_MAX = 0.1f; // 0.10
    const float STP_PCT_STEP = 0.001f; // 0.001

    const int NUM_CHN_LEN = static_cast<int>(std::ceil((CHN_LEN_MAX - CHN_LEN_MIN + CHN_LEN_STEP) / CHN_LEN_STEP));
    const int NUM_STP_PCT = static_cast<int>(std::ceil((STP_PCT_MAX - STP_PCT_MIN + STP_PCT_STEP) / STP_PCT_STEP));
    
    std::size_t fileSize = 2 * SIZE * NUM_CHN_LEN * sizeof(float); // 64 MB for example
    std::cout << "Size of param space:" << NUM_CHN_LEN * NUM_STP_PCT << std::endl;
    // ASSET /////////////////////////////////////////////////////

    auto bars = readData(filename);

    std::vector<float> highs;
    std::vector<float> lows;

    highs.reserve(SIZE);
    lows.reserve(SIZE);
    
    for (int i = 0; i < bars.size(); i++) {
        highs.push_back(bars.at(i).high);
        lows.push_back(bars.at(i).low);
    }

    create_HH_LL_Vector(
        HHFilename_, fileSize,
        CHN_LEN_MIN, CHN_LEN_MAX, CHN_LEN_STEP,
        highs,
        true);

    create_HH_LL_Vector(
        LLFilename_, fileSize,
        CHN_LEN_MIN, CHN_LEN_MAX, CHN_LEN_STEP,
        lows,
        false);


    const unsigned long long start_date = 1990'1001'0000;
    const unsigned long long end_date   = 2003'0401'0000;

    const int in_sample_years = 1;
    const int out_sample_months = 3;

    auto t1 = std::chrono::high_resolution_clock::now();
    TimeWindowEngine::run(
        ASSET,
        start_date, end_date,
        in_sample_years, out_sample_months,
        NUM_CONTRACTS, POINT_VALUE, SLPG, 
        bars, HHFilename_, LLFilename_, 
        CHN_LEN_MIN, CHN_LEN_MAX, CHN_LEN_STEP, NUM_CHN_LEN,
        STP_PCT_MIN, STP_PCT_MAX, STP_PCT_STEP, NUM_STP_PCT 
        );
    auto t2 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "Execution in: " << dt << " milliseconds" << std::endl;

}