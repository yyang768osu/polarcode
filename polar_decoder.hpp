#ifndef POLAR_DECODER_HPP
#define POLAR_DECODER_HPP

#include <vector>

std::vector<int> polar_sc_decoder(std::vector<double> llr, std::vector<bool> frozen_mask);

#endif