#ifndef BACKTESTENGINE_H
#define BACKTESTENGINE_H

#include <vector>
#include <thread>
#include <mutex>
#include <utility>
#include <limits>
#include <cmath>

#include "Constants.hpp"
#include "Bar.hpp"
#include "Strategy.hpp"
#include "DataReader.hpp"
#include "utils.hpp"


// 3 - Pseudo code for 3. 
// use threading and allow for explicit date start and date end.
// std::vector<float> pseudoCodeParmaterSearch(const std::vector<Bar>& bars, long long int start_date, long long int end_date) {


class BackTestEngine {
    public:
    static std::pair<float,float> run(
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
        unsigned long long start_date, 
        unsigned long long end_date,
        bool recordStrat = false,
        std::string fileName = "../output/results.csv", 
        std::vector<int> out_sample_lengths_in_month = {}, 
        int execution_counter = -1) 
    {
        std::mutex mtx;
        std::vector<std::vector<float>> results;

        const int num_threads = 4;
        std::vector<std::thread> threads;
        int workload_per_thread = NumChnLen / num_threads;

        auto processRange = [&](int startRange, int endRange) {
            for (int i = startRange; i < endRange; ++i) {
                float ChnLen = ChnLenMin + i * ChnLenStep;
                int k = (int) (ChnLen - ChnLenMin) / ChnLenStep;
                std::vector<float> HHs = read_kth_vector(HHFilename_, bars.size(), k);
                std::vector<float> LLs = read_kth_vector(LLFilename_, bars.size(), k);
                for (float StpPct = StpPctMin; StpPct < StpPctMax + StpPctStep; StpPct += StpPctStep) {
                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, ChnLen, StpPct);
                    StrategyEngine::run(strat, bars, HHs, LLs, start_date, end_date, recordStrat);
                    recordStrategy(strat, results, mtx);
                }
            }
        };
        
        // Distribute the workload to each thread
        for (int i = 0; i < num_threads; ++i) {
            int startRange = i * workload_per_thread;
            int endRange = (i + 1 == num_threads) ? NumChnLen : startRange + workload_per_thread;
            threads.emplace_back(processRange, startRange, endRange);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        if (out_sample_lengths_in_month.size() == 0) {
            writeResultsToCSV(fileName, results);
        } else {
            for (auto out_sample_length_in_month : out_sample_lengths_in_month) {
                if (execution_counter % out_sample_length_in_month == 0) {
                    std::string inSampleOutputFolder = fileName + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/" + "insample/";
                    auto inSampleOutputFilename = inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(start_date) + "_INSAMPLE_END_" + std::to_string(end_date) + ".csv";
                    writeResultsToCSV(inSampleOutputFilename, results);
                }
            }
        }

        float optiChn;
        float optiStp;
        float maxNPWDD = std::numeric_limits<float>::lowest();
        for (auto& result: results) {
            auto NPWDD = result.at(3) / std::abs(result.at(5));
            if (NPWDD > maxNPWDD) {
                maxNPWDD = NPWDD;
                optiChn = result.at(0);
                optiStp = result.at(1);
            }
        }
        std::cout << "Optimal Parameters Choosen for the in-sample dates: " << start_date << " - " << end_date << std::endl;
        std::cout << "CHANNEL LENGTH: " << optiChn << " STP PCT: " << optiStp << std::endl;
        std::cout << std::endl;
        return {optiChn, optiStp};

    }

    static std::pair<float,float> run2(
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
        float tolerance,
        unsigned long long start_date, 
        unsigned long long end_date,
        bool recordStrat = false,
        std::string fileName = "../output/results.csv", 
        std::vector<int> out_sample_lengths_in_month = {}, 
        int execution_counter = -1) 
    {
        std::mutex mtx;
        std::vector<std::vector<float>> results;

        const int num_threads = 4;
        std::vector<std::thread> threads;
        int workload_per_thread = NumChnLen / num_threads;

        auto processRange = [&](int startRange, int endRange) {
            for (int i = startRange; i < endRange; ++i) {
                float ChnLen = ChnLenMin + i * ChnLenStep;
                int k = (int) (ChnLen - ChnLenMin) / ChnLenStep;
                std::vector<float> HHs = read_kth_vector(HHFilename_, bars.size(), k);
                std::vector<float> LLs = read_kth_vector(LLFilename_, bars.size(), k);
                for (float StpPct = StpPctMin; StpPct < StpPctMax + StpPctStep; StpPct += StpPctStep) {
                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, ChnLen, StpPct, tolerance);
                    StrategyEngine::run2(strat, bars, HHs, LLs, start_date, end_date, recordStrat);
                    recordStrategy(strat, results, mtx);
                }
            }
        };
        
