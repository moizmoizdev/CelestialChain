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
    if (!db) {
        std::cerr << "ERROR: Database not available for processing transaction" << std::endl;
        return false;
    }
    
    // Special case: Skip the Genesis block's genesis transaction
    if (sender == "Genesis" && receiver == "Genesis") {
        std::cout << "Skipping Genesis-to-Genesis transaction" << std::endl;
        return true;  // Consider it processed successfully, but we don't change any balances
    }
    
    // Handle mining rewards (Genesis sender)
    if (sender == "Genesis") {
        std::cout << "Processing mining reward of " << amount << " to " << receiver << std::endl;
        return processCoinGeneration(receiver, amount);
    }
    
    // Get sender balance
    double senderBalance = 0.0;
    if (!getBalance(sender, senderBalance)) {
        std::cerr << "ERROR: Failed to retrieve sender balance" << std::endl;
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
        std::cerr << "ERROR: Failed to retrieve receiver balance" << std::endl;
        return false;
    }
    
    // Calculate new balances
    double newSenderBalance = senderBalance - amount;
    double newReceiverBalance = receiverBalance + amount;
    
    // Create a batch update to ensure atomicity
    std::vector<std::pair<std::string, std::string>> operations;
    
    // Update sender balance
    operations.push_back(std::make_pair(
        "balance:" + sender, 
        std::to_string(newSenderBalance)
    ));
    
    // Update receiver balance
    operations.push_back(std::make_pair(
        "balance:" + receiver, 
        std::to_string(newReceiverBalance)
    ));
    
    // Execute the batch update
    bool success = db->writeBatch(operations);
    
    if (success) {
        std::cout << "Transaction processed successfully:" << std::endl;
        std::cout << "- " << sender << ": " << senderBalance << " -> " << newSenderBalance << std::endl;
        std::cout << "- " << receiver << ": " << receiverBalance << " -> " << newReceiverBalance << std::endl;
    } else {
        std::cerr << "ERROR: Failed to write transaction to database" << std::endl;
    }
    
    return success;
}

bool BalanceMapping::processCoinGeneration(const std::string& receiver, double amount) {
    if (!db) {
        std::cerr << "ERROR: Database not available for processing coin generation" << std::endl;
        return false;
    }
    
    // For coin generation, we only need to credit the receiver
    double receiverBalance = 0.0;
    if (!getBalance(receiver, receiverBalance)) {
        std::cerr << "ERROR: Failed to retrieve receiver balance for coin generation" << std::endl;
        return false;
    }
    
    // Calculate new balance
    double newBalance = receiverBalance + amount;
    
    // Update receiver balance
    bool success = updateBalance(receiver, newBalance);
    
    if (success) {
        std::cout << "Mining reward processed:" << std::endl;
        std::cout << "- " << receiver << ": " << receiverBalance << " -> " << newBalance << std::endl;
    } else {
        std::cerr << "ERROR: Failed to update balance for mining reward" << std::endl;
    }
    
    return success;
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


