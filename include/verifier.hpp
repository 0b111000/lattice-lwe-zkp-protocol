// include/verifier.hpp
#pragma once
#include "lwe_proof.hpp"
#include <random>

class Verifier {
public:
    Verifier(const LWEParams& params);
    
    // Second move of the protocol
    Challenge challenge();
    
    // Verification step
    bool verify(
        const LWEInstance& instance,
        const Commitment& commitment,
        const Challenge& challenge,
        const Response& response
    );

private:
    LWEParams params_;
    std::mt19937 rng_;
};