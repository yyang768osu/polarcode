#ifndef POLAR_DECODER_HPP
#define POLAR_DECODER_HPP

#include <vector>

std::vector<int> polar_sc_decoder(std::vector<double> llr, std::vector<bool> frozen_mask);

class PolarScDecoder{
    int m;
    int n;
    std::vector<std::vector<double> > P;
    std::vector<std::vector<std::vector<int> > > B;
    void recursivelyCalcP(int lambda, int phi);
    void recursivelyCalcB(int lambda, int phi);
public:
    PolarScDecoder(int m);
    std::vector<int> polar_sc_decoder(std::vector<double> llr, std::vector<bool> info_mask);
};

#endif