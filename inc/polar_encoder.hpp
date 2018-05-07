#ifndef POLAR_ENCODER_HPP
#define POLAR_ENCODER_HPP

#include <vector>

//https://arxiv.org/pdf/0807.3917.pdf
void polar_encoder(std::vector<int> & bit_sequence);

std::vector<int> polar_info_to_u_domain_mapping(std::vector<int> & info_bits, std::vector<bool> & info_bit_location);

#endif