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

#include "BackTestEngine.hpp"
#include "DataReader.hpp"
#include "Bar.hpp"
#include "Constants.hpp"
#include "Strategy.hpp"
#include "utils.hpp"

// Steps for the BackTesting Engine. 
// 1 - Read the data OK
// 2 - Implement ONE Trend Following Channel (With DrawdownControl)
// 2a - Handle P&L with Slippage (See TF Data for Slippage in col. V)
// 2b - Create useful metrics (Measure various statistical performance properties of both equity curve and trade-by-trade table: Average Rate of Return, Standard Deviation of Returns, and the Ratio of the two above (known as the Sharpe or Information Ratio), Total Number of Trades, % of Winners or % of Profitable Trades, Return on the Account (Net Profit to Worst Drawdown), Average Winner, Average Loser, Profit Factor, etc. along with all other characteristics that you will deem useful to gaining insight into the behavior of your market. The quantity and choices of the performance characteristics will make a difference on the grade for your Final Project.)
// 3 - Create a loop to optimize as the BackTestEngine
// 4 - Choose another market to run optimization 
// 5 - Run optimization on the whole dataset and compare values 
// 6 - Handle output of optimization


int main() {

    std::string filename = "../data/" + ASSET + ".csv"; // Replace with your actual file name
    std::string high_low_file = "../data/" + ASSET + "_high_low.csv";

    std::cout << "Size of param space:" << NUM_CHN_LEN * NUM_STP_PCT << std::endl;

    // Reading the Data
    auto start = std::chrono::high_resolution_clock::now();
    auto bars = readData(filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::map<unsigned long long, int> dates_to_indices;
    std::vector<unsigned long long> dates;
    std::vector<double> highs;
    std::vector<double> lows;

    dates.reserve(SIZE);
    highs.reserve(SIZE);
    lows.reserve(SIZE);
    
    for (int i = 0; i < bars.size(); i++) {
        dates_to_indices[bars.at(i).timestamp] = i;
        dates.push_back(bars.at(i).timestamp);
        highs.push_back(bars.at(i).high);
        lows.push_back(bars.at(i).low);
    }

    // Constructing the HIGH and LOW FILES
    // constructHHLLFile(high_low_file, 9900, 10000, 10, highs, lows);
    constructHHLLFile(high_low_file, 100, 200, 10, highs, lows);

    // Reading the HIGH files
    // Reading the LOW  files

    // Constructing some other necessary Data

    // unsigned long long start_date = 2007'1002'0000;
    // unsigned long long end_date   = 2023'0221'0000;

    // double chnLen = 10000;

    // auto HHs = MinMaxSlidingWindow(highs, chnLen, true);
    // auto LLs = MinMaxSlidingWindow(lows,  chnLen, false);

    // ChannelBreakout strat1 = ChannelBreakout(chnLen, 0.015);
    // ChannelBreakout strat2 = ChannelBreakout(chnLen, 0.016);
    // ChannelBreakout strat3 = ChannelBreakout(chnLen, 0.017);
    // ChannelBreakout strat4 = ChannelBreakout(chnLen, 0.017);

    // auto t1 = std::chrono::high_resolution_clock::now();
    // StrategyEngine::run(strat1, bars, HHs, LLs, start_date, end_date);
    // StrategyEngine::run(strat2, bars, HHs, LLs, start_date, end_date);
    // StrategyEngine::run(strat3, bars, HHs, LLs, start_date, end_date);
    // StrategyEngine::run(strat4, bars, HHs, LLs, start_date, end_date);
    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    // // Example: print the close prices

    // Example: print the close prices
    // for (int i = 0; i < 100; i++) {
    //     std::cout << "Date: " << bars.at(i).timestamp << " Close: " << bars.at(i).close << std::endl;
    // }

    // auto t1 = std::chrono::high_resolution_clock::now();
    // auto results = pseudoCodeParmaterSearch(bars);
    //auto t2 = std::chrono::high_resolution_clock::now();

    // auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "Data Read in: " << duration << " milliseconds" << std::endl;
    // std::cout << "Pseudo Strategy execution in: " << dt << " milliseconds" << std::endl;


    return 0;
}

