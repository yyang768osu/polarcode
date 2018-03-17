#include "polar_list_decoder.hpp"
#include <vector>
#include <cmath>
#include <assert.h>


inline double f(double a, double b)
{
    return ((a > 0) - (a < 0)) * ((b > 0) - (b < 0)) * std::min(std::abs(a), std::abs(b));
}

inline double g(double a, double b, int bit)
{
    return bit == 1 ? b - a : b + a;
}

PolarListDecoder::PolarListDecoder(int m, int L): m(m), L(L){
    n = (1<<m);  
    buildMemory();
}

void PolarListDecoder::buildMemory(void){
    // construct P and B
    for (int lambda = 0; lambda <= m; lambda++)
    {
        P.push_back(std::vector<std::vector<double> >());
        B.push_back(std::vector<std::vector<std::vector<int> > >());
        for(int s = 0; s < L; s++){
            P[lambda].push_back(std::vector<double>(1 << (m - lambda), 0));
            B[lambda].push_back(std::vector<std::vector<int> >(1 << (m - lambda), std::vector<int>(2)));
        }
    }
    // construct inactivePathIndexStack
    assert(inactivePathIndexStack.size()==0);
    for(int s = L-1; s >= 0; s--){
        inactivePathIndexStack.push(s);
    }
    // construct activePath bool array
    activePath = std::vector<bool>(L, false);
    // construct inactiveArrayIndexStack
    inactiveArrayIndexStack = std::vector<std::stack<int> >(m, std::stack<int>());
    for(int lambda = 0; lambda < m; lambda++){
        for(int s = L-1; s >=0; s--){
            inactiveArrayIndexStack[lambda].push(s);
        }
    }
    // construct arrayReferenceCount of P and B
    arrayReferenceCount = std::vector<std::vector<int> >(m, std::vector<int>(L, 0));
    // construct path to B/P array index
    pathToArrayIndex = std::vector<std::vector<int> >(m, std::vector<int>(L, 0));
}

void PolarListDecoder::initializeMemory(void){
    while(inactivePathIndexStack.size()) inactivePathIndexStack.pop();
    for(int list = L-1; list >= 0; list--){
        inactivePathIndexStack.push(list);
        activePath[list] = false;
    }
    for(int lambda = 0; lambda < m; lambda++){
        while(inactiveArrayIndexStack[lambda].size()) inactiveArrayIndexStack[lambda].pop();
        for(int s = L-1; s >=0; s--){
            inactiveArrayIndexStack[lambda].push(s);
            arrayReferenceCount[lambda][s] = 0;
        }
    }
}

int PolarListDecoder::getInitialPath(void){
    // intialize memory
    initializeMemory();
    // get the initial path
    int initialPathIndex = inactivePathIndexStack.top();
    inactivePathIndexStack.pop();
    // update activePath
    activePath[initialPathIndex] = true;
    // assign array to path
    for(int lambda=0; lambda<m; lambda++){
        int array_index = inactiveArrayIndexStack[lambda].top();
        inactiveArrayIndexStack[lambda].pop();
        pathToArrayIndex[lambda][initialPathIndex] = array_index;
        arrayReferenceCount[lambda][array_index] = 1;
    }
    return initialPathIndex;
}
