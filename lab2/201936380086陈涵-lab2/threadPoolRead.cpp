/*
** Author: Concyclics(Chen Han)
** Date_Created: March 31, 2022
** Date_Modified: March 31, 2022
** Description: This program will group by strings. It's for parallel processing cource.
** compile(macOS/Clang): g++ -Ofast split.cpp -o split -lpthread
** compile(Linux/GCC): g++ -Ofast split.cpp -o split -lpthread
** run: ./split <input.txt>  <using thread|default: system's> <output.txt|default: (input_prefix)Result.txt>
** link: https://github.com/Concyclics/ParallelProcessing
*/

#include <stdio.h>
#include <pthread.h>
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
#include <utility>
#include <omp.h>
#include <malloc/malloc.h>
using namespace std;

//get timestamp by ms
int64_t getTime()
{
    int64_t timems = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timems;
}

//fast read and process
char* buffer;
int64_t buffer_size;
int64_t buffer_pos;
vector<int64_t> split_point;
vector<pthread_mutex_t> split_point_mutex;
int thread_num;
vector<vector<string> > split_result;

void init(int init_thread_num)
{
    thread_num = init_thread_num;
    buffer_pos = 0;
    split_point.resize(thread_num+1);
    split_point[0] = 0;
    split_point_mutex.resize(thread_num+1);
    for(int i=0;i<thread_num+1;i++)
    {
        pthread_mutex_init(&split_point_mutex[i], NULL);
    }
    split_result.resize(thread_num);
}

void* one_thread_preprocess(void* arg)
{
    int thread_id = *(int*)arg;
    pthread_mutex_lock(&split_point_mutex[thread_id]);
    uint64_t start = getTime();
    int64_t start_pos = split_point[thread_id];
    int64_t end_pos = split_point[thread_id+1];
    string tmp;
    for(int64_t i=start_pos;i<end_pos;i++)
    {
        if(buffer[i]=='\n')
        {
            split_result[thread_id].push_back(tmp);
            tmp.clear();
        }
        else
        {
            tmp += buffer[i];
        }
    }
    pthread_mutex_unlock(&split_point_mutex[thread_id]);
    uint64_t end = getTime();
    cout<<"thread "+to_string(thread_id)+"\t preprocess time: "+to_string(end-start)+" ms\n";
    return NULL;
}

void read(const char* filename)
{
    uint64_t start = getTime();
    std::ifstream in(filename, std::ios::binary);
    if (!in)
    {
        std::cout << "open file failed" << std::endl;
        return;
    }
    buffer_size = in.seekg(0, std::ios::end).tellg();
    buffer = (char*)malloc(buffer_size+1);
    buffer[buffer_size] = '\n';
    int64_t each = buffer_size/thread_num;
    for (int i = 1; i < thread_num; i++)
    {
        split_point[i] = split_point[i-1] + each;
    }
    split_point[thread_num] = buffer_size;
    for(int i = 0; i <= thread_num; i++)
    {
        pthread_mutex_lock(&split_point_mutex[i]);
    }
    pthread_t thread_id[thread_num];
    int thread_id_arg[thread_num];
    for(int i=0;i<thread_num;i++)
    {
        thread_id_arg[i] = i;
        pthread_create(&thread_id[i], NULL, one_thread_preprocess, &thread_id_arg[i]);
    }
    for(int i = 0; i < thread_num; i++)
    {
        in.seekg(split_point[i], std::ios::beg).read(buffer+split_point[i], split_point[i+1]-split_point[i]);
        while(buffer[split_point[i+1]-1] != '\n')
        {
            split_point[i+1]--;
        }
        pthread_mutex_unlock(&split_point_mutex[i]);
    }
    in.close();
    uint64_t end = getTime();
    std::cout << "read file cost: " << end-start << "ms" << std::endl;
    for(int i = 0; i < thread_num; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
    //delete[] buffer;
}

int main(int argc, char* argv[])
{
    uint64_t start = getTime();
    init(256);
    read("80M_low.txt");
    uint64_t end = getTime();
    std::cout << "total cost: " << end-start << "ms" << std::endl;
    return 0;
}