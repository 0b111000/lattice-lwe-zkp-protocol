# Lattice-Based Zero-Knowledge Proof System
A C++ implementation of a zero-knowledge proof system based on the Learning With Errors (LWE) problem, providing post-quantum security guarantee.

## Overview
This project implements a Σ-protocol (sigma protocol : 3-move protocol) that allows a prover to convince a verifier that they know a solution to an LWE instance without revealing the solution itself. The implementation focuses on security, efficiency, and careful parameter selection.

## Key Features
- LWE-based zero-knowledge proof system
- Multiple security levels (128-bit, 192-bit, 256-bit)
- Various parameter sets (standard, NIST-compliant)
- Safe modular arithmetic implementation
- Gaussian sampling for lattice-based cryptography
- Comprehensive test suite and benchmarking tools

## Protocol Description
The implemented protocol follows this structure:

1. Setup:

- Public parameters: Matrix A ∈ Zq^(m×n), vector t = As + e mod q
- Prover's secret: vectors s, e

2. Protocol Flow:

```
+---------------+-+-----------------+
| Prover        | | Verifier        |
+---------------+-+-----------------+
|                                   |
| y ← D_σ^n                         |
| r ← D_σ^m                         |
| u = Ay + r                        |
|                                   |
| u     ------------------>         |
|                                   |
|                       c ← {0,1}^λ |
|        <----------------- c       |
|                                   |
| Respond:                          |
| z = y + cs                        |
| r' = r + ce                       |
|                                   |
| z, r'  ---------------------->    |
|                                   |
|               Verify:             |
|               ||z|| ≤ β           |
|               Az + r' = u + ct    |
|                                   |
+---------------+ +-----------------+

```



### Public Parameters:

- Matrix A ∈ Zq^(m×n)
-Vector t = As + e mod q

### Prover's Secret:
- Vectors s, e

### Security Parameters:
- q: modulus (large prime)
- σ: Gaussian parameter
- β: norm bound
- λ: challenge space parameter


## Building and Installation
### Prerequisites
- C++17 compatible compiler
- CMake 3.10 or higher
- Google Test (for testing)
- Google Benchmark (for benchmarking)

## Build Instructions
```
mkdir build
cd build
cmake ..
make
```

## Running Tests
```
./zkp_tests
```

## Running Benchmarks
```
./benchmark_zkp
```

## Usage Example
```
#include "prover.hpp"
#include "verifier.hpp"

int main() {
// Initialize with 128-bit security parameters
LWEParams params(128, "standard");


// Create prover and verifier
Prover prover(params);
Verifier verifier(params);

// Execute protocol
auto instance = prover.setup();
auto commitment = prover.commit();
auto challenge = verifier.challenge();
auto response = prover.respond(challenge);

// Verify proof
bool result = verifier.verify(instance, commitment, challenge, response);
}
```

## Security Parameters
The implementation supports multiple parameter sets:

- Standard: Balanced security/performance trade-off
- NIST: Compliant with NIST PQC recommendations (the prime number chosen is near to 2^30 - in other words nearly 30 bits at the moment)

Each set is available in three security levels:

128-bit quantum security
192-bit quantum security
256-bit quantum security

## Project Structure
```
|-- CMakeLists.txt
|-- LICENSE
|-- README.md
|-- examples
|   `-- main.cpp
|-- include
|   |-- lwe_params.hpp
|   |-- lwe_proof.hpp
|   |-- lwe_utils.hpp
|   |-- prover.hpp
|   `-- verifier.hpp
|-- src
|   |-- lwe_utils.cpp
|   |-- prover.cpp
|   `-- verifier.cpp
`-- test
    |-- benchmark_zkp.cpp
    |-- test_cases.cpp
    `-- test_zkp.cpp

```


# Contributing
Contributions are welcome! Please ensure:

- Proper test coverage for new features
- Safe implementation of cryptographic operations
- Parameter validation and security checks
- Documentation for new functionality

## License
`Apache License Version 2.0`

## References
- [Lattice-Based Zero-Knowledge Proofs and Applications:
Shorter, Simpler, and More General](https://eprint.iacr.org/2022/284.pdf)


# Future Improvements
- Integration with NTL for optimized number theory operations
- Additional parameter sets for specific use cases
- Enhanced side-channel protection
- Batch proof generation capabilities
- Non-interactive version using Fiat-Shamir transform