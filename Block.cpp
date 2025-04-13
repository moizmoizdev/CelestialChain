#include "Block.h"

Block::Block(int blockNumber, std::vector<Transaction> txs, std::string prevHash, int diff) {
    this->blockNumber = blockNumber;
    transactions = txs;
    previousHash = prevHash;
    nonce = 0;
    difficulty = diff;
    timestamp = time(nullptr);
    hash = mineBlock();
}


std::string Block::calculateHash() const {
    std::string data = std::to_string(blockNumber) + std::to_string(timestamp) + previousHash + std::to_string(nonce);
    for (const auto& tx : transactions) {
        data += tx.sender + tx.receiver + std::to_string(tx.amount);
    }
    return simpleHash(data);
}

std::string Block::simpleHash(const std::string& str) const {
    std::hash<std::string> hasher;
    size_t hash = hasher(str);
    
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    
    std::string result = ss.str();
    while (result.length() < 64) {
        result += result;
    }
    
    return result.substr(0, 64);
}

std::string Block::mineBlock() {
    std::string target = ""; 
    for (int i = 0; i < difficulty; i++) {
        target += "0";
    }
    while (true) { 

        nonce++;
        std::string hash = calculateHash();
        if (hash.substr(0, difficulty) == target) {
            return hash;
        }
    }
} 