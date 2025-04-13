#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <cstdint> 
class SHA256 {
public:
    SHA256();
    std::string hash(const std::string& message);

private:
    uint32_t h[8];  // Initial hash values
    static const uint32_t k[64];  // Round constants
    static const uint32_t initial_hash[8];  // Initial hash values

    std::string preprocess(const std::string& message);
    std::string sha256(const std::string& message);
    uint32_t rightRotate(uint32_t value, uint32_t amount);
    uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    uint32_t sigma0(uint32_t x);
    uint32_t sigma1(uint32_t x);
    uint32_t Sigma0(uint32_t x);
    uint32_t Sigma1(uint32_t x);
};

#endif // SHA256_H
