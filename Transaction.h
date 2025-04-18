#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Wallet;

class Transaction {
public:
    std::string sender;
    std::string receiver;
    double amount;
    std::string hash;
    std::string signature;
    unsigned long timestamp;
    
    Transaction(std::string sender, std::string receiver, double amount);
    Transaction(std::string sender, std::string receiver, double amount, std::string signature);
    
    std::string calculateHash() const;
    bool verifySignature() const;
    bool isValid() const;
    void print() const;
    void sign(const Wallet& wallet);
};

#endif 