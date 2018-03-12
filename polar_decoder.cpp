#include <vector>
#include <assert.h>
#include "polar_decoder.hpp"
#include "utilities.hpp"

std::vector<int> polar_sc_decoder(std::vector<double> llr, std::vector<bool> frozen_mask){
    int n = llr.size();
    assert(n == frozen_mask.size());
    int m = log2_of_2power(n);
    std::vector<vector <double>> P;
    for(int lambda = 0; lambda <= m; lambda++){
        P.push_back(std::vector<double>(1>>(m-lambda),0));
    }

}