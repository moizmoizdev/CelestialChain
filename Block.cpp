#include "Block.h"
#include "sha.h"

Block::Block(int blockNumber, std::vector<Transaction> txs, std::string prevHash, int diff) {
    try {
        this->blockNumber = blockNumber;
        transactions = txs;
        previousHash = prevHash;
        nonce = 0;
        difficulty = diff < 1 ? 1 : diff;  // Ensure minimum difficulty of 1
        timestamp = time(nullptr);
        hash = mineBlock();
    } catch (const std::exception& e) {
        std::cerr << "ERROR in Block constructor: " << e.what() << std::endl;
        // Set defaults to avoid having an invalid block
        this->blockNumber = blockNumber;
        transactions.clear();  // Empty the transactions to avoid issues
        previousHash = prevHash;
        nonce = 0;
        difficulty = 1;
        timestamp = time(nullptr);
        // Create a simple hash without transactions to avoid errors
        std::string data = std::to_string(blockNumber) + std::to_string(timestamp) + previousHash + "0";
        hash = "0x" + computeSHA256(data);
    }
}

std::string Block::calculateHash() const {
    std::string data = std::to_string(blockNumber) + std::to_string(timestamp) + previousHash + std::to_string(nonce);
    for (const auto& tx : transactions) {
        data += tx.sender + tx.receiver + std::to_string(tx.amount);
    }
    return "0x" + computeSHA256(data);
}

std::string Block::simpleHash(const std::string& str) const {
    return "0x" + computeSHA256(str);
}

std::string Block::mineBlock() {
    std::string target = ""; // adding leading zeros to the target hash
    for (int i = 0; i < difficulty; i++) {
        target += "0";
    }
    while (true) { // bruteforce the nonce until
                   // the hash with leading zeros is found
        nonce++;
        std::string hash = calculateHash();
        
        // Strip 0x prefix for difficulty check
        std::string hashNoPrefix = hash;
        if (hashNoPrefix.substr(0, 2) == "0x") {
            hashNoPrefix = hashNoPrefix.substr(2);
        }
        
        if (hashNoPrefix.substr(0, difficulty) == target) {
            return hash;
        }
    }
} 