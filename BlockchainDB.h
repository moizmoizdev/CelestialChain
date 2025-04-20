#ifndef BLOCKCHAIN_DB_H
#define BLOCKCHAIN_DB_H

#include <string>
#include <vector>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include "Block.h"
#include "Transaction.h"

class BlockchainDB {
private:
    leveldb::DB* db;
    mutable std::string lastError;  // Make lastError mutable so it can be modified in const functions

public:
    BlockchainDB(const std::string& dbPath);
    ~BlockchainDB();

    bool isOpen() const;
    std::string getLastError() const { return lastError; }

    bool put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value) const;
    bool remove(const std::string& key);
    bool writeBatch(const std::vector<std::pair<std::string, std::string>>& operations);

    // Blockchain specific operations
    bool saveBlock(const Block& block);
    bool getBlock(size_t blockNumber, Block& block) const;
    bool saveTransaction(const Transaction& tx);
    bool getTransaction(const std::string& txHash, Transaction& tx) const;
    std::vector<std::string> getAllKeys(const std::string& prefix = "") const;

    // Database integrity
    bool verifyDatabaseIntegrity(bool repairCorrupted = false);

    // Helper methods
    std::string serializeBlock(const Block& block) const;
    Block deserializeBlock(const std::string& data) const;
    std::string serializeTransaction(const Transaction& tx) const;
    Transaction deserializeTransaction(const std::string& data) const;
};

#endif // BLOCKCHAIN_DB_H 