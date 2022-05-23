#include<iostream>
#include<cstdio>
#include<cstring>
#include<utility>
#include<vector>
using namespace std;
typedef unsigned int uint;
typedef unsigned long long ull;
const uint char_cnt=127;
struct Trie
{
    Trie* to[char_cnt]={nullptr};
    ull cnt=0;

    void insert(string &str)
    {
        ins(str.c_str(),this);
        return ;
    }

    void ins(const char* ch,Trie *node)
    {
        if (*ch=='\0')
        {
            node->cnt+=1;
            return ;
        }
        if (node->to[*ch]==nullptr)
            node->to[*ch]=new Trie;
        ins(ch+1,node->to[*ch]);
        return ;
    }

    void search(vector< pair<string,int> > &ans)
    {
        string str;
        ans.clear();
        dfs(this,str,ans);
        return;
    }

    void dfs(Trie* node,string &str,vector< pair<string,int> > &ans)
    {
        if (node->cnt!=0)
            ans.push_back(make_pair(str,node->cnt));
        for (int i=0;i<char_cnt;++i)
        {
            if (node->to[i]!=nullptr)
            {
                str.push_back(i);
                dfs(node->to[i],str,ans);
                str.pop_back();
            }
        }
        return ;
    }
};


