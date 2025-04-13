#include "wallet.h"
#include "sha.h"
#include <iostream>
#include <ctime>
#include <random>
#include <iomanip>
#include <sstream>

std::string generateRandomHex(size_t length) {
    static const char hex_chars[] = "0123456789abcdef";
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += hex_chars[dis(gen)];
    }
    
    return result;
}

Wallet::Wallet() {
    generateKeyPair();
}

void Wallet::generateKeyPair() {
    // In a real blockchain implementation, this would use proper cryptographic libraries
    // such as OpenSSL, libsecp256k1, or similar to generate ECDSA key pairs
    
    // For our simplified demonstration, we'll generate a random private key (64 hex chars)
    privateKey = generateRandomHex(64);
    
    // In real systems, the public key would be derived from the private key
    // using elliptic curve cryptography
    SHA256 sha;
    publicKey = sha.hash(privateKey);
    
    // Generate an address from the public key
    address = deriveAddress(publicKey);
    
    std::cout << "New wallet created:" << std::endl;
    std::cout << "  Address: " << address << std::endl;
}

std::string Wallet::deriveAddress(const std::string& pubKey) const {
    SHA256 sha;
    std::string hash = sha.hash(pubKey);
    std::string addr = "0x" + hash.substr(0, 40);
    
    return addr;
}

std::string Wallet::sign(const std::string& message) const {
    SHA256 sha;
    std::string hash = sha.hash(message + privateKey);
    
    return hash;
}

bool Wallet::verifySignature(const std::string& message, const std::string& signature, const std::string& publicKey) {
    SHA256 sha;
    std::string expectedHash = sha.hash(message + publicKey.substr(0, 32));
    
    return (signature.substr(0, 16) == expectedHash.substr(0, 16));
}

void Wallet::sendMoney(const std::string& recipient, int amount, Transaction& tx) {
    if (amount <= 0) {
        std::cout << "Transaction amount must be positive" << std::endl;
        return;
    }
    
    if (amount > balance) {
        std::cout << "Insufficient funds" << std::endl;
        return;
    }
    
    tx = Transaction(address, recipient, amount);
    
    std::string message = tx.calculateHash();
    tx.signature = sign(message);
    tx.hash = message;
    
    balance -= amount;
    
    std::cout << "Transaction created: " << amount << " sent to " << recipient << std::endl;
    std::cout << "Transaction hash: " << tx.hash << std::endl;
    std::cout << "Remaining balance: " << balance << std::endl;
}

void Wallet::receiveMoney(int amount) {
    if (amount <= 0) {
        std::cout << "Received amount must be positive" << std::endl;
        return;
    }
    
    balance += amount;
    std::cout << "Wallet " << address << " received " << amount << ". New balance: " << balance << std::endl;
}

