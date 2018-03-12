#include <iostream>
#include <vector>
#include "polar_encoder.hpp"
#include "utilities.hpp"
#include "channel.hpp"

int main()
{
    std::vector<int> sequence(8,0);
    for(int i = 0; i < sequence.size(); i++){
        sequence[i] = i;
        std::cout << "sequence " << i << " is " << sequence[i] << std::endl;
    }
    bit_reversal_interleaver<int>(sequence);
    for(int i = 0; i < sequence.size(); i++){
        std::cout << "interleaved_sequence " << i << " is " << sequence[i] << std::endl;
    }

    for(int i = 0; i < sequence.size(); i++){
        sequence[i] = i%2;
        std::cout << "sequence " << i << " is " << sequence[i] << std::endl;
    }
    polar_encoder(sequence);
    for(int i = 0; i < sequence.size(); i++){
        std::cout << "sequence " << i << " is " << sequence[i] << std::endl;
    }

    std::vector<double> llr;
    llr = bits_to_llr(sequence, 0.5);
     for(int i = 0; i < sequence.size(); i++){
        std::cout << "sequence " << i << " is " << llr[i] << std::endl;
    }   

}