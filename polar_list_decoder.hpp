#ifndef POLAR_LIST_DECODER_HPP
#define POLAR_LIST_DECODER_HPP

#include <vector>

class PolarListDecoder{
    int m;
    int n;
    std::vector<std::vector<std::vector<double> > > P;
    std::vector<std::vector<std::vector<std::vector<int> > > > B;
    void recursivelyCalcP(int lambda, int phi);
    void recursivelyCalcB(int lambda, int phi);
    double Pget(int lambda, int list, int beta);
    double & Pset(int lambda, int list, int beta);
    int Bget(int lambda, int list, int beta, int parity);
    int & Bset(int lambda, int list, int beta, int parity);
public:
    PolarListDecoder(int m);
    std::vector<int> decode(std::vector<double> llr, std::vector<bool> info_mask);
};

#endif