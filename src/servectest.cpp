#include <fstream>
#include <vector>
#include <iostream>

void serialize_vector(const std::vector<int>& vec, std::ofstream& outFile, std::ofstream& indexFile) {
    // Record the start position of the vector in the data file
    std::streampos startPosition = outFile.tellp();

    // Serialize the vector
    size_t size = vec.size();
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(size));  // Write the size of the vector
    outFile.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(int));  // Write the vector data

    // Write the start position and the size to the index file
    indexFile.write(reinterpret_cast<const char*>(&startPosition), sizeof(startPosition));
    indexFile.write(reinterpret_cast<const char*>(&size), sizeof(size));
}

std::vector<int> deserialize_vector(int vectorIndex, const std::string& dataFilename, const std::string& indexFilename) {
    std::ifstream dataFile(dataFilename, std::ios::binary);
    std::ifstream indexFile(indexFilename, std::ios::binary);

    // Calculate the index position
    indexFile.seekg(vectorIndex * (sizeof(std::streampos) + sizeof(size_t)));

    // Read the position and size from the index file
    std::streampos position;
    size_t size;
    indexFile.read(reinterpret_cast<char*>(&position), sizeof(position));
    indexFile.read(reinterpret_cast<char*>(&size), sizeof(size));

    // Seek to the position in the data file
    dataFile.seekg(position);

    // Deserialize the vector
    std::vector<int> vec(size);
    dataFile.read(reinterpret_cast<char*>(vec.data()), size * sizeof(int));

    dataFile.close();
    indexFile.close();

    return vec;
}

int main() {

    std::ofstream dataFile("vectors.dat", std::ios::binary);
    std::ofstream indexFile("index.dat", std::ios::binary);

    // Example of generating and serializing multiple vectors
    for (int i = 0; i < 10; ++i) {
        std::vector<int> vec(i + 1, 2 * i);  // Create a vector with i+1 elements, all initialized to i
        for (int num: vec) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
        serialize_vector(vec, dataFile, indexFile);
    }

    dataFile.close();
    indexFile.close();

    for (int i = 0; i < 10; ++i) {
        std::vector<int> vec = deserialize_vector(i, "vectors.dat", "index.dat");
        
        // Do something with the vector, e.g., print it
        std::cout << "Vector " << i << ": ";
        for (int num : vec) {
            std::cout << num << " ";
        }
        std::cout << std::endl;
    }

    dataFile.close();
    indexFile.close();
    return 0;
}