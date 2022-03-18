#pragma once
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <stdio.h>
#include <fstream>
using namespace std;
class Random {
public:
	Random(string fileName,size_t N,int low,int high) {
        this->fileName = fileName;
        this->answerFileName = "answer_" + fileName;
        this->N = N;
        this->low = low;
        this->high = high;
        srand(time(NULL));
        data = new string[this->N];
	}
    ~Random() {
        delete[]data;
    }
    void shuffle()
    {
        string* a = data;
        size_t n = N;
        string tmp;
        size_t index, i;
        //srand(time(NULL));
        for (i = 0; i < n; i++)
        {
            index = rand() % (n - i) + i;
            if (index != i)
            {
                tmp = a[i];
                a[i] = a[index];
                a[index] = tmp;
            }
        }
    }
    void writeArray() {
        for (size_t i = 0; i < N; i++) {
            outfile << data[i];
        }
    }
    void outfileOpen(string fileName){
        outfile.open(fileName);
    }
    void outfileClose() {
        outfile.close();
    }
    void outAnswerFileOpen() {
        outAnswerFile.open(this->answerFileName);
    }
    void outAnswerFileClose() {
        outAnswerFile.close();
    }
    void infileOpen(string fileName) {
        infile.open(fileName);
    }
    void infileClose() {
        infile.close();
    }
    void readArray() {
        for (size_t i = 0; i < N; i++) {
            infile >> data[i];
            data[i] += '\n';
        }
    }
    void print() {
        for (size_t i = 0; i < N; i++) {
            cout << data[i];
        }
    }
    void generate() {
        int len;
        for (size_t i = 0; i < N; i++) {
            len = 32 + rand() % 118;
            randstr(len);
            size_t j = low+rand() % high;
            if (i + j > N) {
                j = N - i;
            }
            outAnswerFile << tmp << "=" << j << "\n";
            tmp += "\n";
            for (size_t k = 0; k < j; k++) {
                outfile << tmp;
            }
            i += (j-1);
            tmp = "";
        }

    }
    void randstr(const int len)
    {
        size_t i;
        for (i = 0; i < len; ++i)
        {
            switch ((rand() % 3))
            {
            case 1:
                tmp += 'A' + rand() % 26;
                break;
            case 2:
                tmp += 'a' + rand() % 26;
                break;
            default:
                tmp += '0' + rand() % 10;
                break;
            }
        }
        //tmp += '\n';
    }

    size_t N;
    string fileName;
    string answerFileName;
    int low;
    int high;
    string tmp;
    ofstream outfile;
    ifstream  infile;
    ofstream outAnswerFile;
    string* data;
    
};