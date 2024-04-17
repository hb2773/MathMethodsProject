#include "Bar.hpp"
class Strategy {
    public: 
    int position = 0;
    void update(Bar& bar);
}; 

class ChannelBreakout: public Strategy {

}; 


