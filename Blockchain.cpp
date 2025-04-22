#include "Blockchain.h"
#include <iostream>


Blockchain::Blockchain(int difficulty) : difficulty(difficulty) {
    // genesis block
    std::vector<Transaction> genesisTransactions;
    Transaction genesisTx("Genesis", "Genesis", 0);
    genesisTx.hash = genesisTx.calculateHash();
    genesisTransactions.push_back(genesisTx);
    
    // prev hash for genesis block will be 0
    Block genesisBlock(0, genesisTransactions, "0x0", difficulty);
    genesisBlock.timestamp = GENESIS_TIMESTAMP;
    genesisBlock.nonce     = GENESIS_NONCE;
    genesisBlock.hash      = GENESIS_HASH;
    chain.push_back(genesisBlock);
    
    std::cout << "Blockchain initialized with genesis block: " << genesisBlock.hash << std::endl;
}

void Blockchain::addBlock(const std::vector<Transaction>& transactions) {
    Block newBlock(chain.size(), transactions, getLatestBlock().hash, difficulty);
    chain.push_back(newBlock);
    
    // Persist the block to database
    if (db && !db->saveBlock(newBlock)) {
        std::cerr << "Failed to save block to database: " << db->getLastError() << std::endl;
    }
    
    std::cout << "Block #" << newBlock.blockNumber << " added to the blockchain." << std::endl;
    std::cout << "Hash: " << newBlock.hash << std::endl;
}

// Push a block received from the network (doesnt remine)
void Blockchain::addExistingBlock(const Block& block) {
    if(block.previousHash != getLatestBlock().hash){
        throw std::runtime_error("Blockchain integrity compromised. Previous hash mismatch.");
    }
    chain.push_back(block);
    
    // Persist the block to database
    if (db && !db->saveBlock(block)) {
        std::cerr << "Failed to save block to database: " << db->getLastError() << std::endl;
    }
    
    mempool.clear();
    std::cout << "Block #" << block.blockNumber << " added to the blockchain." << std::endl;
}

void Blockchain::addTransaction(const Transaction& transaction) {
    for (auto& old : mempool) {
        if (old.hash == transaction.hash) return;
    }
    
    // Add to mempool
    mempool.push_back(transaction);
    
    // Persist transaction to database
    if (db && !db->saveTransaction(transaction)) {
        std::cerr << "Failed to save transaction to database: " << db->getLastError() << std::endl;
    }
    
    std::cout << "Transaction added to mempool: " << transaction.sender << " -> " 
              << transaction.receiver << ": " << transaction.amount << std::endl;
}

