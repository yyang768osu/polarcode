#include <vector>
#include <assert.h>
#include "polar_decoder.hpp"
#include "utilities.hpp"
#include "polar_encoder.hpp"
#include <cmath>

inline double f(double a, double b)
{
    return ((a > 0) - (a < 0)) * ((b > 0) - (b < 0)) * std::min(std::abs(a), std::abs(b));
}

inline double g(double a, double b, int bit)
{
    return bit == 1 ? b - a : b + a;
}

double PolarScDecoder::Pget(int lambda, int beta){
    return P[lambda][beta];
}
double & PolarScDecoder::Pset(int lambda, int beta){
    return P[lambda][beta];
}
int PolarScDecoder::Bget(int lambda, int beta, int parity){
    return B[lambda][beta][parity];
}
int & PolarScDecoder::Bset(int lambda, int beta, int parity){
    return B[lambda][beta][parity];
}

PolarScDecoder::PolarScDecoder(int m): m(m){
    n = (1<<m);
    for (int lambda = 0; lambda <= m; lambda++)
    {
        P.push_back(std::vector<double>(1 << (m - lambda), 0));
        B.push_back(std::vector<std::vector<int> >(1 << (m - lambda), std::vector<int>(2)));
    }    
}

void PolarScDecoder::recursivelyCalcP(int lambda, int phi)
{
    if (lambda == 0) return;
    if (phi % 2 == 0)
        recursivelyCalcP(lambda - 1, phi / 2);
    for (int beta = 0; beta < 1 << (m - lambda); beta++)
    {
        if (phi % 2 == 0)
        {
            P[lambda][beta] = f(P[lambda - 1][2 * beta], P[lambda - 1][2 * beta + 1]);
        }
        else
        {
            int bit = B[lambda][beta][0];
            P[lambda][beta] = g(P[lambda - 1][2 * beta], P[lambda - 1][2 * beta + 1], bit);
        }
    }
}

void PolarScDecoder::recursivelyCalcB(int lambda, int phi)
{
    if (phi % 2 == 0)
        return;
    for (int beta = 0; beta < (1 << (m - lambda)); beta++)
    {
        B[lambda - 1][2 * beta][(phi / 2) % 2] = B[lambda][beta][0] ^ B[lambda][beta][1];
        B[lambda - 1][2 * beta + 1][(phi / 2) % 2] = B[lambda][beta][1];
    }
    recursivelyCalcB(lambda - 1, phi / 2);
}

std::vector<int> PolarScDecoder::decode(std::vector<double> llr, std::vector<bool> info_mask)
{
    assert(llr.size()==info_mask.size());
    assert(llr.size()==n);
    bit_reversal_interleaver(llr);
    for (int beta = 0; beta < n; beta++)
    {
        P[0][beta] = llr[beta];
    }
    for (int phi = 0; phi < n; phi++)
    {
        recursivelyCalcP(m, phi);
        if (info_mask[phi] == 0)
        {
            B[m][0][phi % 2] = 0;
        }
        else
        {
            B[m][0][phi % 2] = P[m][0] > 0 ? 0 : 1;
        }
        recursivelyCalcB(m, phi);
    }
    std::vector<int> bits(n, 0);
    for(int beta=0; beta<n; beta++){
        bits[beta] = B[0][beta][0];
    }
    bit_reversal_interleaver(bits);
    polar_encoder(bits);
    return bits;
}