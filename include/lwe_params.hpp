#pragma once
#include <stdexcept>
#include <cmath>

struct LWEParams {
    size_t n;      // lattice dimension
    size_t m;      // number of samples
    int64_t q;     // modulus
    double sigma;  // Gaussian parameter
    double alpha;  // witness width parameter
    double beta;   // verification bound
    int64_t challenge_bound; // maximum challenge value
    
    LWEParams(size_t security_level = 128) {
        switch(security_level) {
            case 128:
                n = 512;
                m = 1024;
                q = 40961;
                sigma = 8.0;
                alpha = 4.0;
                challenge_bound = 20; // Significantly reduce challenge space
                beta = sigma * sqrt(n) * (challenge_bound + 1); // Adjust beta according to challenge bound
                break;
            default:
                throw std::invalid_argument("Unsupported security level");
        }
    }
};