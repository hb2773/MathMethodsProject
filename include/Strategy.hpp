#ifndef STRATEGY_H
#define STRATEGY_H

#include <set>
#include <queue>
#include <functional>
#include "utils.hpp"
#include "Bar.hpp"
#include "Constants.hpp"


class ChannelBreakout {
    public: 

    // Measure various statistical performance properties of both equity curve and trade-by-trade table: Average Rate of Return, Standard Deviation of Returns, and the Ratio of the two above (known as the Sharpe or Information Ratio), Total Number of Trades, % of Winners or % of Profitable Trades, Return on the Account (Net Profit to Worst Drawdown), Average Winner, Average Loser, Profit Factor, etc.
    
    // STATE
    int position; // OK
    bool traded; // OK
    int n; // OK
    float prevClose; // OK
    float prevHigh; // OK
    float prevLow; // OK

    // CONSTANTS
    const float NUM_CONTRACTS; 
    const float POINT_VALUE;
    const float SLPG;
    const int ChnLen;
    const float StpPct;
    const float tolerance;

    // EQ CURVE
    float equity; // OK
    float equityMax; // OK

    // TRADES
    float numTrades; // OK
    float numPositiveTrades; // OK
    float avgWinner;
    float avgLooser;
    
    // DRAWDOWNS
    float drawdown; // OK
    float maxDrawdown; // OK

    // DELTA AND DELTA STATS
    float delta; // OK
    float deltaMean; // OK
    float deltaSum2_C; // OK
    float deltaSum3_C; // OK
    float deltaSum4_C;

    float HH;
    float LL;

    bool buy;
    bool sell;

    float benchmarkLong;
    float benchmarkShort;

    bool buyLong;
    bool sellShort;
    

    std::queue<std::pair<float, int>> lastHs;
    std::queue<std::pair<float, int>> lastLs;
    std::set<std::pair<float, int>, std::greater<std::pair<float, int>>> setLastHs;
    std::set<std::pair<float, int>, std::less<std::pair<float, int>>> setLastLs;

    ChannelBreakout(const float NUM_CONTRACTS, const float POINT_VALUE, const float SLPG, int ChnLen, float StpPct) : 

        position(0.),
        traded(false),
        n(0),
        prevClose(0.),

        NUM_CONTRACTS(NUM_CONTRACTS), 
        POINT_VALUE(POINT_VALUE), 
        SLPG(SLPG),
        ChnLen(ChnLen), 
        StpPct(StpPct),
        tolerance(0.), 

        equity(INIT_EQUITY),
        equityMax(INIT_EQUITY),

        numTrades(0.),
        numPositiveTrades(0.),
        avgWinner(0.),
        avgLooser(0.),

        drawdown(0.),
        maxDrawdown(0.),

        delta(0.), 
        deltaMean(0.),
        deltaSum2_C(0.),
        deltaSum3_C(0.),
        deltaSum4_C(0.),
        
        buy(false), 
        sell(false), 
        benchmarkLong(0.), 
        benchmarkShort(0.),

        buyLong(false), 
        sellShort(false)
        
        {};

        ChannelBreakout(const float NUM_CONTRACTS, const float POINT_VALUE, const float SLPG, int ChnLen, float StpPct, float tolerance) : 

        position(0.),
        traded(false),
        n(0),
        prevClose(0.),
        prevHigh(0.),
        prevLow(0.),

        NUM_CONTRACTS(NUM_CONTRACTS), 
        POINT_VALUE(POINT_VALUE), 
        SLPG(SLPG),
        ChnLen(ChnLen), 
        StpPct(StpPct),
        tolerance(tolerance), 

        equity(INIT_EQUITY),
        equityMax(INIT_EQUITY),

        numTrades(0.),
        numPositiveTrades(0.),
        avgWinner(0.),
        avgLooser(0.),

        drawdown(0.),
        maxDrawdown(0.),

        delta(0.), 
        deltaMean(0.),
        deltaSum2_C(0.),
        deltaSum3_C(0.),
        deltaSum4_C(0.),
        
        buy(false), 
        sell(false), 
        benchmarkLong(0.), 
        benchmarkShort(0.),

        buyLong(false), 
        sellShort(false)
        
        {};

