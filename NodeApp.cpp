#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <vector>
#include "NetworkNode.h"
#include "BlockchainDB.h"
#include "Blockchain.h"
#include "wallet.h"
#include "Types.h"
#include <stdexcept>
#include <filesystem>
//#include "NetworkManager.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printFullNodeMenu() {
    std::cout << "\n========== Blockchain Node ==========" << std::endl;
    std::cout << "1. View blockchain" << std::endl;
    std::cout << "2. View mempool" << std::endl;
    std::cout << "3. Mine block" << std::endl;
    std::cout << "4. Create transaction" << std::endl;
    std::cout << "5. View wallet" << std::endl;
    std::cout << "6. Connect to peer" << std::endl;
    std::cout << "7. Request blockchain from peers" << std::endl;
    std::cout << "8. View connected peers" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << "Enter your choice: ";
}

void printWalletNodeMenu() {
    std::cout << "\n========== Blockchain Wallet ==========" << std::endl;
    std::cout << "1. View blockchain" << std::endl;
    std::cout << "2. View mempool" << std::endl;
    std::cout << "3. Create transaction" << std::endl;
    std::cout << "4. View wallet" << std::endl;
    std::cout << "5. Connect to peer" << std::endl;
    std::cout << "6. Request blockchain from peers" << std::endl;
    std::cout << "7. View connected peers" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Enter your choice: ";
}

