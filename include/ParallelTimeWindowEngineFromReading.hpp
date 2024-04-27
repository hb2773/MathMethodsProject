#ifndef TIMEWINDOWENGINE_H
#define TIMEWINDOWENGINE_H

#include <vector>
#include <filesystem>

#include "BackTestEngine.hpp"
#include "Bar.hpp"
#include "DateEncoder.hpp"

namespace fs = std::filesystem;

class ParallelTimeWindowEngineFromReading {
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
        const std::vector<int> out_sample_lengths_in_month = {1, 2, 3, 6}
        ) 
    {
        unsigned long long in_sample_start = start_date;
        unsigned long long in_sample_end;
        unsigned long long out_sample_start;
        unsigned long long out_sample_end;
        unsigned long long out_sample_end_max;

        int execution_counter = 0;

        for (const auto out_sample_length_in_month : out_sample_lengths_in_month) {

            std::string inputFolder = "../" + asset + "_INSAMP_" + std::to_string(in_sample_length_in_year) + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/";

            std::string inSampleInputFolder = inputFolder + "insample/";
            std::string outSampleOutputFolder = inputFolder + "outsample_in/";

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

            // Strategy execution here
            std::string outputFolder = "../" + asset + "_INSAMP_" + std::to_string(in_sample_length_in_year);
            auto [optiChn, optiStp] = // readfromfile;

            std::vector<int> out_sample_ends(out_sample_lengths_in_month.size());
            std::transform(
                out_sample_lengths_in_month.begin(), 
                out_sample_lengths_in_month.end(), 
                std::back_inserter(out_sample_ends), 
                [in_sample_end](int months) {
                    return incrementDate(in_sample_end, 0, months);
                });
            
            for (const auto out_sample_length_in_month : out_sample_lengths_in_month) {

                if (execution_counter % out_sample_length_in_month == 0) {

                    out_sample_end = incrementDate(out_sample_start, 0, out_sample_length_in_month);
                    out_sample_end_max = std::max(out_sample_end, out_sample_end_max);

                    std::string outputFolder = "../" + asset + "_INSAMP_" + std::to_string(in_sample_length_in_year) + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/";
                    std::string outSampleOutputFolder = outputFolder + "outsample/";

                    auto outSampleFileName = outSampleOutputFolder + "OUTSAMPLE_START_" + std::to_string(out_sample_start) + "_OUTSAMPLE_END_" + std::to_string(out_sample_end) + ".csv";

                    // std::cout << outSampleFileName << std::endl;

                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, optiChn, optiStp);
                    int k = (int) (optiChn - ChnLenMin) / ChnLenStep;
                    std::vector<float> HHs = read_kth_vector(HHFilename_, bars.size(), k);
                    std::vector<float> LLs = read_kth_vector(LLFilename_, bars.size(), k);
                    StrategyEngine::run(strat, bars, HHs, LLs, out_sample_start, out_sample_end, true, outSampleFileName);
                }

            }
            // Time Increment
            in_sample_start = incrementDate(in_sample_start, 0, 1);
            std::cout << execution_counter << std::endl;
            execution_counter += 1;
        } 
        while (out_sample_end_max < end_date);
    }
};

#endif