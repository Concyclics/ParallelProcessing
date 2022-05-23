/*
** Author: Concyclics(Chen Han)
** Date_Created: March 23, 2022
** Date_Modified: March 23, 2022
** Description: This program will group by strings. It's for parallel processing cource.
** compile(macOS/Clang): g++ -Ofast -Xpreprocessor -fopenmp -I/usr/local/include -L/usr/local/lib -lomp split.cpp -o split
** compile(Linux/GCC): g++ -Ofast -fopenmp split.cpp -o split
** run: ./split <input.txt>  <using thread|default: system's> <output.txt|default: (input_prefix)Result.txt>
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
#include <utility>
#pragma GCC optimize("Ofast")

//get timestamp by ms
int64_t getTime()
{
    int64_t timems = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timems;
}

class Hash_Solution
{
private:
    unsigned int thread_using;
    std::string input_filename;
    std::string output_filename;
    unsigned int table_ctoi[255];

    void init_ctoi()
    {
        memset(table_ctoi, 0, sizeof(table_ctoi));
        unsigned int cnt=0;
        for(char c='0';c<='9';c++)
        {
            table_ctoi[c] = cnt;
            cnt++;
        }
        for(char c='a';c<='z';c++)
        {
            table_ctoi[c] = cnt;
            cnt++;
        }
        for(char c='A';c<='Z';c++)
        {
            table_ctoi[c] = cnt;
            cnt++;
        }
    }
    
    std::vector<std::unordered_map<std::string, unsigned int> > hash_split[63];
    void work()
    {
        for(unsigned int i=0;i<63;i++)
        {
            hash_split[i].resize(thread_using);
        }
        
        //read file into buffer
        std::ifstream fin(input_filename, std::ios::binary);
        if(fin.is_open()==false)
        {
            std::cout<<"Fail to open the file!\n";
            return;
        }
        std::vector<char> buf(fin.seekg(0, std::ios::end).tellg());
        fin.seekg(0, std::ios::beg).read(&buf[0], static_cast<std::streamsize>(buf.size()));
        if(buf[buf.size()-1]!='\n') buf.push_back('\n');
        fin.close();
        
        //split buffer into N parts
        uint64_t split_part[thread_using+1];
        split_part[0]=0;
        split_part[thread_using]=buf.size();
        uint64_t each_part=buf.size()/thread_using;
        for(unsigned int i=1;i<thread_using;i++)
        {
            split_part[i]=split_part[i-1]+each_part;
        }
        
        for(unsigned int i=1;i<thread_using;i++)
        {
            while(buf[split_part[i]]!='\n')
            {
                split_part[i]++;
            }
        }

        //multicore-hashing
        #pragma omp parallel for num_threads(thread_using)
        for(unsigned int i=0;i<thread_using;i++)
        {
            std::string tmp;
            tmp.clear();
            for(uint64_t idx=split_part[i];idx<split_part[i+1];idx++)
            {
                if(buf[idx]=='\n')
                {
                    if(hash_split[table_ctoi[tmp[0]]][i].find(tmp)==hash_split[table_ctoi[tmp[0]]][i].end())
                    {
                        hash_split[table_ctoi[tmp[0]]][i][tmp]=1;
                    }
                    else
                    {
                        hash_split[table_ctoi[tmp[0]]][i][tmp]++;
                    }
                    tmp.clear();
                }
                else
                {
                    tmp.push_back(buf[idx]);
                }
            }
        }
        buf.clear();
        buf.shrink_to_fit();
        
        //merge
        #pragma omp parallel for num_threads(thread_using)
        for(unsigned int i=0;i<63;i++)
        {
            for(unsigned int t=1;t<thread_using;t++)
            {
                for(auto &it:hash_split[i][t])
                {
                    if(hash_split[i][0].find(it.first)==hash_split[i][0].end())
                    {
                        hash_split[i][0][it.first]=it.second;
                    }
                    else
                    {
                        hash_split[i][0][it.first]+=it.second;
                    }
                }
            }
        }
    }
    
    void output()
    {
        std::ofstream fout(output_filename);
        for(int i=0;i<63;i++)
        {
            for(auto &each:hash_split[i][0])
            {
                fout<<each.first<<'\n'<<each.second<<'\n';
            }
        }
        fout.close();
    }

public:
    void set_thread_using(unsigned int thread_using)
    {
        if(thread_using == 0||thread_using>omp_get_num_procs())
        {
            this->thread_using = omp_get_num_procs();
        }
        else
        {
            this->thread_using = thread_using;
        }
    }
    
    unsigned int get_thread_using()
    {
        return thread_using;
    }
    
    Hash_Solution(std::string input_filename, std::string output_filename, unsigned int thread_using=0)
    {
        this->input_filename = input_filename;
        this->output_filename = output_filename;
        set_thread_using(thread_using);
        init_ctoi();
    }
    
    Hash_Solution()
    {
        set_thread_using(0);
        init_ctoi();
    }
    
    void run(std::string input_filename="", std::string output_filename="")
    {
        if(input_filename!="") this->input_filename = input_filename;
        if(output_filename!="") this->output_filename = output_filename;
        auto start = getTime();
        work();
        output();
        auto end = getTime();
        std::cout << "Time: " << end-start << "ms" << std::endl;
    }
};  

//my split function
std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

int main(int argc, char *argv[])
{
    if(argc<2)
    {
        std::cout << "Usage: ./split <input.txt>  <using thread|default: system's> <output.txt|default: (input_prefix)Result.txt>" << std::endl;
        return 0;
    }

    std::string input_file = argv[1];
    std::string output_file;
    unsigned int using_threads=0;
    output_file=split(input_file, '.')[0]+"Result.txt";
    
    if(argc>=4)
    {
        output_file = argv[3];
    }
    
    if(argc>=3)
    {
        using_threads=std::atoi(argv[2]);
    }
    
    Hash_Solution A(input_file, output_file, using_threads);
    A.run();
    
}
