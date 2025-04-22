#include "balanceMapping.h"
#include <iostream>

BalanceMapping::BalanceMapping(BlockchainDB* database) : db(database) {
    if (!db || !db->isOpen()) {
        std::cerr << "Error: Invalid database connection for BalanceMapping" << std::endl;
    }
}

bool BalanceMapping::updateBalance(const std::string& address, double newBalance) {
    if (!db) return false;
    
    std::string key = "balance:" + address;
    return db->put(key, std::to_string(newBalance));
}

bool BalanceMapping::getBalance(const std::string& address, double& balance) const {
    if (!db) return false;
    
    std::string key = "balance:" + address;
    std::string value;
    
    if (!db->get(key, value)) {
        // Not finding a balance isn't an error - it's a new address
        balance = 0.0;
        return true;
    }
    
    try {
        balance = std::stod(value);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing balance: " << e.what() << std::endl;
        return false;
    }
}

bool BalanceMapping::processTransaction(const std::string& sender, const std::string& receiver, double amount) {
    if (!db) return false;
    
    // Handle Genesis transactions or coin generation transactions
    if (sender == "Genesis") {
        return processCoinGeneration(receiver, amount);
    }
    
    // Get sender balance
    double senderBalance = 0.0;
    if (!getBalance(sender, senderBalance)) {
        return false;
    }
    
    // Verify sender has enough funds
    if (senderBalance < amount) {
        std::cerr << "Insufficient funds: " << sender << " has " << senderBalance 
                  << " but attempted to send " << amount << std::endl;
        return false;
    }
    
    // Get receiver balance
    double receiverBalance = 0.0;
    if (!getBalance(receiver, receiverBalance)) {
        return false;
    }
    
    // Create a batch update to ensure atomicity
    std::vector<std::pair<std::string, std::string>> operations;
    
    // Update sender balance
    operations.push_back(std::make_pair(
        "balance:" + sender, 
        std::to_string(senderBalance - amount)
    ));
    
    // Update receiver balance
    operations.push_back(std::make_pair(
        "balance:" + receiver, 
        std::to_string(receiverBalance + amount)
    ));
    
    return db->writeBatch(operations);
}

bool BalanceMapping::processCoinGeneration(const std::string& receiver, double amount) {
    if (!db) return false;
    
    // For coin generation, we only need to credit the receiver
    double receiverBalance = 0.0;
    if (!getBalance(receiver, receiverBalance)) {
        return false;
    }
    
    // Update receiver balance (no batch needed since we're only updating one entry)
    return updateBalance(receiver, receiverBalance + amount);
}

std::map<std::string, double> BalanceMapping::getAllBalances() const {
    std::map<std::string, double> balances;
    if (!db) return balances;
    
    std::string prefix = "balance:";
    auto keys = db->getAllKeys(prefix);
    
    for (const auto& key : keys) {
        std::string value;
        if (db->get(key, value)) {
            std::string address = key.substr(prefix.length());
            try {
                double balance = std::stod(value);
                balances[address] = balance;
            } catch (const std::exception& e) {
                // Skip invalid balance values
            }
        }
    }
    
    return balances;
}


