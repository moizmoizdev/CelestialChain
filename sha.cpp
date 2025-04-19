#include "sha.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <openssl/sha.h>
#include <openssl/evp.h>

// // Constants for SHA-256
// const uint32_t SHA256::initial_hash[8] = {
//     0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
//     0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
// };

// const uint32_t SHA256::k[64] = {
//     0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
//     0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
//     0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
//     0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
//     0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
//     0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
//     0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
//     0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
//     0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
//     0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
//     0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
//     0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
//     0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
//     0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
//     0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
//     0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
// };

SHA256::SHA256() {
    // Initialize OpenSSL
    OpenSSL_add_all_digests();
}

// std::string SHA256::hash(const std::string& message) {
//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     SHA256_CTX sha256;
//     SHA256_Init(&sha256);
//     SHA256_Update(&sha256, message.c_str(), message.length());
//     SHA256_Final(hash, &sha256);
    
//     // Convert to hex string
//     std::stringstream ss;
//     for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//         ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
//     }
    
//     return ss.str();
// }

// Wrapper function for easier hashing
std::string computeSHA256(const std::string& message) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message.c_str(), message.length());
    SHA256_Final(hash, &sha256);
    
    // Convert to hex string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

// Hash binary data directly
std::string computeSHA256(const unsigned char* data, size_t length) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, length);
    SHA256_Final(hash, &sha256);
    
    // Convert to hex string
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    
    return ss.str();
}

// // The following functions would be used in a full SHA-256 implementation
// uint32_t SHA256::rightRotate(uint32_t value, uint32_t amount) {
//     return (value >> amount) | (value << (32 - amount));
// }

// uint32_t SHA256::ch(uint32_t x, uint32_t y, uint32_t z) {
//     return (x & y) ^ (~x & z);
// }

// uint32_t SHA256::maj(uint32_t x, uint32_t y, uint32_t z) {
//     return (x & y) ^ (x & z) ^ (y & z);
// }

// uint32_t SHA256::sigma0(uint32_t x) {
//     return rightRotate(x, 7) ^ rightRotate(x, 18) ^ (x >> 3);
// }

// uint32_t SHA256::sigma1(uint32_t x) {
//     return rightRotate(x, 17) ^ rightRotate(x, 19) ^ (x >> 10);
// }

// uint32_t SHA256::Sigma0(uint32_t x) {
//     return rightRotate(x, 2) ^ rightRotate(x, 13) ^ rightRotate(x, 22);
// }

// uint32_t SHA256::Sigma1(uint32_t x) {
//     return rightRotate(x, 6) ^ rightRotate(x, 11) ^ rightRotate(x, 25);
// }
