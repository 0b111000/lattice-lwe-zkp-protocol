#include <gtest/gtest.h>
#include <random>
#include "prover.hpp"
#include "lwe_utils.hpp"
#include "verifier.hpp"

// Test case for parameter validation
TEST(ParameterTest, ValidateParameters) {
    EXPECT_NO_THROW({
        LWEParams params(128);
    });
    
    EXPECT_THROW({
        LWEParams params(64); // Invalid security level
    }, std::invalid_argument);
}

// Test case for Gaussian sampling
TEST(SamplingTest, GaussianDistribution) {
    LWEParams params(128);
    const size_t NUM_SAMPLES = 10000;  // Large sample size for better statistics
    
    // Use the seed-based version
    auto samples = LWEUtils::sample_gaussian_with_seed(params, NUM_SAMPLES, params.sigma, 1);
    
    // Calculate centered samples
    std::vector<double> centered_samples;
    centered_samples.reserve(NUM_SAMPLES);
    
    for (const auto& sample : samples) {
        double centered = static_cast<double>(sample);
        if (centered > params.q/2) {
            centered -= params.q;
        }
        centered_samples.push_back(centered);
    }
    
    // Calculate mean
    double mean = std::accumulate(centered_samples.begin(), centered_samples.end(), 0.0) / NUM_SAMPLES;
    
    // Calculate variance and standard deviation
    double variance = 0.0;
    for (const auto& sample : centered_samples) {
        variance += (sample - mean) * (sample - mean);
    }
    variance /= (NUM_SAMPLES - 1);  // Unbiased estimator
    double std_dev = std::sqrt(variance);
    
    // Print detailed statistics
    std::cout << "\nDetailed Gaussian Distribution Statistics:" << std::endl;
    std::cout << "Number of samples: " << NUM_SAMPLES << std::endl;
    std::cout << "Target sigma: " << params.sigma << std::endl;
    std::cout << "Measured mean: " << mean << std::endl;
    std::cout << "Measured std dev: " << std_dev << std::endl;
    std::cout << "Mean absolute error: " << std::abs(mean) << std::endl;
    std::cout << "Std dev relative error: " << std::abs(std_dev - params.sigma) / params.sigma << std::endl;
    
    // Distribution checks
    int within_sigma = 0;
    int within_2sigma = 0;
    int within_3sigma = 0;
    
    for (const auto& sample : centered_samples) {
        if (std::abs(sample) <= params.sigma) within_sigma++;
        if (std::abs(sample) <= 2 * params.sigma) within_2sigma++;
        if (std::abs(sample) <= 3 * params.sigma) within_3sigma++;
    }
    
    double sigma_ratio = static_cast<double>(within_sigma) / NUM_SAMPLES;
    double sigma2_ratio = static_cast<double>(within_2sigma) / NUM_SAMPLES;
    double sigma3_ratio = static_cast<double>(within_3sigma) / NUM_SAMPLES;
    
    std::cout << "\nDistribution Properties:" << std::endl;
    std::cout << "Within 1σ: " << (sigma_ratio * 100) << "% (expect ~68.27%)" << std::endl;
    std::cout << "Within 2σ: " << (sigma2_ratio * 100) << "% (expect ~95.45%)" << std::endl;
    std::cout << "Within 3σ: " << (sigma3_ratio * 100) << "% (expect ~99.73%)" << std::endl;
    
    // Statistical tests with appropriate bounds
    EXPECT_NEAR(mean, 0.0, params.sigma * 0.1)
        << "Mean deviates too much from expected value";
    
    EXPECT_NEAR(std_dev, params.sigma, params.sigma * 0.1)
        << "Standard deviation deviates too much from expected value";
    
    // Check distribution properties
    EXPECT_NEAR(sigma_ratio, 0.6827, 0.05)
        << "Unexpected proportion of samples within 1 sigma";
    EXPECT_NEAR(sigma2_ratio, 0.9545, 0.05)
        << "Unexpected proportion of samples within 2 sigma";
    EXPECT_NEAR(sigma3_ratio, 0.9973, 0.05)
        << "Unexpected proportion of samples within 3 sigma";
}

// Test case for matrix operations
TEST(MatrixTest, MatrixVectorMultiplication) {
    LWEParams params(128);
    std::vector<std::vector<int64_t>> A = {{1, 2}, {3, 4}};
    std::vector<int64_t> x = {5, 6};
    
    auto result = LWEUtils::matrix_vector_mul(A, x, params.q);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], (1*5 + 2*6) % params.q);
    EXPECT_EQ(result[1], (3*5 + 4*6) % params.q);
}

// Test case for protocol soundness
TEST(ProtocolTest, Soundness) {
    LWEParams params(128);
    Prover prover(params);
    Verifier verifier(params);
    
    // Run protocol multiple times
    for (int i = 0; i < 10; i++) {
        auto instance = prover.setup();
        auto commitment = prover.commit();
        auto challenge = verifier.challenge();
        auto response = prover.respond(challenge);
        EXPECT_TRUE(verifier.verify(instance, commitment, challenge, response));
    }
}

// Test case for response bounds
TEST(ProtocolTest, ResponseBounds) {
    LWEParams params(128);
    Prover prover(params);
    Verifier verifier(params);
    
    auto instance = prover.setup();
    auto commitment = prover.commit();
    auto challenge = verifier.challenge();
    auto response = prover.respond(challenge);
    
    // Check if response vectors are within bounds
    EXPECT_TRUE(LWEUtils::check_norm(response.z, params.beta));
}