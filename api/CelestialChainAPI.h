#ifndef CELESTIAL_CHAIN_API_H
#define CELESTIAL_CHAIN_API_H

#include <crow.h>
#include <string>
#include <memory>
#include <thread>
#include "../Blockchain.h"
#include "../wallet.h"
#include "../NetworkNode.h"
#include "../BlockchainDB.h"
#include "../balanceMapping.h"
#include "../explorer.h"

class CelestialChainAPI {
private:
    Blockchain& blockchain;
    Wallet& wallet;
    NetworkManager& networkManager;
    BlockchainDB* dbPtr;
    BalanceMapping* balanceMapPtr;
    NodeType nodeType;
    std::unique_ptr<crow::SimpleApp> app;
    int port;
    std::thread apiThread;
    bool running;

    // CORS middleware
    struct CORSMiddleware {
        struct context {};
        
        void before_handle(crow::request& req, crow::response& res, context& ctx) {
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
        }
        
        void after_handle(crow::request& req, crow::response& res, context& ctx) {
            // Do nothing
        }
    };

    void setupEndpoints();

public:
    CelestialChainAPI(
        Blockchain& blockchain, 
        Wallet& wallet, 
        NetworkManager& networkManager,
        BlockchainDB* dbPtr, 
        BalanceMapping* balanceMapPtr,
        NodeType nodeType,
        int port = 8080);
    
    ~CelestialChainAPI();
    
    void start();
    void stop();
    bool isRunning() const { return running; }
    int getPort() const { return port; }
};

#endif // CELESTIAL_CHAIN_API_H 