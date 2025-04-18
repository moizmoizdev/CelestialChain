#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <vector>
#include <cstdint>

class SHA256 {
public:
    SHA256();
    void update(const std::vector<uint8_t>& data);
    void update(const std::string& data);
    std::vector<uint8_t> finalize();
    std::string finalizeHex();
    static std::string hash(const std::string& data);
    static std::string hash(const std::vector<uint8_t>& data);

private:
    uint32_t h[8];
    std::vector<uint8_t> buffer;
    uint64_t totalLength;
    bool finalized;

    void processBlock(const uint8_t* block);
    void pad();
    void reset();
    
    // Helper functions
    static uint32_t rightRotate(uint32_t value, uint32_t count);
    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t sigma0(uint32_t x);
    static uint32_t sigma1(uint32_t x);
    static uint32_t Sigma0(uint32_t x);
    static uint32_t Sigma1(uint32_t x);
};

// Standalone functions for hashing
std::string computeSHA256(const std::string& message);
std::string computeSHA256(const unsigned char* data, size_t length);

#endif // SHA256_H
