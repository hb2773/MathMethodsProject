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
    double prevClose; // OK

    // CONSTANTS
    const double NUM_CONTRACTS; 
    const double POINT_VALUE;
    const double SLPG;
    const int ChnLen;
    const double StpPct;

    // EQ CURVE
    double equity; // OK
    double equityMax; // OK

    // TRADES
    double numTrades; // OK
    double numPositiveTrades; // OK
    double avgWinner;
    double avgLooser;
    
    // DRAWDOWNS
    double drawdown; // OK
    double maxDrawdown; // OK

    // DELTA AND DELTA STATS
    double delta; // OK
    double deltaMean; // OK
    double deltaSum2_C; // OK
    double deltaSum3_C; // OK
    double deltaSum4_C;

    double HH;
    double LL;

    bool buy;
    bool sell;

    double benchmarkLong;
    double benchmarkShort;

    bool buyLong;
    bool sellShort;
    

    std::queue<std::pair<double, int>> lastHs;
    std::queue<std::pair<double, int>> lastLs;
    std::set<std::pair<double, int>, std::greater<std::pair<double, int>>> setLastHs;
    std::set<std::pair<double, int>, std::less<std::pair<double, int>>> setLastLs;

    ChannelBreakout(const double NUM_CONTRACTS, const double POINT_VALUE, const double SLPG, int ChnLen, double StpPct) : 

        position(0.),
        traded(false),
        n(0),
        prevClose(0.),

        NUM_CONTRACTS(NUM_CONTRACTS), 
        POINT_VALUE(POINT_VALUE), 
        SLPG(SLPG),
        ChnLen(ChnLen), 
        StpPct(StpPct), 

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

    void updateTrades(double num) {

        numTrades += num;
        if (delta > 0.) {
            numPositiveTrades += num;
            avgWinner += delta / (2 * numPositiveTrades);
        } else {
            avgLooser += delta / (2 * (numTrades - numPositiveTrades));
        }

    }    
    void update(const Bar& bar, double HH, double LL, const int counter) {

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
        double deviation = delta - deltaMean;
        double deviation_n = deviation / n;
        double deviation_n2 = deviation_n * deviation_n;
        double term1 = deviation * deviation_n * (n - 1);
        deltaMean += deviation / n;
        deltaSum4_C += term1 * deviation_n2 * (n * n - 3 * n + 3) + 6 * deviation_n2 * deltaSum2_C - 4 * deviation_n * deltaSum3_C;
        deltaSum3_C += term1 * deviation_n * (n - 2) - 3 * deviation_n * deltaSum2_C;
        deltaSum2_C += term1;

        // State UPDATE
        prevClose = bar.close;
        return;
    }
}; 

class StrategyEngine {
    public:

    static void recordStrategy(const ChannelBreakout& strat, std::vector<std::vector<double>>& results);

    static void recordStrategyEquity(const ChannelBreakout& strat, const Bar& bar, std::vector<std::vector<double>>& results);

    static void recordStrategyEquity(const ChannelBreakout& strat, std::vector<std::vector<double>>& results);

    static void run(
        ChannelBreakout& strat, 
        const std::vector<Bar>& bars, 
        std::vector<double> HHs, 
        std::vector<double> LLs, 
        unsigned long long start_date, unsigned long long end_date,
        bool recordStrat = false) {
        
        std::vector<std::vector<double>> stratResults;
        int counter = -1;
        for (const auto& bar : bars) {
            counter++;
            if (counter < BARS_BACK || bar.timestamp < start_date) {
                continue;
            } else if (bar.timestamp < end_date) {
                strat.update(bar, HHs.at(counter), LLs.at(counter), counter);
                if (recordStrat) {
                    recordStrategyEquity(strat, bar, stratResults);
                }
            } else {
                std::cout << std::endl;
                std::cout << "STRATEGY RUN " << std::endl;
                std::cout << "CHNL LENGTH  " << strat.ChnLen               << std::endl;
                std::cout << "STP PCT      " << strat.StpPct               << std::endl;
                std::cout << "PNL          " << strat.equity - INIT_EQUITY << std::endl;
                std::cout << "MAX DRAWDOWN " << strat.maxDrawdown          << std::endl;
                std::cout << "NUM TRADES   " << strat.numTrades            << std::endl;
                std::cout << "EQUITY MAX   " << strat.equityMax            << std::endl;
                std::cout << std::endl;
                // TODO: OUTPUT IN A CSV FILE THE RESULTS OF THE STRATEGY
                break;
            }
            
        }
        if (recordStrat) {
            writeStratEquityResultsToCSV("../output/results2.csv", stratResults);
        }
    }


}; 

void StrategyEngine::recordStrategyEquity(const ChannelBreakout& strat, const Bar& bar, std::vector<std::vector<double>>& results) {

    auto [year, month, day, hour, minute] = seperateDate(bar.timestamp);

    results.push_back( { 
        (double) year,
        (double) month,
        (double) day,
        (double) hour,
        (double) minute,
        (double) strat.ChnLen, 
        strat.StpPct, 
        strat.equity, 
        strat.drawdown,
        (double) strat.position
    } );
}

void StrategyEngine::recordStrategy(const ChannelBreakout& strat, std::vector<std::vector<double>>& results) {
    double pctPosTrades = strat.numPositiveTrades / strat.numTrades;

    double var  = (1. / strat.n) * strat.deltaSum2_C;
    double std_ = std::sqrt(var);
    double skew = strat.n * (strat.deltaSum3_C / std::pow (strat.deltaSum2_C, 1.5));
    double xkurt = strat.n * (strat.deltaSum4_C / std::pow(strat.deltaSum2_C, 2.0)) - 3.;


    results.push_back( { 
        (double) strat.ChnLen, 
        strat.StpPct, 
        (double) strat.n,
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

void recordStrategy(const ChannelBreakout& strat, std::vector<std::vector<double>>& results, std::mutex& mtx) {
    std::lock_guard<std::mutex> lock(mtx);
    StrategyEngine::recordStrategy(strat, results);
}
#endif



