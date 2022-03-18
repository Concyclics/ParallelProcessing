#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <random>
#include <ctime>

std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

void randomData(std::string filename, int64_t size)
{
    std::ofstream file;
    file.open(filename);
    for (int64_t i = 0; i < size; i++)
    {
        file << rng() % 100 << "\n";
    }
    file.close();
}

int main()
{
    std::string filename = "data.txt";
    int64_t size = 1e8;
    randomData(filename, size);
    return 0;
}