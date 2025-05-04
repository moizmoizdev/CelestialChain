#include <crow.h>
#include <iostream>
#include <string>
#include <sstream>
#include "../Blockchain.h"
#include "../wallet.h"
#include "../NetworkNode.h"
#include "../BlockchainDB.h"
#include "../balanceMapping.h"
#include "../explorer.h"
#include "../Types.h"
#include "CelestialChainAPI.h"
#include <vector>

CelestialChainAPI::CelestialChainAPI(
    Blockchain& blockchain, 
    Wallet& wallet, 
    NetworkManager& networkManager,
    BlockchainDB* dbPtr, 
    BalanceMapping* balanceMapPtr,
    NodeType nodeType,
    int port)
    : blockchain(blockchain),
      wallet(wallet),
      networkManager(networkManager),
      dbPtr(dbPtr),
      balanceMapPtr(balanceMapPtr),
      nodeType(nodeType),
      app(new crow::SimpleApp),
      port(port),
      running(false) {
    
    setupEndpoints();
}

CelestialChainAPI::~CelestialChainAPI() {
    stop();
}

void CelestialChainAPI::start() {
    if (running) return;
    
    running = true;
    std::cout << "Starting CelestialChain API server on port " << port << std::endl;
    
    // Start the server in a separate thread
    apiThread = std::thread([this]() {
        try {
            app->port(port).run();
        } catch (const std::exception& e) {
            std::cerr << "API server error: " << e.what() << std::endl;
            running = false;
        }
    });
}

void CelestialChainAPI::stop() {
    if (!running) return;
    
    running = false;
    std::cout << "Stopping CelestialChain API server..." << std::endl;
    
    // Stop the Crow application
    app->stop();
    
    // Wait for the thread to join
    if (apiThread.joinable()) {
        apiThread.join();
    }
    
    std::cout << "API server stopped successfully" << std::endl;
}

