#include <vector>

#include "constants.hpp"
#include "Bar.hpp"

// 3 - Pseudo code for 3. 
// use threading and allow for explicit date start and date end.
// std::vector<double> pseudoCodeParmaterSearch(const std::vector<Bar>& bars, long long int start_date, long long int end_date) {
std::vector<double> pseudoCodeParmaterSearch(const std::vector<Bar>& bars) {
    std::vector<double> results(NUM_CHN_LEN * NUM_STP_PCT);
    for (int ChnLen = CHN_LEN_MIN; ChnLen < CHN_LEN_MAX; ChnLen += CHN_LEN_STEP) {
        for (double StpPct = STP_PCT_MIN; StpPct <= STP_PCT_MAX; StpPct += STP_PCT_STEP) {
            double curr_sum = 0.;
            // while not start_date
            for (const auto& bar : bars) {
                // std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;
                curr_sum += bar.high - bar.low;
                // if (bar.timestamp > end_date) {
                //     break
                // }
            }
            results.emplace_back(curr_sum);
        }
    }
    return results;
}