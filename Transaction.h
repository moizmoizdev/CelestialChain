#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Wallet;

class Transaction {
public:
    std::string sender;         // Sender's address (0x...) 
    std::string senderPublicKey; // Sender's public key (0x04...)
    std::string receiver;        // Receiver's address
    double amount;
    std::string hash;
    std::string signature;
    unsigned long timestamp;
    
    // Constructor for creating new transactions
    Transaction(std::string sender, std::string receiver, double amount);
    
    // Constructor for recreating transactions from network/storage
    Transaction(std::string sender, std::string senderPublicKey, std::string receiver, 
                double amount, std::string hash, std::string signature, unsigned long timestamp);
    
    std::string calculateHash() const;
    bool verifySignature() const;
    bool verifyAddress() const; // Verify the public key matches the claimed address
    bool isValid() const;
    void print() const;
    void sign(const Wallet& wallet);
};

#endif 