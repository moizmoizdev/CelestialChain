#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include <openssl/ec.h>
#include "crypto_utils.h"
#include "BlockchainDB.h"

class Transaction;

class Wallet {
private:
    EC_KEY* key_pair;
    std::string address;
    std::string publicKey;
    double balance;
    BlockchainDB* db;  // Database connection for transactions
    
    void generateKeyPair();
    std::string deriveAddress(const std::string& pubKey) const;
    std::string sign(const std::string& message) const;
    
    // INI file operations
    bool saveToIniFile() const;
    bool loadFromIniFile(const std::string& walletAddress);
    std::string getIniFilePath() const;
    
public:
    Wallet();
    Wallet(BlockchainDB* database);  // Constructor with database
    ~Wallet();
    
    std::string getPublicKeyHex() const;
    std::string getAddress() const;
    double getBalance() const;
    
    std::string signMessage(const std::string& message) const;
    static bool verifySignature(const std::string& message, const std::string& signature, const std::string& publicKeyOrAddress);
    
    bool sendMoney(double amount, const std::string& receiverAddress, Transaction& transaction);
    void receiveMoney(double amount);

    // Database operations (for transactions only)
    void setDatabase(BlockchainDB* database);
    bool updateBalance();
    std::vector<Transaction> getTransactionHistory() const;

    // Method to synchronize wallet balance with the database
    void synchronizeBalance(double newBalance);
};

#endif // WALLET_H