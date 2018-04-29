#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <vector>

std::vector<double> bits_to_llr(std::vector<int> bit_seq, double stddev);

#endif