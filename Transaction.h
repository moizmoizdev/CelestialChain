#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

class Transaction {
public:
    std::string sender;
    std::string receiver;
    int amount;
    std::string hash;
    std::string signature;
    unsigned long timestamp;
    
    Transaction(std::string sender, std::string receiver, int amount);
    std::string calculateHash() const;
    bool verifySignature() const;
    bool isValid() const;
    void print() const;
};

#endif 