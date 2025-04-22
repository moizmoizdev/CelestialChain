#ifndef BLOCKCHAIN_DB_H
#define BLOCKCHAIN_DB_H

#include <string>
#include <memory>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include "Block.h"
#include "Transaction.h"

class BlockchainDB {
public:
    BlockchainDB(const std::string& db_path);
    ~BlockchainDB();

    // Basic operations
    bool put(const std::string& key, const std::string& value);
    bool get(const std::string& key, std::string& value) const;
    bool remove(const std::string& key);

    // Blockchain specific operations
    bool saveBlock(const Block& block);
    bool getBlock(size_t blockNumber, Block& block) const;
    bool saveTransaction(const Transaction& tx);
    bool getTransaction(const std::string& txHash, Transaction& tx) const;
    
    // Batch operations
    bool writeBatch(const std::vector<std::pair<std::string, std::string>>& operations);

    // Iterator operations
    std::vector<std::string> getAllKeys(const std::string& prefix = "") const;
    
    // Status check
    bool isOpen() const { return db != nullptr; }
    std::string getLastError() const { return lastError; }

private:
    std::unique_ptr<leveldb::DB> db;
    mutable std::string lastError;

    // Helper methods
    std::string serializeBlock(const Block& block) const;
    Block deserializeBlock(const std::string& data) const;
    std::string serializeTransaction(const Transaction& tx) const;
    Transaction deserializeTransaction(const std::string& data) const;
};

#endif // BLOCKCHAIN_DB_H 