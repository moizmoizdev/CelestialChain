#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include <string>
#include "Block.h"
#include "Transaction.h"
#include "wallet.h"
#include "Types.h"

class Blockchain {
private:
    std::vector<Block> chain;
    std::vector<Transaction> mempool;
    int difficulty;
    

public:
    Blockchain(int difficulty = 4);
    
    void addBlock(const std::vector<Transaction>& transactions);
    void addTransaction(const Transaction& transaction);
    Block& mineBlock(std::vector<Wallet>& wallets, NodeType nodeType = NodeType::FULL_NODE);
    
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
};

#endif // BLOCKCHAIN_H 