void CelestialChainAPI::setupEndpoints() {
    // Define the API endpoints without middleware
    
    // Helper for CORS headers
    auto addCorsHeaders = [](crow::response& res) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    };

    // Add OPTIONS route handler for CORS preflight requests
    CROW_ROUTE((*app), "/api/<path>").methods(crow::HTTPMethod::OPTIONS)
    ([addCorsHeaders](const crow::request& req, const std::string& path) {
        crow::response res;
        res.code = 204; // No content
        addCorsHeaders(res);
        return res;
    });

    // 1. View blockchain
    CROW_ROUTE((*app), "/api/blockchain")
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        try {
            res.body = blockchain.toString();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 2. View mempool
    CROW_ROUTE((*app), "/api/mempool")
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        try {
            std::stringstream ss;
            const auto& mempool = blockchain.getMempool();
            
            ss << "{\n";
            ss << "  \"count\": " << mempool.size() << ",\n";
            ss << "  \"transactions\": [\n";
            
            for (size_t i = 0; i < mempool.size(); i++) {
                const auto& tx = mempool[i];
                ss << "    {\n";
                ss << "      \"hash\": \"" << tx.hash << "\",\n";
                ss << "      \"sender\": \"" << tx.sender << "\",\n";
                ss << "      \"receiver\": \"" << tx.receiver << "\",\n";
                ss << "      \"amount\": " << tx.amount << "\n";
                ss << "    }";
                if (i < mempool.size() - 1) ss << ",";
                ss << "\n";
            }
            
            ss << "  ]\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 3. Mine block
    CROW_ROUTE((*app), "/api/mine").methods(crow::HTTPMethod::OPTIONS)
    ([addCorsHeaders](const crow::request&) {
        crow::response res;
        res.code = 204; // No content
        addCorsHeaders(res);
        return res;
    });
    
    CROW_ROUTE((*app), "/api/mine").methods(crow::HTTPMethod::POST)
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        try {
            std::vector<Wallet*> wallets = { &wallet };
            Block& minedBlock = blockchain.mineBlock(wallets, nodeType);
            networkManager.broadcastBlock(minedBlock);
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"message\": \"Block mined successfully\",\n";
            ss << "  \"blockNumber\": " << minedBlock.blockNumber << ",\n";
            ss << "  \"hash\": \"" << minedBlock.hash << "\",\n";
            ss << "  \"nonce\": " << minedBlock.nonce << ",\n";
            ss << "  \"timestamp\": " << minedBlock.timestamp << ",\n";
            ss << "  \"transactionCount\": " << minedBlock.transactions.size() << "\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Mining failed: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 4. Create transaction
    CROW_ROUTE((*app), "/api/transaction").methods(crow::HTTPMethod::OPTIONS)
    ([addCorsHeaders](const crow::request&) {
        crow::response res;
        res.code = 204; // No content
        addCorsHeaders(res);
        return res;
    });
    
    CROW_ROUTE((*app), "/api/transaction").methods(crow::HTTPMethod::POST)
    ([this, addCorsHeaders](const crow::request& req) {
        crow::response res;
        try {
            auto json = crow::json::load(req.body);
            if (!json || !json.has("receiver") || !json.has("amount")) {
                res.body = "Invalid request. 'receiver' and 'amount' fields are required";
                res.code = 400;
                addCorsHeaders(res);
                return res;
            }
            
            std::string receiver = json["receiver"].s();
            double amount = json["amount"].d();
            
            Transaction tx("", "", 0);
            wallet.sendMoney(amount, receiver, tx);
            blockchain.addTransaction(tx);
            networkManager.broadcastTransaction(tx);
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"message\": \"Transaction created successfully\",\n";
            ss << "  \"hash\": \"" << tx.hash << "\",\n";
            ss << "  \"sender\": \"" << tx.sender << "\",\n";
            ss << "  \"receiver\": \"" << tx.receiver << "\",\n";
            ss << "  \"amount\": " << tx.amount << "\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Transaction failed: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 5. View wallet
    CROW_ROUTE((*app), "/api/wallet")
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        try {
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"address\": \"" << wallet.getAddress() << "\",\n";
            ss << "  \"balance\": " << wallet.getBalance() << "\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 6. Connect to peer
    CROW_ROUTE((*app), "/api/peers/connect").methods(crow::HTTPMethod::OPTIONS)
    ([addCorsHeaders](const crow::request&) {
        crow::response res;
        res.code = 204; // No content
        addCorsHeaders(res);
        return res;
    });
    
    CROW_ROUTE((*app), "/api/peers/connect").methods(crow::HTTPMethod::POST)
    ([this, addCorsHeaders](const crow::request& req) {
        crow::response res;
        try {
            auto json = crow::json::load(req.body);
            if (!json || !json.has("address") || !json.has("port")) {
                res.body = "Invalid request. 'address' and 'port' fields are required";
                res.code = 400;
                addCorsHeaders(res);
                return res;
            }
            
            std::string address = json["address"].s();
            int port = json["port"].i();
            
            bool success = networkManager.connectToPeer(address, port);
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"success\": " << (success ? "true" : "false") << ",\n";
            if (success) {
                ss << "  \"message\": \"Successfully connected to peer " << address << ":" << port << "\"\n";
            } else {
                ss << "  \"message\": \"Failed to connect to peer " << address << ":" << port << "\"\n";
            }
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 7. Request blockchain from peers
    CROW_ROUTE((*app), "/api/blockchain/sync").methods(crow::HTTPMethod::POST)
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        networkManager.requestBlockchain();
        
        res.body = "{ \"message\": \"Blockchain synchronization requested from peers\" }";
        res.code = 200;
        addCorsHeaders(res);
        return res;
    });
    
    // 8. View connected peers
    CROW_ROUTE((*app), "/api/peers")
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        try {
            auto peers = networkManager.getConnectedPeers();
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"count\": " << peers.size() << ",\n";
            ss << "  \"peers\": [\n";
            
            for (size_t i = 0; i < peers.size(); i++) {
                const auto& peer = peers[i];
                ss << "    {\n";
                ss << "      \"id\": \"" << peer.id << "\",\n";
                ss << "      \"address\": \"" << peer.address << "\",\n";
                ss << "      \"port\": " << peer.port << ",\n";
                ss << "      \"type\": \"" << (peer.type == NodeType::FULL_NODE ? "full" : "wallet") << "\"\n";
                ss << "    }";
                if (i < peers.size() - 1) ss << ",";
                ss << "\n";
            }
            
            ss << "  ]\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 9. View blockchain statistics
    CROW_ROUTE((*app), "/api/statistics")
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        try {
            std::stringstream ss;
            ss << "{\n";
            
            if (dbPtr && balanceMapPtr) {
                Explorer explorer(&blockchain, dbPtr, balanceMapPtr);
                double totalSupply = blockchain.getTotalSupply();
                double currentReward = blockchain.getCurrentMiningReward();
                size_t blockCount = explorer.getBlockCount();
                size_t txCount = explorer.getTransactionCount();
                
                // Calculate halving information
                time_t currentTime = time(nullptr);
                double secondsSinceGenesis = difftime(currentTime, Blockchain::GENESIS_TIMESTAMP);
                int daysSinceGenesis = static_cast<int>(secondsSinceGenesis / (60 * 60 * 24));
                const int HALVING_INTERVAL = 30; // 30 days per halving
                int numberOfHalvings = daysSinceGenesis / HALVING_INTERVAL;
                int daysUntilNextHalving = HALVING_INTERVAL - (daysSinceGenesis % HALVING_INTERVAL);
                
                ss << "  \"blockCount\": " << blockCount << ",\n";
                ss << "  \"transactionCount\": " << txCount << ",\n";
                ss << "  \"uniqueAddresses\": " << balanceMapPtr->getAllBalances().size() << ",\n";
                ss << "  \"totalSupply\": " << totalSupply << ",\n";
                ss << "  \"currentReward\": " << currentReward << ",\n";
                ss << "  \"halving\": {\n";
                ss << "    \"halvingsOccurred\": " << numberOfHalvings << ",\n";
                ss << "    \"daysUntilNextHalving\": " << daysUntilNextHalving << "\n";
                ss << "  },\n";
                ss << "  \"difficulty\": " << blockchain.getDifficulty() << "\n";
            } else {
                ss << "  \"error\": \"Explorer requires database connection to display statistics.\"\n";
            }
            
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 10. View block by index
    CROW_ROUTE((*app), "/api/explorer/block/<int>")
    ([this, addCorsHeaders](int blockIndex) {
        crow::response res;
        try {
            if (blockIndex < 0 || blockIndex >= blockchain.getChainSize()) {
                res.body = "{ \"error\": \"Block index out of range\" }";
                res.code = 404;
                addCorsHeaders(res);
                return res;
            }
            
            const Block& block = blockchain.getBlock(blockIndex);
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"blockNumber\": " << block.blockNumber << ",\n";
            ss << "  \"hash\": \"" << block.hash << "\",\n";
            ss << "  \"previousHash\": \"" << block.previousHash << "\",\n";
            ss << "  \"timestamp\": " << block.timestamp << ",\n";
            ss << "  \"nonce\": " << block.nonce << ",\n";
            ss << "  \"difficulty\": " << block.difficulty << ",\n";
            ss << "  \"transactionCount\": " << block.transactions.size() << ",\n";
            
            ss << "  \"transactions\": [\n";
            for (size_t i = 0; i < block.transactions.size(); i++) {
                const auto& tx = block.transactions[i];
                ss << "    {\n";
                ss << "      \"hash\": \"" << tx.hash << "\",\n";
                ss << "      \"sender\": \"" << tx.sender << "\",\n";
                ss << "      \"receiver\": \"" << tx.receiver << "\",\n";
                ss << "      \"amount\": " << tx.amount << "\n";
                ss << "    }";
                if (i < block.transactions.size() - 1) ss << ",";
                ss << "\n";
            }
            ss << "  ]\n";
            
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 11. View address details
    CROW_ROUTE((*app), "/api/explorer/address/<string>")
    ([this, addCorsHeaders](const std::string& address) {
        crow::response res;
        try {
            if (!dbPtr || !balanceMapPtr) {
                res.body = "{ \"error\": \"Explorer requires database connection\" }";
                res.code = 500;
                addCorsHeaders(res);
                return res;
            }
            
            double balance = 0.0;
            balanceMapPtr->getBalance(address, balance);
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"address\": \"" << address << "\",\n";
            ss << "  \"balance\": " << balance << "\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 12. Set difficulty (POST)
    CROW_ROUTE((*app), "/api/difficulty").methods(crow::HTTPMethod::OPTIONS)
    ([addCorsHeaders](const crow::request&) {
        crow::response res;
        res.code = 204; // No content
        addCorsHeaders(res);
        return res;
    });

    CROW_ROUTE((*app), "/api/difficulty").methods(crow::HTTPMethod::POST)
    ([this, addCorsHeaders](const crow::request& req) {
        crow::response res;
        try {
            auto json = crow::json::load(req.body);
            if (!json || !json.has("difficulty")) {
                res.body = "Invalid request. 'difficulty' field is required";
                res.code = 400;
                addCorsHeaders(res);
                return res;
            }
            
            int newDifficulty = json["difficulty"].i();
            
            // Only full nodes can change difficulty
            if (nodeType != NodeType::FULL_NODE) {
                res.body = "{ \"error\": \"Only full nodes can change mining difficulty\" }";
                res.code = 403;
                addCorsHeaders(res);
                return res;
            }
            
            // Validate and set the new difficulty
            blockchain.setDifficulty(newDifficulty);
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"message\": \"Mining difficulty updated\",\n";
            ss << "  \"newDifficulty\": " << blockchain.getDifficulty() << "\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 13. Get difficulty (GET)
    CROW_ROUTE((*app), "/api/difficulty").methods(crow::HTTPMethod::GET)
    ([this, addCorsHeaders](const crow::request&) {
        crow::response res;
        try {
            int difficulty = blockchain.getDifficulty();
            
            std::stringstream ss;
            ss << "{\n";
            ss << "  \"difficulty\": " << difficulty << "\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });

    // 14. View transaction details by hash
    CROW_ROUTE((*app), "/api/explorer/transaction/<string>")
    ([this, addCorsHeaders](const std::string& txHash) {
        crow::response res;
        try {
            bool found = false;
            std::stringstream ss;
            ss << "{\n";
            
            // Check mempool first
            for (const auto& tx : blockchain.getMempool()) {
                if (tx.hash == txHash) {
                    ss << "  \"hash\": \"" << tx.hash << "\",\n";
                    ss << "  \"sender\": \"" << tx.sender << "\",\n";
                    ss << "  \"receiver\": \"" << tx.receiver << "\",\n";
                    ss << "  \"amount\": " << tx.amount << ",\n";
                    ss << "  \"status\": \"Pending\",\n";
                    ss << "  \"blockNumber\": null\n";
                    found = true;
                    break;
                }
            }
            
            // Then check the blockchain
            if (!found) {
                for (size_t i = 0; i < blockchain.getChainSize(); i++) {
                    const Block& block = blockchain.getBlock(i);
                    for (const auto& tx : block.transactions) {
                        if (tx.hash == txHash) {
                            ss << "  \"hash\": \"" << tx.hash << "\",\n";
                            ss << "  \"sender\": \"" << tx.sender << "\",\n";
                            ss << "  \"receiver\": \"" << tx.receiver << "\",\n";
                            ss << "  \"amount\": " << tx.amount << ",\n";
                            ss << "  \"status\": \"Confirmed\",\n";
                            ss << "  \"blockNumber\": " << block.blockNumber << "\n";
                            found = true;
                            break;
                        }
                    }
                    if (found) break;
                }
            }
            
            ss << "}";
            
            if (!found) {
                res.body = "{ \"error\": \"Transaction not found\" }";
                res.code = 404;
            } else {
                res.body = ss.str();
                res.code = 200;
            }
            
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 15. Get top addresses by balance
    CROW_ROUTE((*app), "/api/explorer/top-addresses")
    ([this, addCorsHeaders](const crow::request& req) {
        crow::response res;
        try {
            // Get limit parameter (default to 5 if not specified)
            int limit = 5;
            auto limitParam = req.url_params.get("limit");
            if (limitParam) {
                try {
                    limit = std::stoi(limitParam);
                    if (limit <= 0) limit = 5;
                    if (limit > 100) limit = 100; // Cap at 100
                } catch (...) {
                    // If conversion fails, use default
                }
            }
            
            std::stringstream ss;
            ss << "{\n";
            
            if (balanceMapPtr) {
                auto balances = balanceMapPtr->getAllBalances();
                // Convert to vector for sorting
                std::vector<std::pair<std::string, double>> balanceList(balances.begin(), balances.end());
                // Sort by balance (highest first)
                std::sort(balanceList.begin(), balanceList.end(), 
                    [](const auto& a, const auto& b) { return a.second > b.second; });
                
                size_t count = std::min(balanceList.size(), static_cast<size_t>(limit));
                
                ss << "  \"count\": " << count << ",\n";
                ss << "  \"addresses\": [\n";
                
                for (size_t i = 0; i < count; i++) {
                    ss << "    {\n";
                    ss << "      \"address\": \"" << balanceList[i].first << "\",\n";
                    ss << "      \"balance\": " << balanceList[i].second << "\n";
                    ss << "    }";
                    if (i < count - 1) ss << ",";
                    ss << "\n";
                }
                
                ss << "  ]\n";
            } else {
                ss << "  \"error\": \"Balance mapping not available\"\n";
            }
            
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 16. Get latest transactions
    CROW_ROUTE((*app), "/api/explorer/latest-transactions")
    ([this, addCorsHeaders](const crow::request& req) {
        crow::response res;
        try {
            // Get limit parameter (default to 10 if not specified)
            int limit = 10;
            auto limitParam = req.url_params.get("limit");
            if (limitParam) {
                try {
                    limit = std::stoi(limitParam);
                    if (limit <= 0) limit = 10;
                    if (limit > 100) limit = 100; // Cap at 100
                } catch (...) {
                    // If conversion fails, use default
                }
            }
            
            std::stringstream ss;
            ss << "{\n";
            
            // Store transactions in a vector to collect them from different blocks
            std::vector<std::pair<Transaction, std::pair<size_t, bool>>> transactions;
            
            // First add pending transactions from mempool (mark as pending with block number 0)
            const auto& mempool = blockchain.getMempool();
            for (const auto& tx : mempool) {
                transactions.push_back({tx, {0, true}});
            }
            
            // Add confirmed transactions from blocks (newest first)
            size_t txCollected = 0;
            for (int i = blockchain.getChainSize() - 1; i >= 0 && txCollected < limit; i--) {
                const Block& block = blockchain.getBlock(i);
                for (const auto& tx : block.transactions) {
                    transactions.push_back({tx, {block.blockNumber, false}});
                    txCollected++;
                    if (txCollected >= limit) break;
                }
            }
            
            // Now output transactions
            size_t count = std::min(transactions.size(), static_cast<size_t>(limit));
            ss << "  \"count\": " << count << ",\n";
            ss << "  \"transactions\": [\n";
            
            for (size_t i = 0; i < count; i++) {
                const auto& tx = transactions[i].first;
                const auto& blockInfo = transactions[i].second;
                bool isPending = blockInfo.second;
                size_t blockNumber = blockInfo.first;
                
                ss << "    {\n";
                ss << "      \"hash\": \"" << tx.hash << "\",\n";
                ss << "      \"sender\": \"" << tx.sender << "\",\n";
                ss << "      \"receiver\": \"" << tx.receiver << "\",\n";
                ss << "      \"amount\": " << tx.amount << ",\n";
                ss << "      \"status\": \"" << (isPending ? "Pending" : "Confirmed") << "\"";
                
                if (!isPending) {
                    ss << ",\n      \"blockNumber\": " << blockNumber;
                }
                
                ss << "\n    }";
                if (i < count - 1) ss << ",";
                ss << "\n";
            }
            
            ss << "  ]\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
    
    // 17. Get latest blocks
    CROW_ROUTE((*app), "/api/explorer/latest-blocks")
    ([this, addCorsHeaders](const crow::request& req) {
        crow::response res;
        try {
            // Get limit parameter (default to 5 if not specified)
            int limit = 5;
            auto limitParam = req.url_params.get("limit");
            if (limitParam) {
                try {
                    limit = std::stoi(limitParam);
                    if (limit <= 0) limit = 5;
                    if (limit > 50) limit = 50; // Cap at 50
                } catch (...) {
                    // If conversion fails, use default
                }
            }
            
            std::stringstream ss;
            ss << "{\n";
            
            size_t chainSize = blockchain.getChainSize();
            size_t count = std::min(chainSize, static_cast<size_t>(limit));
            
            ss << "  \"count\": " << count << ",\n";
            ss << "  \"blocks\": [\n";
            
            // Start from the newest block and go backwards
            for (size_t i = 0; i < count; i++) {
                size_t blockIndex = chainSize - 1 - i;
                const Block& block = blockchain.getBlock(blockIndex);
                
                ss << "    {\n";
                ss << "      \"blockNumber\": " << block.blockNumber << ",\n";
                ss << "      \"hash\": \"" << block.hash << "\",\n";
                ss << "      \"previousHash\": \"" << block.previousHash << "\",\n";
                ss << "      \"timestamp\": " << block.timestamp << ",\n";
                ss << "      \"nonce\": " << block.nonce << ",\n";
                ss << "      \"difficulty\": " << block.difficulty << ",\n";
                ss << "      \"transactionCount\": " << block.transactions.size() << "\n";
                ss << "    }";
                if (i < count - 1) ss << ",";
                ss << "\n";
            }
            
            ss << "  ]\n";
            ss << "}";
            
            res.body = ss.str();
            res.code = 200;
        } catch (const std::exception& e) {
            res.body = std::string("Error: ") + e.what();
            res.code = 500;
        }
        addCorsHeaders(res);
        return res;
    });
} 