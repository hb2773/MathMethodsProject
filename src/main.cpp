#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Steps for the BackTesting Engine. 
// 1 - Read the data 
// 2 - Implement ONE Trend Following Channel With DrawdownControl
// 2 - Create a loop with 
// 3 - I
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
    std::string filename = "../HO-5minHLV.csv"; // Replace with your actual file name
    auto bars = readData(filename);

    // Example: print the close prices
    for (const auto& bar : bars) {
        std::cout << "Close: " << bar.close << std::endl;
    }

    return 0;
}

