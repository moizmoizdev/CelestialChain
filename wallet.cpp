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
    return ::getPublicKeyHex(key_pair);
}

std::string Wallet::signMessage(const std::string& message) const {
    // Delegate signing to crypto_utils
    return ::signMessage(key_pair, message);
}

bool Wallet::verifySignature(const std::string& message,
                              const std::string& signature,
                              const std::string& publicKeyHex) {
    // Delegate verification to crypto_utils
    return ::verifySignature(message, signature, publicKeyHex);
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

