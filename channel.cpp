#include <random>
#include <vector>
#include "channel.hpp"

std::vector<double> bits_to_llr(std::vector<int> bit_seq, double stddev){
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(0, stddev);
    std::vector<double> res(bit_seq.size(), 0);
    for(int i = 0; i < bit_seq.size(); i++){
        res[i] = 2*(double)bit_seq[i] - 1 + distribution(generator);
        res[i] = -2*res[i]/(stddev*stddev);
    }
    return res;
}