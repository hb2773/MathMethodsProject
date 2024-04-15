#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Steps for the BackTesting Engine. 
// 1 - Read the data 
// 2 - Implement ONE Trend Following Channel With DrawdownControl
// 2a - Handle P&L with Slippage (See TF Data for Slippage in col. V)
// 2b - Create useful metrics (Measure various statistical performance properties of both equity curve and trade-by-trade table: Average Rate of Return, Standard Deviation of Returns, and the Ratio of the two above (known as the Sharpe or Information Ratio), Total Number of Trades, % of Winners or % of Profitable Trades, Return on the Account (Net Profit to Worst Drawdown), Average Winner, Average Loser, Profit Factor, etc. along with all other characteristics that you will deem useful to gaining insight into the behavior of your market. The quantity and choices of the performance characteristics will make a difference on the grade for your Final Project.)
// 3 - Create a loop to optimize 


// 3 - Pseudo code for 3. 
void pseudoCodeParmaterSearch() {
    for (int ChnLen = 500; ChnLen < 10000; ChnLen++) {
        for (double StpPct = 0.005; StpPct <= 0.10; StpPct += 0.001  ) {
            std::cout << "Run Strategy" << std::endl;
        }
    }
}

// 2a - Need to handle this constant
const double SLPG = 1;

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
    auto bars = readData(filename);

    // Example: print the close prices
    for (const auto& bar : bars) {
        std::cout << "Close: " << bar.close << std::endl;
    }

    pseudoCodeParmaterSearch();

    return 0;
}

