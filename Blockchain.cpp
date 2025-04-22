#include "Blockchain.h"
#include <iostream>


Blockchain::Blockchain(int difficulty) : difficulty(difficulty), db(nullptr), balanceMap(nullptr) {
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
    
    // Validate the transactions in the new block
    if (!newBlock.validateTransactions()) {
        throw std::runtime_error("ERROR: Block contains invalid transactions");
    }
    
    chain.push_back(newBlock);
    
    // Update balances for this block
    if (balanceMap) {
        updateBalancesForBlock(newBlock);
    }
    
    // Persist the block to database
    if (db && !db->saveBlock(newBlock)) {
        std::cerr << "Failed to save block to database: " << db->getLastError() << std::endl;
    }
    
    std::cout << "Block #" << newBlock.blockNumber << " added to the blockchain." << std::endl;
    std::cout << "Hash: " << newBlock.hash << std::endl;
}

// Push a block received from the network (doesnt remine)
void Blockchain::addExistingBlock(const Block& block) {
    if(block.previousHash != getLatestBlock().hash && block.blockNumber!=getLatestBlock().blockNumber){
        throw std::runtime_error("Blockchain integrity compromised. Previous hash mismatch. Previous hash: " + block.previousHash + " Current hash: " + getLatestBlock().hash);
    }
    
    if (block.blockNumber==getLatestBlock().blockNumber && block.hash==getLatestBlock().hash && block.previousHash==getLatestBlock().previousHash){
        return;
    }
    
    if (block.blockNumber==getLatestBlock().blockNumber && block.hash!=getLatestBlock().hash ){
        throw std::runtime_error("Blockchain integrity compromised. Block hash mismatch even though BlockNumber was same. Block hash: " + block.hash + " Current hash: " + getLatestBlock().hash);
    }
    
    // Validate the transactions in the block
    if (!block.validateTransactions()) {
        throw std::runtime_error("ERROR: Received block contains invalid transactions");
    }
    
    chain.push_back(block);
    
    // Update balances for this block
    if (balanceMap) {
        updateBalancesForBlock(block);
    }
    
    // Persist the block to database
    if (db && !db->saveBlock(block)) {
        std::cerr << "Failed to save block to database: " << db->getLastError() << std::endl;
    }
    
    mempool.clear();
    std::cout << "Block #" << block.blockNumber << " added to the blockchain." << std::endl;
}