    void updateTrades(float num) {

        numTrades += num;
        if (delta > 0.) {
            numPositiveTrades += num;
            avgWinner += delta / (2 * numPositiveTrades);
        } else {
            avgLooser += delta / (2 * (numTrades - numPositiveTrades));
        }

    }    
    void update(const Bar& bar, float HH, float LL, const int counter) {

        n += 1;
        traded = false;
        delta = POINT_VALUE * (bar.close - prevClose) * position;
        
        if (position == 0) {

            buy = (bar.high >= HH);
            sell = (bar.low <= LL);

            if (buy && sell) {
                delta = - SLPG + POINT_VALUE * (LL - HH);
                updateTrades(1.);
            } else {
                if (buy) {
                    delta = - SLPG / 2. + POINT_VALUE * (bar.close - HH);
                    position = 1;
                    traded = true;
                    benchmarkLong = bar.high;
                    updateTrades(0.5);
                }
                if (sell) {
                    delta = - SLPG / 2. - POINT_VALUE * (bar.close - LL);
                    position = -1;
                    traded = true;
                    benchmarkShort = bar.low;
                    updateTrades(0.5);
                }
            }
        } 
        if (position == 1  && !traded) {
            sellShort = bar.low <= LL;
            sell = (bar.low <= benchmarkLong * (1 - StpPct));
            if (sellShort && sell) {
                delta += - SLPG - 2. * POINT_VALUE * (bar.close - LL);
                position = -1;
                benchmarkShort = bar.low;
                updateTrades(1.);
            } else {
                if (sell) {
                    delta += - SLPG / 2. - POINT_VALUE * (bar.close - benchmarkLong * (1. - StpPct));
                    position = 0;
                    updateTrades(0.5);
                }
                if (sellShort) {
                    delta += - SLPG - 2. * POINT_VALUE * (bar.close - LL);
                    position = -1;
                    benchmarkShort = bar.low;
                    updateTrades(1.);
                }
            }
            benchmarkLong = std::max(bar.high, benchmarkLong);
        } 
        if (position == -1  && !traded) {
            buyLong = bar.high >= HH;
            buy = (bar.high >= benchmarkShort * (1. + StpPct));
            if (buyLong && buy) {
                delta += - SLPG + 2. * POINT_VALUE * (bar.close - HH);
                position = 1;
                benchmarkLong = bar.high;
                updateTrades(1.);
            } else {
                if (buy) {
                    delta += - SLPG / 2. + POINT_VALUE * (bar.close - benchmarkShort * (1. + StpPct));
                    position = 0;
                    updateTrades(0.5);
                }
                if (buyLong) {
                    delta += - SLPG + 2. * POINT_VALUE * (bar.close - HH);
                    position = 1;
                    benchmarkLong = bar.high;
                    updateTrades(1.);
                }
            }
            benchmarkShort = std::min(bar.low, benchmarkShort);
        } 
        if (position ==  0 && traded) {

        } 
        if (position ==  1 && traded) {

        } 
        if (position == -1 && traded) {
            
        }
        // Equity UPDATE  
        equity += delta;
        equityMax = std::max(equityMax, equity);
        
        // Drawdown UPDATE
        drawdown = equity - equityMax;
        maxDrawdown = std::min(drawdown, maxDrawdown);

        // Delta Stat UPDATE
        float deviation = delta - deltaMean;
        float deviation_n = deviation / n;
        float deviation_n2 = deviation_n * deviation_n;
        float term1 = deviation * deviation_n * (n - 1);
        deltaMean += deviation / n;
        deltaSum4_C += term1 * deviation_n2 * (n * n - 3 * n + 3) + 6 * deviation_n2 * deltaSum2_C - 4 * deviation_n * deltaSum3_C;
        deltaSum3_C += term1 * deviation_n * (n - 2) - 3 * deviation_n * deltaSum2_C;
        deltaSum2_C += term1;

        // State UPDATE
        prevClose = bar.close;
        return;
    }

