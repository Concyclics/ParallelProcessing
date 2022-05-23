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
using namespace std;

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

//define of Trie Tree
class TrieTree
{
private:
    //define of Trie Tree Node
    class TrieTreeNode
    {
    private:
        vector<TrieTreeNode *> sons;
        TrieTreeNode * parent;
        unsigned int count;
        unsigned int prefixCount;
        bool IsLeaf;
        
    public:
        const unsigned int size;//size of characters
        
        TrieTreeNode(unsigned int const S=128)//size of characters
        :size(S)
        {
            sons.resize(size,nullptr);
            parent=nullptr;
            IsLeaf=true;
        }
        
        void setParent(TrieTreeNode *const P=nullptr)
        {
            parent=P;
        }
        
        TrieTreeNode * getParent() const
        {
            return parent;
        }
        
        bool isLeaf() const
        {
            return IsLeaf;
        }
        
        unsigned int getCount() const
        {
            return count;
        }
        
        unsigned int getPrefixCount() const
        {
            return prefixCount;
        }
        
        TrieTreeNode * getSon(unsigned int const charNO) const
        {
            if(charNO>=size)
            {
                return nullptr;
            }
            
            return sons[charNO];
            
        }
        
        void addCount(unsigned int const X=1)
        {
            count+=X;
            addPrefixCount(X);
        }
        
        void addPrefix(unsigned int const X=1)
        {
            prefixCount+=X;
        }
        
        void addPrefixCount(unsigned int const X=1)
        {
            TrieTreeNode * P=this;
            while(P!=nullptr)
            {
                P->addPrefix(X);
                P=P->getParent();
            }
        }
        
        TrieTreeNode * makeSon(unsigned int const charNO)
        {
            if(charNO>=size)
            {
                return nullptr;
            }
            
            if(sons[charNO]!=nullptr)
            {
                return sons[charNO];
            }
        
            
            TrieTreeNode * son=new TrieTreeNode(size);
            sons[charNO]=son;
            
            son->setParent(this);
            
            IsLeaf=false;
            
            return son;
        }
    };
    
    TrieTreeNode * root;
    
public:
    TrieTree(unsigned int const S=128)//size of characters
    {
        root=new TrieTreeNode(S);
    }
    
    bool empty() const
    {
        return root->isLeaf();
    }
    
    void insert(string const &S,const unsigned int number=1)
    {
        TrieTreeNode * now=root;
        for(auto &ch:S)
        {
            now=now->makeSon(ch);
        }
        now->addCount(number);
    }
    
    unsigned int countPrefix(string const &S) const
    {
        TrieTreeNode * now=root;
        for(auto &ch:S)
        {
            now=now->makeSon(ch);
            if(now==nullptr)break;
        }

        if(now==nullptr)
        {
            return 0;
        }
        else
        {
            return now->getPrefixCount();
        }
            
    }
    
    unsigned int count(string const &S) const
    {
        TrieTreeNode * now=root;
        for(auto &ch:S)
        {
            now=now->makeSon(ch);
            if(now==nullptr)break;
        }
        
        if(now==nullptr)
        {
            return 0;
        }
        else
        {
            return now->getCount();
        }
    }
    
    bool exist(string const &S) const
    {
        TrieTreeNode * now=root;
        for(auto &ch:S)
        {
            now=now->makeSon(ch);
            if(now==nullptr)break;
        }
        
        if(now==nullptr)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    
    unsigned int size() const
    {
        return root->getPrefixCount();
    }
    
};


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

    

    T1=getTime();
    output(final_ans_hash, output_file);
    T2=getTime();
    std::cout<<"Output time: "<<T2-T1<<" ms"<<std::endl;
}
