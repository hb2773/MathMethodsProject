#ifndef CONSTANTS_H
#define CONSTANTS_H

// INITIAL WEALTH
const double EQUITY = 200'000.;
// BARS BACK
const int BARS_BACK = 0; //17'001;

// CONTRACT SPECIFIC
// 2a - Need to handle these constants and maybe find them on Bloomberg or in the files he gave
const double POINT_VALUE = 64'000.;
const double SLPG = 65.;
const double NUM_CONTRACTS = 1.; 

// PARAMETER SEARCH
const int CHN_LEN_MIN = 500; // 500
const int CHN_LEN_MAX = 10'000; // 10000
const int CHN_LEN_STEP = 100; // 10

const double STP_PCT_MIN = 0.005; // 0.005
const double STP_PCT_MAX = 0.10; // 0.10
const double STP_PCT_STEP = 0.1; // 0.001

const int NUM_CHN_LEN = (int) ((CHN_LEN_MAX - CHN_LEN_MIN) / CHN_LEN_STEP) + 1;
const int NUM_STP_PCT = (int) ((STP_PCT_MAX - STP_PCT_MIN) / STP_PCT_STEP) + 1;

#endif