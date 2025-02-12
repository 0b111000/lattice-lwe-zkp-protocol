// src/lwe_utils.cpp
#include "lwe_utils.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

std::vector<int64_t> LWEUtils::sample_gaussian(
    const LWEParams& params,
    size_t size,
    double std_dev,
    std::mt19937& rng
) {
    std::vector<int64_t> result(size);
    std::normal_distribution<double> dist(0.0, std_dev);
    
    for (size_t i = 0; i < size; ++i) {
        double sample = 0;
        // Use Box-Muller transform
        for (int j = 0; j < 12; j++) { // Increase samples for better distribution
            sample += dist(rng);
        }
        sample /= 12;
        
        int64_t discrete_sample = static_cast<int64_t>(std::round(sample));
        
        // Centered modular reduction
        discrete_sample = ((discrete_sample % params.q) + params.q) % params.q;
        if (discrete_sample > params.q/2) {
            discrete_sample -= params.q;
        }
        result[i] = discrete_sample;
    }
    
    return result;
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
