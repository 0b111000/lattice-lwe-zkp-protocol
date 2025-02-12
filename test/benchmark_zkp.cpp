// benchmark/benchmark_zkp.cpp
#include <benchmark/benchmark.h>
#include "prover.hpp"
#include "verifier.hpp"
#include <chrono>
#include <numeric>

// Helper function to calculate size in bytes
size_t calculate_proof_size(const Response& response) {
    return (response.z.size() + response.r_prime.size()) * sizeof(int64_t);
}

size_t calculate_commitment_size(const Commitment& commitment) {
    return commitment.u.size() * sizeof(int64_t);
}

// Benchmark complete protocol execution
static void BM_CompleteProtocol(benchmark::State& state) {
    LWEParams params(state.range(0)); // Security parameter as input
    Prover prover(params);
    Verifier verifier(params);

    for (auto _ : state) {
        auto instance = prover.setup();
        auto commitment = prover.commit();
        auto challenge = verifier.challenge();
        auto response = prover.respond(challenge);
        bool result = verifier.verify(instance, commitment, challenge, response);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_CompleteProtocol)->Arg(128)->Arg(256);

// Benchmark proof generation (Prover's work)
static void BM_ProverOperations(benchmark::State& state) {
    LWEParams params(state.range(0));
    Prover prover(params);
    auto instance = prover.setup();
    
    for (auto _ : state) {
        auto commitment = prover.commit();
        Challenge challenge{static_cast<int64_t>(state.range(0) % 20)};
        auto response = prover.respond(challenge);
        benchmark::DoNotOptimize(response);
    }
}
BENCHMARK(BM_ProverOperations)->Arg(128)->Arg(256);

// Benchmark verification (Verifier's work)
static void BM_VerifierOperations(benchmark::State& state) {
    LWEParams params(state.range(0));
    Prover prover(params);
    Verifier verifier(params);
    
    auto instance = prover.setup();
    auto commitment = prover.commit();
    auto challenge = verifier.challenge();
    auto response = prover.respond(challenge);
    
    for (auto _ : state) {
        bool result = verifier.verify(instance, commitment, challenge, response);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_VerifierOperations)->Arg(128)->Arg(256);

// Benchmark proof size
static void BM_ProofSize(benchmark::State& state) {
    LWEParams params(state.range(0));
    Prover prover(params);
    Verifier verifier(params);
    
    std::vector<size_t> proof_sizes;
    std::vector<size_t> commitment_sizes;
    
    for (auto _ : state) {
        auto instance = prover.setup();
        auto commitment = prover.commit();
        auto challenge = verifier.challenge();
        auto response = prover.respond(challenge);
        
        size_t proof_size = calculate_proof_size(response);
        size_t commitment_size = calculate_commitment_size(commitment);
        
        proof_sizes.push_back(proof_size);
        commitment_sizes.push_back(commitment_size);
    }
    
    // Calculate average sizes
    double avg_proof_size = std::accumulate(proof_sizes.begin(), proof_sizes.end(), 0.0) / proof_sizes.size();
    double avg_commitment_size = std::accumulate(commitment_sizes.begin(), commitment_sizes.end(), 0.0) / commitment_sizes.size();
    
    state.counters["ProofSizeBytes"] = avg_proof_size;
    state.counters["CommitmentSizeBytes"] = avg_commitment_size;
    state.counters["TotalSizeBytes"] = avg_proof_size + avg_commitment_size;
}
BENCHMARK(BM_ProofSize)->Arg(128)->Arg(256);

// Add the main function for the benchmark
BENCHMARK_MAIN();