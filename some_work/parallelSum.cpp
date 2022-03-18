#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <algorithm>
#include <chrono>
#include "TimeClick/TimeClick.h"
using namespace std;

std::vector<int64_t> data;

void readData(std::string filename)
{
    std::ifstream file;
    file.open(filename);
    int64_t value;
    while (file >> value)
    {
        data.push_back(value);
    }
    file.close();
}

// This function is called by each thread.
void parallelSum(int64_t &sum, int start, int end)
{
    sum = 0;
    for (int i = start; i < end; i++)
    {
        sum += data[i];
    }
}

// This function is calculating the sum of the data in parallel.
int64_t calculateSum(int numThreads)
{
    int64_t sum = 0;
    int chunkSize = data.size() / numThreads;
    std::vector<std::thread> threads(numThreads);
    int64_t sumLocal[numThreads];
    for (int i = 0; i < numThreads; i++)
    {
        int start = i * chunkSize;
        int end = (i + 1) * chunkSize;
        if (i == numThreads - 1)
        {
            end = data.size();
        }
        threads[i] = std::thread(parallelSum, std::ref(sumLocal[i]), start, end);
    }
    for (int i = 0; i < numThreads; i++)
    {
        threads[i].join();
        sum += sumLocal[i];
    }
    return sum;  
}




int main()
{
    readData("data.txt");
    int64_t sum = 0;
    sum = calculateSum(16);
    cout<< "sum = " << sum << endl;
    return 0;
}

//g++ -std=c++11 -o parallelSum parallelSum.cpp -lpthread