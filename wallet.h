#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include <openssl/ec.h>
#include "crypto_utils.h"

class Transaction;

class Wallet {
private:
    EC_KEY* key_pair;
    std::string address;
    std::string publicKey;
    double balance;
    
    void generateKeyPair();
    std::string deriveAddress(const std::string& pubKey) const;
    std::string sign(const std::string& message) const;
    
public:
    Wallet();
    ~Wallet();
    
    std::string getPublicKeyHex() const;
    std::string getAddress() const;
    double getBalance() const;
    
    std::string signMessage(const std::string& message) const;
    static bool verifySignature(const std::string& message, const std::string& signature, const std::string& publicKeyOrAddress);
    
    bool sendMoney(double amount, const std::string& receiverAddress, Transaction& transaction);
    void receiveMoney(double amount);
};

#endif // WALLET_H