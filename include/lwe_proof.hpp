// include/lwe_proof.hpp
#pragma once
#include "lwe_params.hpp"
#include <vector>

struct LWEInstance {
    std::vector<std::vector<int64_t>> A;  // public matrix
    std::vector<int64_t> t;               // public target vector
};

struct LWEWitness {
    std::vector<int64_t> s;  // secret vector
    std::vector<int64_t> e;  // error vector
};

struct Commitment {
    std::vector<int64_t> u;  // commitment vector
};

struct Challenge {
    int64_t c;  // challenge value
};

struct Response {
    std::vector<int64_t> z;   // response vector
    std::vector<int64_t> r_prime;  // randomness
};