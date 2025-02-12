// examples/main.cpp
#include "prover.hpp"
#include "verifier.hpp"
#include <iostream>

int main() {
    try {
        LWEParams params(128);
        
        std::cout << "Parameters:" << std::endl;
        std::cout << "n: " << params.n << std::endl;
        std::cout << "m: " << params.m << std::endl;
        std::cout << "q: " << params.q << std::endl;
        std::cout << "sigma: " << params.sigma << std::endl;
        std::cout << "alpha: " << params.alpha << std::endl;
        std::cout << "beta: " << params.beta << std::endl;
        std::cout << "challenge_bound: " << params.challenge_bound << std::endl;
        
        Prover prover(params);
        Verifier verifier(params);
        
        std::cout << "\nStarting ZKP protocol..." << std::endl;
        
        auto instance = prover.setup();
        std::cout << "Setup completed." << std::endl;
        
        auto commitment = prover.commit();
        std::cout << "Commitment sent." << std::endl;
        
        auto challenge = verifier.challenge();
        std::cout << "Challenge generated: " << challenge.c << std::endl;
        
        auto response = prover.respond(challenge);
        std::cout << "Response generated." << std::endl;
        
        bool result = verifier.verify(instance, commitment, challenge, response);
        std::cout << "Verification result: " << (result ? "SUCCESS" : "FAILURE") << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}