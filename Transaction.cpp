#include "Transaction.h"
#include "sha.h"
#include "wallet.h"
#include <iostream>
#include <sstream>
#include <ctime>

Transaction::Transaction(std::string sender, std::string receiver, int amount)
    : sender(sender), receiver(receiver), amount(amount), timestamp(time(nullptr)) {
    // The hash and signature will be set later
}

std::string Transaction::calculateHash() const {
    SHA256 sha;
    std::string data = sender + receiver + std::to_string(amount) + std::to_string(timestamp);
    return sha.hash(data);
}

bool Transaction::verifySignature() const {
    // In a real blockchain, this would verify the signature using the sender's public key
    // For our demo, we'll use the Wallet's static verification method
    
    // Skip verification for the genesis transaction
    if (sender == "Genesis" && receiver == "Genesis") {
        return true;
    }
    
    return Wallet::verifySignature(hash, signature, sender);
}

bool Transaction::isValid() const {
    // Check if the transaction is valid
    
    // Special case for genesis transaction
    if (sender == "Genesis" && receiver == "Genesis") {
        return true;
    }
    
    // Regular transaction validations
    if (sender.empty() || receiver.empty()) {
        return false;
    }
    
    if (amount <= 0) {
        return false;
    }
    
    // Verify the hash matches
    if (hash != calculateHash()) {
        return false;
    }
    
    // Verify signature
    return verifySignature();
}

void Transaction::print() const {
    std::cout << "Transaction: " << sender << " -> " << receiver << ": " << amount << std::endl;
    std::cout << "Hash: " << hash << std::endl;
    std::cout << "Signature: " << (!signature.empty() ? signature.substr(0, 20) + "..." : "unsigned") << std::endl;
    std::cout << "Timestamp: " << timestamp << std::endl;
}