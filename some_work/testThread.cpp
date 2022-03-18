#include <thread>
#include <iostream>
#include <chrono>
#include <stdio.h>
using namespace std;

void fun(int a)
{
    cout << "fun " << a << endl;
}

int main()
{
    int a = 1;
    thread t1(fun, a);
    t1.join();
    return 0;
}

//gcc -o testThread testThread.cpp -lstdc++ -std=c++17 -lpthread
//g++ -std=c++11 -o testThread testThread.cpp -lpthread