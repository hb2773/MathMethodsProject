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
#include "utils.hpp"


// 3 - Pseudo code for 3. 
// use threading and allow for explicit date start and date end.
// std::vector<double> pseudoCodeParmaterSearch(const std::vector<Bar>& bars, long long int start_date, long long int end_date) {


class BackTestEngine {
    public:
    static std::pair<double,double> run(
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
        double StpPctStep, 
        unsigned long long start_date, 
        unsigned long long end_date,
        bool recordStrat = false,
        std::string folder = "../output/results.csv") 
    {
        std::mutex mtx;
        std::vector<std::vector<double>> results;

        int numChnLen = std::ceil((ChnLenMax - ChnLenMin) / ChnLenStep) + 1;

        const int num_threads = 4;
        std::vector<std::thread> threads;
        int workload_per_thread = numChnLen / num_threads;

        auto processRange = [&](int startRange, int endRange) {
            for (int i = startRange; i < endRange; ++i) {
                double ChnLen = ChnLenMin + i * ChnLenStep;
                auto HHs = MinMaxSlidingWindow(highs, ChnLen, true);
                auto LLs = MinMaxSlidingWindow(lows, ChnLen, false);
                for (double StpPct = StpPctMin; StpPct <= StpPctMax; StpPct += StpPctStep) {
                    ChannelBreakout strat = ChannelBreakout(NUM_CONTRACTS, POINT_VALUE, SLPG, ChnLen, StpPct);
                    StrategyEngine::run(strat, bars, HHs, LLs, start_date, end_date, recordStrat);
                    recordStrategy(strat, results, mtx);
                }
            }
        };
        
        // Distribute the workload to each thread
        for (int i = 0; i < num_threads; ++i) {
            int startRange = i * workload_per_thread;
            int endRange = (i + 1 == num_threads) ? numChnLen : startRange + workload_per_thread;
            threads.emplace_back(processRange, startRange, endRange);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
        
        writeResultsToCSV(folder, results);

        double optiChn;
        double optiStp;
        double maxNPWDD = std::numeric_limits<double>::lowest();
        for (auto& result: results) {
            auto NPWDD = result.at(3) / std::abs(result.at(5));
            if (NPWDD > maxNPWDD) {
                maxNPWDD = NPWDD;
                optiChn = result.at(0);
                optiStp = result.at(1);
            }
        }
        std::cout << "Optimal Parameters Choosen for the in-sample dates:" << start_date << ":" << end_date << std::endl;
        std::cout << "CHANNEL LENGTH: " << optiChn << " STP PCT: " << optiStp << std::endl;
        std::cout << std::endl;
        return {optiChn, optiStp};

    }
};

#endif
