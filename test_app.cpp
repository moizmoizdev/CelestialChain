#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include "BlockchainDB.h"
#include "Blockchain.h"
#include "wallet.h"
#include "Types.h"
#include "NetworkNode.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main(int argc, char* argv[]) {
    clearScreen();
    std::cout << "Starting Test Application" << std::endl;

    // Default parameters
    std::string host = "127.0.0.1";
    int port = 8000;
    NodeType nodeType = NodeType::FULL_NODE;
    int difficulty = 4;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--host" && i + 1 < argc) host = argv[++i];
        else if (arg == "--port" && i + 1 < argc) port = std::stoi(argv[++i]);
        else if (arg == "--type" && i + 1 < argc) {
            std::string type = argv[++i];
            nodeType = (type == "wallet") ? NodeType::WALLET_NODE : NodeType::FULL_NODE;
        }
        else if (arg == "--difficulty" && i + 1 < argc) difficulty = std::stoi(argv[++i]);
    }

    std::cout << "Configuration:" << std::endl;
    std::cout << "  Host: " << host << std::endl;
    std::cout << "  Port: " << port << std::endl;
    std::cout << "  Node type: " << (nodeType == NodeType::FULL_NODE ? "Full" : "Wallet") << std::endl;
    std::cout << "  Difficulty: " << difficulty << std::endl;

    try {
        // Initialize blockchain
        std::cout << "\nInitializing blockchain..." << std::endl;
        Blockchain blockchain(difficulty);
        std::cout << "Blockchain initialized" << std::endl;

        // Create unique database path
        std::string dbPath = "./test_db_" + std::to_string(port);
        std::cout << "\nCreating database at: " << dbPath << std::endl;
        std::unique_ptr<BlockchainDB> db(new BlockchainDB(dbPath));

        if (!db->isOpen()) {
            std::cerr << "Failed to open database: " << db->getLastError() << std::endl;
            return 1;
        }

        // Connect blockchain to database
        std::cout << "\nConnecting blockchain to database..." << std::endl;
        blockchain.setDatabase(db.get());
        try {
            blockchain.loadFromDatabase();
            std::cout << "Loaded " << blockchain.getChain().size() << " blocks from database." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error loading blockchain from database: " << e.what() << std::endl;
            return 1;
        }

        // Create wallet
        std::cout << "\nCreating wallet..." << std::endl;
        Wallet wallet;
        std::cout << "Wallet created with address: " << wallet.getAddress() << std::endl;

        // Initialize network manager
        std::cout << "\nInitializing network manager..." << std::endl;
        try {
            NetworkManager networkManager(blockchain, wallet, host, port, nodeType);
            
            // Start network manager
            std::cout << "\nStarting network manager..." << std::endl;
            networkManager.start();
            
            std::cout << "\nPress Enter to exit..." << std::endl;
            std::cin.get();
            
            // Stop network manager
            std::cout << "\nStopping network manager..." << std::endl;
            networkManager.stop();
            
        } catch (const std::exception& e) {
            std::cerr << "Error initializing or starting network: " << e.what() << std::endl;
            return 1;
        }

        std::cout << "\nExiting normally." << std::endl;
    } 
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 