#ifndef BAR_H
#define BAR_H

#include <boost/date_time/posix_time/posix_time.hpp>

struct Bar {

    // boost::posix_time::ptime timestamp;
    std::string timestamp = "";  // Default to empty string
    double open = 0.0;           // Default to 0.0
    double high = 0.0;           // Default to 0.0
    double low = 0.0;            // Default to 0.0
    double close = 0.0;          // Default to 0.0
    int volume = 0;              // Default to 0

    Bar() = default;             // Default constructor

    Bar(const std::string& csvLine) {

        std::istringstream stream(csvLine);
        std::string date, time, token;

        std::getline(stream, date, ',');
        std::getline(stream, time, ','); // timestamp = std::move(date + " " + time);
        std::getline(stream, token, ','); open = std::stod(token);
        std::getline(stream, token, ','); high = std::stod(token);
        std::getline(stream, token, ','); low = std::stod(token);
        std::getline(stream, token, ','); close = std::stod(token);
        std::getline(stream, token); volume = std::stoi(token);

        // Combine date and time into a single datetime object
        // timestamp = 
        
        // std::string dateTime = date + " " + time;
        // std::istringstream dateTimeStream(dateTime);
        // dateTimeStream.imbue(std::locale(dateTimeStream.getloc(),
        //     new boost::posix_time::time_input_facet("%m/%d/%Y %H:%M")));

        // dateTimeStream >> timestamp;
    }

};

#endif // MARKETDATA_H
