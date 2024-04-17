#include "Bar.hpp"
class Strategy {
    public: 
    int position = 0;
    void update(Bar& bar) {};
}; 

class ChannelBreakout: public Strategy {
    public: 

    int ChnLen;
    double StpPct;

    double highestHigh;
    // double maxDrawdown;
    double lowestLow;

    ChannelBreakout(int ChnLen, double StpPct) : ChnLen(ChnLen), StpPct(StpPct) {};

    void update(Bar& bar) {
        
    }
}; 

void recordStragey(Strategy& strat) {
    // Output in a csv file : parameters of the strat, PNL, Maxdrawdown, Number of trades, Number of positives trades, (Profit / Maxdrawdown)
}



