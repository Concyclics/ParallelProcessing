#include <cstdio>
#include "random.h"
int main(int argc,char *argv[])
{
    //printf("%s 向你问好!\n", "randomString");
    string fileName = argv[1];//文件名(20M_low.txt)
    size_t N = atoll(argv[2]);//字符串数量
    int low= atoi(argv[3]);//频率最低值
    int high= atoi(argv[4]);//频率最高值
    Random* testModel = new Random(fileName, N, low, high);
    //Random* testModel = new Random("myfile.txt",16,1,3);

    testModel->outAnswerFileOpen();

    testModel->outfileOpen(fileName);
    testModel->generate();
    testModel->outfileClose();

    testModel->infileOpen(fileName);
    testModel->readArray();
    //testModel->print();
    testModel->shuffle();
    testModel->infileClose();

    testModel->outfileOpen(fileName);
    testModel->writeArray();
    testModel->outfileClose();

    testModel->outAnswerFileClose();

    delete testModel;
    return 0;
}