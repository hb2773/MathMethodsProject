#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <chrono>

// Steps for the BackTesting Engine. 
// 1 - Read the data 
// 2 - Implement ONE Trend Following Channel With DrawdownControl
// 2a - Handle P&L with Slippage (See TF Data for Slippage in col. V)
// 2b - Create useful metrics (Measure various statistical performance properties of both equity curve and trade-by-trade table: Average Rate of Return, Standard Deviation of Returns, and the Ratio of the two above (known as the Sharpe or Information Ratio), Total Number of Trades, % of Winners or % of Profitable Trades, Return on the Account (Net Profit to Worst Drawdown), Average Winner, Average Loser, Profit Factor, etc. along with all other characteristics that you will deem useful to gaining insight into the behavior of your market. The quantity and choices of the performance characteristics will make a difference on the grade for your Final Project.)
// 3 - Create a loop to optimize as the BackTestEngine
// 4 - Choose another market to run optimization 
// 5 - Run optimization on the whole dataset and compare values 


// 3 - Pseudo code for 3. 
void pseudoCodeParmaterSearch() {
    for (int ChnLen = 500; ChnLen < 10000; ChnLen++) {
        for (double StpPct = 0.005; StpPct <= 0.10; StpPct += 0.001) {
            std::cout << "Run Strategy" << std::endl;
        }
    }
}

class BackTestEngine {
    // This class should be able to read the data and run a backtest on windows of parameters and output 
    // files similar to HO Optimization.xlsx for instance
    // Will have to contain several metrics
};

// 2a - Need to handle these constants and maybe find them on Bloomberf or in the files he gave
const double POINT_VALUE = 0.;
const double SLPG = 1.;

struct Bar {
    std::string date;
    std::string time;
    double open;
    double high;
    double low;
    double close;
    int volume;

    // Constructor to initialize the structure from a CSV line
    Bar(const std::string& csvLine) {
        std::istringstream stream(csvLine);
        std::string token;

        std::getline(stream, date, ',');
        std::getline(stream, time, ',');
        std::getline(stream, token, ','); open = std::stod(token);
        std::getline(stream, token, ','); high = std::stod(token);
        std::getline(stream, token, ','); low = std::stod(token);
        std::getline(stream, token, ','); close = std::stod(token);
        std::getline(stream, token, ','); volume = std::stoi(token);
    }
};

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

    // Clock
    auto start = std::chrono::high_resolution_clock::now();
    auto bars = readData(filename);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();


    // Example: print the close prices
    for (const auto& bar : bars) {
        std::cout << "Close: " << bar.close << std::endl;
    }

    pseudoCodeParmaterSearch();

    std::cout << "Data Read in: " << duration << " milliseconds" << std::endl;


    return 0;
}

