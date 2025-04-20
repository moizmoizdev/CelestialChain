#include "BlockchainDB.h"
#include <sstream>
#include <iostream>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/options.h>
#include <leveldb/write_batch.h>
#include <filesystem>
#include <thread>
#include <chrono>

// Helper function to split string by delimiter (local to this file)
namespace {
    std::vector<std::string> splitString(const std::string& str, char delim) {
        if (str.empty()) {
            return std::vector<std::string>();
        }
        
        std::vector<std::string> parts;
        try {
            std::stringstream ss(str);
            std::string part;
            while (std::getline(ss, part, delim)) {
                parts.push_back(part);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error in splitString: " << e.what() << std::endl;
            std::cerr << "Input string: " << str << std::endl;
            // Return whatever parts we've processed so far
        }
        return parts;
    }
}

BlockchainDB::BlockchainDB(const std::string& dbPath) : db(nullptr) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.write_buffer_size = 32 * 1024 * 1024;  // 32MB write buffer
    options.max_file_size = 2 * 1024 * 1024;       // 2MB max file size
    options.compression = leveldb::kSnappyCompression;
    options.error_if_exists = false;  // Don't fail if the database exists

    std::cout << "Opening database at: " << dbPath << std::endl;
    
    // Add a retry loop in case there are issues with opening the database
    int retryCount = 0;
    const int maxRetries = 3;
    
