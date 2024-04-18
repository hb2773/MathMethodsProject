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
    std::string filename = "../data/HO-5minHLV.csv"; // Replace with your actual file name

    std::cout << "Size of param space:" << NUM_CHN_LEN * NUM_STP_PCT << std::endl;

    // Reading the Data
    auto start = std::chrono::high_resolution_clock::now();
    auto bars = readData(filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::map<unsigned long long, int> dates_to_indices;
    std::vector<unsigned long long> dates;
    
    for (int i = 0; i < bars.size(); i++) {
        dates_to_indices[bars.at(i).timestamp] = i;
        dates.push_back(bars.at(i).timestamp);
    }

    // Constructing some other necessary Data

    ChannelBreakout strat = ChannelBreakout(10000, 0.015);

    // // Example: print the close prices
    unsigned long long start_date = 1985'1002'0000;
    unsigned long long end_date   = 2023'0221'0000;
    int counter = 0;
    for (const auto& bar : bars) {
        if (counter < BARS_BACK || bar.timestamp < start_date) {
            counter++;
            continue;
        } else if (bar.timestamp < end_date) {
            strat.update(bar, counter);
            // TODO: RECORD STRATEGY
        } else {
            std::cout << "EQUITY       " << strat.equity - INIT_EQUITY << std::endl;
            std::cout << "MAX DRAWDOWN " << strat.maxDrawdown          << std::endl;
            std::cout << "NUM TRADES   " << strat.numTrades            << std::endl;
            std::cout << "EQUITY MAX   " << strat.equityMax            << std::endl;
            // TODO: OUTPUT IN A CSV FILE THE RESULTS OF THE STRATEGY
            break;
        }
        counter++;
    }

    // Example: print the close prices
    // for (int i = 0; i < 100; i++) {
    //     std::cout << "Date: " << bars.at(i).timestamp << " Close: " << bars.at(i).close << std::endl;
    // }

    auto t1 = std::chrono::high_resolution_clock::now();
    // auto results = pseudoCodeParmaterSearch(bars);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "Data Read in: " << duration << " milliseconds" << std::endl;
    std::cout << "Pseudo Strategy execution in: " << dt << " milliseconds" << std::endl;


    return 0;
}

