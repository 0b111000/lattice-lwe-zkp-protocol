#include <gtest/gtest.h>
#include "prover.hpp"
#include "verifier.hpp"
#include <chrono>
#include <vector>
#include <sstream>

class ZKPTest : public ::testing::Test {
protected:
    void SetUp() override {
        params_128 = LWEParams(128);
        params_256 = LWEParams(256, "nist");
    }

    LWEParams params_128;
    LWEParams params_256;
};

// Basic functionality test
TEST_F(ZKPTest, BasicProtocolTest) {
    Prover prover(params_128);
    Verifier verifier(params_128);

    auto instance = prover.setup();
    auto commitment = prover.commit();
    auto challenge = verifier.challenge();
    auto response = prover.respond(challenge);

    EXPECT_TRUE(verifier.verify(instance, commitment, challenge, response));
}

// Test with invalid response
TEST_F(ZKPTest, InvalidResponseTest) {
    Prover prover(params_128);
    Verifier verifier(params_128);

    auto instance = prover.setup();
    auto commitment = prover.commit();
    auto challenge = verifier.challenge();
    auto response = prover.respond(challenge);
    
    // Corrupt the response
    response.z[0] += params_128.q/2;
    
    EXPECT_FALSE(verifier.verify(instance, commitment, challenge, response));
}

// Test different security levels
TEST_F(ZKPTest, SecurityLevelsTest) {
    std::vector<LWEParams> params_list = {params_128, params_256};
    
    for (const auto& params : params_list) {
        Prover prover(params);
        Verifier verifier(params);

        auto instance = prover.setup();
        auto commitment = prover.commit();
        auto challenge = verifier.challenge();
        auto response = prover.respond(challenge);

        EXPECT_TRUE(verifier.verify(instance, commitment, challenge, response));
    }
}