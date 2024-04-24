#ifndef DATAENCODER_H
#define DATAENCODER_H

#include <tuple>
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

int getYear(unsigned long long date) {
    return date / 1'00'00'00'00;
}

int getMonth(unsigned long long date) {
    return (date / 1'00'00'00) % 1'00;
}

int getDay(unsigned long long date) {
    return (date / 1'00'00) % 1'00;
}

int getHour(unsigned long long date) {
    return (date / 1'00) % 1'00;
}

int getMinute(unsigned long long date) {
    return date % 1'00;
}

std::tuple<int, int, int, int, int>seperateDate(unsigned long long date) {
    return {getYear(date), getMonth(date), getDay(date), getHour(date), getMinute(date)};
}

unsigned long long incrementDate(unsigned long long date, int addYears, int addMonths) {

    auto [year, month, day, hour, minute] = seperateDate(date);

    year += addYears;
    int month_temp = month + addMonths;
    year += (month_temp - 1) / 12; 
    month = (month_temp - 1) % 12 + 1; 

    unsigned long long newDate = year * 1'00'00'00'00ULL + month * 1'00'00'00 + day * 1'00'00 + hour * 1'00 + minute;
    return newDate;
}



#endif