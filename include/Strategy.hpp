#ifndef STRATEGY_H
#define STRATEGY_H

#include <set>
#include <queue>
#include <functional>
#include "Bar.hpp"
#include "Constants.hpp"

class ChannelBreakout {
    public: 

    const int ChnLen;
    const double StpPct;

    int position;
    bool traded;

    double equity;
    double numTrades;
    double equityMax;
    double drawdown;
    double maxDrawdown;

    double HH;
    double LL;

    double delta;
    double prevClose;

    bool buy = false;
    bool sell = false;

    double benchmarkLong;
    double benchmarkShort;

    bool sellShort;
    bool buyLong;

    std::queue<std::pair<double, int>> lastHs;
    std::queue<std::pair<double, int>> lastLs;
    std::set<std::pair<double, int>, std::greater<std::pair<double, int>>> setLastHs;
    std::set<std::pair<double, int>, std::less<std::pair<double, int>>> setLastLs;

    ChannelBreakout(int ChnLen, double StpPct) : 
        ChnLen(ChnLen), 
        StpPct(StpPct), 
        position(0.),
        traded(false),
        equity(INIT_EQUITY),
        numTrades(0.),
        equityMax(INIT_EQUITY),
        drawdown(0.),
        maxDrawdown(0.),
        delta(0.), 
        prevClose(0.),
        buy(false), 
        sell(false), 
        benchmarkLong(0.), 
        benchmarkShort(0.),
        sellShort(0.),
        buyLong(0.) {};

    void update(const Bar& bar, double HH, double LL, const int counter) {

        // setLastHs.insert({bar.high, counter});
        // lastHs.push({bar.high, counter});
        // setLastLs.insert({bar.low,  counter});
        // lastLs.push({bar.low, counter});

        // if (lastHs.size() <= ChnLen) {
        //     prevClose = bar.close;
        //     return;
        // } else {
        //     setLastHs.erase(lastHs.front());
        //     setLastLs.erase(lastLs.front());
        //     lastHs.pop();
        //     lastLs.pop();
        // }

        // HH = setLastHs.begin()->first;
        // LL = setLastLs.begin()->first;

        delta = POINT_VALUE * (bar.close - prevClose) * position;
        
        if (position == 0) {
            // std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;

            buy = (bar.high >= HH);
            sell = (bar.low <= LL);

            if (buy && sell) {
                delta = -SLPG + POINT_VALUE * (LL - HH);
                numTrades += 1.;
            } else {
                if (buy) {
                    delta = - SLPG / 2. + POINT_VALUE * (bar.close - HH);
                    position = 1;
                    traded = true;
                    benchmarkLong = bar.high;
                    numTrades += 0.5;
                }
                if (sell) {
                    delta = - SLPG / 2. - POINT_VALUE * (bar.close - LL);
                    position = -1;
                    traded = true;
                    benchmarkShort = bar.low;
                    numTrades += 0.5;
                }
            }
        } else if (position == 1  && !traded) {
            sellShort = bar.low <= LL;
            sell = (bar.low <= benchmarkLong * (1 - StpPct));
            if (sellShort && sell) {
                delta += - SLPG - 2. * POINT_VALUE * (bar.close - LL);
                position = -1;
                benchmarkShort = bar.low;
                numTrades += 1.;
            } else {
                if (sell) {
                    delta += - SLPG / 2. - POINT_VALUE * (bar.close - benchmarkLong * (1. - StpPct));
                    position = 0;
                    numTrades += 0.5;
                }
                if (sellShort) {
                    delta += - SLPG - 2. * POINT_VALUE * (bar.close - LL);
                    position = -1;
                    benchmarkShort = bar.low;
                    numTrades += 1.;
                }
            }
            benchmarkLong = std::max(bar.high, benchmarkLong);
        } else if (position == -1  && !traded) {
            buyLong = bar.high >= HH;
            buy = (bar.high >= benchmarkShort * (1. + StpPct));
            if (buyLong && buy) {
                delta -= SLPG + 2. * POINT_VALUE * (bar.close - HH);
                position = 1;
                benchmarkLong = bar.high;
                numTrades += 1.;
            } else {
                if (buy) {
                    delta += - SLPG / 2. + POINT_VALUE * (bar.close - benchmarkShort * (1. + StpPct));
                    position = 0;
                    numTrades += .5;
                }
                if (buyLong) {
                    delta += - SLPG + 2. * POINT_VALUE * (bar.close - HH);
                    position = 1;
                    benchmarkLong = bar.high;
                    numTrades += 1;
                }
            }
        } else if (position == 0 && !traded) {

        } else if (position == 1  &&  traded) {

        } else if (position == 0  &&  traded) {
            
        }
        equity += delta;
        equityMax = std::max(equityMax, equity);
        drawdown = equity - equityMax;
        maxDrawdown = std::min(drawdown, maxDrawdown);
        // std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;
        prevClose = bar.close;
        return;
    }
}; 

class StrategyEngine {
    public:
    static void run(ChannelBreakout& strat, const std::vector<Bar>& bars, std::vector<double> HHs, std::vector<double> LLs, unsigned long long start_date, unsigned long long end_date) {
        int counter = -1;
        for (const auto& bar : bars) {
            counter++;
            if (counter < BARS_BACK || bar.timestamp < start_date) {
                continue;
            } else if (bar.timestamp < end_date) {
                strat.update(bar, HHs.at(counter), LLs.at(counter), counter);
                // TODO: RECORD STRATEGY
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
    }
}; 

void recordStrategy(ChannelBreakout& strat) {
    // Output in a csv file : parameters of the strat, PNL, Maxdrawdown, Number of trades, Number of positives trades, (Profit / Maxdrawdown)
    // Have two possibilities, record the P&L or do not record the P&L curve. However still current P&L should be recorded.
}

#endif



