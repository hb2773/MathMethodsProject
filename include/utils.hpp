#ifndef UTILS_H
#define UTILS_H 

#include <fstream>
#include <vector>
#include <set>
#include <functional>
#include <queue>
#include <iostream>

template<typename Comparator>
std::vector<double> SlidingWindowImpl(const std::vector<double>& arr, int k, Comparator comp) {
    std::vector<double> ans;
    std::set<std::pair<double, int>, Comparator> st(comp);

    for (int i = 0; i < k - 1; i++) {
        st.insert({arr[i], i});
        ans.push_back(-1.);
    }
    st.insert({arr[k-1], k-1});
    ans.push_back(st.begin()->first);

    for (int i = k; i < arr.size(); i++) {
        st.insert({arr[i], i});
        st.erase({arr[i - k], i - k});
        ans.push_back(st.begin()->first);
    }

    return ans;
}

std::vector<double> MinMaxSlidingWindow(const std::vector<double>& arr, int k, bool max) {
    if (max) {
        return SlidingWindowImpl(arr, k, std::greater<std::pair<double, int>>());
    } else {
        return SlidingWindowImpl(arr, k, std::less<std::pair<double, int>>());
    }
}

template<typename T>
void writeVectorToCSV(std::ofstream& outFile, const std::vector<T>& vec, const int ChnLen, bool high) {
    if (high) {
        outFile << "HH," << ChnLen;  // Write the size of the vector
    } else {
        outFile << "LL," << ChnLen;
    }
    for (const T& element : vec) {
        outFile << "," << element;  // Write each element separated by a comma
    }
    outFile << "\n";
}

void writeResultsToCSV(const std::string& filename, const std::vector<std::vector<double>>& data) {
    std::ofstream outFile(filename);

    // Check if file is open
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing.\n";
        return;
    }

    // Writing header
    outFile << "Chnlen,StpPct,nObs,Equity,EquityMax,MaxDrawdown,NumTrades,NumPositiveTrades,PctPosTrade,AvgWinner,AvgLooser,DeltaMean,DeltaNegMean,DeltaSum2_C,DeltaSum3_C,Var,Std,Skew\n"; 

    // Writing data
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            outFile << row[i];
            if (i != row.size() - 1) outFile << ",";
        }
        outFile << "\n";
    }

    outFile.close();
}

void constructHHLLFile(std::string& high_low_file, int cmin, int cmax, int step, std::vector<double> highs, std::vector<double> lows) {
    std::ofstream outHighLowFile(high_low_file);
    if (!outHighLowFile) {
        std::cerr << "Error opening file for writing." << std::endl;
        return; // Exit if file cannot be opened
    }
    for (int ChnLen = cmin; ChnLen < cmax; ChnLen += step) {
        auto HHs = MinMaxSlidingWindow(highs, ChnLen, true);
        auto LLs = MinMaxSlidingWindow(lows, ChnLen, true);
        writeVectorToCSV(outHighLowFile, HHs, ChnLen, true);
        writeVectorToCSV(outHighLowFile, LLs, ChnLen, false);
    }
    outHighLowFile.close();
}



#endif