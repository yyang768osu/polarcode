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

int PolarListDecoder::clonePath(int list_index){
    // grab an inactive path
    int new_list_index = inactivePathIndexStack.top();
    // activate the path
    inactivePathIndexStack.pop();
    activePath[new_list_index] = true;
    // register the array association
    for(int lambda=0; lambda<m; lambda++){
        int array_index = pathToArrayIndex[lambda][list_index];
        pathToArrayIndex[lambda][new_list_index] = array_index;
        arrayReferenceCount[lambda][array_index] ++;
    }
    return new_list_index;
}

void PolarListDecoder::killPath(int list_index){
    // deactive the path
    inactivePathIndexStack.push(list_index);
    activePath[list_index] = false;
    // clean up the array association
    for(int lambda=0; lambda<n; lambda++){
        int array_index = pathToArrayIndex[lambda][list_index];
        // decrement the array reference count
        arrayReferenceCount[lambda][array_index] --;
        // mark array as inactive if there is no reference left
        if(arrayReferenceCount[lambda][array_index] == 0){
            inactiveArrayIndexStack[lambda].push(array_index);
        }
    }
}

double PolarListDecoder::Pget(int lambda, int list, int beta){
    int array_index = pathToArrayIndex[lambda][list];
    return P[lambda][array_index][beta];
}

double & PolarListDecoder::Pset(int lambda, int list, int beta){
    // obtain the array index corresponds to lambda and list
    int array_index = pathToArrayIndex[lambda][list];
    // if there is only one reference to that array, then simply return value in P
    if(arrayReferenceCount[lambda][array_index]==1) return P[lambda][array_index][beta];
    // if there is more than one reference to that array, then grab a new array and clone
    arrayReferenceCount[lambda][array_index] -- ;
    int new_array_index = inactiveArrayIndexStack[lambda].top();
    inactiveArrayIndexStack[lambda].pop();
    // below is supposed to be deep copy; give it the benefit of the doubt
    P[lambda][new_array_index] = P[lambda][array_index];
    B[lambda][new_array_index] = B[lambda][array_index]; 
    return P[lambda][new_array_index][beta];
}

int PolarListDecoder::Bget(int lambda, int list, int beta, int parity){
    int array_index = pathToArrayIndex[lambda][list];
    return B[lambda][array_index][beta][parity];
}

int & PolarListDecoder::Bset(int lambda, int list, int beta, int parity){
    // obtain the array index corresponds to lambda and list
    int array_index = pathToArrayIndex[lambda][list];
    // if there is only one reference to that array, then simply return value in B
    if(arrayReferenceCount[lambda][array_index]==1) return B[lambda][array_index][beta][parity];
    // if there is more than one reference to that array, then grab a new array and clone
    arrayReferenceCount[lambda][array_index] -- ;
    int new_array_index = inactiveArrayIndexStack[lambda].top();
    inactiveArrayIndexStack[lambda].pop();
    // below is supposed to be deep copy; give it the benefit of the doubt
    P[lambda][new_array_index] = P[lambda][array_index];
    B[lambda][new_array_index] = B[lambda][array_index]; 
    return B[lambda][new_array_index][beta][parity];
}

void PolarListDecoder::recursivelyCalcB(int lambda, int phi)
{
    // if phi is even number, then further update in B is blocked
    if (phi % 2 == 0)
        return;
    // if phi is odd, proceed with update; loop through all branch number
    for (int beta = 0; beta < (1 << (m - lambda)); beta++)
    {
        // loop through all active path
        for( int list_index = 0; list_index < L; list_index++){
            if(activePath[list_index]){
                Bset(lambda - 1,list_index,2 * beta,(phi / 2) % 2) = Bget(lambda,list_index,beta,0) ^ Bget(lambda,list_index,beta,1);
                Bset(lambda - 1,list_index,2 * beta + 1,(phi / 2) % 2) = Bget(lambda,list_index,beta,1);
            }
        }
    }
    // keep updating the next layer: layer lambda-1
    recursivelyCalcB(lambda - 1, phi / 2);
}

void PolarListDecoder::recursivelyCalcP(int lambda, int phi)
{
    // if lambda == 0, then there is no need to calculate, it is always the same copy of LLRs
    if (lambda == 0) return;
    // if phi is even, then the next layer P data is not ready yet, recursive into deeper memory
    if (phi % 2 == 0)
        recursivelyCalcP(lambda - 1, phi / 2);
    // if phi is odd, then the next layer P data is ready; if phi is even the next layer P data is also ready because of the two lines of code above
    // start updating the P
    // loop through all branchs
    for (int beta = 0; beta < 1 << (m - lambda); beta++)
    {
        // loop through all active paths
        for (int list_index = 0; list_index < L; list_index++)
        {
            if (activePath[list_index])
            {
                if (phi % 2 == 0)
                {
                    Pset(lambda, list_index, beta) =
                        f(Pget(lambda - 1, list_index, 2 * beta), Pget(lambda - 1, list_index, 2 * beta + 1));
                }
                else
                {
                    int bit = Bget(lambda, list_index, beta, 0);
                    Pset(lambda, list_index, beta) =
                        g(Pget(lambda - 1, list_index, 2 * beta), Pget(lambda - 1, list_index, 2 * beta + 1), bit);
                }
            }
        }
    }
}