#include <vector>

#include "Constants.hpp"
#include "Bar.hpp"
#include "Strategy.hpp"

// 3 - Pseudo code for 3. 
// use threading and allow for explicit date start and date end.
// std::vector<double> pseudoCodeParmaterSearch(const std::vector<Bar>& bars, long long int start_date, long long int end_date) {

class BackTestEngine {
    public:
    static std::vector<double> run(
        const std::vector<Bar>& bars,
        const std::string& high_low_file,
        double StpPctMin, 
        double StpPctMax, 
        double StpPctStep, 
        unsigned long long start_date, 
        unsigned long long end_date
        ) 
    {
        std::ifstream file(high_low_file);
        std::string high_line;
        std::string low_line;
        std::vector<double> dummy_results;
        while (std::getline(file, high_line) && std::getline(file, low_line)) {

            std::istringstream high_stream(high_line), low_stream(low_line);
            std::string item;
            std::vector<double> HHs, LLs;
            int ChnLen;
            getline(high_stream, item, ','); getline(high_stream, item, ','); ChnLen = std::stoi(item);
            while (getline(high_stream, item, ',')) {
                HHs.push_back(std::stod(item));
            }
            getline(low_stream,  item, ','); getline(low_stream,  item, ',');
            while (getline(low_stream, item, ',')) {
                LLs.push_back(std::stod(item));
            }

            for (double StpPct = StpPctMin; StpPct <= StpPctMax; StpPct += STP_PCT_STEP) {
                double dummy_sum = 0.;
                ChannelBreakout strat = ChannelBreakout(ChnLen, StpPct);
                StrategyEngine::run(strat, bars, HHs, LLs, start_date, end_date);
            }
        }
        return dummy_results;
    }
};
