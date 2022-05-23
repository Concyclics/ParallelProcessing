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
#include <atomic>
#include <omp.h>
#include <mutex>
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
#include "trie.hpp"
#include "trie.h"
#include "config.h"

//using threads N
#ifndef N
#define N 8
#endif

//get timestamp by ms
int64_t getTime()
{
    int64_t timems = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timems;
}

//read strings from file
std::vector<std::string> readFile(std::string fileName)
{
    //read the file by character into memory
    std::ifstream fin(fileName, std::ios::binary);
    std::vector<char> buf(fin.seekg(0, std::ios::end).tellg());
    fin.seekg(0, std::ios::beg).read(&buf[0], static_cast<std::streamsize>(buf.size()));
    
    //store the origin strings
    std::vector<std::string> origin;  
    origin.reserve(2e7);
    std::string line;

    for(auto c: buf)
    {
        if(c != '\n')
        {
            line.push_back(c);
        }
        else
        {
            origin.push_back(line);
            line.clear();
        }
    }
    return origin;
}
/*
//fast read binary from file and multi-process into strings
std::vector<std::string> readFile_multi(std::string fileName)
{
    //if the file is larger than 1GB, we can use multi-process to read it
    //else, we can use single-process to read it
    int64_t T1, T2;
    T1=getTime();
    std::ifstream fin(fileName, std::ios::binary);
    int64_t char_size=fin.seekg(0, std::ios::end).tellg();
    if(char_size<=(1<<30))
    {
        fin.close();
        return readFile(fileName);
    }

    //read the file by character into memory
    std::vector<char> buf(char_size);
    fin.seekg(0, std::ios::beg).read(&buf[0], static_cast<std::streamsize>(buf.size()));
    fin.close();
    T2=getTime();
    std::cout<<"read file time: "<<T2-T1<<"ms"<<std::endl;
    //split the file into N parts
    int64_t split[N+1];
    split[0] = 0;
    split[N] = buf.size();
    int64_t each = buf.size() / N;
    for (int i = 1; i < N; i++)
    {
        split[i] = split[i - 1] + each;
    }

    for(int i=1;i<N;i++)
    {
        //move to \n
        while(buf[split[i]]!='\n')
        {
            split[i]++;
        }
    }

    //read the strings
    std::vector<std::string> strings;
    strings.reserve(2e7);
    omp_lock_t lock;
    omp_init_lock(&lock);
    #pragma omp parallel for num_threads(N)
    for(int i=0;i<N;i++)
    {
        std::string tmp;
        for(int64_t j=split[i];j<split[i+1];j++)
        {
            if(buf[j]!='\n')
            {
                tmp.push_back(buf[j]);
            }
            else
            {
                omp_set_lock(&lock);
                strings.push_back(tmp);
                omp_unset_lock(&lock);
                tmp.clear();
            }
        }
    }

    return strings;
}
*/
//out put answer from unordered_map
template <typename T>
void output(T &answer, std::string fileName)
{
    std::ofstream out(fileName);
    for (auto &i : answer)
    {
        out << i.first << " " << i.second << std::endl;
    }
}

//group strings with hash multi-thread
std::unordered_map<std::string, int> work_hash_multithread(std::vector<std::string> &origin)
{
    std::unordered_map<std::string, int> hashTable_split[N];//for each thread
    std::unordered_map<std::string, int> final_ans_hash;//for final answer
    
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
        hashTable_split[i].reserve(split_edge[i+1]-split_edge[i]);
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
    //final_ans_hash.reserve(origin.size());
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
    return final_ans_hash;
}

//group strings with prefix split and hash
std::vector<std::pair<std::string, int> > work_prefix_hash_multithread(std::vector<std::string> &origin)
{
    std::unordered_map<std::string, int> hashTable_split[N];//for each thread
    std::vector<std::pair<std::string, int> > final_ans_hash;//for final answer

    //split by prefix
    std::vector<std::string> prefix_split[N];
    for(int i=0;i<N;i++)
    {
        //prefix_split[i].reserve(origin.size());
    }
    for(auto &s:origin)
    {
        prefix_split[s[0]%N].push_back(s);
    }

    //hash with unordered_map
    #pragma omp parallel for num_threads(N)
    for(int i=0;i<N;i++)
    {
        hashTable_split[i].reserve(prefix_split[i].size());
        for(auto &s:prefix_split[i])
        {
            if(hashTable_split[i].find(s) == hashTable_split[i].end())
            {
                hashTable_split[i][s] = 1;
            }
            else
            {
                hashTable_split[i][s]++;
            }
        }
    }

    //write final ans into vector
    //final_ans_hash.reserve(origin.size());
    for(int i=0;i<N;i++)
    {
        for(auto &s:hashTable_split[i])
        {
            final_ans_hash.push_back(std::make_pair(s.first, s.second));
        }
    }
    return final_ans_hash;
}

