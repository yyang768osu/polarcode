#include <iostream>
#include <vector>
#include <assert.h>
#include "polar_encoder.hpp"

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
std::vector<T> bit_reversal_interleaver(std::vector<T> sequence){
    int len = sequence.size();
    int m = log2_of_2power(len);
    std::vector<T> res(len, 0);
    for(int i = 0; i < len; i++){
        res[i] = sequence[bit_reversed(i, m)];
    }
    return res;
}

int main()
{
    int bit = 10;
    int rbit = bit_reversed(bit, 4);
    std::cout << "bit reserve of " << bit << " is " << rbit << std::endl;

    std::vector<int> sequence(8,0);
    for(int i = 0; i < sequence.size(); i++){
        sequence[i] = i;
        std::cout << "sequence " << i << " is " << sequence[i] << std::endl;
    }
    std::vector<int> interleaved_sequence;
    interleaved_sequence = bit_reversal_interleaver<int>(sequence);
    for(int i = 0; i < sequence.size(); i++){
        std::cout << "interleaved_sequence " << i << " is " << interleaved_sequence[i] << std::endl;
    }

    for(int i = 0; i < sequence.size(); i++){
        sequence[i] = i%2;
        std::cout << "sequence " << i << " is " << sequence[i] << std::endl;
    }
    polar_encoder(sequence);
    for(int i = 0; i < sequence.size(); i++){
        std::cout << "sequence " << i << " is " << sequence[i] << std::endl;
    }

}