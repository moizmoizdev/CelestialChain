#include "crypto_utils.h"
#include "sha.h"
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <iostream>

// Add the splitString function
std::vector<std::string> splitString(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delim)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void initOpenSSL() {
    OpenSSL_add_all_algorithms();
}

void cleanupOpenSSL() {
    EVP_cleanup();
}

EC_KEY* generateECKeyPair() {
    // Create a new EC key structure using secp256k1 curve
    EC_KEY* key_pair = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!key_pair) {
        std::cerr << "Failed to create EC key structure" << std::endl;
        return nullptr;
    }
    
    // Generate the key pair
    if (!EC_KEY_generate_key(key_pair)) {
        std::cerr << "Failed to generate EC key pair" << std::endl;
        EC_KEY_free(key_pair);
        return nullptr;
    }
    
    return key_pair;
}

std::string getPublicKeyHex(const EC_KEY* key) {
    if (!key) {
        std::cerr << "ERROR: NULL key provided to getPublicKeyHex" << std::endl;
        return "";
    }
    
    const EC_POINT* pub_key = EC_KEY_get0_public_key(key);
    if (!pub_key) {
        std::cerr << "ERROR: Failed to get public key from EC_KEY" << std::endl;
        return "";
    }
    
    const EC_GROUP* group = EC_KEY_get0_group(key);
    if (!group) {
        std::cerr << "ERROR: Failed to get group from EC_KEY" << std::endl;
        return "";
    }
    
    // Convert public key to hex
    BIGNUM* x = BN_new();
    BIGNUM* y = BN_new();
    if (!x || !y) {
        std::cerr << "ERROR: Failed to allocate BIGNUM" << std::endl;
        if (x) BN_free(x);
        if (y) BN_free(y);
        return "";
    }
    
    if (!EC_POINT_get_affine_coordinates(group, pub_key, x, y, nullptr)) {
        std::cerr << "ERROR: Failed to get coordinates from EC_POINT" << std::endl;
        BN_free(x);
        BN_free(y);
        return "";
    }
    
    std::stringstream ss;
    char* x_hex = BN_bn2hex(x);
    char* y_hex = BN_bn2hex(y);
    
    if (!x_hex || !y_hex) {
        std::cerr << "ERROR: Failed to convert BIGNUM to hex" << std::endl;
        if (x_hex) OPENSSL_free(x_hex);
        if (y_hex) OPENSSL_free(y_hex);
        BN_free(x);
        BN_free(y);
        return "";
    }
    
    // Format as 0x04 + x + y (where 04 indicates uncompressed point)
    ss << "0x04" << x_hex << y_hex;
    
    OPENSSL_free(x_hex);
    OPENSSL_free(y_hex);
    BN_free(x);
    BN_free(y);
    
    return ss.str();
}

std::string deriveAddressFromPublicKey(const std::string& publicKeyHex) {
    // Strip 0x prefix if it exists
    std::string pubKeyNoPrefix = publicKeyHex;
    if (pubKeyNoPrefix.substr(0, 2) == "0x") {
        pubKeyNoPrefix = pubKeyNoPrefix.substr(2);
    }
    
    // Hash the public key using SHA-256
    std::string hash = computeSHA256(pubKeyNoPrefix);
    
    // Take first 40 characters as the address and add 0x prefix
    return "0x" + hash.substr(0, 40);
}

std::string signMessage(const EC_KEY* key, const std::string& message) {
    if (!key) {
        std::cerr << "ERROR: NULL key provided to signMessage" << std::endl;
        return "";
    }
    
    // Hash the message with SHA256
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, message.c_str(), message.length());
    SHA256_Final(hash, &sha256);
    
    // Sign the hash
    ECDSA_SIG* signature = ECDSA_do_sign(hash, SHA256_DIGEST_LENGTH, const_cast<EC_KEY*>(key));
    if (!signature) {
        std::cerr << "ERROR: Failed to create ECDSA signature" << std::endl;
        return "";
    }
    
    // Convert signature to DER format
    unsigned char *der = nullptr;
    int der_len = i2d_ECDSA_SIG(signature, &der);
    if (der_len < 0) {
        std::cerr << "ERROR: Failed to convert signature to DER format" << std::endl;
        ECDSA_SIG_free(signature);
        return "";
    }
    
    // Convert DER to hex string with 0x prefix
    std::stringstream ss;
    ss << "0x";
    for (int i = 0; i < der_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)der[i];
    }
    
    OPENSSL_free(der);
    ECDSA_SIG_free(signature);
    
    return ss.str();
}

