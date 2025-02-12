// src/verifier.cpp
#include "verifier.hpp"
#include "lwe_utils.hpp"
#include <random>
#include <chrono>
#include <iostream>

Verifier::Verifier(const LWEParams& params)
    : params_(params)
    , rng_(std::chrono::system_clock::now().time_since_epoch().count()) 
{}

Challenge Verifier::challenge() {
    Challenge chal;
    std::uniform_int_distribution<int64_t> dist(1, params_.challenge_bound);
    chal.c = dist(rng_);
    return chal;
}

bool Verifier::verify(
    const LWEInstance& instance,
    const Commitment& commitment,
    const Challenge& challenge,
    const Response& response
) {
    
    if (response.z.empty() || response.r_prime.empty()) {
        std::cout << "Invalid response received" << std::endl;
        return false;
    }
    
    if (!LWEUtils::check_norm(response.z, params_.beta)) {
        std::cout << "Response norm check failed" << std::endl;
        return false;
    }
    
    // Compute Az + r'
    auto Az = LWEUtils::matrix_vector_mul(instance.A, response.z, params_.q);
    std::vector<int64_t> left_side(params_.m);
    for (size_t i = 0; i < params_.m; ++i) {
        left_side[i] = LWEUtils::centered_mod(
            Az[i] + response.r_prime[i],
            params_.q
        );
    }
    
    // Compute u + ct
    std::vector<int64_t> right_side(params_.m);
    for (size_t i = 0; i < params_.m; ++i) {
        int64_t u_i = LWEUtils::centered_mod(commitment.u[i], params_.q);
        int64_t t_i = LWEUtils::centered_mod(instance.t[i], params_.q);
        int64_t prod = (challenge.c * t_i) % params_.q;
        right_side[i] = LWEUtils::centered_mod(u_i + prod, params_.q);
    }

    // Check equality
    for (size_t i = 0; i < params_.m; ++i) {
        if (left_side[i] != right_side[i]) {
            return false;
        }
    }
    
    return true;
}