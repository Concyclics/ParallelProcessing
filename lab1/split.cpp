/*
** Author: Concyclics(Chen Han)
** Date_Created: March 17, 2022
** Date_Modified: March 17, 2022
** Description: This program will group by strings. It's for parallel processing cource.
** compile(macOS/Clang): g++ -Ofast -Xpreprocessor -fopenmp -I/usr/local/include -L/usr/local/lib -lomp split.cpp -o split 
** compile(Linux/GCC): g++ -Ofast -fopenmp split.cpp -o split
** run: ./split input_filename output_filename
** link: https://github.com/Concyclics/ParallelProcessing
*/

#include <stdio.h>
#include <omp.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <thread>

//using threads N
#define N 16

//Time clock
int64_t getTime()
{
    int64_t timems = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timems;
}

//store the origin strings
std::vector<std::string> origin;

//read strings from file
void readFile(std::string fileName)
{
    origin.reserve(2e7);
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line))
    {
        origin.push_back(line);
    }
}

//group strings with hash
std::unordered_map<std::string, int> hashTable;
void work_hash()
{
    #pragma omp parallel for num_threads(N)
    for(auto &s:origin)
    {
        if(hashTable.find(s) == hashTable.end())
        {
            hashTable[s] = 1;
        }
        else
        {
            hashTable[s]++;
        }
    }
}

//group strings with hash multi-thread
std::unordered_map<std::string, int> hashTable_split[N];//for each thread
std::unordered_map<std::string, int> final_ans_hash;//for final answer
void work_hash_multithread()
{
    //split by N threads
    //each thread process in[split_edge[i], split_edge[i+1])
    int one_part = origin.size()/N;
    int split_edge[N+1];
    split_edge[0]=0;
    split_edge[N]=origin.size();
    for(int i=1;i<N;i++)
    {
        split_edge[i] = split_edge[i-1] + one_part;
    }

    //parallel group
    #pragma omp parallel for num_threads(N)
    for(int i=0;i<N;i++)
    {
        for(int j=split_edge[i];j<split_edge[i+1];j++)
        {
            if(hashTable_split[i].find(origin[j]) == hashTable_split[i].end())
            {
                hashTable_split[i][origin[j]] = 1;
            }
            else
            {
                hashTable_split[i][origin[j]]++;
            }
        }
    }

    //merge
    for(int i=0;i<N;i++)
    {
        for(auto &s:hashTable_split[i])
        {
            if(final_ans_hash.find(s.first) == final_ans_hash.end())
            {
                final_ans_hash[s.first] = s.second;
            }
            else
            {
                final_ans_hash[s.first] += s.second;
            }
        }
    }
}

//group strings with sort
std::unordered_map<std::string, int> sortTable;
void work_sort()
{
    std::sort(origin.begin(), origin.end());
    
    int i,cnt;
    std::string str="\0";
    for(auto &s:origin)
    {
        if(str == s)
        {
            cnt++;
        }
        else
        {
            if(str != "\0")
            {
                sortTable[str] = cnt;
            }
            str = s;
            cnt = 1;
        }
    }
    sortTable[str] = cnt;
}

//group strings with sort multi-thread
std::unordered_map<std::string, int> final_ans_sort;//for final answer
void work_sort_multithread()
{
    //split by N threads
    //each thread process in[split_edge[i], split_edge[i+1])
    int one_part = origin.size()/N;
    int split_edge[N+1];
    split_edge[0]=0;
    split_edge[N]=origin.size();
    for(int i=1;i<N;i++)
    {
        split_edge[i] = split_edge[i-1] + one_part;
    }

    //parallel sort
    #pragma omp parallel for num_threads(N)
    for(int i=0;i<N;i++)
    {
        sort(origin.begin()+split_edge[i], origin.begin()+split_edge[i+1]);
    }

    //merge
    #pragma omp parallel for num_threads(N)
    for(int i=0;i<N;i+=2)
    {
        merge(origin.begin()+split_edge[i], origin.begin()+split_edge[i+1], origin.begin()+split_edge[i+1], origin.begin()+split_edge[i+2], origin.begin()+split_edge[i]);
    }

    for(int i=0;i<N;i+=4)
    {
        merge(origin.begin()+split_edge[i], origin.begin()+split_edge[i+2], origin.begin()+split_edge[i+2], origin.begin()+split_edge[i+4], origin.begin()+split_edge[i]);
    }

    for(int i=0;i<N;i+=8)
    {
        merge(origin.begin()+split_edge[i], origin.begin()+split_edge[i+4], origin.begin()+split_edge[i+4], origin.begin()+split_edge[i+8], origin.begin()+split_edge[i]);
    }

    //merge
    std::string str="\0";
    int cnt=0;
    for(auto &s:origin)
    {
        if(str == s)
        {
            cnt++;
        }
        else
        {
            if(str != "\0")
            {
                final_ans_sort[str] = cnt;
            }
            str = s;
            cnt = 1;
        }
    }
    final_ans_sort[str] = cnt;
}

void output(std::unordered_map<std::string, int> &table, std::string fileName="output.txt")
{
    std::ofstream file(fileName);
    for(auto &s:table)
    {
        file << s.first << " " << s.second << std::endl;
    }
}


int main(int argc, char *argv[])
{
    int64_t T1, T2;

    if(argc<3)
    {
        std::cout << "Usage: ./a.out <input.txt> <output.txt>" << std::endl;
        return 0;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    T1=getTime();
    readFile(input_file);
    T2=getTime();
    std::cout<<"Read file time: "<<T2-T1<<" ms"<<std::endl;

    /*
    T1=getTime();
    work_hash();
    T2=getTime();
    std::cout<<"Hash time: "<<T2-T1<<" ms"<<std::endl;
    */

    T1=getTime();
    work_hash_multithread();
    T2=getTime();
    std::cout<<"Hash multithread time: "<<T2-T1<<" ms"<<std::endl;

    /*
    T1=getTime();
    work_sort();
    T2=getTime();
    std::cout<<"Sort time: "<<T2-T1<<" ms"<<std::endl;
    */

    T1=getTime();
    work_sort_multithread();
    T2=getTime();
    std::cout<<"Sort multithread time: "<<T2-T1<<" ms"<<std::endl;

    T1=getTime();
    output(final_ans_hash, output_file);
    T2=getTime();
    std::cout<<"Output time: "<<T2-T1<<" ms"<<std::endl;
}
