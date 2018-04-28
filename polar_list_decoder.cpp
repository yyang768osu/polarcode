#include "polar_list_decoder.hpp"
#include "utilities.hpp"
#include "polar_encoder.hpp"
#include <vector>
#include <cmath>
#include <assert.h>
#include <utility>
#include <iostream>
#include <algorithm>


inline double llr_sign(double x){
    return (x>0) - (x<0);
}

inline double f(double a, double b)
{
    return llr_sign(a) * llr_sign(b) * std::min(std::abs(a), std::abs(b));
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
    inactiveArrayIndexStack = std::vector<std::stack<int> >(m+1, std::stack<int>());
    for(int lambda = 0; lambda <= m; lambda++){
        for(int s = L-1; s >=0; s--){
            inactiveArrayIndexStack[lambda].push(s);
        }
    }
    // construct arrayReferenceCount of P and B
    arrayReferenceCount = std::vector<std::vector<int> >(m+1, std::vector<int>(L, 0));
    // construct path to B/P array index
    pathToArrayIndex = std::vector<std::vector<int> >(m+1, std::vector<int>(L, 0));
    // construct path metric vector
    pathMetric = std::vector<double>(L, 0);
}

void PolarListDecoder::initializeMemory(void){
    // clean up active path 
    while(inactivePathIndexStack.size()) inactivePathIndexStack.pop();
    for(int list = L-1; list >= 0; list--){
        inactivePathIndexStack.push(list);
        activePath[list] = false;
    }
    // clean up inactiveArrayIndexStack and array reference count
    for(int lambda = 0; lambda <= m; lambda++){
        while(inactiveArrayIndexStack[lambda].size()) inactiveArrayIndexStack[lambda].pop();
        for(int s = L-1; s >=0; s--){
            inactiveArrayIndexStack[lambda].push(s);
            arrayReferenceCount[lambda][s] = 0;
        }
    }
    // clean up path metric
    for(int list = 0; list < L; list++){
        pathMetric[list] = 0;
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
    for(int lambda=0; lambda<=m; lambda++){
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
    for(int lambda=0; lambda<=m; lambda++){
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
    for(int lambda=0; lambda<=m; lambda++){
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
    assert(activePath[list]);
    int array_index = pathToArrayIndex[lambda][list];
    return P[lambda][array_index][beta];
}

double & PolarListDecoder::Pset(int lambda, int list, int beta){
    assert(activePath[list]);
    // obtain the array index corresponds to lambda and list
    int array_index = pathToArrayIndex[lambda][list];
    // if there is only one reference to that array, then simply return value in P
    if(arrayReferenceCount[lambda][array_index]==1) return P[lambda][array_index][beta];
    // if there is more than one reference to that array, then grab a new array and clone
    arrayReferenceCount[lambda][array_index] -- ;
    int new_array_index = inactiveArrayIndexStack[lambda].top();
    inactiveArrayIndexStack[lambda].pop();
    pathToArrayIndex[lambda][list] = new_array_index;
    assert(arrayReferenceCount[lambda][new_array_index] == 0);
    arrayReferenceCount[lambda][new_array_index] ++ ;
    // below is supposed to be deep copy; give it the benefit of the doubt
    P[lambda][new_array_index] = P[lambda][array_index];
    B[lambda][new_array_index] = B[lambda][array_index]; 
    return P[lambda][new_array_index][beta];
}

int PolarListDecoder::Bget(int lambda, int list, int beta, int parity){
    assert(activePath[list]);
    int array_index = pathToArrayIndex[lambda][list];
    return B[lambda][array_index][beta][parity];
}

int & PolarListDecoder::Bset(int lambda, int list, int beta, int parity){
    assert(activePath[list]);
    // obtain the array index corresponds to lambda and list
    int array_index = pathToArrayIndex[lambda][list];
    // if there is only one reference to that array, then simply return value in B
    if(arrayReferenceCount[lambda][array_index]==1) return B[lambda][array_index][beta][parity];
    // if there is more than one reference to that array, then grab a new array and clone
    arrayReferenceCount[lambda][array_index] -- ;
    int new_array_index = inactiveArrayIndexStack[lambda].top();
    inactiveArrayIndexStack[lambda].pop();
    pathToArrayIndex[lambda][list] = new_array_index;
    assert(arrayReferenceCount[lambda][new_array_index] == 0);
    arrayReferenceCount[lambda][new_array_index] ++ ;
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

void  PolarListDecoder::updatePathFrozen(int phase){
    for(int list=0; list<L; list++){
        if(!activePath[list]) continue;
        if(Pget(m,list,0)<0){
            pathMetric[list] += std::abs(Pget(m,list,0));
        }
        Bset(m,list,0,phase % 2) = 0;
    }
}

void  PolarListDecoder::updatePathNonFrozen(int phase){
    // build length 2*L branchPaths vector contaning branched path metrics
    std::vector<std::pair<double, int> > originalBranchedPath(2*L, std::pair<double,int>());
    for(int list = 0; list< L; list++){
        if(activePath[list]){
            if(Pget(m,list,0)<0){
                originalBranchedPath[list].first = pathMetric[list] + std::abs(Pget(m,list,0));
                originalBranchedPath[list+L].first = pathMetric[list];
            }else{
                originalBranchedPath[list].first = pathMetric[list];
                originalBranchedPath[list+L].first = pathMetric[list] + std::abs(Pget(m,list,0));
            }
        }else{
            originalBranchedPath[list].first = -1;
            originalBranchedPath[list+L].first = -1;
        }
        originalBranchedPath[list].second = list;
        originalBranchedPath[list+L].second = list+L;
    }
    std::vector<std::pair<double, int> > sortedBranchedPath = originalBranchedPath;
    std::sort(sortedBranchedPath.begin(), sortedBranchedPath.end());

    // Among the 2*L branched paths, mark those that survive
    std::vector<std::vector<bool> > survivedBranchedPath(L, std::vector<bool>(2, false));
    int numInactivePath = inactivePathIndexStack.size();
    for(int list_rank = 2*numInactivePath; list_rank< std::min(L+2*numInactivePath, 2*L); list_rank++){
        int list = sortedBranchedPath[list_rank].second;
        survivedBranchedPath[list % L][list / L] = true;
    }
    // kill the list with none of the branch survived 
    for(int list = 0; list< L; list++){
        if(activePath[list]){
            if(!survivedBranchedPath[list][0] && !survivedBranchedPath[list][1]){
                killPath(list);
            }
        }
    } 
    for(int list = 0; list< L; list++){
        if(survivedBranchedPath[list][0] && survivedBranchedPath[list][1]){
            int new_list = clonePath(list);
            Bset(m,list,0,phase % 2) = 0;
            pathMetric[list] = originalBranchedPath[list].first;
            Bset(m,new_list,0,phase % 2) = 1;
            pathMetric[new_list] = originalBranchedPath[list+L].first;
        }else if(survivedBranchedPath[list][0]){
            Bset(m,list,0,phase % 2) = 0;
            pathMetric[list] = originalBranchedPath[list].first;
        }else if(survivedBranchedPath[list][1]){
            Bset(m,list,0,phase % 2) = 1;
            pathMetric[list] = originalBranchedPath[list+L].first;
        }
    }
}

int findMinElementIndex(std::vector<double> input){
    if(input.size()==0) return -1;
    int res = 0;
    double min_so_far = input[0];
    for(int i = 1; i<input.size();i++){
        if(input[i] < min_so_far){
            min_so_far = input[i];
            res = i;
        }
    }
    return res;
}

std::vector<int> PolarListDecoder::decode(std::vector<double> llr, std::vector<bool> info_mask)
{
    // assert input
    assert(llr.size()==info_mask.size());
    assert(llr.size()==n);
    // apply bit reversal interleaver on the llr; assuming the encoder applied natural order
    bit_reversal_interleaver(llr);
    // get the inital path (during which memory is re-initialized)
    int ini_list = getInitialPath();
    // populate the llrs in the right most layer of the memory for the intial list
    for (int beta = 0; beta < n; beta++)
    {
        Pset(0,ini_list,beta) = llr[beta];
    }
    // main sc-list loop
    for (int phi = 0; phi < n; phi++)
    {
        recursivelyCalcP(m, phi);
        if (info_mask[phi] == 0)
        {
        //    Bset(m,0,phi % 2) = 0;
            updatePathFrozen(phi);
        }
        else
        {
        //    Bset(m,0,phi % 2) = Pget(m,0) > 0 ? 0 : 1;
            updatePathNonFrozen(phi);
        }
        recursivelyCalcB(m, phi);
    
        std::cout<< "ActivePath at bit " << phi << "  " ;
        for(int i=0; i<activePath.size();i++) std::cout << activePath[i] << " ";
        std::cout << std::endl;

        std::cout<< "PathMetric at bit " << phi << "  " ;
        for(int i=0; i<pathMetric.size();i++) std::cout << pathMetric[i] << " ";
        std::cout << std::endl;
    }
    int final_list = findMinElementIndex(pathMetric);

    std::vector<int> bits(n, 0);
    for(int beta=0; beta<n; beta++){
        bits[beta] = Bget(0,final_list,beta,0);
    }
    bit_reversal_interleaver(bits);
    polar_encoder(bits);
    return bits;
}