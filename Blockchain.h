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

class Blockchain {
private:
    std::vector<Block> chain;
    std::vector<Transaction> mempool;
    int difficulty;
    BlockchainDB* db;  // Database connection

    static const time_t GENESIS_TIMESTAMP;
    static const int     GENESIS_NONCE;
    static const std::string GENESIS_HASH;
public:
    Blockchain(int difficulty = 4) ;
    
    void addBlock(const std::vector<Transaction>& transactions);
    void addExistingBlock(const Block& block);
    void addTransaction(const Transaction& transaction);
    Block& mineBlock(Wallet& wallet, NodeType nodeType = NodeType::FULL_NODE);
    
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

    // Database operations
    void setDatabase(BlockchainDB* database);
    void loadFromDatabase();
};

#endif // BLOCKCHAIN_H 