#pragma once

#include <stdexcept>
#include <cmath>
#include <string>
#include <sstream>

struct LWEParams {
    size_t n;              // lattice dimension
    size_t m;              // number of samples
    int64_t q;            // modulus (odd prime only)
    double sigma;         // Gaussian parameter
    double alpha;         // witness width parameter
    double beta;          // verification bound
    int64_t challenge_bound; // maximum challenge value
    size_t security_bits;  // actual security bits
    std::string param_set; // parameter set identifier

    // Constructor with security level and parameter set choice
    LWEParams(size_t security_level = 128, const std::string& parameter_set = "standard") {
        param_set = parameter_set;
        if (parameter_set == "standard") {
            set_standard_params(security_level);
        } else if (parameter_set == "nist") {
            set_nist_params(security_level);
        } else {
            throw std::invalid_argument("Unknown parameter set: " + parameter_set);
        }
        calculate_derived_params();
    }

private:
void set_standard_params(size_t security_level) {
    switch(security_level) {
        case 128:
            n = 512;
            m = 1024;
            q = 1073741823;
            sigma = 3.2;
            alpha = 2.0;
            challenge_bound = 20;
            security_bits = 128;
            break;

        case 192:
            n = 768;
            m = 1536;
            q = 1073741823;
            sigma = 3.4;
            alpha = 2.2;
            challenge_bound = 24;
            security_bits = 192;
            break;

        case 256:
            n = 1024;
            m = 2048;
            q = 1073741823;
            sigma = 3.6;
            alpha = 2.4;
            challenge_bound = 28;
            security_bits = 256;
            break;
        
        default:
            throw std::invalid_argument("Unsupported security level for performance parameters");
    }
}

void set_nist_params(size_t security_level) {
    switch(security_level) {
        case 128: // NIST Level 1
            n = 640;
            m = 1280;
            q = 1073741823;
            sigma = 3.0;
            alpha = 2.0;
            challenge_bound = 16;
            security_bits = 128;
            break;

        case 192: // NIST Level 3
            n = 864;
            m = 1728;
            q = 1073741823;
            sigma = 3.2;
            alpha = 2.2;
            challenge_bound = 20;
            security_bits = 192;
            break;

        case 256: // NIST Level 5
            n = 1024;
            m = 2048;
            q = 1073741823;
            sigma = 3.4;
            alpha = 2.4;
            challenge_bound = 24;
            security_bits = 256;
            break;

        default:
            throw std::invalid_argument("Unsupported security level for performance parameters");
    }
}


    void calculate_derived_params() {
        double base_beta = sigma * std::sqrt(static_cast<double>(n));
        double challenge_factor = static_cast<double>(challenge_bound + 1);
        double security_factor = 1.0 + (security_bits / 128.0) * 0.1;
        beta = base_beta * challenge_factor * security_factor;
    }

public:
    bool is_prime(int64_t n) const {
        if (n < 2) return false;
        if (n % 2 == 0) return n == 2;
        
        for (int64_t i = 3; i * i <= n; i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }

    bool validate_params() const {
        // Basic parameter checks
        if (n < 256 || m < n || q < 1000) return false;
        if (sigma <= 0 || alpha <= 0 || beta <= 0) return false;
        if (challenge_bound < 2) return false;

        // Prime modulus check
        if (!is_prime(q)) return false;
        if (q % 2 == 0) return false;  // Ensure odd prime

        // Security checks
        double log_q = std::log2(q);
        if (log_q < 15 || log_q > 20) return false;

        // LWE-specific checks
        if (m < 2 * n) return false;
        if (sigma * std::sqrt(n) > q/4) return false;

        return true;
    }

    double estimate_security() const {
        // Core-SVP hardness estimation
        double delta = std::pow(q/sigma, 1.0/n);
        double log_delta = std::log2(delta);
        return 0.292 * n * log_delta * log_delta;
    }

    std::string get_detailed_description() const {
        std::stringstream ss;
        ss << "Parameter Set: " << param_set << "\n"
           << "Security Level: " << security_bits << " bits\n"
           << "Lattice Dimension (n): " << n << "\n"
           << "Number of Samples (m): " << m << "\n"
           << "Modulus (q): " << q << " (Prime: " << (is_prime(q) ? "Yes" : "No") << ")\n"
           << "Gaussian Parameter (σ): " << sigma << "\n"
           << "Witness Width (α): " << alpha << "\n"
           << "Verification Bound (β): " << beta << "\n"
           << "Challenge Bound: " << challenge_bound << "\n"
           << "Estimated Security: " << estimate_security() << " bits\n";
        return ss.str();
    }
};