#include "wallet.h"
#include "Transaction.h"
#include "Blockchain.h"
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <sstream>
#include <iomanip>
#include <vector>

Wallet::Wallet() {
    
    // Create a new EVP_PKEY context
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_PKEY context");
    }
    
    // Initialize the key generation context
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize key generation");
    }

    // Set the curve parameters
    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to set curve parameters");
    }

    // Generate the key pair
    EVP_PKEY* pkey = NULL;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to generate key pair");
    }

    // Convert to EC_KEY for compatibility
    key_pair = EVP_PKEY_get1_EC_KEY(pkey);
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    if (!key_pair) {
        throw std::runtime_error("Failed to get EC_KEY from EVP_PKEY");
    }
    std::cout<<key_pair;
    balance = 100;
}

Wallet::~Wallet() {
    if (key_pair) {
        EC_KEY_free(key_pair);
    }
}

std::string Wallet::getPublicKeyHex() const {
    const EC_POINT* pub_key = EC_KEY_get0_public_key(key_pair);
    const EC_GROUP* group = EC_KEY_get0_group(key_pair);
    
    BIGNUM* x = BN_new();
    BIGNUM* y = BN_new();
    if (!x || !y) {
        if (x) BN_free(x);
        if (y) BN_free(y);
        throw std::runtime_error("Failed to allocate BIGNUM");
    }
    
    if (!EC_POINT_get_affine_coordinates_GFp(group, pub_key, x, y, NULL)) {
        BN_free(x);
        BN_free(y);
        throw std::runtime_error("Failed to get public key coordinates");
    }
    
    char* x_hex = BN_bn2hex(x);
    char* y_hex = BN_bn2hex(y);
    if (!x_hex || !y_hex) {
        OPENSSL_free(x_hex);
        OPENSSL_free(y_hex);
        BN_free(x);
        BN_free(y);
        throw std::runtime_error("Failed to convert coordinates to hex");
    }
    
    std::string result = std::string(x_hex) + std::string(y_hex);
    
    OPENSSL_free(x_hex);
    OPENSSL_free(y_hex);
    BN_free(x);
    BN_free(y);
    
    return result;
}

std::string Wallet::signMessage(const std::string& message) const {
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    if (!md_ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }
    
    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey) {
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("Failed to create EVP_PKEY");
    }
    
    if (!EVP_PKEY_set1_EC_KEY(pkey, key_pair)) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("Failed to set EC key");
    }
    
    if (!EVP_DigestSignInit(md_ctx, NULL, EVP_sha256(), NULL, pkey)) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("Failed to initialize signing");
    }
    
    if (!EVP_DigestSignUpdate(md_ctx, message.c_str(), message.length())) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("Failed to update signing context");
    }
    
    size_t sig_len;
    if (!EVP_DigestSignFinal(md_ctx, NULL, &sig_len)) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("Failed to get signature length");
    }
    
    std::vector<unsigned char> signature(sig_len);
    if (!EVP_DigestSignFinal(md_ctx, signature.data(), &sig_len)) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(md_ctx);
        throw std::runtime_error("Failed to finalize signature");
    }
    
    EVP_PKEY_free(pkey);
    EVP_MD_CTX_free(md_ctx);
    
    // Convert signature to hex string
    std::stringstream ss;
    for (unsigned char byte : signature) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return ss.str();
}

bool Wallet::verifySignature(const std::string& message, const std::string& signature, const std::string& publicKeyHex)  {
    // Convert hex strings to binary data
    std::vector<unsigned char> sig_bytes;
    for (size_t i = 0; i < signature.length(); i += 2) {
        std::string byteString = signature.substr(i, 2);
        sig_bytes.push_back(static_cast<unsigned char>(std::stoi(byteString, nullptr, 16)));
    }
    
    // Create EC_KEY from public key hex
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
    
    // Split public key hex into x and y coordinates
    std::string x_hex = publicKeyHex.substr(0, publicKeyHex.length()/2);
    std::string y_hex = publicKeyHex.substr(publicKeyHex.length()/2);
    
    BIGNUM* x = BN_new();
    BIGNUM* y = BN_new();
    if (!x || !y) {
        if (x) BN_free(x);
        if (y) BN_free(y);
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    if (!BN_hex2bn(&x, x_hex.c_str()) || !BN_hex2bn(&y, y_hex.c_str())) {
        BN_free(x);
        BN_free(y);
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    if (!EC_POINT_set_affine_coordinates_GFp(group, point, x, y, NULL)) {
        BN_free(x);
        BN_free(y);
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    if (!EC_KEY_set_public_key(pub_key, point)) {
        BN_free(x);
        BN_free(y);
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    // Create EVP_PKEY from EC_KEY
    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey) {
        BN_free(x);
        BN_free(y);
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    if (!EVP_PKEY_set1_EC_KEY(pkey, pub_key)) {
        EVP_PKEY_free(pkey);
        BN_free(x);
        BN_free(y);
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    // Verify signature
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    if (!md_ctx) {
        EVP_PKEY_free(pkey);
        BN_free(x);
        BN_free(y);
        EC_POINT_free(point);
        EC_KEY_free(pub_key);
        return false;
    }
    
    bool result = false;
    if (EVP_DigestVerifyInit(md_ctx, NULL, EVP_sha256(), NULL, pkey)) {
        if (EVP_DigestVerifyUpdate(md_ctx, message.c_str(), message.length())) {
            result = (EVP_DigestVerifyFinal(md_ctx, sig_bytes.data(), sig_bytes.size()) == 1);
        }
    }
    
    // Cleanup
    EVP_MD_CTX_free(md_ctx);
    EVP_PKEY_free(pkey);
    BN_free(x);
    BN_free(y);
    EC_POINT_free(point);
    EC_KEY_free(pub_key);
    
    return result;
}

void Wallet::sendMoney(const std::string& recipient, double amount, Transaction& tx) {
    if (amount <= 0) {
        throw std::runtime_error("Amount must be positive");
    }
    if (amount > balance) {
        throw std::runtime_error("Insufficient balance");
    }
    
    // Update the transaction with sender, recipient, and amount
    tx.sender = getPublicKeyHex();
    tx.receiver = recipient;
    tx.amount = amount;
    
    // Calculate the hash and sign it
    tx.hash = tx.calculateHash();
    tx.signature = signMessage(tx.hash);
    
    // Update balance
    balance -= amount;
}

void Wallet::receiveMoney(double amount) {
    if (amount <= 0) {
        throw std::runtime_error("Amount must be positive");
    }
    balance += amount;
}

