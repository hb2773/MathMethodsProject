#ifndef BAR_H
#define BAR_H

#include "DateEncoder.hpp"

struct Bar {

    unsigned long long timestamp = 0; 
    float open = 0.f;           // Default to 0.0
    float high = 0.f;           // Default to 0.0
    float low = 0.f;            // Default to 0.0
    float close = 0.f;          // Default to 0.0
    int volume = 0;              // Default to 0

    Bar() = default;             // Default constructor

    Bar(const std::string& csvLine) {

        std::istringstream stream(csvLine);
        std::string date, time, token;

        std::getline(stream, date, ',');
        std::getline(stream, time, ','); timestamp = encodeDateTime(date, time);
        std::getline(stream, token, ','); open = std::stod(token);
        std::getline(stream, token, ','); high = std::stod(token);
        std::getline(stream, token, ','); low = std::stod(token);
        std::getline(stream, token, ','); close = std::stod(token);
        std::getline(stream, token); volume = std::stoi(token);
    }

};

#endif // MARKETDATA_H
