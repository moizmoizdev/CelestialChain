#include "BlockchainDB.h"
#include <sstream>
#include <iostream>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/options.h>
#include <leveldb/write_batch.h>
#include <boost/lexical_cast.hpp>
#include <algorithm>

BlockchainDB::BlockchainDB(const std::string& dbPath) : db(nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.write_buffer_size = 32 * 1024 * 1024;  // 32MB write buffer
    options.max_file_size = 2 * 1024 * 1024;       // 2MB file size
    options.block_cache = leveldb::NewLRUCache(64 * 1024 * 1024);  // 64MB cache
    
    leveldb::DB* raw_db;
    leveldb::Status status = leveldb::DB::Open(options, dbPath, &raw_db);
    if (!status.ok()) {
        lastError = status.ToString();
        std::cerr << "Failed to open database: " << lastError << std::endl;
    } else {
        db.reset(raw_db);
    }
}

BlockchainDB::~BlockchainDB() {
    // std::unique_ptr will automatically delete the db pointer
}

bool BlockchainDB::put(const std::string& key, const std::string& value) {
    if (!db) {
        lastError = "Database not open";
        return false;
    }
    
    leveldb::Status status = db->Put(leveldb::WriteOptions(), key, value);
    if (!status.ok()) {
        lastError = status.ToString();
        return false;
    }
    return true;
}

bool BlockchainDB::get(const std::string& key, std::string& value) const {
    if (!db) {
        lastError = "Database not open";
        return false;
    }
    
    leveldb::Status status = db->Get(leveldb::ReadOptions(), key, &value);
    if (!status.ok()) {
        lastError = status.ToString();
        return false;
    }
    return true;
}

bool BlockchainDB::remove(const std::string& key) {
    if (!db) {
        lastError = "Database not open";
        return false;
    }
    
    leveldb::Status status = db->Delete(leveldb::WriteOptions(), key);
    if (!status.ok()) {
        lastError = status.ToString();
        return false;
    }
    return true;
}

std::string BlockchainDB::serializeBlock(const Block& block) const {
    std::stringstream ss;
    ss << block.blockNumber << "|"
       << block.timestamp << "|"
       << block.previousHash << "|"
       << block.hash << "|"
       << block.nonce << "|"
       << block.difficulty << "|"
       << block.transactions.size();
    
    for (const auto& tx : block.transactions) {
        ss << "|" << serializeTransaction(tx);
    }
    
    return ss.str();
}

Block BlockchainDB::deserializeBlock(const std::string& data) const {
    std::vector<std::string> parts;
    std::string part;
    std::stringstream ss(data);
    
    while (std::getline(ss, part, '|')) {
        parts.push_back(part);
    }
    
    if (parts.size() < 7) {
        throw std::runtime_error("Invalid block data format");
    }
    
    int blockNumber = std::stoi(parts[0]);
    time_t timestamp = std::stoull(parts[1]);
    std::string previousHash = parts[2];
    std::string hash = parts[3];
    int nonce = std::stoi(parts[4]);
    int difficulty = std::stoi(parts[5]);
    size_t txCount = std::stoull(parts[6]);
    
    std::vector<Transaction> transactions;
    size_t currentIndex = 7;
    for (size_t i = 0; i < txCount && currentIndex < parts.size(); i++) {
        std::string txData;
        while (currentIndex < parts.size()) {
            txData += parts[currentIndex++] + "|";
        }
        if (!txData.empty()) {
            transactions.push_back(deserializeTransaction(txData));
        }
    }
    
    Block block(blockNumber, transactions, previousHash, difficulty);
    block.timestamp = timestamp;
    block.hash = hash;
    block.nonce = nonce;
    
    return block;
}

std::string BlockchainDB::serializeTransaction(const Transaction& tx) const {
    std::stringstream ss;
    ss << tx.sender << "|"
       << tx.senderPublicKey << "|"
       << tx.receiver << "|"
       << tx.amount << "|"
       << tx.timestamp << "|"
       << tx.hash << "|"
       << tx.signature;
    return ss.str();
}

Transaction BlockchainDB::deserializeTransaction(const std::string& data) const {
    std::vector<std::string> parts;
    std::string part;
    std::stringstream ss(data);
    
    while (std::getline(ss, part, '|')) {
        parts.push_back(part);
    }
    
    if (parts.size() < 7) {
        throw std::runtime_error("Invalid transaction data format");
    }
    
    return Transaction(
        parts[0],                    // sender
        parts[1],                    // senderPublicKey
        parts[2],                    // receiver
        std::stod(parts[3]),        // amount
        parts[5],                    // hash
        parts[6],                    // signature
        std::stoull(parts[4])       // timestamp
    );
}

bool BlockchainDB::saveBlock(const Block& block) {
    std::string key = "block:" + std::to_string(block.blockNumber);
    std::string serialized = serializeBlock(block);
    return put(key, serialized);
}

bool BlockchainDB::getBlock(size_t blockNumber, Block& block) const {
    std::string key = "block:" + std::to_string(blockNumber);
    std::string value;
    
    if (!get(key, value)) {
        return false;
    }
    
    try {
        block = deserializeBlock(value);
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

bool BlockchainDB::saveTransaction(const Transaction& tx) {
    std::string key = "tx:" + tx.hash;
    std::string serialized = serializeTransaction(tx);
    return put(key, serialized);
}

bool BlockchainDB::getTransaction(const std::string& txHash, Transaction& tx) const {
    std::string key = "tx:" + txHash;
    std::string value;
    
    if (!get(key, value)) {
        return false;
    }
    
    try {
        tx = deserializeTransaction(value);
        return true;
    } catch (const std::exception& e) {
        lastError = e.what();
        return false;
    }
}

bool BlockchainDB::writeBatch(const std::vector<std::pair<std::string, std::string>>& operations) {
    if (!db) {
        lastError = "Database not open";
        return false;
    }
    
    leveldb::WriteBatch batch;
    for (const auto& op : operations) {
        batch.Put(op.first, op.second);
    }
    
    leveldb::Status status = db->Write(leveldb::WriteOptions(), &batch);
    if (!status.ok()) {
        lastError = status.ToString();
        return false;
    }
    return true;
}

std::vector<std::string> BlockchainDB::getAllKeys(const std::string& prefix) const {
    std::vector<std::string> keys;
    if (!db) {
        lastError = "Database not open";
        return keys;
    }
    
    std::unique_ptr<leveldb::Iterator> it(db->NewIterator(leveldb::ReadOptions()));
    for (it->Seek(prefix); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        // Check if the key starts with the prefix
        if (key.compare(0, prefix.length(), prefix) == 0) {
            keys.push_back(key);
        } else {
            // If we moved past keys with the prefix, we can stop
            break;
        }
    }
    
    return keys;
} 