    while (retryCount < maxRetries) {
        leveldb::Status status = leveldb::DB::Open(options, dbPath, &db);
        if (status.ok()) {
            std::cout << "Database opened successfully" << std::endl;
            return;
        } else {
            lastError = status.ToString();
            std::cerr << "Failed to open database (attempt " << (retryCount + 1) << "/" << maxRetries 
                      << "): " << lastError << std::endl;
            
            // If database appears to be locked, wait and retry
            if (status.ToString().find("lock") != std::string::npos) {
                std::cerr << "Database appears to be locked. Waiting before retry..." << std::endl;
                
                // Try to delete any existing lock files
                try {
                    for (const auto& entry : std::filesystem::directory_iterator(dbPath)) {
                        std::string filename = entry.path().filename().string();
                        if (filename.find("LOCK") != std::string::npos) {
                            std::cerr << "Found lock file: " << entry.path().string() << std::endl;
                            std::filesystem::remove(entry.path());
                            std::cerr << "Deleted lock file" << std::endl;
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error handling lock files: " << e.what() << std::endl;
                }
                
                // Wait before retrying
                std::this_thread::sleep_for(std::chrono::milliseconds(500 * (retryCount + 1)));
            } else {
                // For other errors, check if the directory exists and is accessible
                std::cerr << "Checking database directory..." << std::endl;
                try {
                    if (!std::filesystem::exists(dbPath)) {
                        std::cerr << "Database directory does not exist, creating it..." << std::endl;
                        std::filesystem::create_directories(dbPath);
                    } else if (!std::filesystem::is_directory(dbPath)) {
                        std::cerr << "Path exists but is not a directory!" << std::endl;
                        break;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Filesystem error: " << e.what() << std::endl;
                    break;
                }
            }
        }
        retryCount++;
    }
    
    db = nullptr;
    std::cerr << "Failed to open database after " << maxRetries << " attempts" << std::endl;
}

BlockchainDB::~BlockchainDB() {
    if (db) {
        std::cout << "Closing database connection..." << std::endl;
        try {
            // Make sure all writes are flushed to disk
            delete db;
            db = nullptr;
            std::cout << "Database connection closed successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error closing database: " << e.what() << std::endl;
        }
    }
}

bool BlockchainDB::isOpen() const {
    return db != nullptr;
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

bool BlockchainDB::saveBlock(const Block& block) {
    std::string key = "block:" + std::to_string(block.blockNumber);
    return put(key, serializeBlock(block));
}

bool BlockchainDB::getBlock(size_t blockNumber, Block& block) const {
    std::string value;
    std::string key = "block:" + std::to_string(blockNumber);

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
    return put(key, serializeTransaction(tx));
}

bool BlockchainDB::getTransaction(const std::string& txHash, Transaction& tx) const {
    std::string value;
    std::string key = "tx:" + txHash;

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

std::vector<std::string> BlockchainDB::getAllKeys(const std::string& prefix) const {
    std::vector<std::string> keys;
    if (!db) {
        lastError = "Database not open";
        return keys;
    }

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->Seek(prefix); it->Valid() && it->key().starts_with(prefix); it->Next()) {
        keys.push_back(it->key().ToString());
    }

    delete it;
    return keys;
}

// Helper method to serialize a transaction consistently
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

// Helper method to deserialize a transaction consistently
Transaction BlockchainDB::deserializeTransaction(const std::string& data) const {
    try {
        // Debug the transaction data
        std::cout << "Deserializing transaction data: ";
        // Print a safe version of the data (first 30 chars)
        std::cout << (data.length() > 30 ? data.substr(0, 30) + "..." : data) << std::endl;
        
        std::vector<std::string> parts;
        try {
            parts = splitString(data, '|');
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to split transaction data: " + std::string(e.what()));
        }
        
        if (parts.size() != 7) {
            throw std::runtime_error("Invalid transaction format: expected 7 parts, got " + 
                                     std::to_string(parts.size()));
        }
        
        // Validate individual parts
        if (parts[0].empty()) {
            throw std::runtime_error("Empty sender field");
        }
        
        // Parse amount safely
        double amount;
        try {
            amount = std::stod(parts[3]);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid amount: " + parts[3] + " - " + std::string(e.what()));
        }
        
        // Parse timestamp safely
        unsigned long timestamp;
        try {
            timestamp = std::stoul(parts[4]);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid timestamp: " + parts[4] + " - " + std::string(e.what()));
        }
        
        // Build transaction with all the safety checks
        return Transaction(
            parts[0],            // sender
            parts[1],            // senderPublicKey
            parts[2],            // receiver
            amount,              // amount
            parts[5],            // hash
            parts[6],            // signature
            timestamp            // timestamp
        );
    } catch (const std::exception& e) {
        // Log the error
        std::cerr << "ERROR in deserializeTransaction: " << e.what() << std::endl;
        // Re-throw to be handled by caller
        throw;
    }
}

// Helper method to serialize a block consistently
std::string BlockchainDB::serializeBlock(const Block& block) const {
    std::stringstream ss;
    ss << block.blockNumber << "|"
       << block.timestamp << "|"
       << block.previousHash << "|"
       << block.hash << "|"
       << block.nonce << "|"
       << block.difficulty << "|"
       << block.transactions.size();

    // For each transaction, serialize and then escape any '|' characters in the transaction data
    // to prevent conflict with the block serialization format
    for (const auto& tx : block.transactions) {
        // Instead of using the serializeTransaction method directly, we'll serialize
        // the transaction fields individually to avoid nesting delimiters
        ss << "|" << tx.sender
           << "|" << tx.senderPublicKey
           << "|" << tx.receiver
           << "|" << tx.amount
           << "|" << tx.timestamp
           << "|" << tx.hash
           << "|" << tx.signature;
    }

    return ss.str();
}

// Helper method to deserialize a block consistently
Block BlockchainDB::deserializeBlock(const std::string& data) const {
    try {
        std::cout << "Deserializing block data: ";
        // Print a safe version of the data (first 30 chars)
        std::cout << (data.length() > 30 ? data.substr(0, 30) + "..." : data) << std::endl;
        
        std::vector<std::string> parts;
        try {
            parts = splitString(data, '|');
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to split block data: " + std::string(e.what()));
        }

        if (parts.size() < 7) {
            throw std::runtime_error("Invalid block data format: expected at least 7 parts, got " + 
                                    std::to_string(parts.size()));
        }

        // Safely parse block header data
        int blockNumber;
        time_t timestamp;
        std::string previousHash;
        std::string hash;
        int nonce;
        int difficulty;
        size_t txCount;
        
        try {
            blockNumber = std::stoi(parts[0]);
            timestamp = std::stoul(parts[1]);
            previousHash = parts[2];
            hash = parts[3];
            nonce = std::stoi(parts[4]);
            difficulty = std::stoi(parts[5]);
            txCount = std::stoull(parts[6]);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to parse block header: " + std::string(e.what()));
        }

        // Validate basic block fields
        if (previousHash.empty() || hash.empty()) {
            throw std::runtime_error("Block has empty hash fields");
        }

        std::vector<Transaction> transactions;
        
        // Each transaction uses 7 parts, so index accordingly
        bool txError = false;
        for (size_t i = 0; i < txCount; i++) {
            // Calculate the base index for this transaction
            size_t baseIdx = 7 + i * 7;
            
            // Check if we have enough parts
            if (baseIdx + 6 >= parts.size()) {
                std::cerr << "Warning: Truncated transaction data at index " << i << 
                          ". Expected " << 7 + txCount * 7 << " parts, got " << parts.size() << std::endl;
                txError = true;
                break;
            }
            
            // Create transaction directly from the parts
            try {
                std::string sender = parts[baseIdx];
                std::string senderPubKey = parts[baseIdx + 1];
                std::string receiver = parts[baseIdx + 2];
                double amount = std::stod(parts[baseIdx + 3]);
                unsigned long txTimestamp = std::stoul(parts[baseIdx + 4]);
                std::string txHash = parts[baseIdx + 5];
                std::string signature = parts[baseIdx + 6];
                
                Transaction tx(
                    sender,         // sender
                    senderPubKey,   // senderPublicKey
                    receiver,       // receiver
                    amount,         // amount
                    txHash,         // hash
                    signature,      // signature
                    txTimestamp     // timestamp
                );
                transactions.push_back(tx);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Error parsing transaction at index " << i << 
                          ": " << e.what() << std::endl;
                // Continue with next transaction
                txError = true;
            }
        }
        
        // If we had transaction errors, log but continue
        if (txError) {
            std::cerr << "Warning: Some transactions were skipped due to errors" << std::endl;
        }

        std::cout << "before creating block: " << blockNumber << std::endl;
        std::cout << "Initializing block with details:" << std::endl;
        std::cout << "Block Number: " << blockNumber << std::endl;
        std::cout << "Previous Hash: " << previousHash << std::endl;
        std::cout << "Difficulty: " << difficulty << std::endl;
        std::cout << "Number of Transactions: " << transactions.size() << std::endl;
        
        // Create a block even if we had some transaction errors
        Block block(blockNumber, transactions, previousHash, difficulty);
        block.timestamp = timestamp;
        block.nonce = nonce;
        block.hash = hash;
        
        std::cout << "after creating block: " << blockNumber << std::endl;
        return block;
    } catch (const std::exception& e) {
        std::cerr << "ERROR in deserializeBlock: " << e.what() << std::endl;
        // Create an empty genesis-like block as a fallback
        std::vector<Transaction> emptyTxs;
        Block emptyBlock(0, emptyTxs, "0x0", 1);
        return emptyBlock;
    }
}

// Database integrity verification and repair
bool BlockchainDB::verifyDatabaseIntegrity(bool repairCorrupted) {
    if (!db) {
        lastError = "Database not open";
        return false;
    }
    
    std::cout << "Verifying database integrity..." << std::endl;
    bool hasErrors = false;
    int blocksChecked = 0;
    int blocksErrorCount = 0;
    int txChecked = 0;
    int txErrorCount = 0;
    
    try {
        // Check all blocks
        auto blockKeys = getAllKeys("block:");
        std::cout << "Found " << blockKeys.size() << " blocks in database" << std::endl;
        for (const auto& key : blockKeys) {
            std::string value;
            bool keyValid = false;
            
            try {
                keyValid = get(key, value);
            } catch (const std::exception& e) {
                std::cerr << "Error reading key " << key << ": " << e.what() << std::endl;
                keyValid = false;
            }
            
            if (keyValid) {
                try {
                    // Try to deserialize the block
                    Block block = deserializeBlock(value);
                    std::cout<<"blockschecked"<<blocksChecked<<std::endl;
                    blocksChecked++;
                    std::cout<<"blockschecked"<<blocksChecked<<std::endl;

                    // Further validation can be added here if needed
                } 
                catch (const std::exception& e) {
                    blocksErrorCount++;
                    hasErrors = true;
                    std::cerr << "Error in block " << key << ": " << e.what() << std::endl;
                    
                    if (repairCorrupted) {
                        try {
                            std::cout << "Removing corrupted block entry: " << key << std::endl;
                            if (remove(key)) {
                                std::cout << "Successfully removed corrupted block" << std::endl;
                            } else {
                                std::cerr << "Failed to remove corrupted block: " << lastError << std::endl;
                            }
                        } catch (const std::exception& removeEx) {
                            std::cerr << "Exception during block removal: " << removeEx.what() << std::endl;
                        }
                    }
                }
            } 
            else {
                // Key couldn't be read
                blocksErrorCount++;
                hasErrors = true;
                std::cerr << "Could not read block data for key " << key << std::endl;
                
                if (repairCorrupted) {
                    try {
                        std::cout << "Removing unreadable block entry: " << key << std::endl;
                        if (remove(key)) {
                            std::cout << "Successfully removed unreadable block" << std::endl;
                        } else {
                            std::cerr << "Failed to remove unreadable block: " << lastError << std::endl;
                        }
                    } catch (const std::exception& removeEx) {
                        std::cerr << "Exception during block removal: " << removeEx.what() << std::endl;
                    }
                }
            }
        }
        std::cout << "Finished checking blocks" << std::endl;
        
        // Check all transactions
        auto txKeys = getAllKeys("tx:");
        std::cout << "Found " << txKeys.size() << " transactions in database" << std::endl;
        for (const auto& key : txKeys) {
            std::string value;
            bool keyValid = false;
            
            try {
                keyValid = get(key, value);
            } catch (const std::exception& e) {
                std::cerr << "Error reading key " << key << ": " << e.what() << std::endl;
                keyValid = false;
            }
            
            if (keyValid) {
                bool txValid = true;
                Transaction tx("", "", 0); // Default transaction
                
                try {
                    // Try to deserialize the transaction
                    tx = deserializeTransaction(value);
                    std::cout << "Deserialized transaction: " << tx.hash << std::endl;
                    txChecked++;
                    
                    // Further validation can be added here if needed
                } catch (const std::exception& e) {
                    txErrorCount++;
                    hasErrors = true;
                    txValid = false;
                    
                    std::cerr << "Error in transaction " << key << ": " << e.what() << std::endl;
                    
                    if (repairCorrupted) {
                        try {
                            std::cout << "Removing corrupted transaction entry: " << key << std::endl;
                            bool removed = false;
                            try {
                                removed = remove(key);
                            } catch (const std::exception& removeEx) {
                                std::cerr << "Exception during removal: " << removeEx.what() << std::endl;
                            }
                            
                            if (removed) {
                                std::cout << "Successfully removed corrupted transaction" << std::endl;
                            } else {
                                std::cerr << "Failed to remove corrupted transaction: " << lastError << std::endl;
                            }
                        } catch (const std::exception& removeEx) {
                            std::cerr << "Exception during transaction removal: " << removeEx.what() << std::endl;
                        }
                    }
                }
            } else {
                // Key couldn't be read
                txErrorCount++;
                hasErrors = true;
                std::cerr << "Could not read transaction data for key " << key << std::endl;
                
                if (repairCorrupted) {
                    try {
                        std::cout << "Removing unreadable transaction entry: " << key << std::endl;
                        if (remove(key)) {
                            std::cout << "Successfully removed unreadable transaction" << std::endl;
                        } else {
                            std::cerr << "Failed to remove unreadable transaction: " << lastError << std::endl;
                        }
                    } catch (const std::exception& removeEx) {
                        std::cerr << "Exception during transaction removal: " << removeEx.what() << std::endl;
                    }
                }
            }
        }
        std::cout << "Finished checking transactions" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error during database integrity check: " << e.what() << std::endl;
        hasErrors = true;
    }
    
    std::cout << "Database integrity check complete:" << std::endl;
    std::cout << "  Blocks checked: " << blocksChecked << std::endl;
    std::cout << "  Blocks with errors: " << blocksErrorCount << std::endl;
    std::cout << "  Transactions checked: " << txChecked << std::endl;
    std::cout << "  Transactions with errors: " << txErrorCount << std::endl;
    
    return !hasErrors;
} 