#include <vector>
#include <assert.h>
#include "utilities.hpp"


int bit_reversed(int bit, int bit_width){
    int res = 0;
    for(int i = 0; i < bit_width; i++){
        res <<= 1;
        res |= (bit & 1);
        bit >>= 1;
    }
    return res;
}

int log2_of_2power(int x){
    int res = -1;
    assert((x & (x-1)) == 0);
    while(x != 0){
        x >>= 1;
        res += 1;
    }
    return res;
}

template <typename T>
void bit_reversal_interleaver(std::vector<T> & sequence){
    int len = sequence.size();
    int m = log2_of_2power(len);
    std::vector<T> temp = sequence;
    for(int i = 0; i < len; i++){
        sequence[i] = temp[bit_reversed(i, m)];
    }
}

// explicit installation of template
template void bit_reversal_interleaver(std::vector<int>&);