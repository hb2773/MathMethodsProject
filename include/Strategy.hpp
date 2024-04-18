#ifndef STRATEGY_H
#define STRATEGY_H

#include <set>
#include <queue>
#include <functional>
#include "Bar.hpp"
#include "Constants.hpp"

class Strategy {
    public: 
    int position = 0;
    bool traded = false;
    void update(const Bar& bar) {};
}; 

class ChannelBreakout: public Strategy {
    public: 

    const int ChnLen;
    const double StpPct;

    double equity = INIT_EQUITY;
    double numTrades = 0.;
    double equityMax = INIT_EQUITY;
    double drawdown = 0.;
    double maxDrawdown = 0.;

    double HH;
    double LL;

    double delta;
    double prevClose;

    bool buy;
    bool sell;

    double benchmarkLong;
    double benchmarkShort;

    bool sellShort;
    bool buyLong;

    std::queue<std::pair<double, int>> lastHs;
    std::queue<std::pair<double, int>> lastLs;
    std::set<std::pair<double, int>, std::greater<std::pair<double, int>>> setLastHs;
    std::set<std::pair<double, int>, std::less<std::pair<double, int>>> setLastLs;

    ChannelBreakout(int ChnLen, double StpPct) : ChnLen(ChnLen), StpPct(StpPct) {};

    void update(const Bar& bar, const int counter) {

        setLastHs.insert({bar.high, counter});
        lastHs.push({bar.high, counter});
        setLastLs.insert({bar.low,  counter});
        lastLs.push({bar.low, counter});

        if (lastHs.size() <= ChnLen) {
            prevClose = bar.close;
            return;
        } else {
            setLastHs.erase(lastHs.front());
            setLastLs.erase(lastLs.front());
            lastHs.pop();
            lastLs.pop();
        }

        HH = setLastHs.begin()->first;
        LL = setLastLs.begin()->first;

        delta = POINT_VALUE * (bar.close - prevClose) * position;
        
        if (position == 0) {
            std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;

            buy = bar.high >= HH;
            sell = bar.low >= LL;

            if (buy && sell) {
                delta = -SLPG + POINT_VALUE * (LL - HH);
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
        } else if (position == 1  && ~traded) {
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
        } else if (position == -1  && ~traded) {
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
        } else if (position == 0 && ~traded) {

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

void recordStrategy(Strategy& strat) {
    // Output in a csv file : parameters of the strat, PNL, Maxdrawdown, Number of trades, Number of positives trades, (Profit / Maxdrawdown)
    // Have two possibilities, record the P&L or do not record the P&L curve. However still current P&L should be recorded.
}

#endif



