#ifndef POLAR_ENCODER_HPP
#define POLAR_ENCODER_HPP

#include <vector>

void polar_encoder(std::vector<int> & bit_sequence);

std::vector<int> polar_info_to_u_domain_mapping(std::vector<int> & info_bits, std::vector<bool> & info_bit_location);

#endif