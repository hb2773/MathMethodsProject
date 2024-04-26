#ifndef TIMEWINDOWENGINE_H
#define TIMEWINDOWENGINE_H

#include <vector>
#include <filesystem>

#include "BackTestEngine.hpp"
#include "Bar.hpp"
#include "DateEncoder.hpp"

namespace fs = std::filesystem;

class ParallelTimeWindowEngine {
    public: 
    static void run(
        const std::string asset,
        const unsigned long long start_date,
        const unsigned long long end_date,
        const int in_sample_length_in_year,
        const float NUM_CONTRACTS,
        const float POINT_VALUE, 
        const float SLPG,
        const std::vector<Bar>& bars,
        const char* HHFilename_,
        const char* LLFilename_,
        float ChnLenMin,
        float ChnLenMax,
        float ChnLenStep,
        int NumChnLen,
        float StpPctMin, 
        float StpPctMax, 
        float StpPctStep, 
        int NumStpPct,
        const std::vector<int> out_sample_lengths_in_month = {1, 2, 3, 4, 5, 6}
        ) 
    {
        unsigned long long in_sample_start = start_date;
        unsigned long long in_sample_end;
        unsigned long long out_sample_start;
        unsigned long long out_sample_end;
        unsigned long long out_sample_end_max;

        int execution_counter = 0;

        for (const auto out_sample_length_in_month : out_sample_lengths_in_month) {

            std::string outputFolder = "../" + asset + "_INSAMP_" + std::to_string(in_sample_length_in_year) + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/";

            std::string inSampleOutputFolder = outputFolder + "insample/";
            std::string outSampleOutputFolder = outputFolder + "outsample/";

            if (!fs::exists(outputFolder)) {
                fs::create_directories(outputFolder);
            }

            if (!fs::exists(inSampleOutputFolder)) {
                fs::create_directories(inSampleOutputFolder);
            }

            if (!fs::exists(outSampleOutputFolder)) {
                fs::create_directories(outSampleOutputFolder);
            }
        }

        do {
            // Time Generation

            // IN SAMPLE
            in_sample_end = incrementDate(in_sample_start, in_sample_length_in_year, 0);
            out_sample_start = in_sample_end;

            std::cout << "In-sample: " << in_sample_start << ":" << in_sample_end; 
            std::cout << ", Out-sample: " << out_sample_start << ":" << out_sample_end << std::endl;

            auto inSampleFileName =  inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(in_sample_start) + "_INSAMPLE_END_" + std::to_string(in_sample_end) + ".csv";

            // Strategy execution here
            auto [optiChn, optiStp] = BackTestEngine::run(
                NUM_CONTRACTS, POINT_VALUE, SLPG,
                bars, HHFilename_, LLFilename_,
                ChnLenMin, ChnLenMax, ChnLenStep, NumChnLen,
                StpPctMin, StpPctMax, StpPctStep, NumStpPct,
                in_sample_start, in_sample_end, 
                false, inSampleFileName);

            out_sample_end_max = std::max(out_sample_lengths_in_month);

            std::vector<int> out_sample_ends(out_sample_lengths_in_month.size());
            out_sample_ends = std::transform(
                out_sample_lengths_in_month.begin(), 
                out_sample_lengths_in_month.end(), 
                out_sample_end_max.begin(), 
                [in_sample_end](int months) {
                    return incrementDate(in_sample_end, 0, months);
                });
            
            for (const auto out_sample_length_in_month : out_sample_lengths_in_month) {

                if (execution_counter % out_sample_length_in_month == 0) {

                    out_sample_end = incrementDate(out_sample_start, 0, out_sample_length_in_month);

                    std::string outputFolder = "../" + asset + "_INSAMP_" + std::to_string(in_sample_length_in_year) + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/";
                    std::string outSampleOutputFolder = outputFolder + "outsample/";

                    auto outSampleFileName = outSampleOutputFolder + "OUTSAMPLE_START_" + std::to_string(out_sample_start) + "_OUTSAMPLE_END_" + std::to_string(out_sample_end) + ".csv";

                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, optiChn, optiStp);
                    int k = (int) (optiChn - ChnLenMin) / ChnLenStep;
                    std::vector<float> HHs = read_kth_vector(HHFilename_, bars.size(), k);
                    std::vector<float> LLs = read_kth_vector(LLFilename_, bars.size(), k);
                    StrategyEngine::run(strat, bars, HHs, LLs, out_sample_start, out_sample_end, true, outSampleFileName);
                }

            }
            // Time Increment
            in_sample_start = incrementDate(in_sample_start, 0, 1);
            execution_counter += 1;
        } 
        while (out_sample_end_max < end_date);
    }
};

#endif