// src/lwe_utils.cpp
#include "lwe_utils.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <random>

std::vector<int64_t> LWEUtils::sample_gaussian(
    const LWEParams& params,
    size_t size,
    double std_dev,
    std::mt19937 rng
) {
    std::vector<int64_t> result(size);
    
    // Box-Muller transform with proper scaling
    auto sample_gaussian_real = [&rng, std_dev]() -> double {
        std::uniform_real_distribution<double> uniform(0.0, 1.0);
        
        double u1 = uniform(rng);
        double u2 = uniform(rng);
        
        double radius = std::sqrt(-2.0 * std::log(u1));
        double theta = 2.0 * M_PI * u2;
        
        return std_dev * radius * std::cos(theta);
    };
    
    // Discrete Gaussian sampling with proper scaling
    auto sample_discrete_gaussian = [&sample_gaussian_real, q = params.q]() -> int64_t {
        double sample = sample_gaussian_real();
        int64_t discrete = static_cast<int64_t>(std::round(sample));
        
        // Centered modular reduction
        discrete = ((discrete % q) + q) % q;
        if (discrete > q/2) {
            discrete -= q;
        }
        return discrete;
    };
    
    // Generate samples
    for (size_t i = 0; i < size; ++i) {
        result[i] = sample_discrete_gaussian();
    }
    
    return result;
}


std::vector<int64_t> LWEUtils::sample_gaussian_with_seed(
    const LWEParams& params,
    size_t size,
    double std_dev,
    uint32_t seed
) {
    std::mt19937 rng(seed);
    return sample_gaussian(params, size, std_dev, rng);
}

std::vector<std::vector<int64_t>> LWEUtils::generate_matrix_A(
    const LWEParams& params,
    std::mt19937& rng
) {
    std::vector<std::vector<int64_t>> A(params.m, std::vector<int64_t>(params.n));
    std::uniform_int_distribution<int64_t> dist(0, params.q - 1);
    
    for (size_t i = 0; i < params.m; ++i) {
        for (size_t j = 0; j < params.n; ++j) {
            A[i][j] = dist(rng);
        }
    }
    
    return A;
}

bool LWEUtils::check_norm(const std::vector<int64_t>& vec, double bound) {
    double norm_squared = 0.0;
    
    for (const auto& x : vec) {
        // Center around q/2
        int64_t centered = x;
        if (centered > bound/2) {
            centered -= bound;
        }
        double val = static_cast<double>(centered);
        norm_squared += val * val;
    }
    
    double norm = std::sqrt(norm_squared);
    std::cout << "Vector norm: " << norm << " (bound: " << bound << ")" << std::endl;
    return norm <= bound;
}


std::vector<int64_t> LWEUtils::matrix_vector_mul(
    const std::vector<std::vector<int64_t>>& A,
    const std::vector<int64_t>& x,
    int64_t q
) {
    if (A.empty() || A[0].size() != x.size()) {
        throw std::invalid_argument("Invalid dimensions for matrix-vector multiplication");
    }
    
    std::vector<int64_t> result(A.size(), 0);
    
    for (size_t i = 0; i < A.size(); ++i) {
        int64_t sum = 0;
        for (size_t j = 0; j < x.size(); ++j) {
            int64_t a_ij = centered_mod(A[i][j], q);
            int64_t x_j = centered_mod(x[j], q);
            int64_t prod = (a_ij * x_j) % q;
            sum = centered_mod(sum + prod, q);
        }
        result[i] = sum;
    }
    
    return result;
}
