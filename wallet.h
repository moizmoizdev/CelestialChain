#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include "Transaction.h"

class Wallet {
private:
    int balance = 0;
    std::string privateKey;
    std::string publicKey;
    std::string address;

public:
    Wallet();
    void sendMoney(const std::string& recipient, int amount, Transaction& tx);
    void receiveMoney(int amount);
    
    // Getters
    int getBalance() const { return balance; }
    std::string getPublicKey() const { return publicKey; }
    std::string getAddress() const { return address; }
    
    // Key management
    void generateKeyPair();
    std::string deriveAddress(const std::string& pubKey) const;
    std::string sign(const std::string& message) const;
    static bool verifySignature(const std::string& message, const std::string& signature, const std::string& publicKey);
};

#endif // WALLET_H