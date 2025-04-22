#ifndef BALANCEMAPPING_H
#define BALANCEMAPPING_H

#include <string>
#include <map>
#include "BlockchainDB.h"

class BalanceMapping {
private:
    BlockchainDB* db; // Database connection

public:
    // Constructor takes a connection to the database
    BalanceMapping(BlockchainDB* database);
    
    // Core balance operations
    bool updateBalance(const std::string& address, double newBalance);
    bool getBalance(const std::string& address, double& balance) const;
    
    // Process a transaction by updating sender and receiver balances
    bool processTransaction(const std::string& sender, const std::string& receiver, double amount);
    
    // Special method for genesis/coin generation transactions
    bool processCoinGeneration(const std::string& receiver, double amount);
    
    // Get all balances for reporting/display
    std::map<std::string, double> getAllBalances() const;
};

#endif
