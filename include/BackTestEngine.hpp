#include <vector>
#include <thread>

#include "Constants.hpp"
#include "Bar.hpp"
#include "Strategy.hpp"
#include "utils.hpp"


// 3 - Pseudo code for 3. 
// use threading and allow for explicit date start and date end.
// std::vector<double> pseudoCodeParmaterSearch(const std::vector<Bar>& bars, long long int start_date, long long int end_date) {


class BackTestEngine {
    public:
    static void run(
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
        unsigned long long end_date
        ) 
    {
        int totalChnLenSteps = std::ceil((ChnLenMax - ChnLenMin) / ChnLenStep) + 1;

        // Number of threads
        const int num_threads = 4;

        std::vector<std::thread> threads;

        auto processRange = [&](int start, int end) {
            for (int i = start; i < end; ++i) {
                double ChnLen = ChnLenMin + i * ChnLenStep;
                auto HHs = MinMaxSlidingWindow(highs, ChnLen, true);
                auto LLs = MinMaxSlidingWindow(lows, ChnLen, false);
                for (double StpPct = StpPctMin; StpPct <= StpPctMax; StpPct += StpPctStep) {
                    ChannelBreakout strat = ChannelBreakout(ChnLen, StpPct);
                    StrategyEngine::run(strat, bars, HHs, LLs, start_date, end_date);
                }
            }
        };

        // Calculate workload per thread
        int workload_per_thread = totalChnLenSteps / num_threads;

        // Distribute the workload to each thread
        for (int i = 0; i < num_threads; ++i) {
            int start = i * workload_per_thread;
            int end = (i + 1 == num_threads) ? totalChnLenSteps : start + workload_per_thread;
            threads.emplace_back(processRange, start, end);
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
    }
};
