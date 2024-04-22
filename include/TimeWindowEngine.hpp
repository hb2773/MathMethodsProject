#include <vector>
#include <filesystem>

#include "BackTestEngine.hpp"
#include "Bar.hpp"
#include "DateEncoder.hpp"

namespace fs = std::filesystem;

class TimeWindowEngine {
    public: 
    static void run(
        const std::string asset,
        const unsigned long long start_date,
        const unsigned long long end_date,
        const int in_sample_length_in_year,
        const int out_sample_length_in_month,
        const double NUM_CONTRACTS,
        const double POINT_VALUE, 
        const double SLPG,
        const std::vector<Bar>& bars,
        const std::vector<double>& highs,
        const std::vector<double>& lows,
        double ChnLenMin,
        double ChnLenMax,
        double ChnLenStep,
        double StpPctMin, 
        double StpPctMax, 
        double StpPctStep 
        ) 
    {
        unsigned long long in_sample_start = start_date;
        unsigned long long in_sample_end;
        unsigned long long out_sample_start;
        unsigned long long out_sample_end;

        std::string outputFolder = "../" + asset + "_INSAMP_" + std::to_string(in_sample_length_in_year) + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/";

        if (!fs::exists(outputFolder)) {
            fs::create_directories(outputFolder);
        }

        do {
            // Time Generation
            in_sample_end = incrementDate(in_sample_start, in_sample_length_in_year, 0);
            out_sample_start = in_sample_end;
            out_sample_end = incrementDate(in_sample_end, 0, out_sample_length_in_month);

            std::cout << "In-sample: " << in_sample_start << ":" << in_sample_end; 
            std::cout << ", Out-sample: " << out_sample_start << ":" << out_sample_end << std::endl;

            // Strategy execution here
            auto [optiChn, optiStp] = BackTestEngine::run(
                NUM_CONTRACTS, POINT_VALUE, SLPG,
                bars, highs, lows,
                ChnLenMin, ChnLenMax, ChnLenStep,
                StpPctMin, StpPctMax, StpPctStep,
                start_date, end_date, 
                false, outputFolder);

            ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, optiChn, optiStp);
            auto HHs = MinMaxSlidingWindow(highs, optiChn, true);
            auto LLs = MinMaxSlidingWindow(lows, optiChn, false);
            StrategyEngine::run(strat, bars, HHs, LLs, out_sample_start, out_sample_end, true);

            // Time Increment
            in_sample_start = incrementDate(in_sample_start, 0, out_sample_length_in_month);
        } 
        while (out_sample_end < end_date);
    }
};