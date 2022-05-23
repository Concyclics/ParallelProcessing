/*
** Author: Concyclics(Chen Han)
** Date_Created: March 19, 2022
** Date_Modified: March 19, 2022
** Description: This is my trie tree template
** link: https://github.com/Concyclics/ParallelProcessing
*/

#include <string>
#include <vector>
#include <string.h>
#include <unordered_map>
#include <map>
#include <utility>
#include <queue>

#ifndef Trie_Tree
#define Trie_Tree

//define of Trie Tree
class TrieTree
{
public:
    //define of Trie Tree Node
    class TrieTreeNode
    {
    private:
        TrieTreeNode * parent;
        unsigned int count;
        unsigned int prefixCount;
     
        bool IsLeaf;
        
    public: 
        std::map<unsigned int, TrieTreeNode *> sons;
        TrieTreeNode()
        {
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
        
        TrieTreeNode * getSon(unsigned int const charNO)
        {
            if(sons.find(charNO)==sons.end())
            {
                return nullptr;
            }
            else
            {
                return sons[charNO];
            }
            return nullptr;
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
            if(sons.find(charNO)==sons.end())
            {
                TrieTreeNode * newSon=new TrieTreeNode();
                newSon->setParent(this);
                sons[charNO]=newSon;
                IsLeaf=false;
                return newSon;
            }
            else
            {
                return sons[charNO];
            }
            return nullptr;
        }

        void clear()
        {
            for(auto &son:sons)
            {
                son.second->clear();
                delete son.second;
            }
        }

        ~TrieTreeNode()
        {
            clear();
        }
    };
    
    TrieTree()//size of characters
    {
        root=new TrieTreeNode();
    }
    
    bool empty() const
    {
        return root->isLeaf();
    }
    
    void insert(std::string const &S, const unsigned int number=1)
    {
        TrieTreeNode * now=root;
        for(auto &ch:S)
        {
            now=now->makeSon(ch);
        }
        now->addCount(number);
    }
    
    unsigned int countPrefix(std::string const &S) const
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
    
    unsigned int count(std::string const &S) const
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
    
    bool exist(std::string const &S) const
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

    void clear()
    {
        root->setParent(nullptr);
        root->clear();
        delete root;
        root=new TrieTreeNode();
    }
    
    ~TrieTree()
    {
        clear();
    }

    TrieTreeNode * getRoot()
    {
        return root;
    }

    private:
        TrieTreeNode * root;
};

#endif

#ifndef dfs_search_trie
#define dfs_search_trie

namespace dfs_tire_tree
{
    std::vector<std::pair<std::string, int> > search;

    void init(int size=2e7)
    {
        search.clear();
        search.reserve(size);
    }

    std::string tmp;

    void dfs(TrieTree::TrieTreeNode *now)
    {
        if(now->getCount()!=0)
        {
            search.push_back(std::make_pair(tmp, now->getCount()));
        }

        for(auto &son:now->sons)
        {
            tmp.push_back(char(son.first));
            dfs(son.second);
            tmp.pop_back();
        }
    }

    std::vector<std::pair<std::string, int> > search_trie(TrieTree &trie)
    {
        init();
        std::cout<<"searching..."<<std::endl;
        dfs(trie.getRoot());
        std::cout<<"searching finished"<<std::endl;
        return search;
    }
}

#endif