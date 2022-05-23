import os
import pandas as pd
import platform

thread_count = 16

config_name='config.h'
config='#define Hash_A %d'
compile_cmd='g++ -Ofast split_hash_pthread_my_hash.cpp -o my_hash'

for i in range(31, 200):
    with open(config_name,'w') as f:
        f.write(config % i)
    os.system(compile_cmd)
    os.system('./my_hash 20M_low.txt 16')
        


    
        