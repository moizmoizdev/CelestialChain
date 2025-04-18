#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/evp.h>

class Transaction;

class Wallet {
private:
    EC_KEY* key_pair;
    std::string address;
    double balance;
    
    void generateKeyPair();
    std::string deriveAddress(const std::string& pubKey) const;
    std::string sign(const std::string& message) const;
    
public:
    Wallet();
    ~Wallet();
    
    std::string getPublicKeyHex() const;
    std::string signMessage(const std::string& message) const;
    static bool verifySignature(const std::string& message, const std::string& signature, const std::string& publicKeyHex);
    
    void sendMoney(const std::string& recipient, double amount, Transaction& tx);
    void receiveMoney(double amount);
    
    std::string getAddress() const { return address; }
    double getBalance() const { return balance; }
};

#endif // WALLET_H