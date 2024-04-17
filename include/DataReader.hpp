#ifndef DATA_READER_H
#define DATA_READER_H

#include <vector>
#include "Bar.hpp"
#include <fstream>
#include <sstream>

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

#endif