int main(int argc, char* argv[]) {
    clearScreen();
    std::cout << "Starting Blockchain Node Application" << std::endl;

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
        else if (arg == "--clean" || arg == "--fresh") {
            std::string dbPath = "./blockchain_db_" + std::to_string(port);
            try {
                std::cout << "Cleaning database directory: " << dbPath << std::endl;
                if (std::filesystem::exists(dbPath)) {
                    std::filesystem::remove_all(dbPath);
                    std::cout << "Removed database directory for clean start." << std::endl;
                } else {
                    std::cout << "Database directory doesn't exist yet." << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error cleaning database directory: " << e.what() << std::endl;
                // Continue anyway
            }
        }
        else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]\n";
            std::cout << "  --host HOST       Set the host address\n";
            std::cout << "  --port PORT       Set the port number\n";
            std::cout << "  --type TYPE       Set the node type (full or wallet)\n";
            std::cout << "  --difficulty DIFF Set the mining difficulty\n";
            std::cout << "  --clean, --fresh  Start with a clean database\n";
            std::cout << "  --help            Display this help message\n";
            return 0;
        }
    }

    // Initialize blockchain with specified difficulty
    Blockchain blockchain(difficulty);

    // Create unique database path based on port
    std::string dbPath = "./blockchain_db_" + std::to_string(port);
    std::unique_ptr<BlockchainDB> db(new BlockchainDB(dbPath));
    
    std::cout << "Database path: " << dbPath << std::endl;
    if (!db->isOpen()) {
        std::cerr << "Failed to open database: " << db->getLastError() << std::endl;
        return 1;
    }

    // Verify database integrity and repair if needed
    std::cout << "Verifying database integrity..." << std::endl;
    bool integrityCheck = false;
    try {
        integrityCheck = db->verifyDatabaseIntegrity(true);
        if (!integrityCheck) {
            std::cout << "Database had corrupted entries that were repaired. Continuing..." << std::endl;
        } else {
            std::cout << "Database integrity verified." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during database integrity check: " << e.what() << std::endl;
        std::cerr << "Attempting to continue anyway..." << std::endl;
    }

    // Connect blockchain to database and load existing data
    blockchain.setDatabase(db.get());
    try {
        blockchain.loadFromDatabase();
        std::cout << "Loaded " << blockchain.getChain().size() << " blocks from database." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading blockchain from database: " << e.what() << std::endl;
        std::cerr << "Starting with fresh blockchain..." << std::endl;
        // Just continue with the genesis block
    }

    // Create a single wallet for the node
    Wallet wallet;
    std::cout << "Node wallet created with address: " << wallet.getAddress() << std::endl;

    // Initialize network manager with blockchain and wallet
    try {
        NetworkManager networkManager(blockchain, wallet, host, port, nodeType);

        // Bootstrap connections
        std::vector<std::pair<std::string, int>> bootstrap = {
            {"127.0.0.1", 8001},
            {"127.0.0.1", 8002}
        };

        // Connect to bootstrap peers - with error handling
        std::cout << "Attempting to connect to bootstrap peers..." << std::endl;
        for (const auto& peer : bootstrap) {
            const std::string& bhost = peer.first;
            const int bport = peer.second;
            if (bport != port) {  // Don't connect to self
                try {
                    std::cout << "Trying to connect to peer at " << bhost << ":" << bport << "..." << std::endl;
                    bool connected = networkManager.connectToPeer(bhost, bport);
                    if (connected) {
                        std::cout << "Successfully connected to peer at " << bhost << ":" << bport << std::endl;
                    } else {
                        std::cerr << "Failed to connect to peer at " << bhost << ":" << bport << std::endl;
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Failed to connect to bootstrap peer " 
                              << bhost << ":" << bport << ": " << e.what() << std::endl;
                    std::cerr << "Continuing without this connection..." << std::endl;
                }
            } else {
                std::cout << "Skipping self-connection to " << bhost << ":" << bport << std::endl;
            }
        }
        std::cout << "Finished bootstrap peer connection attempts" << std::endl;

        // Start network manager
        try {
            networkManager.start();
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Failed to start network manager: " << e.what() << std::endl;
            std::cerr << "This may be caused by port " << port << " already being in use." << std::endl;
            std::cerr << "Try using a different port with the --port parameter." << std::endl;
            return 1;
        }

        int choice;
        do {
            clearScreen();
            if (nodeType == NodeType::FULL_NODE) printFullNodeMenu();
            else                                  printWalletNodeMenu();

            std::cin >> choice;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                choice = -1;
            }

            std::string peerAddress, receiver;
            int peerPort;double amount;

            switch (choice) {
                case 1:
                    clearScreen();
                    std::cout << blockchain.toString() << std::endl;
                    break;
                case 2:
                    clearScreen();
                    blockchain.printMempool();
                    break;
                case 3:
                    if (nodeType == NodeType::FULL_NODE) {
                        clearScreen();
                        blockchain.mineBlock(wallet, nodeType);
                        networkManager.broadcastBlock(blockchain.getLatestBlock());
                    } else {
                        clearScreen();
                        std::cout << "Enter receiver: "; std::cin >> receiver;
                        std::cout << "Enter amount: ";   std::cin >> amount;
                        Transaction tx("", "", 0);
                        std::cout << "Transaction is created" << std::endl;
                        wallet.sendMoney(amount, receiver, tx);
                        blockchain.addTransaction(tx);
                        networkManager.broadcastTransaction(tx);
                    }
                    break;
                case 4:
                    if (nodeType == NodeType::FULL_NODE) {
                        clearScreen();
                        std::cout << "Enter receiver: "; std::cin >> receiver;
                        std::cout << "Enter amount: ";   std::cin >> amount;
                        Transaction tx("", "", 0);
                        std::cout << "Transaction is created" << std::endl;
                        wallet.sendMoney(amount, receiver, tx);
                        std::cout << "Money is sent" << std::endl;
                        blockchain.addTransaction(tx);
                        std::cout << "Transaction is added" << std::endl;
                        networkManager.broadcastTransaction(tx);
                        std::cout << "Transaction is broadcasted" << std::endl;
                    } else {
                        clearScreen();
                        std::cout << "Address: " << wallet.getAddress() << std::endl;
                        std::cout << "Balance: " << wallet.getBalance() << std::endl;
                    }
                    break;
                case 5:
                    if (nodeType == NodeType::FULL_NODE) {
                        clearScreen();
                        std::cout << "Address: " << wallet.getAddress() << std::endl;
                        std::cout << "Balance: " << wallet.getBalance() << std::endl;
                    } else {
                        clearScreen();
                        std::cout << "Peer address: "; std::cin >> peerAddress;
                        std::cout << "Peer port: ";   std::cin >> peerPort;
                        networkManager.connectToPeer(peerAddress, peerPort);
                    }
                    break;
                case 6:
                    if (nodeType == NodeType::FULL_NODE) {
                        clearScreen();
                        std::cout << "Peer address: "; std::cin >> peerAddress;
                        std::cout << "Peer port: ";   std::cin >> peerPort;
                        networkManager.connectToPeer(peerAddress, peerPort);
                    } else {
                        clearScreen();
                        networkManager.requestBlockchain();
                    }
                    break;
                case 7:
                    if (nodeType == NodeType::FULL_NODE) {
                        clearScreen();
                        networkManager.requestBlockchain();
                    } else {
                        clearScreen();
                        auto peers = networkManager.getConnectedPeers();
                        for (auto& p : peers)
                            std::cout << p.id << " @ " << p.address << ":" << p.port << std::endl;
                    }
                    break;
                case 8:
                    if (nodeType == NodeType::FULL_NODE) {
                        clearScreen();
                        auto peers = networkManager.getConnectedPeers();
                        for (auto& p : peers)
                            std::cout << p.id << " @ " << p.address << ":" << p.port << std::endl;
                    }
                    break;
                case 0:
                    std::cout << "Exiting. Goodbye!" << std::endl;
                    
                    // Graceful shutdown of the database
                    std::cout << "Performing graceful shutdown..." << std::endl;
                    
                    // Flush any pending transactions to the database
                    if (!blockchain.getMempool().empty()) {
                        std::cout << "Saving " << blockchain.getMempool().size() << " pending transactions..." << std::endl;
                        for (const auto& tx : blockchain.getMempool()) {
                            db->saveTransaction(tx);
                        }
                    }
                    
                    // Verify database is in good state on exit
                    try {
                        std::cout << "Final database cleanup..." << std::endl;
                        db->verifyDatabaseIntegrity(false);  // Check but don't repair
                    } catch (const std::exception& e) {
                        std::cerr << "Warning: Database verification failed during shutdown: " << e.what() << std::endl;
                    }
                    
                    // Ensure network resources are freed before database is closed
                    networkManager.stop();
                    std::cout << "Network connections closed." << std::endl;
                    
                    break;
                default:
                    std::cout << "Invalid choice." << std::endl;
            }
            if (choice != 0) {
                std::cout << "\nPress Enter to continue...";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cin.get();
            }
        } while (choice != 0);

        // Clean shutdown
        networkManager.stop();
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: Failed to initialize network: " << e.what() << std::endl;
        std::cerr << "Try running with a different port using --port parameter." << std::endl;
        return 1;
    }

    return 0;
}
