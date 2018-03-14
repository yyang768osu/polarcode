#include <iostream>
#include <vector>
#include "polar_encoder.hpp"
#include "utilities.hpp"
#include "channel.hpp"
#include "polar_construction.hpp"
#include "polar_decoder.hpp"

int main()
{
    std::vector<bool> info_location;
    info_location = polar_construction_by_beta_expansion(3, 4);
    for (int i = 0; i < info_location.size(); i++)
    {
        std::cout << "info_location " << i << " is " << info_location[i] << std::endl;
    }
    std::cout << std::endl;

    std::vector<int> info_bits;
    info_bits.push_back(1);
    info_bits.push_back(1);
    info_bits.push_back(0);
    info_bits.push_back(1);
    std::vector<int> u_domain_bits;
    u_domain_bits = polar_info_to_u_domain_mapping(info_bits, info_location);
    for (int i = 0; i < u_domain_bits.size(); i++)
    {
        std::cout << "u_domain_bits " << i << " is " << u_domain_bits[i] << std::endl;
    }
    std::cout << std::endl;

    polar_encoder(u_domain_bits);
    for (int i = 0; i < u_domain_bits.size(); i++)
    {
        std::cout << "x_domain_bits " << i << " is " << u_domain_bits[i] << std::endl;
    }
    std::cout << std::endl;

    std::vector<double> llr;
    llr = bits_to_llr(u_domain_bits, 0.5);
    for (int i = 0; i < u_domain_bits.size(); i++)
    {
        std::cout << "llr " << i << " is " << llr[i] << std::endl;
    }
    std::cout << std::endl;

    std::vector<int> bits;
    bits = polar_sc_decoder(llr, info_location);
    for (int i = 0; i <bits.size(); i++)
    {
        std::cout << "bits " << i << " is " << bits[i] << std::endl;
    }
    std::cout << std::endl;

    PolarScDecoder decoder(3);
    bits = decoder.polar_sc_decoder(llr, info_location);
    for (int i = 0; i <bits.size(); i++)
    {
        std::cout << "bits " << i << " is " << bits[i] << std::endl;
    }
    std::cout << std::endl;

}