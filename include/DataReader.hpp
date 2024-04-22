#ifndef DATA_READER_H
#define DATA_READER_H

#include <vector>
#include <fstream>
#include <sstream>

#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>

#include "Bar.hpp"
#include "Constants.hpp"
#include "utils.hpp"

std::vector<Bar> readData(const std::string& filename) {
    std::vector<Bar> data;
    std::ifstream file(filename);
    std::string line;

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        data.emplace_back(line);
    }

    return data;
}

using namespace boost::interprocess;

typedef allocator<float, managed_mapped_file::segment_manager> Allocator;
typedef vector<float, Allocator> MappedVector;
void create_HH_LL_Vector(

    const char* fileName, const std::size_t fileSize, 
    const int CHN_LEN_MIN, const int CHN_LEN_MAX, const int CHN_LEN_STEP, 
    const std::vector<float>& vec, bool high) {

    file_mapping::remove(fileName);
    managed_mapped_file mfile(create_only, fileName, fileSize);

    // Construct a vector in the mapped file
    MappedVector *myVector = mfile.find_or_construct<MappedVector>("MyVector")(mfile.get_segment_manager());

    std::string coutS;
    if (high) {
        coutS = "Highest Highs";
    } else {
        coutS = "Lowest Lows";
    }
    std::cout << "Pre-computing the " << coutS << " and storing them for reuse" << std::endl;

    for (int ChnLen = CHN_LEN_MIN; ChnLen < CHN_LEN_MAX + CHN_LEN_STEP; ChnLen += CHN_LEN_STEP) {
        auto newVector = MinMaxSlidingWindow(vec, ChnLen, high);
        std::cout << "CHN LEN: " << ChnLen << std::endl;
        myVector->insert(myVector->end(), newVector.begin(), newVector.end());
    }
    
}

std::vector<float> read_kth_vector(const char* fileName, size_t vector_length, size_t k) {
    managed_mapped_file mfile(open_only, fileName);
    MappedVector *myVector = mfile.find<MappedVector>("MyVector").first;
    
    if (!myVector) {
        throw std::runtime_error("Vector not found in the file!");
    }

    // Calculate the start and end indices of the kth vector
    size_t start_index = k * vector_length;
    size_t end_index = start_index + vector_length;

    if (end_index > myVector->size()) {
        throw std::runtime_error("Index out of range!");
    }

    // Copy elements from the mapped vector to a standard vector
    return std::vector<float>(myVector->begin() + start_index, myVector->begin() + end_index);
}

#endif