#include "Blockchain.h"
#include <iostream>
#include <cmath> // For pow function

// Define the halving interval constant 
const int Blockchain::HALVING_INTERVAL_DAYS = 30;

// Update the constants with the new initial reward name
const time_t     Blockchain::GENESIS_TIMESTAMP = 1745026508;
const int Blockchain::GENESIS_NONCE     =  27701;
const std::string Blockchain::GENESIS_HASH      = "0x0000eb99d08f42f3c322b891f18212c85aa05365166964973a56d03e7da36f80";
const double Blockchain::INITIAL_MINING_REWARD = 50.0; // Initial mining reward of 50 coins per block

// Calculate the current mining reward based on time since genesis
double Blockchain::calculateCurrentMiningReward() const {
    // Get current time
    time_t currentTime = time(nullptr);
    
    // Calculate days since genesis
    double secondsSinceGenesis = difftime(currentTime, GENESIS_TIMESTAMP);
    int daysSinceGenesis = static_cast<int>(secondsSinceGenesis / (60 * 60 * 24));
    
    // Calculate number of halvings that should have occurred
    int numberOfHalvings = daysSinceGenesis / HALVING_INTERVAL_DAYS;
    
    // Calculate current reward: initial_reward / (2^numberOfHalvings)
    double currentReward = INITIAL_MINING_REWARD;
    for (int i = 0; i < numberOfHalvings; i++) {
        currentReward /= 2.0;
    }
    
    // Ensure minimum reward of 0.01 coins
    if (currentReward < 0.01) {
        currentReward = 0.01;
    }
    
    return currentReward;
}

// Public method to get the current mining reward
double Blockchain::getCurrentMiningReward() const {
    return calculateCurrentMiningReward();
}

Blockchain::Blockchain(int difficulty) : difficulty(difficulty), db(nullptr), balanceMap(nullptr) {
    std::vector<Transaction> genesisTransactions;
    Transaction genesisTx("Genesis", "Genesis", 0);
    genesisTx.hash = genesisTx.calculateHash();
    genesisTransactions.push_back(genesisTx);
    
    Block genesisBlock(0, genesisTransactions, "0x0", difficulty);
    genesisBlock.timestamp = GENESIS_TIMESTAMP;
    genesisBlock.nonce     = GENESIS_NONCE;
    genesisBlock.hash      = GENESIS_HASH;
    chain.push_back(genesisBlock);
    
    std::cout << "Blockchain initialized with genesis block: " << genesisBlock.hash << std::endl;
}

void Blockchain::addBlock(const std::vector<Transaction>& transactions) {
    Block newBlock(chain.size(), transactions, getLatestBlock().hash, difficulty);
    
    if (!newBlock.validateTransactions()) {
        throw std::runtime_error("ERROR: Block contains invalid transactions");
    }
    
    chain.push_back(newBlock);
    
    if (balanceMap) {
        updateBalancesForBlock(newBlock);
    }
    
    if (db && !db->saveBlock(newBlock)) {
        std::cerr << "Failed to save block to database: " << db->getLastError() << std::endl;
    }
    
    std::cout << "Block #" << newBlock.blockNumber << " added to the blockchain." << std::endl;
    std::cout << "Hash: " << newBlock.hash << std::endl;
}

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
    
    if (!block.validateTransactions()) {
        throw std::runtime_error("ERROR: Received block contains invalid transactions");
    }
    
    chain.push_back(block);
    
    if (balanceMap) {
        updateBalancesForBlock(block);
    }
    
    if (db && !db->saveBlock(block)) {
        std::cerr << "Failed to save block to database: " << db->getLastError() << std::endl;
    }
    
    mempool.clear();
    std::cout << "Block #" << block.blockNumber << " added to the blockchain." << std::endl;
}