        // Distribute the workload to each thread
        for (int i = 0; i < num_threads; ++i) {
            int startRange = i * workload_per_thread;
            int endRange = (i + 1 == num_threads) ? NumChnLen : startRange + workload_per_thread;
            threads.emplace_back(processRange, startRange, endRange);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        if (out_sample_lengths_in_month.size() == 0) {
            writeResultsToCSV(fileName, results);
        } else {
            for (auto out_sample_length_in_month : out_sample_lengths_in_month) {
                if (execution_counter % out_sample_length_in_month == 0) {
                    std::string inSampleOutputFolder = fileName + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/" + "insample/";
                    auto inSampleOutputFilename = inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(start_date) + "_INSAMPLE_END_" + std::to_string(end_date) + ".csv";
                    writeResultsToCSV(inSampleOutputFilename, results);
                }
            }
        }

        float optiChn;
        float optiStp;
        float maxNPWDD = std::numeric_limits<float>::lowest();
        for (auto& result: results) {
            auto NPWDD = result.at(3) / std::abs(result.at(5));
            if (NPWDD > maxNPWDD) {
                maxNPWDD = NPWDD;
                optiChn = result.at(0);
                optiStp = result.at(1);
            }
        }
        std::cout << "Optimal Parameters Choosen for the in-sample dates: " << start_date << " - " << end_date << std::endl;
        std::cout << "CHANNEL LENGTH: " << optiChn << " STP PCT: " << optiStp << std::endl;
        std::cout << std::endl;
        return {optiChn, optiStp};

    }

    static std::pair<float,float> run(
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
        int NumStpPct,
        unsigned long long start_date, 
        unsigned long long end_date,
        bool recordStrat = false,
        std::string fileName = "../output/results.csv", 
        std::vector<int> out_sample_lengths_in_month = {}) 
    {
        std::mutex mtx;
        std::vector<std::vector<float>> results;

        const int num_threads = 4;
        std::vector<std::thread> threads;
        int workload_per_thread = NumChnLen / num_threads;

        auto processRange = [&](int startRange, int endRange) {
            for (int i = startRange; i < endRange; ++i) {
                float ChnLen = ChnLenMin + i * ChnLenStep;
                auto HHs = MinMaxSlidingWindow(highs, ChnLen, true);
                auto LLs = MinMaxSlidingWindow(lows, ChnLen, false);
                for (float StpPct = StpPctMin; StpPct < StpPctMax + StpPctStep; StpPct += StpPctStep) {
                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, ChnLen, StpPct);
                    StrategyEngine::run(strat, bars, HHs, LLs, start_date, end_date, recordStrat);
                    recordStrategy(strat, results, mtx);
                }
            }
        };
        
        // Distribute the workload to each thread
        for (int i = 0; i < num_threads; ++i) {
            int startRange = i * workload_per_thread;
            int endRange = (i + 1 == num_threads) ? NumChnLen : startRange + workload_per_thread;
            threads.emplace_back(processRange, startRange, endRange);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        if (out_sample_lengths_in_month.size() == 0) {
            writeResultsToCSV(fileName, results);
        } else {
            for (auto out_sample_length_in_month : out_sample_lengths_in_month) {
                std::string inSampleOutputFolder = fileName + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/" + "insample/";
                auto inSampleOutputFilename = inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(start_date) + "_INSAMPLE_END_" + std::to_string(end_date) + ".csv";
                writeResultsToCSV(inSampleOutputFilename, results);
            }
        }


        float optiChn;
        float optiStp;
        float maxNPWDD = std::numeric_limits<float>::lowest();
        for (auto& result: results) {
            auto NPWDD = result.at(3) / std::abs(result.at(5));
            if (NPWDD > maxNPWDD) {
                maxNPWDD = NPWDD;
                optiChn = result.at(0);
                optiStp = result.at(1);
            }
        }
        std::cout << "Optimal Parameters Choosen for the in-sample dates: " << start_date << " - " << end_date << std::endl;
        std::cout << "CHANNEL LENGTH: " << optiChn << " STP PCT: " << optiStp << std::endl;
        std::cout << std::endl;
        return {optiChn, optiStp};
    }

    static std::pair<float,float> run(
        const float NUM_CONTRACTS,
        const float POINT_VALUE, 
        const float SLPG,
        const std::vector<Bar>& bars,
        const std::vector<std::vector<float>>& BigHHS,
        const std::vector<std::vector<float>>& BigLLS,
        float ChnLenMin,
        float ChnLenMax,
        float ChnLenStep,
        int NumChnLen,
        float StpPctMin, 
        float StpPctMax, 
        float StpPctStep, 
        int NumStpPct,
        unsigned long long start_date, 
        unsigned long long end_date,
        bool recordStrat = false,
        std::string fileName = "../output/results.csv", 
        std::vector<int> out_sample_lengths_in_month = {}) 
    {
        std::mutex mtx;
        std::vector<std::vector<float>> results;

        const int num_threads = 4;
        std::vector<std::thread> threads;
        int workload_per_thread = NumChnLen / num_threads;

        auto processRange = [&](int startRange, int endRange) {
            for (int i = startRange; i < endRange; ++i) {
                float ChnLen = ChnLenMin + i * ChnLenStep;
                const auto HHs = BigHHS[i];
                const auto LLs = BigLLS[i];
                for (float StpPct = StpPctMin; StpPct < StpPctMax + StpPctStep; StpPct += StpPctStep) {
                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, ChnLen, StpPct);
                    StrategyEngine::run(strat, bars, HHs, LLs, start_date, end_date, recordStrat);
                    recordStrategy(strat, results, mtx);
                }
            }
        };
        
