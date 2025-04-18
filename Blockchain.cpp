#include "Blockchain.h"
#include <iostream>


Blockchain::Blockchain(int difficulty) : difficulty(difficulty) {
    // Create genesis block
    std::vector<Transaction> genesisTransactions;
    Transaction genesisTx("Genesis", "Genesis", 0);
    genesisTx.hash = genesisTx.calculateHash();
    genesisTransactions.push_back(genesisTx);
    
    // Previous hash for genesis block is 0
    Block genesisBlock(0, genesisTransactions, "0", difficulty);
    chain.push_back(genesisBlock);
    
    std::cout << "Blockchain initialized with genesis block: " << genesisBlock.hash << std::endl;
}

void Blockchain::addBlock(const std::vector<Transaction>& transactions) {
    Block newBlock(chain.size(), transactions, getLatestBlock().hash, difficulty);
    chain.push_back(newBlock);
    
    std::cout << "Block #" << newBlock.blockNumber << " added to the blockchain." << std::endl;
    std::cout << "Hash: " << newBlock.hash << std::endl;
}

void Blockchain::addTransaction(const Transaction& transaction) {
    mempool.push_back(transaction);
    std::cout << "Transaction added to mempool: " << transaction.sender << " -> " << transaction.receiver << ": " << transaction.amount << std::endl;
}

Block& Blockchain::mineBlock(std::vector<Wallet>& wallets, NodeType nodeType) {
    // Check if this is a wallet node trying to mine
    if (nodeType == NodeType::WALLET_NODE) {
        
        throw "ERROR: Wallet nodes cannot mine blocks.";
    }
    
    if (mempool.empty()) {
        throw "No transactions in mempool to mine.";
    }
    
    std::cout << "Mining new block with " << mempool.size() << " transactions..." << std::endl;
    
    // Create a new block with all transactions in the mempool
    Block newBlock(chain.size(), mempool, getLatestBlock().hash, difficulty);
    chain.push_back(newBlock);
    
    std::cout << "Block #" << newBlock.blockNumber << " mined successfully!" << std::endl;
    std::cout << "Hash: " << newBlock.hash << std::endl;
    std::cout << "Nonce: " << newBlock.nonce << std::endl;
    
    // Process transactions for wallet balances
    for (const auto& tx : mempool) {
        // Find receiver wallet and update balance
        for (auto& wallet : wallets) {
            if (wallet.getPublicKeyHex() == tx.receiver) {
                wallet.receiveMoney(tx.amount);
                break;
            }
        }
    }
    
    // Clear the mempool after mining
    mempool.clear();
    return newBlock;
}

const Block& Blockchain::getLatestBlock() const {
    return chain.back();
}

size_t Blockchain::getChainSize() const {
    return chain.size();
}

const Block& Blockchain::getBlock(size_t index) const {
    if (index >= chain.size()) {
        throw std::out_of_range("Block index out of range");
    }
    return chain[index];
}

const std::vector<Block>& Blockchain::getChain() const {
    return chain;
}

size_t Blockchain::getMempoolSize() const {
    return mempool.size();
}

const std::vector<Transaction>& Blockchain::getMempool() const {
    return mempool;
}

bool Blockchain::isValidChain() const {
    // Check if chain is empty
    if (chain.empty()) {
        return false;
    }
    
    // Check each block
    for (size_t i = 1; i < chain.size(); i++) {
        const Block& currentBlock = chain[i];
        const Block& previousBlock = chain[i - 1];
        
        // Check if the previous hash matches
        if (currentBlock.previousHash != previousBlock.hash) {
            return false;
        }
        
        // Check if the hash is valid
        if (currentBlock.hash != currentBlock.calculateHash()) {
            return false;
        }
    }
    
    return true;
}

std::string Blockchain::toString() const {
    std::string result = "Blockchain:\n";
    for (const auto& block : chain) {
        result += "Block #" + std::to_string(block.blockNumber) + "\n";
        result += "  Hash: " + block.hash + "\n";
        result += "  Previous Hash: " + block.previousHash + "\n";
        result += "  Nonce: " + std::to_string(block.nonce) + "\n";
        result += "  Timestamp: " + std::to_string(block.timestamp) + "\n";
        result += "  Transactions: " + std::to_string(block.transactions.size()) + "\n";
        for (const auto& tx : block.transactions) {
            result += "    - " + tx.sender + " -> " + tx.receiver + ": " + std::to_string(tx.amount) + "\n";
        }
    }
    return result;
}

void Blockchain::printBlockchain() const {
    std::cout << toString() << std::endl;
}

void Blockchain::printMempool() const {
    std::cout << "Mempool (" << mempool.size() << " transactions):" << std::endl;
    for (const auto& tx : mempool) {
        std::cout << "  - " << tx.sender << " -> " << tx.receiver << ": " << tx.amount << std::endl;
    }
} 