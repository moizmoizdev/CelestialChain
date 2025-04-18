#include "Block.h"
#include "sha.h"

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
    return computeSHA256(data);
}

std::string Block::simpleHash(const std::string& str) const {
    return computeSHA256(str);
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
        if (hash.substr(0, difficulty) == target) {
            return hash;
        }
    }
} 