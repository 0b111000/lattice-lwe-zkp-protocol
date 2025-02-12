// src/prover.cpp
#include "prover.hpp"
#include "lwe_utils.hpp"
#include <random>
#include <chrono>
#include <iostream>

Prover::Prover(const LWEParams& params)
    : params_(params)
    , rng_(std::chrono::system_clock::now().time_since_epoch().count()) 
{}

LWEInstance Prover::setup() {
    // Generate the public matrix A
    instance_.A = LWEUtils::generate_matrix_A(params_, rng_);
    
    // Sample the secret vector s
    witness_.s = LWEUtils::sample_gaussian(params_, params_.n, params_.alpha, rng_);
    
    // Sample the error vector e
    witness_.e = LWEUtils::sample_gaussian(params_, params_.m, params_.sigma, rng_);
    
    // Compute t = As + e with centered modular arithmetic
    auto As = LWEUtils::matrix_vector_mul(instance_.A, witness_.s, params_.q);
    instance_.t.resize(params_.m);
    for (size_t i = 0; i < params_.m; ++i) {
        instance_.t[i] = LWEUtils::centered_mod(
            As[i] + witness_.e[i],
            params_.q
        );
    }
    
    return instance_;
}

Commitment Prover::commit() {
    // Sample random vectors y and r
    y_ = LWEUtils::sample_gaussian(params_, params_.n, params_.sigma, rng_);
    r_ = LWEUtils::sample_gaussian(params_, params_.m, params_.sigma, rng_);
    
    // Compute u = Ay + r
    Commitment comm;
    comm.u = LWEUtils::matrix_vector_mul(instance_.A, y_, params_.q);
    for (size_t i = 0; i < params_.m; ++i) {
        comm.u[i] = (comm.u[i] + r_[i]) % params_.q;
    }
    
    return comm;
}

Response Prover::respond(const Challenge& challenge) {
    Response resp;
    
    // Compute z = y + c*s
    resp.z.resize(params_.n);
    for (size_t i = 0; i < params_.n; ++i) {
        int64_t s_i = LWEUtils::centered_mod(witness_.s[i], params_.q);
        int64_t y_i = LWEUtils::centered_mod(y_[i], params_.q);
        int64_t prod = (challenge.c * s_i) % params_.q;
        resp.z[i] = LWEUtils::centered_mod(y_i + prod, params_.q);
    }
    
    // Compute r' = r + c*e
    resp.r_prime.resize(params_.m);
    for (size_t i = 0; i < params_.m; ++i) {
        int64_t e_i = LWEUtils::centered_mod(witness_.e[i], params_.q);
        int64_t r_i = LWEUtils::centered_mod(r_[i], params_.q);
        int64_t prod = (challenge.c * e_i) % params_.q;
        resp.r_prime[i] = LWEUtils::centered_mod(r_i + prod, params_.q);
    }
    
    std::cout << "Challenge value: " << challenge.c << std::endl;
    
    if (!LWEUtils::check_norm(resp.z, params_.beta)) {
        std::cout << "Warning: Response norm too large!" << std::endl;
        resp.z.clear();
        resp.r_prime.clear();
    }
    
    return resp;
}