    void update2(const Bar& bar, float HH, float LL, const int counter) {
        n += 1;
        traded = false;

        if (position == 0) {
            if (prevHigh >= HH) {
                if (HH <= bar.high && HH * (1 + tolerance) >= bar.low) {
                    auto entryBuyPrice = std::min(HH * (1 + tolerance), bar.open);
                    updateTrades(.5);
                    position = 1;
                    traded = true;
                    benchmarkLong = prevHigh;
                    delta = POINT_VALUE * (bar.close - entryBuyPrice);
                }
            } else if (prevLow <= LL) {
                if (LL >= bar.low && LL * (1 - tolerance) <= bar.high) {
                    auto entrySellPrice = std::max(LL * (1 - tolerance), bar.open);
                    updateTrades(0.5);
                    position = -1;
                    traded = true;
                    benchmarkShort = prevLow;
                    delta = POINT_VALUE * (entrySellPrice - bar.close);
                }
            }
        } 
        else if (position == 1) {
            if (prevLow <= benchmarkLong * (1 - StpPct)) {
                updateTrades(0.5);
                position = 0;
                traded = true;
                benchmarkShort = prevLow;
                delta = POINT_VALUE * (bar.open - prevClose);
            }
            if (prevLow <= LL) {
                if (LL >= bar.low && LL * (1 - tolerance) <= bar.high) {
                    auto entrySellPrice = std::max(LL * (1 - tolerance), bar.open);
                    updateTrades(0.5);
                    position = -1;
                    traded = true;
                    benchmarkShort = std::min(prevLow, benchmarkShort);
                    delta += POINT_VALUE * (entrySellPrice - bar.close);
                }
            }
            benchmarkLong = std::max(prevHigh, benchmarkLong);
        }
        else if (position == -1) {
            if (prevHigh >= benchmarkShort * (1 + StpPct)) {
                updateTrades(0.5);
                position = 0;
                traded = true;
                benchmarkLong = prevHigh;
                delta = POINT_VALUE * (bar.open - prevClose);
            }
            if (prevHigh <= LL) {
                if (HH <= bar.high && HH * (1 + tolerance) >= bar.low) {
                    auto entryBuyPrice = std::min(HH * (1 + tolerance), bar.open);
                    updateTrades(0.5);
                    position = 1;
                    traded = true;
                    benchmarkLong = std::max(prevHigh, benchmarkLong);
                    delta += POINT_VALUE * (bar.close - entryBuyPrice);
                }
            }
            benchmarkShort = std::min(prevLow, benchmarkShort);
        }
        if (traded == false) {
            delta = POINT_VALUE * (bar.close - prevClose) * position;
        }

        // Equity UPDATE  
        equity += delta;
        equityMax = std::max(equityMax, equity);
        
        // Drawdown UPDATE
        drawdown = equity - equityMax;
        maxDrawdown = std::min(drawdown, maxDrawdown);

        // Delta Stat UPDATE
        float deviation = delta - deltaMean;
        float deviation_n = deviation / n;
        float deviation_n2 = deviation_n * deviation_n;
        float term1 = deviation * deviation_n * (n - 1);
        deltaMean += deviation / n;
        deltaSum4_C += term1 * deviation_n2 * (n * n - 3 * n + 3) + 6 * deviation_n2 * deltaSum2_C - 4 * deviation_n * deltaSum3_C;
        deltaSum3_C += term1 * deviation_n * (n - 2) - 3 * deviation_n * deltaSum2_C;
        deltaSum2_C += term1;

        // State UPDATE
        prevClose = bar.close;
        prevHigh = bar.high;
        prevLow = bar.low;
        return;
    }
}; 

class StrategyEngine {
    public:

    static void recordStrategy(const ChannelBreakout& strat, std::vector<std::vector<float>>& results);

    static void recordStrategyEquity(const ChannelBreakout& strat, const Bar& bar, std::vector<std::vector<float>>& results);

    static void recordStrategyEquity(const ChannelBreakout& strat, std::vector<std::vector<float>>& results);

    static void run(
        ChannelBreakout& strat, 
        const std::vector<Bar>& bars, 
        const std::vector<float>& HHs, 
        const std::vector<float>& LLs, 
        unsigned long long start_date, unsigned long long end_date,
        bool recordStrat = false,
        std::string outputFile = "../output/results2.csv") {
        
        std::vector<std::vector<float>> stratResults;
        int counter = -1;
        for (const auto& bar : bars) {
            counter++;
            if (counter < BARS_BACK || bar.timestamp < start_date) {
                continue;
            } else if (bar.timestamp < end_date) {
                strat.update(bar, HHs.at(counter - 1), LLs.at(counter - 1), counter);
                if (recordStrat) {
                    recordStrategyEquity(strat, bar, stratResults);
                }
            } else {
                // std::cout << std::endl;
                // std::cout << "STRATEGY RUN " << std::endl;
                // std::cout << "CHNL LENGTH  " << strat.ChnLen               << std::endl;
                // std::cout << "STP PCT      " << strat.StpPct               << std::endl;
                // std::cout << "PNL          " << strat.equity - INIT_EQUITY << std::endl;
                // std::cout << "MAX DRAWDOWN " << strat.maxDrawdown          << std::endl;
                // std::cout << "NUM TRADES   " << strat.numTrades            << std::endl;
                // std::cout << "EQUITY MAX   " << strat.equityMax            << std::endl;
                // std::cout << std::endl;
                // TODO: OUTPUT IN A CSV FILE THE RESULTS OF THE STRATEGY
                break;
            }
            
        }
        if (recordStrat) {
            writeStratEquityResultsToCSV(outputFile, stratResults);
        }
    }

