// include/lwe_utils.hpp
#pragma once
#include "lwe_params.hpp"
#include <random>

class LWEUtils {
public:
    static std::vector<int64_t> sample_gaussian(
        const LWEParams& params,
        size_t size,
        double std_dev,
        std::mt19937 rng
    );

    static std::vector<int64_t> sample_gaussian_with_seed(
        const LWEParams& params,
        size_t size,
        double std_dev,
        uint32_t seed
    );
    
    static std::vector<std::vector<int64_t>> generate_matrix_A(
        const LWEParams& params,
        std::mt19937& rng
    );
    
    static bool check_norm(
        const std::vector<int64_t>& vec,
        double bound
    );
    
    static std::vector<int64_t> matrix_vector_mul(
        const std::vector<std::vector<int64_t>>& A,
        const std::vector<int64_t>& x,
        int64_t q
    );

    static int64_t centered_mod(int64_t x, int64_t q) {
        x = ((x % q) + q) % q;
        if (x > q/2) {
            x -= q;
        }
        return x;
    }

    static int64_t positive_mod(int64_t x, int64_t q) {
        return ((x % q) + q) % q;
    }
};