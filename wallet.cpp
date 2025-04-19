#include "wallet.h"
#include "crypto_utils.h"
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
#include <stdexcept>
#include <iostream>

Wallet::Wallet() : balance(100.0) {
    // Generate a new EC key pair
    key_pair = generateECKeyPair();
    if (!key_pair) {
        std::cerr << "ERROR: Failed to generate EC key pair" << std::endl;
        return;
    }
    
    // Get the public key in hex format
    publicKey = ::getPublicKeyHex(key_pair);
    
    // Derive the wallet address from the public key
    address = ::deriveAddressFromPublicKey(publicKey);
    
    std::cout << "Created new wallet with address: " << address << std::endl;
    std::cout << "DEBUG - Public Key: " << publicKey << std::endl;
    std::cout << "DEBUG - Derived Address: " << address << std::endl;
}

Wallet::~Wallet() {
    // Free the EC key pair
    if (key_pair) {
        EC_KEY_free(key_pair);
        key_pair = nullptr;
    }
}

std::string Wallet::getPublicKeyHex() const {
    if (!key_pair) {
        std::cerr << "ERROR: Wallet has no key pair" << std::endl;
        return "";
    }
    
    std::string result = ::getPublicKeyHex(key_pair);
    if (result.empty()) {
        std::cerr << "ERROR: Failed to get public key hex" << std::endl;
    }
    return result;
}

std::string Wallet::getAddress() const {
    return address;
}

double Wallet::getBalance() const {
    return balance;
}

std::string Wallet::signMessage(const std::string& message) const {
    if (!key_pair) {
        std::cerr << "ERROR: Cannot sign message - wallet has no key pair" << std::endl;
        return "";
    }
    
    std::string signature = ::signMessage(key_pair, message);
    if (signature.empty()) {
        std::cerr << "ERROR: Failed to sign message" << std::endl;
    } else {
        std::cout << "DEBUG - Message signed successfully" << std::endl;
    }
    return signature;
}

bool Wallet::verifySignature(const std::string& message, const std::string& signature, const std::string& publicKeyOrAddress) {
    std::cout << "DEBUG - Static verify signature called with:" << std::endl;
    std::cout << "  Message: " << message << std::endl;
    std::cout << "  Signature: " << (signature.length() > 20 ? signature.substr(0, 20) + "..." : signature) << std::endl;
    std::cout << "  Public Key/Address: " << publicKeyOrAddress << std::endl;

    // Call the global verification function
    bool result = ::verifySignature(message, signature, publicKeyOrAddress);
    std::cout << "Signature verification result: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    return result;
}

bool Wallet::sendMoney(double amount, const std::string& receiverAddress, Transaction& transaction) {
    if (amount <= 0) {
        std::cerr << "ERROR: Cannot send non-positive amount" << std::endl;
        return false;
    }
    
    if (amount > balance) {
        std::cerr << "ERROR: Insufficient funds. Balance: " << balance << ", Trying to send: " << amount << std::endl;
        return false;
    }
    
    std::cout << "\n===== Creating Transaction =====" << std::endl;
    std::cout << "  Sender address: " << address << std::endl;
    std::cout << "  Receiver address: " << receiverAddress << std::endl;
    std::cout << "  Amount: " << amount << std::endl;
    
    // Create a new transaction with our address as the sender
    transaction = Transaction(address, receiverAddress, amount);
    
    std::cout << "  Transaction hash: " << transaction.hash << std::endl;
    
    // Sign the transaction with our private key
    std::cout << "  Signing with private key..." << std::endl;
    transaction.sign(*this);
    
    // Update the balance
    balance -= amount;
    
    std::cout << "  Transaction created and signed successfully" << std::endl;
    std::cout << "  New balance: " << balance << std::endl;
    std::cout << "===== Transaction Complete =====" << std::endl;
    
    return true;
}

void Wallet::receiveMoney(double amount) {
    if (amount <= 0) {
        std::cerr << "ERROR: Cannot receive non-positive amount" << std::endl;
        return;
    }
    
    balance += amount;
    std::cout << "Received " << amount << " coins. New balance: " << balance << std::endl;
}

// Implement private methods

void Wallet::generateKeyPair() {
    // Already handled in constructor
}

std::string Wallet::deriveAddress(const std::string& pubKey) const {
    return deriveAddressFromPublicKey(pubKey);
}

std::string Wallet::sign(const std::string& message) const {
    return signMessage(message);
}