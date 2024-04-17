#ifndef STRATEGY_H
#define STRATEGY_H

#include "Bar.hpp"
#include "Constants.hpp"

class Strategy {
    public: 
    long long unsigned start_date;
    long long unsigned end_date;
    int position = 0;
    bool traded = false;
    void update(const Bar& bar) {};
}; 

class ChannelBreakout: public Strategy {
    public: 

    int ChnLen;
    double StpPct;

    double HH;
    double LL;
    std::vector<double> lastHs;
    std::vector<double> lastLs;

    ChannelBreakout(int ChnLen, double StpPct) : ChnLen(ChnLen), StpPct(StpPct) {};

    void initialize(const Bar& bar) {
        if (bar.timestamp >= start_date + ChnLen) {

        }
        if (lastHs.size() >= ChnLen) {
            lastHs.emplace_back(bar.high);
            lastLs.emplace_back(bar.low);
            return;
        }
    }

    void update(const Bar& bar) {

        // if (lastHs.size() >= ChnLen) {
        //     lastHs.emplace_back(bar.high);
        //     lastLs.emplace_back(bar.low);
        //     return;
        // }

        if (position == 0) {
            std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;
        } else if (position == 1  && ~traded) {

        } else if (position == 0  && ~traded) {

        } else if (position == -1 && ~traded) {

        } else if (position == 1  &&  traded) {

        } else if (position == 0  &&  traded) {
            
        }
        // std::cout << "Date: " << bar.timestamp << " Close: " << bar.close << std::endl;
        return;
    }
}; 

void recordStrategy(Strategy& strat) {
    // Output in a csv file : parameters of the strat, PNL, Maxdrawdown, Number of trades, Number of positives trades, (Profit / Maxdrawdown)
    // Have two possibilities, record the P&L or do not record the P&L curve. However still current P&L should be recorded.
}

#endif