void Blockchain::addTransaction(const Transaction& transaction) {
    // Verify the transaction has sufficient balance
    if (balanceMap && !verifyTransactionBalance(transaction)) {
        std::cerr << "Transaction rejected: Insufficient balance for " << transaction.sender << std::endl;
        return;
    }
    
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
    
    if (walletList.empty()) {
        throw std::runtime_error("ERROR: No wallet provided for mining reward.");
    }
    
    // Count how many empty blocks we've mined so far
    int emptyBlockCount = 0;
    for (const auto& block : chain) {
        // Count blocks that only have one transaction (the coinbase/reward)
        if (block.transactions.size() <= 1) {
            emptyBlockCount++;
        }
    }
    
    // After 3 empty blocks, require transactions
    if (mempool.empty() && emptyBlockCount >= 3) {
        throw std::runtime_error("ERROR: Already mined 3 empty blocks. Need transactions to mine more blocks.");
    }
    
    // We need at least one wallet to receive the mining reward
    Wallet* minerWallet = walletList[0];
    std::string minerAddress = minerWallet->getAddress();
    
    // Verify all transactions have sufficient balance
    if (balanceMap && !mempool.empty()) {
        for (const auto& tx : mempool) {
            if (!verifyTransactionBalance(tx)) {
                throw std::runtime_error("ERROR: Transaction from " + tx.sender + 
                                      " has insufficient balance.");
            }
        }
    }
    
    // Create a copy of mempool since we'll be adding the reward transaction
    std::vector<Transaction> blockTransactions = mempool;
    
    // Create a mining reward transaction (coinbase)
    Transaction rewardTx("Genesis", minerAddress, MINING_REWARD);
    rewardTx.hash = rewardTx.calculateHash();
    
    // Add the reward transaction to the block transactions
    blockTransactions.push_back(rewardTx);
    
    // Log appropriate message based on whether we're mining with transactions or just reward
    if (mempool.empty()) {
        std::cout << "Mining new block with only coinbase reward transaction (" 
                  << (emptyBlockCount + 1) << " of 3 allowed empty blocks)" << std::endl;
    } else {
        std::cout << "Mining new block with " << blockTransactions.size() << " transactions (including mining reward)..." << std::endl;
    }
    std::cout << "Mining reward of " << MINING_REWARD << " coins will be sent to " << minerAddress << std::endl;
    
    // Create and mine the new block with all transactions including the reward
    Block newBlock(chain.size(), blockTransactions, getLatestBlock().hash, difficulty);
    
    // Validate the transactions in the block before adding it
    if (!newBlock.validateTransactions()) {
        throw std::runtime_error("ERROR: Failed to mine block - invalid transactions");
    }
    
    chain.push_back(newBlock);
    
    std::cout << "Block #" << newBlock.blockNumber << " mined successfully!" << std::endl;
    std::cout << "Hash: " << newBlock.hash << std::endl;
    std::cout << "Nonce: " << newBlock.nonce << std::endl;
    
    // First update balances in the database to ensure persistence
    if (balanceMap) {
        updateBalancesForBlock(newBlock);
    }
    
    // Then synchronize in-memory wallet objects with database
    if (balanceMap) {
        for (auto& wallet : wallets) {
            // Get the current balance from the database
            double dbBalance = 0.0;
            if (balanceMap->getBalance(wallet->getAddress(), dbBalance)) {
                // Synchronize the wallet's in-memory balance with the database
                wallet->synchronizeBalance(dbBalance);
            }
        }
    } 
    // If no balanceMap, fall back to the old method of updating wallets directly
    else {
        // Directly update miner's wallet with the reward
        minerWallet->receiveMoney(MINING_REWARD);
        
        // And process other transactions
        for (const auto& tx : mempool) {
        for (auto& wallet : wallets) {
            if (wallet->getAddress() == tx.receiver) {
                wallet->receiveMoney(tx.amount);
                break;
            }
        }
        }
    }
    
    // Save block to database
    if (db && !db->saveBlock(newBlock)) {
        std::cerr << "Failed to save block to database: " << db->getLastError() << std::endl;
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
        
        // Check block integrity
        if (currentBlock.previousHash != previousBlock.hash) {
            std::cerr << "ERROR: Invalid chain - block " << i << " has incorrect previous hash" << std::endl;
            return false;
        }
        
        if (currentBlock.hash != currentBlock.calculateHash()) {
            std::cerr << "ERROR: Invalid chain - block " << i << " has incorrect hash" << std::endl;
            return false;
        }
        
        // Validate transactions in the block
        if (!currentBlock.validateTransactions()) {
            std::cerr << "ERROR: Invalid chain - block " << i << " contains invalid transactions" << std::endl;
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

// Add a method to set the balance mapping
void Blockchain::setBalanceMapping(BalanceMapping* mapping) {
    balanceMap = mapping;
    std::cout << "Balance mapping " << (mapping ? "connected" : "disabled") << std::endl;
}

// Update balances when processing a block
void Blockchain::updateBalancesForBlock(const Block& block) {
    if (!balanceMap) return;
    
    for (const auto& tx : block.transactions) {
        // Skip genesis transaction
        if (tx.sender == "Genesis" && tx.receiver == "Genesis") continue;
        
        // Process the transaction to update balances
        if (!balanceMap->processTransaction(tx.sender, tx.receiver, tx.amount)) {
            std::cerr << "Failed to update balances for transaction " << tx.hash << std::endl;
        }
    }
}

// Verify a transaction has sufficient balance
bool Blockchain::verifyTransactionBalance(const Transaction& tx) const {
    if (tx.sender == "Genesis") {
        // Genesis transactions or mining rewards are always valid
        return true;
    }
    
    if (!balanceMap) {
        // Without balance mapping, we can't verify
        return true;
    }
    
    double balance = 0.0;
    if (!balanceMap->getBalance(tx.sender, balance)) {
        std::cerr << "Error: Could not retrieve balance for " << tx.sender << std::endl;
        return false;
    }
    
    // Check if sender has enough balance
    if (balance < tx.amount) {
        std::cerr << "Error: Insufficient balance. " << tx.sender 
                  << " has " << balance << " but wants to send " << tx.amount << std::endl;
        return false;
    }
    
    return true;
}

// Add a method to load the blockchain from database
void Blockchain::loadFromDatabase() {
    if (!db || !db->isOpen()) {
        throw std::runtime_error("Cannot load blockchain: no database connection");
    }
    
    // Clear the current chain
    chain.clear();
    
    // We'll scan for blocks by index until we don't find any more
    size_t index = 0;
    bool foundBlocks = false;
    
    // Try to load blocks in sequence until no more blocks are found
    while (true) {
        // We need to create a minimal block that will be filled by getBlock
        // Since Block doesn't have a default constructor, we need to create it with parameters
        std::vector<Transaction> emptyTxs;
        Block tempBlock(index, emptyTxs, "0x0", difficulty);
        
        // Try to get the block by index
        if (db->getBlock(index, tempBlock)) {
            // Block was found, add it to the chain
            chain.push_back(tempBlock);
            foundBlocks = true;
            index++;
        } else {
            // No more blocks found, exit the loop
            break;
        }
    }
    
    // If no blocks were found, initialize with genesis block
    if (!foundBlocks) {
        std::cout << "No blocks found in database, creating genesis block" << std::endl;
        // Create a genesis block directly
        std::vector<Transaction> genesisTransactions;
        Transaction genesisTx("Genesis", "Genesis", 0);
        genesisTx.hash = genesisTx.calculateHash();
        genesisTransactions.push_back(genesisTx);
        
        // Create genesis block
        Block genesisBlock(0, genesisTransactions, "0x0", difficulty);
        genesisBlock.timestamp = GENESIS_TIMESTAMP;
        genesisBlock.nonce = GENESIS_NONCE;
        genesisBlock.hash = GENESIS_HASH;
        
        // Add to chain and save to database
        chain.push_back(genesisBlock);
        if (db) {
            db->saveBlock(genesisBlock);
        }
        return;
    }
    
    // Verify the loaded blockchain
    if (!isValidChain()) {
        throw std::runtime_error("Loaded blockchain is invalid");
    }
    
    // Rebuild balances from transactions
    rebuildBalancesFromTransactions();
}

// Helper method to rebuild balances from transactions
void Blockchain::rebuildBalancesFromTransactions() {
    if (!balanceMap || !db || !db->isOpen()) {
        return;
    }
    
    std::cout << "Rebuilding balances from transaction history..." << std::endl;
    
    // Get all current balances and reset them to zero
    auto allBalances = balanceMap->getAllBalances();
    for (const auto& pair : allBalances) {
        std::string address = pair.first;
        balanceMap->updateBalance(address, 0.0);
    }
    
    // Process all transactions in order
    for (const auto& block : chain) {
        for (const auto& tx : block.transactions) {
            balanceMap->processTransaction(tx.sender, tx.receiver, tx.amount);
        }
    }
    
    std::cout << "Balance rebuilding complete." << std::endl;
}

// Calculate the total supply of coins in the blockchain
double Blockchain::getTotalSupply() const {
    double totalSupply = 0.0;
    
    // If we have a balance mapping, we can use it to get the total supply
    if (balanceMap) {
        auto balances = balanceMap->getAllBalances();
        for (const auto& [address, balance] : balances) {
            totalSupply += balance;
        }
        return totalSupply;
    }
    
    // If no balance mapping, manually calculate by traversing the blockchain
    // This is less efficient but works as a fallback
    std::map<std::string, double> balances;
    
    // Process all transactions in the blockchain
            for (const auto& block : chain) {
                for (const auto& tx : block.transactions) {
            // Skip genesis transaction
            if (tx.sender == "Genesis" && tx.receiver == "Genesis") {
                continue;
            }
            
            // For minted coins (from Genesis), just add to receiver
            if (tx.sender == "Genesis") {
                balances[tx.receiver] += tx.amount;
                    } else {
                // For regular transactions, subtract from sender and add to receiver
                balances[tx.sender] -= tx.amount;
                balances[tx.receiver] += tx.amount;
                    }
                }
            }
    
    // Sum all positive balances
    for (const auto& [address, balance] : balances) {
        if (balance > 0) {
            totalSupply += balance;
        }
    }
    
    return totalSupply;
}

const time_t     Blockchain::GENESIS_TIMESTAMP = 1745026508;
const int Blockchain::GENESIS_NONCE     =  27701;
const std::string Blockchain::GENESIS_HASH      = "0x0000eb99d08f42f3c322b891f18212c85aa05365166964973a56d03e7da36f80";
const double Blockchain::MINING_REWARD   = 50.0; // 50 coins per block