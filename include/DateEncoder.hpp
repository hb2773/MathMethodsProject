#ifndef DATAENCODER_H
#define DATAENCODER_H

#include <iostream>
#include <sstream>
#include <iomanip>

unsigned long long encodeDateTime(const std::string& date, const std::string& time) {
    // unsigned long long integer YYYYMMDDHHmm
    std::tm tm = {};
    std::stringstream ss(date + " " + time);
    ss >> std::get_time(&tm, "%m/%d/%Y %H:%M");

    unsigned long long encodedDate = (tm.tm_year + 1900ULL) * 10000ULL + (tm.tm_mon + 1) * 100ULL + tm.tm_mday;
    unsigned long long encodedTime = tm.tm_hour * 100ULL + tm.tm_min;
    unsigned long long test = encodedDate * 10000ULL + encodedTime; 
    return test;
}

#endif