void Blockchain::addTransaction(const Transaction& transaction) {
    if (balanceMap && !verifyTransactionBalance(transaction)) {
        std::cerr << "Transaction rejected: Insufficient balance for " << transaction.sender << std::endl;
        return;
    }
    
    for (auto& old : mempool) {
        if (old.hash == transaction.hash) return;
    }
    
    mempool.push_back(transaction);
    
    if (db && !db->saveTransaction(transaction)) {
        std::cerr << "Failed to save transaction to database: " << db->getLastError() << std::endl;
    }
    
    std::cout << "Transaction added to mempool: " << transaction.sender << " -> " 
              << transaction.receiver << ": " << transaction.amount << std::endl;
}

Block& Blockchain::mineBlock(std::vector<Wallet*>& walletList, NodeType nodeType) {
    // Store the wallets for future use
    wallets = walletList;
    
    if (nodeType == NodeType::WALLET_NODE) {
        throw std::runtime_error("ERROR: Wallet nodes cannot mine blocks.");
    }
    
    if (walletList.empty()) {
        throw std::runtime_error("ERROR: No wallet provided for mining reward.");
    }
    
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
    
    // Calculate the current mining reward based on halving schedule
    double currentReward = calculateCurrentMiningReward();
    
    // Create a copy of mempool since we'll be adding the reward transaction
    std::vector<Transaction> blockTransactions = mempool;
    
    // Create a mining reward transaction (coinbase)
    Transaction rewardTx("Genesis", minerAddress, currentReward);
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
    std::cout << "Mining reward of " << currentReward << " $CLST will be sent to " << minerAddress << std::endl;
    
    // Calculate and print halving information
    time_t currentTime = time(nullptr);
    double secondsSinceGenesis = difftime(currentTime, GENESIS_TIMESTAMP);
    int daysSinceGenesis = static_cast<int>(secondsSinceGenesis / (60 * 60 * 24));
    int numberOfHalvings = daysSinceGenesis / HALVING_INTERVAL_DAYS;
    int daysUntilNextHalving = HALVING_INTERVAL_DAYS - (daysSinceGenesis % HALVING_INTERVAL_DAYS);
    
    std::cout << "Current block reward: " << currentReward << " $CLST (after " << numberOfHalvings 
              << " halvings)" << std::endl;
    std::cout << "Next halving in " << daysUntilNextHalving << " days" << std::endl;
    
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
        minerWallet->receiveMoney(currentReward);
        
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
    if (!balanceMap) {
        std::cerr << "WARNING: Cannot update balances - balance mapping not available" << std::endl;
        return;
    }
    
    std::cout << "Updating balances for block #" << block.blockNumber << std::endl;
    int processedTransactions = 0;
    int failedTransactions = 0;
    
    for (const auto& tx : block.transactions) {
        // Skip genesis transaction in genesis block
        if (block.blockNumber == 0 && tx.sender == "Genesis" && tx.receiver == "Genesis") {
            std::cout << "Skipping genesis block genesis transaction" << std::endl;
            continue;
        }
        
        std::cout << "Processing transaction: " << tx.sender << " -> " << tx.receiver 
                 << " (" << tx.amount << ")" << std::endl;
        
        // Process the transaction to update balances
        if (balanceMap->processTransaction(tx.sender, tx.receiver, tx.amount)) {
            processedTransactions++;
            
            // Debug: Show the updated balances
            double senderBalance = 0.0, receiverBalance = 0.0;
            balanceMap->getBalance(tx.sender, senderBalance);
            balanceMap->getBalance(tx.receiver, receiverBalance);
            
            std::cout << "Updated balances:" << std::endl;
            std::cout << "- " << tx.sender << ": " << senderBalance << " $CLST" << std::endl;
            std::cout << "- " << tx.receiver << ": " << receiverBalance << " $CLST" << std::endl;
        } else {
            std::cerr << "Failed to update balances for transaction " << tx.hash << std::endl;
            failedTransactions++;
        }
    }
    
    std::cout << "Balance update complete for block #" << block.blockNumber << ":" << std::endl;
    std::cout << "- Processed: " << processedTransactions << " transactions" << std::endl;
    if (failedTransactions > 0) {
        std::cerr << "- Failed: " << failedTransactions << " transactions" << std::endl;
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
                  << " has " << balance << " $CLST but wants to send " << tx.amount << " $CLST" << std::endl;
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
    if (!balanceMap) {
        std::cerr << "ERROR: Cannot rebuild balances - no balance mapping available" << std::endl;
        return;
    }
    
    if (!db || !db->isOpen()) {
        std::cerr << "ERROR: Cannot rebuild balances - no database connection" << std::endl;
        return;
    }
    
    std::cout << "Rebuilding balances from transaction history..." << std::endl;
    
    // Get all current balances and reset them to zero
    auto allBalances = balanceMap->getAllBalances();
    std::cout << "Found " << allBalances.size() << " addresses with balances" << std::endl;
    
    for (const auto& pair : allBalances) {
        std::string address = pair.first;
        balanceMap->updateBalance(address, 0.0);
    }
    
    int processedBlocks = 0;
    int processedTransactions = 0;
    
    // Process all transactions in order
    for (const auto& block : chain) {
        for (const auto& tx : block.transactions) {
            // Skip the genesis block's genesis transaction
            if (block.blockNumber == 0 && tx.sender == "Genesis" && tx.receiver == "Genesis") {
                continue;
            }
            
            if (balanceMap->processTransaction(tx.sender, tx.receiver, tx.amount)) {
                processedTransactions++;
            } else {
                std::cerr << "WARNING: Failed to process transaction: " 
                         << tx.sender << " -> " << tx.receiver 
                         << " (" << tx.amount << ")" << std::endl;
            }
        }
        processedBlocks++;
    }
    
    // Display the results
    std::cout << "Balance rebuilding complete:" << std::endl;
    std::cout << "- Processed " << processedBlocks << " blocks" << std::endl;
    std::cout << "- Processed " << processedTransactions << " transactions" << std::endl;
    
    // Show updated balances
    auto updatedBalances = balanceMap->getAllBalances();
    std::cout << "- Updated " << updatedBalances.size() << " address balances" << std::endl;
    
    // Log details of non-zero balances
    int nonZeroCount = 0;
    for (const auto& [address, balance] : updatedBalances) {
        if (balance > 0.0) {
            nonZeroCount++;
            std::cout << "  > " << address << ": " << balance << std::endl;
        }
    }
    std::cout << "- " << nonZeroCount << " addresses with non-zero balances" << std::endl;
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

// Add these methods to get and set difficulty
int Blockchain::getDifficulty() const {
    return difficulty;
}

void Blockchain::setDifficulty(int newDifficulty) {
    // Ensure difficulty is at least 1 and at most 8 (to prevent excessive mining times)
    if (newDifficulty < 1) {
        std::cout << "Warning: Difficulty cannot be less than 1. Setting to 1." << std::endl;
        difficulty = 1;
    } else if (newDifficulty > 8) {
        std::cout << "Warning: Difficulty cannot be more than 8. Setting to 8." << std::endl;
        difficulty = 8;
    } else {
        difficulty = newDifficulty;
    }
    
    std::cout << "Mining difficulty changed to " << difficulty << std::endl;
    std::cout << "This means blocks require hashes with " << difficulty << " leading zeros" << std::endl;
    
    // Calculate an estimate of mining time
    double estimatedTime = std::pow(16, difficulty) / 10000; // Assuming 10K hashes/sec
    std::cout << "Estimated mining time: ";
    if (estimatedTime < 60) {
        std::cout << estimatedTime << " seconds" << std::endl;
    } else if (estimatedTime < 3600) {
        std::cout << (estimatedTime / 60) << " minutes" << std::endl;
    } else {
        std::cout << (estimatedTime / 3600) << " hours" << std::endl;
    }
}