#ifndef POLAR_LIST_DECODER_HPP
#define POLAR_LIST_DECODER_HPP

#include <vector>
#include <stack>

//https://arxiv.org/abs/1206.0050
class PolarListDecoder{
    int m;
    int L;
    int n;
    // data structres:
    // P, B
    // inactivePathIndexStack, activePath
    // inactiveArrayIndexStack, arrayReferenceCount
    // pathToArrayIndex
    // pathMetric

    // data structure: P[lambda:0-->m][array:0-->L-1][branch: 0-->1<<(m-lambda)]
    std::vector<std::vector<std::vector<double> > > P;
    // data structure: B[lambda:0-->m][array:0-->L-1][branch: 0-->1<<(m-lambda)][parity]
    std::vector<std::vector<std::vector<std::vector<int> > > > B;
    // data structure: stack recording inactive path index
    std::stack<int> inactivePathIndexStack;
    // data structure: size L array recording activate path index
    std::vector<bool> activePath;
    // data structure: size m vector of stack recroding inactive array index
    std::vector<std::stack<int> > inactiveArrayIndexStack;
    // data structure: count the number of references to an array
    std::vector<std::vector<int> > arrayReferenceCount;
    // data structure: register the mapping from path to array index
    std::vector<std::vector<int> > pathToArrayIndex;
    // data structure: path metric
    std::vector<double> pathMetric;

    // initialize data structure
    void buildMemory(void);
    void initializeMemory(void);

    // low level list memory management function
    int getInitialPath(void);
    int clonePath(int list_index);
    void killPath(int list_index);
    
    // getter and setter for P and B
    double Pget(int lambda, int list, int beta);
    double & Pset(int lambda, int list, int beta);
    int Bget(int lambda, int list, int beta, int parity);
    int & Bset(int lambda, int list, int beta, int parity);
    void cloneArrayIfNecessary(int lambda, int path);
    
    // recursively update P for all active list
    void recursivelyCalcP(int lambda, int phi);
    // recursively update B for all active list
    void recursivelyCalcB(int lambda, int phi);

    // high level list metric management function
    void updatePathFrozen(int phase);
    void updatePathNonFrozen(int phase);

public:
    int get_n();
    PolarListDecoder(int m, int L);
    std::vector<int>  decode(std::vector<double> llr, std::vector<bool> info_mask);
};



#endif