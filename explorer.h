#ifndef EXPLORER_H
#define EXPLORER_H

#include <string>
#include <vector>
#include "Blockchain.h"
#include "BlockchainDB.h"
#include "Transaction.h"
#include "Block.h"
#include "balanceMapping.h"

class Explorer {
private:
    Blockchain* blockchain;
    BlockchainDB* db;
    BalanceMapping* balanceMap;

public:
    Explorer(Blockchain* chain, BlockchainDB* database, BalanceMapping* balances);
    
    // Basic explorer functions
    void showExplorerMenu(const std::string& currentWalletAddress = "") const;
    
    // Simple data retrieval methods
    double getAddressBalance(const std::string& address) const;
    Block getBlockByNumber(size_t blockNumber) const;
    size_t getBlockCount() const;
    size_t getTransactionCount() const;
    
    // Simple display helpers
    void displayAddressDetails(const std::string& address) const;
    void displayTransactionDetails(const std::string& txHash) const;
    void displayBlockDetails(size_t blockNumber) const;
    void displayLatestBlocks(size_t count = 5) const;
};

#endif // EXPLORER_H
