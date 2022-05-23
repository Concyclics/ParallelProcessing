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

#include <mpi.h>
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

bool my_cmp(std::pair<int, int> A, std::pair<int, int> B)
{
    return A.second>B.second;
}

unsigned int thread_using;
std::string input_filename;
std::string output_filename;
unsigned int table_ctoi[255];
uint64_t buf_size[63];
char* buf;
std::pair<int, int> prefix_size[63];
bool my_cmp(std::pair<int, int> A, std::pair<int, int> B);

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

void i_to_s_buf(char* const buf, uint64_t &size, unsigned int T)
{
    uint64_t before_size=size;
    while(T)
    {
        buf[size++]=T%10+'0';
        T/=10;
    }
    std::reverse(buf+before_size, buf+size);
}

std::vector<std::unordered_map<std::string, unsigned int> > hash_split[63];
void work()
{
    auto begin=getTime();
    for(unsigned int i=0;i<63;i++)
    {
        hash_split[i].resize(thread_using);
    }
    
    //read file into buffer
    std::ifstream fin(input_filename, std::ios::binary);
    if(fin.is_open()==false)
    {
        std::cout<<"Fail to open the file!"<<input_filename<<"\n";
        return;
    }
    uint64_t read_buf_size=fin.seekg(0, std::ios::end).tellg();
    buf=(char*)malloc(read_buf_size+1);
    fin.seekg(0, std::ios::beg).read(buf, static_cast<std::streamsize>(read_buf_size));
    if(buf[read_buf_size-1]!='\n')
    {
        buf[read_buf_size++]='\n';
    }
    fin.close();
    auto end=getTime();
    std::cout << "read IO Time: " << end-begin << "ms" << std::endl;
    
    begin=getTime();
    //split buffer into N parts
    uint64_t split_part[thread_using+1];
    split_part[0]=0;
    split_part[thread_using]=read_buf_size;
    uint64_t each_part=read_buf_size/thread_using;
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
    
    delete [] buf;
    
    //merge
    for(int i=0;i<63;i++)
    {
        prefix_size[i].first=i;
        prefix_size[i].second=0;
        for(int j=1;j<thread_using;j++)
        {
            prefix_size[i].second+=hash_split[i][j].size();
        }
    }
    sort(prefix_size,prefix_size+63,my_cmp);
    #pragma omp parallel for num_threads(thread_using)
    for(unsigned int I=0;I<thread_using;I++)
    {
        for(int T=I;T<63;T+=thread_using)
        {
            int i=prefix_size[T].first;
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
        for(int T=I;T<63;T+=thread_using)
        {
            int i=prefix_size[T].first;
            buf_size[i]=0;
            for(auto &it:hash_split[i][0])
            {
                buf_size[i]+=it.first.size()+10;
            }
        }

    }
    end=getTime();
    std::cout << "work Time: " << end-begin << "ms" << std::endl;
}

void output_f()
{
    uint64_t size=0;
    uint64_t total_buf_size=0;
    #pragma omp  sections
    { 
        #pragma omp section
        {
            auto start=getTime();
            for(int i=0;i<63;i++)
            {
                total_buf_size+=buf_size[i];
            }
            buf=(char*)malloc(total_buf_size);
            for(int i=0;i<63;i++)
            {
                for(auto &each:hash_split[i][0])
                {
                    //fout<<each.first<<'\n'<<each.second<<'\n';
                    copy(each.first.begin(),each.first.end(),buf+size);
                    size+=each.first.size();
                    buf[size++]='\n';
                    i_to_s_buf(buf,size,each.second);
                    buf[size++]='\n';
                    
                }
            }
            
            for(int i=0;i<63;i++)
            {
                hash_split[i].shrink_to_fit();
            }
            
            auto end=getTime();

            std::cout<<"buffer process time"<<end-start<<"ms\n";
        }
        
        #pragma omp section
        {
            auto start=getTime();
            std::ofstream fout(output_filename, std::ios::binary);
            fout.seekp(0,std::ios::beg).write(buf, static_cast<std::streamsize>(size));
            fout.close();
            delete [] buf;
            auto end=getTime();
            std::cout<<"out IO time"<<end-start<<"ms\n";
        }
    }
}

void set_thread_using(unsigned int T)
{
    if(T==0||T>omp_get_num_procs())
    {
        thread_using=omp_get_num_procs();
    }
    else
    {
        thread_using=T;
    }
}

void run(std::string input="", std::string output="", unsigned int T=omp_get_num_procs())
{
    if(input!="")
    {
        input_filename=input;
    }
    if(output!="")
    {
        output_filename=output;
    }
    set_thread_using(T);
    init_ctoi();
    work();
    output_f();
}

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
    auto start = getTime();
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
    
    run(input_file, output_file, using_threads);
    auto end = getTime();
    std::cout << "Total Time: " << end-start << "ms" << std::endl;
    return 0;
}
