#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include "NetworkNode.h"
#include "Blockchain.h"
#include "wallet.h"
#include "Types.h"

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printFullNodeMenu() {
    std::cout << "\n========== Blockchain Node ==========\n";
    std::cout << "1. View blockchain\n";
    std::cout << "2. View mempool\n";
    std::cout << "3. Mine block\n";
    std::cout << "4. Create transaction\n";
    std::cout << "5. View wallet\n";
    std::cout << "6. Connect to peer\n";
    std::cout << "7. Request blockchain from peers\n";
    std::cout << "8. View connected peers\n";
    std::cout << "0. Exit\n";
    std::cout << "====================================\n";
    std::cout << "Enter your choice: ";
}

void printWalletNodeMenu() {
    std::cout << "\n========== Blockchain Wallet ==========\n";
    std::cout << "1. View blockchain\n";
    std::cout << "2. View mempool\n";
    std::cout << "3. Create transaction\n";
    std::cout << "4. View wallet\n";
    std::cout << "5. Connect to peer\n";
    std::cout << "6. Request blockchain from peers\n";
    std::cout << "7. View connected peers\n";
    std::cout << "0. Exit\n";
    std::cout << "======================================\n";
    std::cout << "Enter your choice: ";
}

int main(int argc, char* argv[]) {
    clearScreen();
    std::cout << "Starting Blockchain Node Application\n";
    
    // Default parameters
    std::string host = "127.0.0.1";
    int port = 8000;
    NodeType nodeType = NodeType::FULL_NODE;
    int difficulty = 4;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--type" && i + 1 < argc) {
            std::string type = argv[++i];
            if (type == "wallet") {
                nodeType = NodeType::WALLET_NODE;
            } else if (type == "full") {
                nodeType = NodeType::FULL_NODE;
            } else {
                std::cerr << "Invalid node type: " << type << std::endl;
                return 1;
            }
        } else if (arg == "--difficulty" && i + 1 < argc) {
            difficulty = std::stoi(argv[++i]);
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]\n";
            std::cout << "Options:\n";
            std::cout << "  --host HOST       Set the host address (default: 127.0.0.1)\n";
            std::cout << "  --port PORT       Set the port number (default: 8000)\n";
            std::cout << "  --type TYPE       Set the node type (full or wallet, default: full)\n";
            std::cout << "  --difficulty DIFF Set the mining difficulty (default: 4)\n";
            std::cout << "  --help            Display this help message\n";
            return 0;
        }
    }
    
    // Initialize blockchain and network manager
    Blockchain blockchain(difficulty);
    NetworkManager networkManager(blockchain, host, port, nodeType);
    
    // Start network services
    try {
        networkManager.start();
    } catch (const std::exception& e) {
        std::cerr << "Failed to start network services: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Network services started. Press Enter to continue...";
    std::cin.get();
    
    // Main application loop
    int choice;
    do {
        clearScreen();
        
        if (nodeType == NodeType::FULL_NODE) {
            printFullNodeMenu();
        } else {
            printWalletNodeMenu();
        }
        
        std::cin >> choice;
        
        // Handle invalid input
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }
        
        std::string peerAddress;
        int peerPort;
        std::string receiver;
        int amount;
        
        switch (choice) {
            case 1: // View blockchain
                clearScreen();
                std::cout << "Current Blockchain Status:\n" << std::endl;
                blockchain.printBlockchain();
                break;
                
            case 2: // View mempool
                clearScreen();
                std::cout << "Current Mempool Status:\n" << std::endl;
                blockchain.printMempool();
                break;
                
            case 3: // Mine block (full node only) / Create transaction (wallet node)
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Mining block..." << std::endl;
                    
                    // Create a vector for the single wallet in our node
                    std::vector<Wallet> wallets;
                    wallets.push_back(networkManager.getWallet());
                    
                    // Mine the block and broadcast it
                    blockchain.mineBlock(wallets, nodeType);
                    networkManager.broadcastBlock(blockchain.getLatestBlock());
                } else {
                    // For wallet nodes, this option creates a transaction
                    clearScreen();
                    std::cout << "Create a new transaction\n";
                    std::cout << "Enter receiver address: ";
                    std::cin >> receiver;
                    std::cout << "Enter amount: ";
                    std::cin >> amount;
                    
                    Transaction tx("", "", 0); // Placeholder
                    networkManager.getWallet().sendMoney(receiver, amount, tx);
                    
                    // Add to local mempool and broadcast
                    blockchain.addTransaction(tx);
                    networkManager.broadcastTransaction(tx);
                }
                break;
                
            case 4: // Create transaction (full node) / View wallet (wallet node)
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Create a new transaction\n";
                    std::cout << "Enter receiver address: ";
                    std::cin >> receiver;
                    std::cout << "Enter amount: ";
                    std::cin >> amount;
                    
                    Transaction tx("", "", 0); // Placeholder
                    networkManager.getWallet().sendMoney(receiver, amount, tx);
                    
                    // Add to local mempool and broadcast
                    blockchain.addTransaction(tx);
                    networkManager.broadcastTransaction(tx);
                } else {
                    // For wallet nodes, this option views the wallet
                    clearScreen();
                    std::cout << "Wallet Information\n";
                    std::cout << "Address: " << networkManager.getWallet().getAddress() << std::endl;
                    std::cout << "Balance: " << networkManager.getWallet().getBalance() << std::endl;
                }
                break;
                
            case 5: // View wallet (full node) / Connect to peer (wallet node)
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Wallet Information\n";
                    std::cout << "Address: " << networkManager.getWallet().getAddress() << std::endl;
                    std::cout << "Balance: " << networkManager.getWallet().getBalance() << std::endl;
                } else {
                    // For wallet nodes, this option connects to a peer
                    clearScreen();
                    std::cout << "Connect to a peer\n";
                    std::cout << "Enter peer address: ";
                    std::cin >> peerAddress;
                    std::cout << "Enter peer port: ";
                    std::cin >> peerPort;
                    
                    if (networkManager.connectToPeer(peerAddress, peerPort)) {
                        std::cout << "Successfully connected to peer at " << peerAddress << ":" << peerPort << std::endl;
                    } else {
                        std::cout << "Failed to connect to peer." << std::endl;
                    }
                }
                break;
                
            case 6: // Connect to peer (full node) / Request blockchain (wallet node)
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Connect to a peer\n";
                    std::cout << "Enter peer address: ";
                    std::cin >> peerAddress;
                    std::cout << "Enter peer port: ";
                    std::cin >> peerPort;
                    
                    if (networkManager.connectToPeer(peerAddress, peerPort)) {
                        std::cout << "Successfully connected to peer at " << peerAddress << ":" << peerPort << std::endl;
                    } else {
                        std::cout << "Failed to connect to peer." << std::endl;
                    }
                } else {
                    // For wallet nodes, this option requests the blockchain
                    clearScreen();
                    std::cout << "Requesting blockchain from peers..." << std::endl;
                    networkManager.requestBlockchain();
                }
                break;
                
            case 7: // Request blockchain (full node) / View connected peers (wallet node)
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Requesting blockchain from peers..." << std::endl;
                    networkManager.requestBlockchain();
                } else {
                    // For wallet nodes, this option views connected peers
                    clearScreen();
                    std::cout << "Connected Peers\n";
                    std::vector<Peer> connectedPeers = networkManager.getConnectedPeers();
                    if (connectedPeers.empty()) {
                        std::cout << "No peers connected." << std::endl;
                    } else {
                        std::cout << "Total connected peers: " << connectedPeers.size() << std::endl;
                        for (const auto& peer : connectedPeers) {
                            std::cout << "- " << peer.id << " at " << peer.address << ":" << peer.port 
                                      << " (" << (peer.type == NodeType::FULL_NODE ? "Full Node" : "Wallet Node") << ")" << std::endl;
                        }
                    }
                }
                break;
                
            case 8: // View connected peers (full node only)
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Connected Peers\n";
                    std::vector<Peer> connectedPeers = networkManager.getConnectedPeers();
                    if (connectedPeers.empty()) {
                        std::cout << "No peers connected." << std::endl;
                    } else {
                        std::cout << "Total connected peers: " << connectedPeers.size() << std::endl;
                        for (const auto& peer : connectedPeers) {
                            std::cout << "- " << peer.id << " at " << peer.address << ":" << peer.port 
                                      << " (" << (peer.type == NodeType::FULL_NODE ? "Full Node" : "Wallet Node") << ")" << std::endl;
                        }
                    }
                } else {
                    std::cout << "Invalid choice. Please try again." << std::endl;
                }
                break;
                
            case 0: // Exit
                std::cout << "Exiting Blockchain Node. Goodbye!" << std::endl;
                break;
                
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    } while (choice != 0);
    
    // Stop network services
    networkManager.stop();
    
    return 0;
} 