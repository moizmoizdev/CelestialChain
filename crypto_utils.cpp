#include "crypto_utils.h"
#include "sha.h"
#include <iostream>
#include <sstream>
#include <iomanip>

void initOpenSSL() {
    OpenSSL_add_all_algorithms();
}

void cleanupOpenSSL() {
    EVP_cleanup();
}

EC_KEY* generateECKeyPair() {
    // Create a new EC key structure
    EC_KEY* key_pair = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!key_pair) {
        throw std::runtime_error("Failed to create EC key structure");
    }
    
    // Generate the key pair
    if (!EC_KEY_generate_key(key_pair)) {
        EC_KEY_free(key_pair);
        throw std::runtime_error("Failed to generate EC key pair");
    }
    
    return key_pair;
}

std::string getPublicKeyHex(EC_KEY* key_pair) {
    if (!key_pair) {
        throw std::runtime_error("Invalid key pair");
    }
    
    // Get the public key in compressed format
    const EC_POINT* pub_key = EC_KEY_get0_public_key(key_pair);
    const EC_GROUP* group = EC_KEY_get0_group(key_pair);
    
    // Convert public key to bytes
    size_t pub_key_len = EC_POINT_point2oct(group, pub_key, 
                                          POINT_CONVERSION_COMPRESSED,
                                          nullptr, 0, nullptr);
    if (pub_key_len == 0) {
        throw std::runtime_error("Failed to get public key length");
    }
    
    std::vector<unsigned char> pub_key_bytes(pub_key_len);
    if (!EC_POINT_point2oct(group, pub_key, 
                           POINT_CONVERSION_COMPRESSED,
                           pub_key_bytes.data(), pub_key_len, nullptr)) {
        throw std::runtime_error("Failed to convert public key to bytes");
    }
    
    // Convert to hex
    return bytesToHex(pub_key_bytes.data(), pub_key_len);
}

std::string deriveAddressFromPublicKey(const std::string& publicKeyHex) {
    // Hash the public key using SHA-256
    std::string hash = computeSHA256(publicKeyHex);
    
    // Take first 40 characters as the address
    return hash.substr(0, 40);
}

std::string signMessage(EC_KEY* key_pair, const std::string& message) {
    // Hash the message
    std::string hash_str = computeSHA256(message);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // Convert hash string back to bytes
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        std::string byte = hash_str.substr(i * 2, 2);
        hash[i] = static_cast<unsigned char>(std::stoi(byte, nullptr, 16));
    }
    
    // Sign the hash
    ECDSA_SIG* signature = ECDSA_do_sign(hash, SHA256_DIGEST_LENGTH, key_pair);
    if (!signature) {
        throw std::runtime_error("Failed to sign message");
    }
    
    // Get the r and s values
    const BIGNUM* r;
    const BIGNUM* s;
    ECDSA_SIG_get0(signature, &r, &s);
    
    // Convert r and s to hex strings
    char* r_hex = BN_bn2hex(r);
    char* s_hex = BN_bn2hex(s);
    
    std::string signature_str = std::string(r_hex) + std::string(s_hex);
    
    // Clean up
    OPENSSL_free(r_hex);
    OPENSSL_free(s_hex);
    ECDSA_SIG_free(signature);
    
    return signature_str;
}

bool verifySignature(const std::string& message, const std::string& signature, const std::string& publicKey) {
    // Convert public key from hex to bytes
    std::vector<unsigned char> pub_key_bytes = hexToBytes(publicKey);
    
    // Create EC key from public key bytes
    EC_KEY* pub_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!pub_key) {
        return false;
    }
    
    const EC_GROUP* group = EC_KEY_get0_group(pub_key);
    EC_POINT* point = EC_POINT_new(group);
    if (!point) {
        EC_KEY_free(pub_key);
        return false;
    }
    
    if (!EC_POINT_oct2point(group, point, pub_key_bytes.data(), pub_key_bytes.size(), nullptr)) {
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    if (!EC_KEY_set_public_key(pub_key, point)) {
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    // Hash the message
    std::string hash_str = computeSHA256(message);
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // Convert hash string back to bytes
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        std::string byte = hash_str.substr(i * 2, 2);
        hash[i] = static_cast<unsigned char>(std::stoi(byte, nullptr, 16));
    }
    
    // Split signature into r and s
    std::string r_str = signature.substr(0, signature.length() / 2);
    std::string s_str = signature.substr(signature.length() / 2);
    
    // Convert r and s to BIGNUMs
    BIGNUM* r = BN_new();
    BIGNUM* s = BN_new();
    BN_hex2bn(&r, r_str.c_str());
    BN_hex2bn(&s, s_str.c_str());
    
    // Create ECDSA signature
    ECDSA_SIG* sig = ECDSA_SIG_new();
    ECDSA_SIG_set0(sig, r, s);
    
    // Verify signature
    int result = ECDSA_do_verify(hash, SHA256_DIGEST_LENGTH, sig, pub_key);
    
    // Clean up
    ECDSA_SIG_free(sig);
    EC_POINT_free(point);
    EC_KEY_free(pub_key);
    
    return result == 1;
}

std::string bytesToHex(const unsigned char* data, size_t length) {
    std::string hex;
    hex.resize(length * 2);
    for (size_t i = 0; i < length; i++) {
        sprintf(&hex[i * 2], "%02x", data[i]);
    }
    return hex;
}

std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        bytes.push_back(static_cast<unsigned char>(std::stoi(byteString, nullptr, 16)));
    }
    return bytes;
} 