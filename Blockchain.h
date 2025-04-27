#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include <algorithm>
#include "Block.h"
#include "Transaction.h"
#include "wallet.h"
#include "Types.h"
#include "BlockchainDB.h"
#include "balanceMapping.h"

class Blockchain {
private:
    std::vector<Block> chain;
    std::vector<Transaction> mempool;
    std::vector<Wallet*> wallets;  // To store wallet pointers for updating balances
    int difficulty;
    BlockchainDB* db;  // Database connection
    BalanceMapping* balanceMap; // Balance tracking
    
    // Calculate the current mining reward based on time since genesis
    double calculateCurrentMiningReward() const;
    
    // Number of days between halvings
    static const int HALVING_INTERVAL_DAYS;

public:
    // Genesis block constants - moved to public section
    static const time_t GENESIS_TIMESTAMP;
    static const int     GENESIS_NONCE;
    static const std::string GENESIS_HASH;
    static const double INITIAL_MINING_REWARD;   // Initial mining reward constant
    
    Blockchain(int difficulty = 4) ;
    
    void addBlock(const std::vector<Transaction>& transactions);
    void addExistingBlock(const Block& block);
    void addTransaction(const Transaction& transaction);
    Block& mineBlock(std::vector<Wallet*>& wallets, NodeType nodeType = NodeType::FULL_NODE);
    
    const Block& getLatestBlock() const;
    size_t getChainSize() const;
    const Block& getBlock(size_t index) const;
    const std::vector<Block>& getChain() const;
    size_t getMempoolSize() const;
    const std::vector<Transaction>& getMempool() const;
    
    std::string toString() const;
    void printBlockchain() const;
    void printMempool() const;
    bool isValidChain() const;
    
    // Get and set difficulty
    int getDifficulty() const;
    void setDifficulty(int newDifficulty);
    
    // Statistics methods
    double getTotalSupply() const;
    double getCurrentMiningReward() const;

    // Database operations
    void setDatabase(BlockchainDB* database);
    void loadFromDatabase();
    void rebuildBalancesFromTransactions();
    
    // Balance mapping operations
    void setBalanceMapping(BalanceMapping* mapping);
    void updateBalancesForBlock(const Block& block);
    bool verifyTransactionBalance(const Transaction& tx) const;
};

#endif // BLOCKCHAIN_H 