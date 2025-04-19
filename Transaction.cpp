#include "Transaction.h"
#include "sha.h"
#include "wallet.h"
#include <iostream>
#include <sstream>
#include <ctime>

Transaction::Transaction(std::string sender, std::string receiver, double amount)
    : sender(sender), receiver(receiver), amount(amount), timestamp(time(nullptr)) {
    // Calculate the hash but leave signature empty
    hash = calculateHash();
}

Transaction::Transaction(std::string sender, std::string receiver, double amount, std::string signature)
    : sender(sender), receiver(receiver), amount(amount), signature(signature), timestamp(time(nullptr)) {
    // Calculate the hash with the given signature
    hash = calculateHash();
}

std::string Transaction::calculateHash() const {
    std::string data = sender + receiver + std::to_string(amount) + std::to_string(timestamp);
    return computeSHA256(data);
}

bool Transaction::verifySignature() const {
    // In a real blockchain, this would verify the signature using the sender's public key
    // For our demo, we'll use the Wallet's verification method
    
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
        std::cout<<"Sender and receiver is false"<<std::endl;
        return false;
    }
    
    if (amount <= 0) {
        std::cout<<"amount is false"<<std::endl;
        return false;
    }
    
    // Verify the hash matches
    if (hash != calculateHash()) {
        std::cout<<"Hash is false"<<std::endl;
        return false;
    }
    // Verify signature
    bool b= verifySignature();
    return b;
}

void Transaction::print() const {
    std::cout << "Transaction: " << sender << " -> " << receiver << ": " << amount << std::endl;
    std::cout << "Hash: " << hash << std::endl;
    std::cout << "Signature: " << (!signature.empty() ? signature.substr(0, 20) + "..." : "unsigned") << std::endl;
    std::cout << "Timestamp: " << timestamp << std::endl;
}

void Transaction::sign(const Wallet& wallet) {
    // Sign the transaction using the wallet's private key
    signature = wallet.signMessage(hash);
}