Block& Blockchain::mineBlock(std::vector<Wallet*>& walletList, NodeType nodeType) {
    // Store the wallets for future use
    wallets = walletList;
    
    // Check if this is a wallet node trying to mine
    if (nodeType == NodeType::WALLET_NODE) {
        throw std::runtime_error("ERROR: Wallet nodes cannot mine blocks.");
    }
    
    if (mempool.empty()) {
        throw std::runtime_error("No transactions in mempool to mine.");
    }
    
    std::cout << "Mining new block with " << mempool.size() << " transactions..." << std::endl;
    
    Block newBlock(chain.size(), mempool, getLatestBlock().hash, difficulty);
    chain.push_back(newBlock);
    
    std::cout << "Block #" << newBlock.blockNumber << " mined successfully!" << std::endl;
    std::cout << "Hash: " << newBlock.hash << std::endl;
    std::cout << "Nonce: " << newBlock.nonce << std::endl;
    
    for (const auto& tx : mempool) {
        for (auto& wallet : wallets) {
            if (wallet->getAddress() == tx.receiver) {
                wallet->receiveMoney(tx.amount);
                break;
            }
        }
    }
    
    mempool.clear();
    return chain.back();
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
    
    for (size_t i = 1; i < chain.size(); i++) {
        const Block& currentBlock = chain[i];
        const Block& previousBlock = chain[i - 1];
        
        if (currentBlock.previousHash != previousBlock.hash) {
            return false;
        }
        
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

// Add a method to set the database
void Blockchain::setDatabase(BlockchainDB* database) {
    db = database;
}

// Add a method to load the blockchain from database
void Blockchain::loadFromDatabase() {
    if (!db) {
        std::cerr << "No database connection available" << std::endl;
        return;
    }
    
    // Clear existing chain except genesis block
    if (!chain.empty()) {
        Block genesisBlock = chain[0];
        chain.clear();
        chain.push_back(genesisBlock);
    }
    
    // Get all block keys
    auto keys = db->getAllKeys("block:");
    
    // Sort keys to ensure blocks are loaded in order
    std::sort(keys.begin(), keys.end(), [](const std::string& a, const std::string& b) {
        // Extract block numbers for numeric comparison
        int numA = std::stoi(a.substr(6)); // "block:".length() == 6
        int numB = std::stoi(b.substr(6));
        return numA < numB;
    });
    
    std::cout << "Found " << keys.size() << " blocks in database" << std::endl;
    
    // Keep track of previously loaded block hash for validation
    std::string prevHash = chain.empty() ? "0x0" : chain.back().hash;
    
    // First pass: load blocks in order and validate basic structure
    std::vector<Block> loadedBlocks;
    
    for (const auto& key : keys) {
        int blockNumber = std::stoi(key.substr(6)); // "block:".length() == 6
        
        // Skip genesis block (already in chain)
        if (blockNumber == 0 && !chain.empty()) {
            std::cout << "Skipping genesis block (already loaded)" << std::endl;
            continue;
        }
        
        try {
            Block block(0, {}, "", difficulty); // Temporary block
            
            if (db->getBlock(blockNumber, block)) {
                // Basic validation
                if (loadedBlocks.empty() && !chain.empty()) {
                    // First loaded block should link to genesis
                    if (block.previousHash != chain.back().hash) {
                        std::cerr << "Warning: Block #" << blockNumber 
                                << " does not link to genesis. Expected: " 
                                << chain.back().hash << ", Got: " << block.previousHash << std::endl;
                        // Still add it for now
                    }
                } else if (!loadedBlocks.empty()) {
                    // Each subsequent block should link to previous
                    if (block.previousHash != loadedBlocks.back().hash) {
                        std::cerr << "Warning: Block #" << blockNumber 
                                << " does not link to previous block. Expected: " 
                                << loadedBlocks.back().hash << ", Got: " << block.previousHash << std::endl;
                        // Still add it for now
                    }
                }
                
                // Additional validation: Check block's hash against its contents
                std::string calculatedHash = block.calculateHash();
                if (block.hash != calculatedHash) {
                    std::cerr << "Warning: Block #" << blockNumber << " has invalid hash. "
                            << "Stored: " << block.hash << ", Calculated: " << calculatedHash << std::endl;
                    // We'll still add it, but be aware it might be corrupted
                }
                
                loadedBlocks.push_back(block);
                std::cout << "Loaded block #" << blockNumber << " with hash " << block.hash << std::endl;
            } else {
                std::cerr << "Failed to load block " << key << ": " << db->getLastError() << std::endl;
                
                // Attempt to repair database by removing the corrupted entry
                if (db->getLastError().find("Invalid block data") != std::string::npos) {
                    std::cerr << "Attempting to remove corrupted block entry: " << key << std::endl;
                    if (db->remove(key)) {
                        std::cerr << "Successfully removed corrupted block entry" << std::endl;
                    } else {
                        std::cerr << "Failed to remove corrupted block entry: " << db->getLastError() << std::endl;
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error loading block #" << blockNumber << ": " << e.what() << std::endl;
            // Continue to next block rather than failing entirely
        }
    }
    
    // Add loaded blocks to the chain
    for (const auto& block : loadedBlocks) {
        chain.push_back(block);
    }
    
    std::cout << "Loaded " << chain.size() << " blocks from database" << std::endl;
    
    // Load mempool transactions
    try {
        auto txKeys = db->getAllKeys("tx:");
        for (const auto& key : txKeys) {
            // Skip transactions that are already in blocks
            std::string txHash = key.substr(3); // "tx:".length() == 3
            bool inChain = false;
            
            for (const auto& block : chain) {
                for (const auto& tx : block.transactions) {
                    if (tx.hash == txHash) {
                        inChain = true;
                        break;
                    }
                }
                if (inChain) break;
            }
            
            if (!inChain) {
                Transaction tx("", "", 0);
                if (db->getTransaction(txHash, tx)) {
                    if (tx.isValid()) {
                        mempool.push_back(tx);
                        std::cout << "Loaded transaction " << txHash << " to mempool" << std::endl;
                    } else {
                        std::cerr << "Skipping invalid transaction " << txHash << std::endl;
                    }
                }
            }
        }
        
        std::cout << "Loaded " << mempool.size() << " transactions to mempool" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading mempool: " << e.what() << std::endl;
    }
}

const time_t     Blockchain::GENESIS_TIMESTAMP = 1745026508;
const int Blockchain::GENESIS_NONCE     =  27701;
const std::string Blockchain::GENESIS_HASH      = "0x0000eb99d08f42f3c322b891f18212c85aa05365166964973a56d03e7da36f80";