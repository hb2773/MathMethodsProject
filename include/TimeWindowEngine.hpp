#ifndef TIMEWINDOWENGINE_H
#define TIMEWINDOWENGINE_H

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
        int NumStpPct
        ) 
    {
        unsigned long long in_sample_start = start_date;
        unsigned long long in_sample_end;
        unsigned long long out_sample_start;
        unsigned long long out_sample_end;

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

        do {
            // Time Generation
            in_sample_end = incrementDate(in_sample_start, in_sample_length_in_year, 0);
            out_sample_start = in_sample_end;
            out_sample_end = incrementDate(in_sample_end, 0, out_sample_length_in_month);

            std::cout << "In-sample: " << in_sample_start << ":" << in_sample_end; 
            std::cout << ", Out-sample: " << out_sample_start << ":" << out_sample_end << std::endl;

            auto inSampleFileName =  inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(in_sample_start) + "_INSAMPLE_END_" + std::to_string(in_sample_end) + ".csv";
            auto outSampleFileName = outSampleOutputFolder + "OUTSAMPLE_START_" + std::to_string(out_sample_start) + "_OUTSAMPLE_END_" + std::to_string(out_sample_end) + ".csv";

            // Strategy execution here
            auto [optiChn, optiStp] = BackTestEngine::run(
                NUM_CONTRACTS, POINT_VALUE, SLPG,
                bars, HHFilename_, LLFilename_,
                ChnLenMin, ChnLenMax, ChnLenStep, NumChnLen,
                StpPctMin, StpPctMax, StpPctStep, NumStpPct,
                start_date, end_date, 
                false, inSampleFileName);

            ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, optiChn, optiStp);
            int k = (int) (optiChn - ChnLenMin) / ChnLenStep;
            std::vector<float> HHs = read_kth_vector(HHFilename_, bars.size(), k);
            std::vector<float> LLs = read_kth_vector(LLFilename_, bars.size(), k);
            StrategyEngine::run(strat, bars, HHs, LLs, out_sample_start, out_sample_end, true, outSampleFileName);

            // Time Increment
            in_sample_start = incrementDate(in_sample_start, 0, out_sample_length_in_month);
        } 
        while (out_sample_end < end_date);
    }

    static void run(
        const std::string asset,
        const unsigned long long start_date,
        const unsigned long long end_date,
        const int in_sample_length_in_year,
        const int out_sample_length_in_month,
        const float NUM_CONTRACTS,
        const float POINT_VALUE, 
        const float SLPG,
        const std::vector<Bar>& bars,
        const std::vector<float>& highs,
        const std::vector<float>& lows,
        float ChnLenMin,
        float ChnLenMax,
        float ChnLenStep,
        int NumChnLen,
        float StpPctMin, 
        float StpPctMax, 
        float StpPctStep, 
        int NumStpPct
        ) 
    {
        unsigned long long in_sample_start = start_date;
        unsigned long long in_sample_end;
        unsigned long long out_sample_start;
        unsigned long long out_sample_end;

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

        do {
            // Time Generation
            in_sample_end = incrementDate(in_sample_start, in_sample_length_in_year, 0);
            out_sample_start = in_sample_end;
            out_sample_end = incrementDate(in_sample_end, 0, out_sample_length_in_month);

            std::cout << "In-sample: " << in_sample_start << ":" << in_sample_end; 
            std::cout << ", Out-sample: " << out_sample_start << ":" << out_sample_end << std::endl;

            auto inSampleFileName =  inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(in_sample_start) + "_INSAMPLE_END_" + std::to_string(in_sample_end) + ".csv";
            auto outSampleFileName = outSampleOutputFolder + "OUTSAMPLE_START_" + std::to_string(out_sample_start) + "_OUTSAMPLE_END_" + std::to_string(out_sample_end) + ".csv";

            // Strategy execution here
            auto [optiChn, optiStp] = BackTestEngine::run(
                NUM_CONTRACTS, POINT_VALUE, SLPG,
                bars, highs, lows,
                ChnLenMin, ChnLenMax, ChnLenStep, NumChnLen,
                StpPctMin, StpPctMax, StpPctStep, NumStpPct,
                start_date, end_date, 
                false, inSampleFileName);

            ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, optiChn, optiStp);
            auto HHs = MinMaxSlidingWindow(highs, optiChn, true);
            auto LLs = MinMaxSlidingWindow(lows, optiChn, false);
            StrategyEngine::run(strat, bars, HHs, LLs, out_sample_start, out_sample_end, true, outSampleFileName);

            // Time Increment
            in_sample_start = incrementDate(in_sample_start, 0, out_sample_length_in_month);
        } 
        while (out_sample_end < end_date);
    }
};

#endif