//group strings with sort multi-thread
std::vector<std::pair<std::string, int> > work_sort_multithread(std::vector<std::string> &origin)
{
    std::vector<std::pair<std::string, int> > final_ans_sort;//for final answer
    
    //split by N threads
    //each thread process in[split_edge[i], split_edge[i+1])
    int one_part = origin.size()/N;
    int split_edge[2*(N+1)];
    split_edge[0]=0;
    split_edge[N]=origin.size();
    for(int i=N+1;i<2*(N+1);i++)
    {
        split_edge[i]=split_edge[N];
    }
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
    for(int merge_size=1;merge_size<N;merge_size*=2)
    {
        #pragma omp parallel for num_threads(N)
        for(int i=0;i<N;i+=2*merge_size)
        {
            std::vector<std::string> temp1(origin.begin()+split_edge[i], origin.begin()+split_edge[i+merge_size]);
            std::vector<std::string> temp2(origin.begin()+split_edge[i+merge_size], origin.begin()+split_edge[i+2*merge_size]);
            merge(temp1.begin(), temp1.end(), temp2.begin(), temp2.end(), origin.begin()+split_edge[i]);
        }
    }

    //write answer into map
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
                final_ans_sort.push_back(std::make_pair(str, cnt));
            }
            str = s;
            cnt = 1;
        }
    }
    if(str != "\0")
    {
        final_ans_sort.push_back(std::make_pair(str, cnt));
    }
    return final_ans_sort;
}

//group strings with split by prefix then sort multi-thread
std::vector<std::pair<std::string, int> > work_prefix_sort_multithread(std::vector<std::string> &origin)
{    
    //split by prefix-one char
    std::vector<std::string> origin_split[N];
    for(auto &s:origin)
    {
        origin_split[s[0]%N].push_back(s);
    }

    //parallel sort
    #pragma omp parallel for num_threads(N)
    for(int i=0;i<N;i++)
    {
        sort(origin_split[i].begin(), origin_split[i].end());
    }

    std::vector<std::pair<std::string, int> > final_ans_prefix_sort;//for final answer
    final_ans_prefix_sort.reserve(origin.size());
    //write answer into map
    std::string str="\0";
    int cnt=0;
    for(int i=0;i<N;i++)
    {
        if(origin_split[i].empty()) continue;
        for(auto &s:origin_split[i])
        {
            if(str == s)
            {
                cnt++;
            }
            else
            {
                if(str != "\0")
                {
                    final_ans_prefix_sort.push_back(std::make_pair(str, cnt));
                }
                str = s;
                cnt = 1;
            }
        }
    }
    if(str != "\0")
    {
        final_ans_prefix_sort.push_back(std::make_pair(str, cnt));
    }
    return final_ans_prefix_sort;
}

//group strings with one-thread trie tree
std::vector<std::pair<std::string, int> > work_trie_tree(std::vector<std::string> &origin)
{
    TrieTree trie_tree;
    for(auto &s:origin)
    {
        trie_tree.insert(s);
    }

    std::cout<<"trie tree build done"<<std::endl;

    std::vector<std::pair<std::string, int> > final_ans_trie=dfs_tire_tree::search_trie(trie_tree);
    std::cout<<"trie tree search done"<<std::endl;

    return final_ans_trie;
}

int main(int argc, char *argv[])
{
    int64_t T1, T2;

    if(argc<3)
    {
        std::cout << "Usage: ./split <input.txt> <output.txt>" << std::endl;
        return 0;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    T1=getTime();
    std::vector<std::string> origin = readFile(input_file);
    T2=getTime();
    std::cout<<"Read file time: "<<T2-T1<<" ms"<<std::endl;
    /*
    freopen("logs.txt", "w", stdout);

    T1=getTime();
    std::unordered_map<std::string, int> final_ans_hash = work_hash_multithread(origin);
    T2=getTime();
    std::cout<<"Hash multithread time: "<<T2-T1<<" ms"<<std::endl;

    T1=getTime();
    std::vector<std::pair<std::string, int> > final_ans_prefix_hash = work_prefix_hash_multithread(origin);
    T2=getTime();
    std::cout<<"Prefix hash multithread time: "<<T2-T1<<" ms"<<std::endl;

    T1=getTime();
    std::vector<std::pair<std::string, int> > final_ans_sort = work_sort_multithread(origin);
    T2=getTime();
    std::cout<<"Sort multithread time: "<<T2-T1<<" ms"<<std::endl;

    T1=getTime();
    std::vector<std::pair<std::string, int> > final_ans_prefix_sort = work_prefix_sort_multithread(origin);
    T2=getTime();
    std::cout<<"Prefix sort multithread time: "<<T2-T1<<" ms"<<std::endl;*/

    T1=getTime();
    final_ans_trie = work_trie_tree(origin);
    T2=getTime();
    std::cout<<"Trie tree time: "<<T2-T1<<" ms"<<std::endl;

    //fclose(stdout);

    //T1=getTime();
    //output(final_ans, output_file);
    //T2=getTime();
    //std::cout<<"Output time: "<<T2-T1<<" ms"<<std::endl;

    //std::cout<<final_ans_hash.size()<<std::endl;
    //std::cout<<final_ans_prefix_hash.size()<<std::endl;
    //std::cout<<final_ans_sort.size()<<std::endl;
    //std::cout<<final_ans_prefix_sort.size()<<std::endl;
    //std::cout<<final_ans_trie.size()<<std::endl;
}