    static void run2(
        ChannelBreakout& strat, 
        const std::vector<Bar>& bars, 
        const std::vector<float>& HHs, 
        const std::vector<float>& LLs, 
        unsigned long long start_date, unsigned long long end_date,
        bool recordStrat = false,
        std::string outputFile = "../output/results2.csv") {
        
        std::vector<std::vector<float>> stratResults;
        int counter = -1;
        for (const auto& bar : bars) {
            counter++;
            if (counter < BARS_BACK || bar.timestamp < start_date) {
                strat.prevClose = bar.close;
                strat.prevHigh = bar.high;
                strat.prevLow = bar.low;
                continue;
            } else if (bar.timestamp < end_date) {
                strat.update2(bar, HHs.at(counter - 2), LLs.at(counter - 2), counter);
                if (recordStrat) {
                    recordStrategyEquity(strat, bar, stratResults);
                }
            } else {
                // std::cout << std::endl;
                // std::cout << "STRATEGY RUN " << std::endl;
                // std::cout << "CHNL LENGTH  " << strat.ChnLen               << std::endl;
                // std::cout << "STP PCT      " << strat.StpPct               << std::endl;
                // std::cout << "PNL          " << strat.equity - INIT_EQUITY << std::endl;
                // std::cout << "MAX DRAWDOWN " << strat.maxDrawdown          << std::endl;
                // std::cout << "NUM TRADES   " << strat.numTrades            << std::endl;
                // std::cout << "EQUITY MAX   " << strat.equityMax            << std::endl;
                // std::cout << std::endl;
                // TODO: OUTPUT IN A CSV FILE THE RESULTS OF THE STRATEGY
                break;
            }
            
        }
        if (recordStrat) {
            writeStratEquityResultsToCSV(outputFile, stratResults);
        }
    }

    static void run(
        ChannelBreakout& strat, 
        const std::vector<Bar>& bars, 
        const std::vector<std::pair<float,float>>& HHs_LLs, 
        const int indexChn,
        unsigned long long start_date, unsigned long long end_date,
        bool recordStrat = false,
        std::string outputFile = "../output/results2.csv") {
        
        std::vector<std::vector<float>> stratResults;
        int counter = -1;
        for (const auto& bar : bars) {
            counter++;
            if (counter < BARS_BACK || bar.timestamp < start_date) {
                continue;
            } else if (bar.timestamp < end_date) {
                auto [HH, LL] = HHs_LLs.at(indexChn * bars.size() + counter - 1);
                strat.update(bar, HH, LL, counter);
                if (recordStrat) {
                    recordStrategyEquity(strat, bar, stratResults);
                }
            } else {
                // std::cout << std::endl;
                // std::cout << "STRATEGY RUN " << std::endl;
                // std::cout << "CHNL LENGTH  " << strat.ChnLen               << std::endl;
                // std::cout << "STP PCT      " << strat.StpPct               << std::endl;
                // std::cout << "PNL          " << strat.equity - INIT_EQUITY << std::endl;
                // std::cout << "MAX DRAWDOWN " << strat.maxDrawdown          << std::endl;
                // std::cout << "NUM TRADES   " << strat.numTrades            << std::endl;
                // std::cout << "EQUITY MAX   " << strat.equityMax            << std::endl;
                // std::cout << std::endl;
                break;
            }
            
        }
        if (recordStrat) {
            writeStratEquityResultsToCSV(outputFile, stratResults);
        }
    }

}; 

void StrategyEngine::recordStrategyEquity(const ChannelBreakout& strat, const Bar& bar, std::vector<std::vector<float>>& results) {

    auto [year, month, day, hour, minute] = seperateDate(bar.timestamp);

    results.push_back( { 
        (float) year,
        (float) month,
        (float) day,
        (float) hour,
        (float) minute,
        (float) strat.ChnLen, 
        strat.StpPct, 
        strat.equity, 
        strat.drawdown,
        (float) strat.position
    } );
}

void StrategyEngine::recordStrategy(const ChannelBreakout& strat, std::vector<std::vector<float>>& results) {
    float pctPosTrades = strat.numPositiveTrades / strat.numTrades;

    float var  = (1. / strat.n) * strat.deltaSum2_C;
    float std_ = std::sqrt(var);
    float skew = strat.n * (strat.deltaSum3_C / std::pow (strat.deltaSum2_C, 1.5));
    float xkurt = strat.n * (strat.deltaSum4_C / std::pow(strat.deltaSum2_C, 2.0)) - 3.;


    results.push_back( { 
        (float) strat.ChnLen, 
        strat.StpPct, 
        (float) strat.n,
        strat.equity, 
        strat.equityMax, 
        strat.maxDrawdown, 
        strat.numTrades, 
        strat.numPositiveTrades,
        pctPosTrades,
        strat.avgWinner,
        strat.avgLooser,
        strat.deltaMean,
        strat.deltaSum2_C, 
        strat.deltaSum3_C,
        strat.deltaSum4_C,
        var,
        std_,
        skew,
        xkurt
    } );
}

void recordStrategy(const ChannelBreakout& strat, std::vector<std::vector<float>>& results, std::mutex& mtx) {
    std::lock_guard<std::mutex> lock(mtx);
    StrategyEngine::recordStrategy(strat, results);
}
#endif



