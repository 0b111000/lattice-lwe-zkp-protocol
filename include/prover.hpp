// include/prover.hpp
#pragma once
#include "lwe_proof.hpp"
#include <random>

class Prover {
public:
    Prover(const LWEParams& params);
    
    // Generate public instance and keep witness
    LWEInstance setup();
    
    // First move of the protocol
    Commitment commit();
    
    // Third move of the protocol
    Response respond(const Challenge& challenge);

private:
    LWEParams params_;
    std::mt19937 rng_;
    LWEInstance instance_;
    LWEWitness witness_;
    std::vector<int64_t> y_;  // commitment randomness
    std::vector<int64_t> r_;  // commitment error
};