        // Distribute the workload to each thread
        for (int i = 0; i < num_threads; ++i) {
            int startRange = i * workload_per_thread;
            int endRange = (i + 1 == num_threads) ? NumChnLen : startRange + workload_per_thread;
            threads.emplace_back(processRange, startRange, endRange);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        if (out_sample_lengths_in_month.size() == 0) {
            writeResultsToCSV(fileName, results);
        } else {
            for (auto out_sample_length_in_month : out_sample_lengths_in_month) {
                std::string inSampleOutputFolder = fileName + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/" + "insample/";
                auto inSampleOutputFilename = inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(start_date) + "_INSAMPLE_END_" + std::to_string(end_date) + ".csv";
                writeResultsToCSV(inSampleOutputFilename, results);
            }
        }

        float optiChn;
        float optiStp;
        float maxNPWDD = std::numeric_limits<float>::lowest();
        for (auto& result: results) {
            auto NPWDD = result.at(3) / std::abs(result.at(5));
            if (NPWDD > maxNPWDD) {
                maxNPWDD = NPWDD;
                optiChn = result.at(0);
                optiStp = result.at(1);
            }
        }
        std::cout << "Optimal Parameters Choosen for the in-sample dates: " << start_date << " - " << end_date << std::endl;
        std::cout << "CHANNEL LENGTH: " << optiChn << " STP PCT: " << optiStp << std::endl;
        std::cout << std::endl;
        return {optiChn, optiStp};
    }

    static std::pair<float,float> run(
        const float NUM_CONTRACTS,
        const float POINT_VALUE, 
        const float SLPG,
        const std::vector<Bar>& bars,
        const std::vector<std::pair<float, float>>& HHs_LLs,
        float ChnLenMin,
        float ChnLenMax,
        float ChnLenStep,
        int NumChnLen,
        float StpPctMin, 
        float StpPctMax, 
        float StpPctStep, 
        int NumStpPct,
        unsigned long long start_date, 
        unsigned long long end_date,
        bool recordStrat = false,
        std::string fileName = "../output/results.csv", 
        std::vector<int> out_sample_lengths_in_month = {}) 
    {
        std::mutex mtx;
        std::vector<std::vector<float>> results;

        const int num_threads = 4;
        std::vector<std::thread> threads;
        int workload_per_thread = NumChnLen / num_threads;

        auto processRange = [&](int startRange, int endRange) {
            for (int i = startRange; i < endRange; ++i) {
                float ChnLen = ChnLenMin + i * ChnLenStep;
                for (float StpPct = StpPctMin; StpPct < StpPctMax + StpPctStep; StpPct += StpPctStep) {
                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, ChnLen, StpPct);
                    StrategyEngine::run(strat, bars, HHs_LLs, i, start_date, end_date, recordStrat);
                    recordStrategy(strat, results, mtx);
                }
            }
        };
        
        // Distribute the workload to each thread
        for (int i = 0; i < num_threads; ++i) {
            int startRange = i * workload_per_thread;
            int endRange = (i + 1 == num_threads) ? NumChnLen : startRange + workload_per_thread;
            threads.emplace_back(processRange, startRange, endRange);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        if (out_sample_lengths_in_month.size() == 0) {
            writeResultsToCSV(fileName, results);
        } else {
            for (auto out_sample_length_in_month : out_sample_lengths_in_month) {
                std::string inSampleOutputFolder = fileName + "_OUTSAMP_" + std::to_string(out_sample_length_in_month) + "_/" + "insample/";
                auto inSampleOutputFilename = inSampleOutputFolder + "INSAMPLE_START_" + std::to_string(start_date) + "_INSAMPLE_END_" + std::to_string(end_date) + ".csv";
                std::cout << inSampleOutputFilename << std::endl;
                writeResultsToCSV(inSampleOutputFolder, results);
            }
        }

        float optiChn;
        float optiStp;
        float maxNPWDD = std::numeric_limits<float>::lowest();
        for (auto& result: results) {
            auto NPWDD = result.at(3) / std::abs(result.at(5));
            if (NPWDD > maxNPWDD) {
                maxNPWDD = NPWDD;
                optiChn = result.at(0);
                optiStp = result.at(1);
            }
        }
        std::cout << "Optimal Parameters Choosen for the in-sample dates: " << start_date << " - " << end_date << std::endl;
        std::cout << "CHANNEL LENGTH: " << optiChn << " STP PCT: " << optiStp << std::endl;
        std::cout << std::endl;
        return {optiChn, optiStp};
    }
};

#endif
