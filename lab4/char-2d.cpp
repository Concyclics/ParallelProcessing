#pragma GCC optimize(2)
#include<iostream>
#include<cstdio>
#include<stdlib.h>
#include<thread>
#include<fstream>
#include<unordered_map>
#include<malloc/malloc.h>
#include<string.h>
using namespace std;
typedef unsigned long long ull;
typedef unsigned int uint;
typedef unsigned short uss;

char ctoi[255];
char itoc[255];
char *buf;
char *ans,*cache;
ull file_size;
uint cpu_cnt;
ull *ind;

struct char_cmp 
{
	bool operator () (const char* a,const char* b) const 
	{
		return strcmp(a, b)==0;
	}
};
struct Hash_map
{
	ull operator () (const char* str) const
	{
		ull hash=0,seed=131;
		while(*str)
		{
			hash=hash*seed+(*str);
			++str;
		}
		return hash;
	}
};

unordered_map <char*,ull,Hash_map,char_cmp> M[16][64];

int64_t getTime()
{
    int64_t timems=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return timems;
}

void initial()
{
    int i;
    for (i=1;i<255;++i)
    {
        if (i>='0' && i<='9')
            ctoi[i]=i-'0'+1;
        if (i>='a' && i<='z')
            ctoi[i]=i-'a'+11;
        if (i>='A' && i<='Z')
            ctoi[i]=i-'A'+37;
        ctoi['\r']=64;
    }
    for (i=1;i<=62;++i)
    {
        if (i>=1 && i<=10)
            itoc[i]=i-1+'0';
        if (i>=11 && i<=36)
            itoc[i]=i-11+'a';
        if (i>=37 && i<=62)
            itoc[i]=i-37+'A';
        itoc[64]='\r';
    }
}

void init(const char* InputFile,char* &buf)
{
	ifstream fin(InputFile,ios::binary);
	file_size=fin.seekg(0, std::ios::end).tellg();
	buf=(char *)malloc(file_size+2);
	fin.seekg(0,ios::beg).read(buf,static_cast<streamsize>(file_size));
	buf[file_size]='\r';
	buf[file_size+1]='\n';
    fin.close();
}

void spilit()
{
	ull len=file_size/cpu_cnt;
	
	ind[0]=0;
	ind[cpu_cnt]=file_size;
	for (ull i=1;i<cpu_cnt;++i)
	{
		ull j=ind[i-1]+len;
		while (buf[j]!='\n')
			++j;
		ind[i]=j;
	}
}

void convert(char *st,char *en)
{
	while (st<en)
	{
		*st=ctoi[*st];
		++st;
	}
}
void thread_convert()
{
	thread T[cpu_cnt];
	for (ull i=0;i<cpu_cnt;++i)
		T[i]=thread(convert,buf+ind[i],buf+ind[i+1]);
	for (ull i=0;i<cpu_cnt;++i)
		T[i].join();
	return ;
}

void solve(uint id,char *st,char *en)
{
	char *now=st;
	while (st<en)
	{
		++M[id][*st][st];
		while (*st)
			++st;
		++st;
	}
}
void thread_solve()
{
	thread T[cpu_cnt];
	for (ull i=0;i<cpu_cnt;++i)
		T[i]=thread(solve,i,buf+ind[i],buf+ind[i+1]);
	for (ull i=0;i<cpu_cnt;++i)
		T[i].join();
	return ;
}

void Count(uint id)
{
	for (int i=id;i<63;i=i+cpu_cnt)
		for (int j=1;j<cpu_cnt;++j)
			for (auto &k:M[j][i])
				M[0][i][k.first]+=k.second;
	
}
void thread_count()
{
	thread T[cpu_cnt];
	for (ull i=0;i<cpu_cnt;++i)
		T[i]=thread(Count, i);
	for (ull i=0;i<cpu_cnt;++i)
		T[i].join();
	return ;
}

void my_itoa(ull num,char *st)
{
	char *now=st;
	while (num)
	{
		*now=itoc[num%10];
		++now;
		num/=10;
	}
	*now='\0';
	reverse(st,now);
}

void opt(const char *OutputFile)
{
	ans=(char *)malloc(file_size*1.2);
	cache=ans;
	char *ptr;
	for (int i=1;i<63;++i)
		for (auto &j:M[0][i])
		{
			ptr=j.first;
			while (*ptr)
			{
				*ptr=itoc[*ptr];
				++ptr;
			}
			strcpy(ans,j.first);
			while (*ans)
				++ans;
			*ans='=';
			++ans;
			my_itoa(j.second,ans);
			while (*ans)
				++ans;
			*ans='\n';
			++ans;
		}
	ofstream fout(OutputFile,ios::binary);
	fout.seekp(0,ios::beg).write(cache,static_cast<std::streamsize>(ans-cache));
	fout.close();
}

int main(int argc,char* argv[])
{	
	
	int64_t st_time=getTime();
	char* InputFile;
    char* OutputFile;
	
    if(argc<3)
    {
        InputFile="5M_high.txt";
        OutputFile="1.ans";
        cpu_cnt=16;
    }
    else
    {
	    InputFile=argv[1];
	    cpu_cnt=atoi(argv[2]);
	    OutputFile=argv[3];
	}
	
	initial();
	
    ind=(ull *)malloc( (cpu_cnt+1)*sizeof(ull) );
    init(InputFile,buf);
    spilit();
    thread_convert();
    thread_solve();
    thread_count();
    
    
    opt(OutputFile);
    ofstream fout("test-ans.txt",ios::app);
	int64_t en_time=getTime();
    fout<<cpu_cnt<<'\t'<<en_time-st_time<<'\n';
    return 0;
}
