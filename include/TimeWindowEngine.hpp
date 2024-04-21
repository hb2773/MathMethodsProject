#include <vector>

#include "Bar.hpp"
#include "DateEncoder.hpp"


class TimeWindowEngine {
    public: 
    static void run(
        const unsigned long long start_date,
        const unsigned long long end_date,
        const int in_sample_length_in_year,
        const int out_sample_length_in_month,
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
        double StpPctStep 
        ) 
    {
        unsigned long long in_sample_start = start_date;
        unsigned long long in_sample_end;
        unsigned long long out_sample_start;
        unsigned long long out_sample_end;
        do {
            in_sample_end = incrementDate(in_sample_start, in_sample_length_in_year, 0);
            out_sample_start = in_sample_end;
            out_sample_end = incrementDate(in_sample_end, 0, out_sample_length_in_month);

            std::cout << "In-sample: " << in_sample_start << ":" << in_sample_end; 
            std::cout << ", Out-sample: " << out_sample_start << ":" << out_sample_end << std::endl;

            in_sample_start = incrementDate(in_sample_start, 0, out_sample_length_in_month);
        } 
        while (out_sample_end < end_date);
    }
};