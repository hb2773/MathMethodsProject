#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <chrono>
#include <climits>
#include "Bar.hpp"

// Steps for the BackTesting Engine. 
// 1 - Read the data OK
// 2 - Implement ONE Trend Following Channel (With DrawdownControl)
// 2a - Handle P&L with Slippage (See TF Data for Slippage in col. V)
// 2b - Create useful metrics (Measure various statistical performance properties of both equity curve and trade-by-trade table: Average Rate of Return, Standard Deviation of Returns, and the Ratio of the two above (known as the Sharpe or Information Ratio), Total Number of Trades, % of Winners or % of Profitable Trades, Return on the Account (Net Profit to Worst Drawdown), Average Winner, Average Loser, Profit Factor, etc. along with all other characteristics that you will deem useful to gaining insight into the behavior of your market. The quantity and choices of the performance characteristics will make a difference on the grade for your Final Project.)
// 3 - Create a loop to optimize as the BackTestEngine
// 4 - Choose another market to run optimization 
// 5 - Run optimization on the whole dataset and compare values 
// 6 - Handle output of optimization

const int DATA_LEN = 611900;

const int CHN_LEN_MIN = 500; // 500
const int CHN_LEN_MAX = 10000; // 10000
const int CHN_LEN_STEP = 100; // 10

const double STP_PCT_MIN = 0.005; // 0.005
const double STP_PCT_MAX = 0.10; // 0.10
const double STP_PCT_STEP = 0.1; // 0.001

const int NUM_CHN_LEN = (int) ((CHN_LEN_MAX - CHN_LEN_MIN) / CHN_LEN_STEP) + 1;
const int NUM_STP_PCT = (int) ((STP_PCT_MAX - STP_PCT_MIN) / STP_PCT_STEP) + 1;

// 3 - Pseudo code for 3. 
std::vector<double> pseudoCodeParmaterSearch(const std::vector<Bar>& bars) {
    std::vector<double> results(NUM_CHN_LEN * NUM_STP_PCT);
    for (int ChnLen = CHN_LEN_MIN; ChnLen < CHN_LEN_MAX; ChnLen += CHN_LEN_STEP) {
        for (double StpPct = STP_PCT_MIN; StpPct <= STP_PCT_MAX; StpPct += STP_PCT_STEP) {
            double curr_sum = 0.;
            for (const auto& bar : bars) {
                // std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;
                curr_sum += bar.high - bar.low;
            }
            results.emplace_back(curr_sum);
        }
    }
    return results;
}

// 2a - Need to handle these constants and maybe find them on Bloomberf or in the files he gave
const double POINT_VALUE = 0.;
const double SLPG = 70.;

std::vector<Bar> readData(const std::string& filename) {
    std::vector<Bar> data;
    std::ifstream file(filename);
    std::string line;

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        data.emplace_back(line);
    }

    return data;
}

int main() {
    std::string filename = "../data/HO-5minHLV.csv"; // Replace with your actual file name

    std::cout << "Size of param space:" << NUM_CHN_LEN * NUM_STP_PCT << std::endl;

    // Clock
    auto start = std::chrono::high_resolution_clock::now();
    auto bars = readData(filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


    // // Example: print the close prices
    for (const auto& bar : bars) {
        std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;
    }

    // Example: print the close prices
    // for (int i = 0; i < 100; i++) {
    //     std::cout << "Date: " << bars.at(i).timestamp << " Close: " << bars.at(i).close << std::endl;
    // }

    auto t1 = std::chrono::high_resolution_clock::now();
    auto results = pseudoCodeParmaterSearch(bars);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "Data Read in: " << duration << " milliseconds" << std::endl;
    std::cout << "Pseudo Strategy execution in: " << dt << " milliseconds" << std::endl;


    return 0;
}

