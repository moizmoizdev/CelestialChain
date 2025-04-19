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
    bool get(const std::string& key, std::string& value);
    bool remove(const std::string& key);

    // Blockchain specific operations
    bool saveBlock(const Block& block);
    bool getBlock(int blockNumber, Block& block);
    bool saveTransaction(const Transaction& tx);
    bool getTransaction(const std::string& txHash, Transaction& tx);
    
    // Batch operations
    bool writeBatch(const std::vector<std::pair<std::string, std::string>>& operations);

    // Iterator operations
    std::vector<std::string> getAllKeys(const std::string& prefix = "");
    
    // Status check
    bool isOpen() const { return db != nullptr; }
    std::string getLastError() const { return lastError; }

private:
    std::unique_ptr<leveldb::DB> db;
    std::string lastError;

    // Helper methods
    std::string serializeBlock(const Block& block);
    Block deserializeBlock(const std::string& data);
    std::string serializeTransaction(const Transaction& tx);
    Transaction deserializeTransaction(const std::string& data);
};

#endif // BLOCKCHAIN_DB_H 