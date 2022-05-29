#include "VirtualMemory.h"

#include <cstdio>
#include <cassert>
#include <iostream> //TODO: delte it at tne and


int main(int argc, char **argv) {
    VMinitialize();
    word_t value;
          VMwrite(13, 3);
          VMread(6,&value);
          VMread(31,&value);
          printTree();
//    for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i) {
//        printf("writing to %llu\n", (long long int) i);
//        VMwrite(5 * i * PAGE_SIZE, i);
//        if (i==0){
//            printTree();
//        }
    }

//    for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i) {
//        word_t value;
//        VMread(5 * i * PAGE_SIZE, &value);
//        printf("reading from %llu %d\n", (long long int) i, value);
//        assert(uint64_t(value) == i);
//    }
//    printf("success\n");

//    return 0;
//}
