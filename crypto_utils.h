#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <string>
#include <vector>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/evp.h>
#include <openssl/sha.h>

// Initialize OpenSSL
void initOpenSSL();

// Clean up OpenSSL
void cleanupOpenSSL();

// EC Key generation
EC_KEY* generateECKeyPair();

// Get public key as a hex string from a key pair
std::string getPublicKeyHex(EC_KEY* key_pair);

// Derive address from public key
std::string deriveAddressFromPublicKey(const std::string& publicKeyHex);

// ECDSA signature functions
std::string signMessage(EC_KEY* key_pair, const std::string& message);
bool verifySignature(const std::string& message, const std::string& signature, const std::string& publicKey);

// Hex conversion utilities
std::string bytesToHex(const unsigned char* data, size_t length);
std::vector<unsigned char> hexToBytes(const std::string& hex);

#endif // CRYPTO_UTILS_H 