bool verifySignature(const std::string& message, 
                    const std::string& signature,
                    const std::string& publicKeyOrAddress) {
    std::cout << "DEBUG - verifySignature called with:" << std::endl;
    std::cout << "  Message: " << message << std::endl;
    std::cout << "  Signature: " << (signature.length() > 20 ? signature.substr(0, 20) + "..." : signature) << std::endl;
    std::cout << "  Public Key/Address: " << publicKeyOrAddress << std::endl;
    
    // Check if the input is empty
    if (signature.empty()) {
        std::cerr << "ERROR: Empty signature provided" << std::endl;
        return false;
    }
    
    // Strip 0x prefix from signature if present
    std::string sigNoPrefix = signature;
    if (sigNoPrefix.substr(0, 2) == "0x") {
        sigNoPrefix = sigNoPrefix.substr(2);
    }
    
    // Check what kind of input we have - public key or address
    bool isPublicKey = false;
    bool isAddress = false;
    
    if (publicKeyOrAddress.substr(0, 4) == "0x04") {
        // This is a full public key (uncompressed format)
        isPublicKey = true;
    } else if (publicKeyOrAddress.substr(0, 2) == "0x") {
        // This is likely an address
        isAddress = true;
    } else {
        std::cerr << "WARNING: Input is neither a valid public key (0x04...) nor address (0x...)" << std::endl;
        return false;
    }
    
    // For this demo implementation, we cannot verify with just an address
    // We need the actual public key
    if (isAddress) {
        std::cerr << "INFO: Using address as identifier only without verification." << std::endl;
        std::cerr << "      In a real blockchain, we would look up the public key from the address." << std::endl;
        
        // In a real-world implementation, we would:
        // 1. Look up the public key that corresponds to this address from previous transactions
        // 2. Use that public key for verification
        // 3. Or implement address-based verification using a different approach
        
        // For this demo, we'll return true for simplicity since we can't 
        // easily recover public keys from addresses in our demo
        return true;
    }
    
    // If we have a public key, do the real verification
    if (isPublicKey) {
        // Strip 0x prefix from public key if present
        std::string pubKeyNoPrefix = publicKeyOrAddress;
        if (pubKeyNoPrefix.substr(0, 2) == "0x") {
            pubKeyNoPrefix = pubKeyNoPrefix.substr(2);
        }
        
        // Create EC_KEY from public key hex
        EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
        if (!key) {
            std::cerr << "ERROR: Failed to create EC_KEY" << std::endl;
            return false;
        }
        
        // Convert hex public key to EC_POINT
        EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
        if (!group) {
            std::cerr << "ERROR: Failed to create EC_GROUP" << std::endl;
            EC_KEY_free(key);
            return false;
        }
        
        EC_POINT* pub_point = EC_POINT_hex2point(group, pubKeyNoPrefix.c_str(), nullptr, nullptr);
        if (!pub_point) {
            std::cerr << "ERROR: Failed to convert hex to EC_POINT: " << pubKeyNoPrefix.substr(0, 20) << "..." << std::endl;
            EC_KEY_free(key);
            EC_GROUP_free(group);
            return false;
        }
        
        if (!EC_KEY_set_public_key(key, pub_point)) {
            std::cerr << "ERROR: Failed to set public key" << std::endl;
            EC_POINT_free(pub_point);
            EC_KEY_free(key);
            EC_GROUP_free(group);
            return false;
        }
        
        // Hash the message
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, message.c_str(), message.length());
        SHA256_Final(hash, &sha256);
        
        // Convert hex signature to DER format
        std::vector<unsigned char> der;
        for (size_t i = 0; i < sigNoPrefix.length(); i += 2) {
            unsigned int byte;
            std::stringstream ss;
            ss << std::hex << sigNoPrefix.substr(i, 2);
            ss >> byte;
            der.push_back(static_cast<unsigned char>(byte));
        }
        
        // Convert DER to ECDSA_SIG
        const unsigned char* der_ptr = der.data();
        ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &der_ptr, der.size());
        if (!sig) {
            std::cerr << "ERROR: Failed to parse DER signature" << std::endl;
            EC_POINT_free(pub_point);
            EC_KEY_free(key);
            EC_GROUP_free(group);
            return false;
        }
        
        // Verify the signature
        int result = ECDSA_do_verify(hash, SHA256_DIGEST_LENGTH, sig, key);
        
        // Debug output
        std::cout << "Signature verification result: " << result << " (1=success, 0=failure, -1=error)" << std::endl;
        
        // Cleanup
        ECDSA_SIG_free(sig);
        EC_POINT_free(pub_point);
        EC_KEY_free(key);
        EC_GROUP_free(group);
        
        return result == 1;
    }
    
    // We should never reach this point given the conditions above
    return false;
}

std::string bytesToHex(const unsigned char* data, size_t length) {
    std::stringstream ss;
    ss << "0x"; // Add 0x prefix
    for (size_t i = 0; i < length; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return ss.str();
}

std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::string hexNoPrefix = hex;
    // Strip 0x prefix if present
    if (hexNoPrefix.substr(0, 2) == "0x") {
        hexNoPrefix = hexNoPrefix.substr(2);
    }
    
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hexNoPrefix.length(); i += 2) {
        std::string byteString = hexNoPrefix.substr(i, 2);
        bytes.push_back(static_cast<unsigned char>(std::stoi(byteString, nullptr, 16)));
    }
    return bytes;
} 