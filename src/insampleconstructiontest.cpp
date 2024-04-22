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

    const double NUM_CONTRACTS = 1.; 
    const double POINT_VALUE = 64'000.;
    const double SLPG = 65.;


    std::string filename = "../data/" + ASSET + ".csv"; // Replace with your actual file name

    const int CHN_LEN_MIN = 1000; // 500
    const int CHN_LEN_MAX = 10000; // 10000
    const int CHN_LEN_STEP = 500; // 10

    const double STP_PCT_MIN = 0.005; // 0.005
    const double STP_PCT_MAX = 0.018; // 0.10
    const double STP_PCT_STEP = 0.001; // 0.001

    const int NUM_CHN_LEN = (int) ((CHN_LEN_MAX - CHN_LEN_MIN) / CHN_LEN_STEP) + 1;
    const int NUM_STP_PCT = (int) ((STP_PCT_MAX - STP_PCT_MIN) / STP_PCT_STEP) + 1;

    std::cout << "Size of param space:" << NUM_CHN_LEN * NUM_STP_PCT << std::endl;
    // ASSET /////////////////////////////////////////////////////

    auto bars = readData(filename);

    std::vector<double> highs;
    std::vector<double> lows;

    highs.reserve(SIZE);
    lows.reserve(SIZE);
    
    for (int i = 0; i < bars.size(); i++) {
        highs.push_back(bars.at(i).high);
        lows.push_back(bars.at(i).low);
    }

    const unsigned long long start_date = 1990'1001'0000;
    const unsigned long long end_date   = 2023'1001'0000;

    const int in_sample_years = 4;
    const int out_sample_months = 3;

    auto t1 = std::chrono::high_resolution_clock::now();
    TimeWindowEngine::run(
        ASSET,
        start_date, end_date,
        in_sample_years, out_sample_months,
        NUM_CONTRACTS, POINT_VALUE, SLPG, 
        bars, highs, lows, 
        CHN_LEN_MIN, CHN_LEN_MAX, CHN_LEN_STEP, 
        STP_PCT_MIN, STP_PCT_MAX, STP_PCT_STEP 
        );
    auto t2 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "Execution in: " << dt << " milliseconds" << std::endl;

}