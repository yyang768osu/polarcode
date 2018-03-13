#include <vector>
#include <assert.h>
#include "polar_decoder.hpp"
#include "utilities.hpp"
#include <cmath>

inline double f(double a, double b)
{
    return ((a > 0) - (a < 0)) * ((b > 0) - (b < 0)) * std::min(std::abs(a), std::abs(b));
}

inline double g(double a, double b, int bit)
{
    return bit = 1 ? b - a : b + a;
}


void recursivelyCalcP(int lambda, int phi, int m,
                      std::vector<std::vector<double> > &P, std::vector<std::vector<std::vector<int> > > &B)
{
    if(lambda == 0) return;
    if(phi % 2 == 0) recursivelyCalcP(lambda-1, phi/2, m, P, B);
    for(int beta = 0; beta < 1<<(m-lambda); beta++ ){
        if(phi % 2 == 0){
            P[lambda][beta] = f(P[lambda-1][2*beta], P[lambda-1][2*beta+1]);
        }else{
            int bit = B[lambda][beta][0];
            P[lambda][beta] = g(P[lambda-1][2*beta], P[lambda-1][2*beta+1], bit);
        }
    }
}

void recursivelyCalcB(int lambda, int phi, int m,
                      std::vector<std::vector<double> > &P, std::vector<std::vector<std::vector<int> > > &B)
{
    if(phi % 2 == 0) return;
    for(int beta=0; beta< (1<<(m-lambda)); beta++){
        B[lambda-1][2*beta][(phi/2)%2] = B[lambda][beta][0] ^ B[lambda][beta][1];
        B[lambda-1][2*beta+1][(phi/2)%2] = B[lambda][beta][1];
    }
    recursivelyCalcB(lambda-1, phi/2, m, P, B);
}


std::vector<int> polar_sc_decoder(std::vector<double> llr, std::vector<bool> info_mask)
{
    int n = llr.size();
    assert(n == info_mask.size());
    int m = log2_of_2power(n);
    std::vector<std::vector<double> > P;
    std::vector<std::vector<std::vector<std::vector<int> > > > B;
    for (int lambda = 0; lambda <= m; lambda++)
    {
        P.push_back(std::vector<double>(1 << (m - lambda), 0));
        B.push_back(std::vector<std::vector<int> >(1 << (m - lambda), std::vector<int>(2,0)));
    }
    for (int beta = 0; beta < n; beta++)
    {
        P[0][beta] = llr[beta];
    }
    for (int phi = 0; phi < n; phi++)
    {
        // calculate P[lambda][0]
        recursivelyCalcP(m, phi, m, P, B);
        if(info_mask[phi]==0){
            B[m][0][phi%2] = 0;
        }else{
            B[m][0][phi%2] = P[m][0]>0?0:1;
        }
        recursivelyCalcB(m, phi, m, P, B);
        // derive B[lambda][0]
        // update B
    }
}
