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
#include <memory>

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
// 7 - Propose a different way than full search optimization.
// 8 - Propose other metrics


int main() {

    // ASSET /////////////////////////////////////////////////////
    const std::string ASSET = "HO-5minHLV";
    // SIZE 
    constexpr int SIZE = 611'839;

    const float NUM_CONTRACTS = 1.f; 
    const float POINT_VALUE = 64'000.f;
    const float SLPG = 65.f;


    std::string filename = "../data/" + ASSET + ".csv"; // Replace with your actual file name
    
    std::string HHFilename = "../data/" + ASSET + "_HH_data.dat";
    std::string LLFilename = "../data/" + ASSET + "_LL_data.dat";

    const char* HHFilename_ = HHFilename.data();
    const char* LLFilename_ = LLFilename.data(); 

    constexpr int CHN_LEN_MIN = 10000; // 500
    constexpr int CHN_LEN_MAX = 10000; // 10000
    constexpr int CHN_LEN_STEP = 1000; // 10

    constexpr float STP_PCT_MIN = 0.015f; // 0.005
    constexpr float STP_PCT_MAX = 0.018f;  // 0.10
    constexpr float STP_PCT_STEP = 0.001f; // 0.001

    const int NUM_CHN_LEN = static_cast<int>(std::ceil((CHN_LEN_MAX - CHN_LEN_MIN + CHN_LEN_STEP) / CHN_LEN_STEP));
    const int NUM_STP_PCT = static_cast<int>(std::ceil((STP_PCT_MAX - STP_PCT_MIN + STP_PCT_STEP) / STP_PCT_STEP));

    std::size_t fileSize = (SIZE + 2) * (NUM_CHN_LEN + 2) * sizeof(float); // 64 MB for example

    std::cout << "NUM_CHN_LEN: " << NUM_CHN_LEN << std::endl;
    std::cout << "NUM_STP_PCT: " << NUM_STP_PCT << std::endl;
    std::cout << "Size of param space: " << NUM_CHN_LEN * NUM_STP_PCT << std::endl;
    // ASSET /////////////////////////////////////////////////////

    //std::map<unsigned long long, int> dates_to_indices;
    // std::vector<unsigned long long> dates;

    // dates_to_indices[bars.at(i).timestamp] = i;
    // dates.push_back(bars.at(i).timestamp);

    // Reading the Data
    auto start = std::chrono::high_resolution_clock::now();
    auto bars = readData(filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::vector<float> highs;
    std::vector<float> lows;

    highs.reserve(SIZE);
    lows.reserve(SIZE);
    
    for (int i = 0; i < bars.size(); i++) {
        highs.push_back(bars.at(i).high);
        lows.push_back(bars.at(i).low);
    }

    // std::vector<std::vector<float>> BigHHS(NUM_CHN_LEN, std::vector<float>(SIZE));
    // std::vector<std::vector<float>> BigLLS(NUM_CHN_LEN, std::vector<float>(SIZE));

    // std::unique_ptr<std::array<float, (size_t) NUM_CHN_LEN>> myArray = std::make_unique<std::array<float, NUM_CHN_LEN>>();

    std::vector<std::pair<float, float>> HHs_LLs;
    HHs_LLs.resize(SIZE * NUM_CHN_LEN);

    for (int i = 0; i < NUM_CHN_LEN; i++) {
        float chnLen = CHN_LEN_MIN + CHN_LEN_STEP * i;
        auto HHs = MinMaxSlidingWindow(highs, chnLen, true);
        auto LLs = MinMaxSlidingWindow(lows, chnLen, false);
        for (int j = 0; j < bars.size(); j++) {
            HHs_LLs.push_back({HHs[j], LLs[j]});
        }
    }
    std::cout << HHs_LLs.size() << std::endl;
    std::cout << bars.size() * NUM_CHN_LEN << std::endl;

    // Need to do a for auto start_date
    unsigned long long start_date = 2007'1002'0000;
    unsigned long long end_date   = 2017'1001'0000;
    bool recordStrat = true;

    auto t1 = std::chrono::high_resolution_clock::now();
    BackTestEngine::run(
        NUM_CONTRACTS, POINT_VALUE, SLPG, 
        bars, HHs_LLs, 
        CHN_LEN_MIN, CHN_LEN_MAX, CHN_LEN_STEP, NUM_CHN_LEN,
        STP_PCT_MIN, STP_PCT_MAX, STP_PCT_STEP, NUM_STP_PCT,
        start_date, end_date,
        recordStrat);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

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
    std::cout << "Back Test Strategy for: " << NUM_CHN_LEN * NUM_STP_PCT << " params" << std::endl;
    std::cout << "Execution in: " << dt << " milliseconds" << std::endl;

    return 0;
}

