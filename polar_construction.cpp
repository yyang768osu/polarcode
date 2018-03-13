#include "polar_construction.hpp"
#include <assert.h>
#include <vector>
#include <math.h>

bool comparator(const std::pair<double, int>& l, const std::pair<double, int>& r){ 
    return l.first > r.first; 
}

std::vector<bool> polar_construction_by_beta_expansion(int m, int k){
    assert(m>0);
    std::vector<bool> res(1<<m, false);
    std::vector<std::pair<double, int> > polarization_weight(1<<m, std::pair<double, int>(0.0,0));
    for(int x = 0; x < (1<<m); x++){
        int b = x;
        int i = 0;
        polarization_weight[x].second = x;
        while(b){
            polarization_weight[x].first += (b & 1)*pow(2, 0.25*i);
            i++;
            b>>=1;
        }
    }
    std::sort(polarization_weight.begin(), polarization_weight.end(), comparator);
    for(int i = 0; i < k; i++){
        res[polarization_weight[i].second] = true;
    }
    return res;
}