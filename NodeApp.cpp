#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <vector>
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
            } else {
                nodeType = NodeType::FULL_NODE;
            }
        } else if (arg == "--difficulty" && i + 1 < argc) {
            difficulty = std::stoi(argv[++i]);
        } else if (arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]\n";
            std::cout << "  --host HOST       Set the host address\n";
            std::cout << "  --port PORT       Set the port number\n";
            std::cout << "  --type TYPE       Set the node type (full or wallet)\n";
            std::cout << "  --difficulty DIFF Set the mining difficulty\n";
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

    // Bootstrap: connect to a few known peers for peer discovery
    std::vector<std::pair<std::string,int>> bootstrap = {
        {"127.0.0.1", 8000},
        {"127.0.0.1", 8001},
        {"127.0.0.1", 8002},
    };
    for (auto& [bhost, bport] : bootstrap) {
        if (bhost == host && bport == port) continue;
        if (networkManager.connectToPeer(bhost, bport)) {
            std::cout << "Bootstrapped from " << bhost << ':' << bport << std::endl;
        }
    }

    std::cout << "Network services started and bootstrapped. Press Enter to continue..." << std::endl;
    std::cin.get();

    int choice;
    do {
        clearScreen();
        if (nodeType == NodeType::FULL_NODE) printFullNodeMenu();
        else                         printWalletNodeMenu();

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
                    std::vector<Wallet*> wallets = { &networkManager.getWallet() };
                    blockchain.mineBlock(wallets, nodeType);
                    networkManager.broadcastBlock(blockchain.getLatestBlock());
                } else {
                    clearScreen();
                    std::cout << "Enter receiver: "; std::cin >> receiver;
                    std::cout << "Enter amount: ";   std::cin >> amount;
                    Transaction tx("","",0);
                    std::cout<<"Transaction is created"<<std::endl;
                    networkManager.getWallet().sendMoney(receiver, amount, tx);
                    blockchain.addTransaction(tx);
                    networkManager.broadcastTransaction(tx);
                }
                break;
            case 4:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Enter receiver: "; std::cin >> receiver;
                    std::cout << "Enter amount: ";   std::cin >> amount;
                    Transaction tx("","",0);
                    std::cout<<"Transaction is created"<<std::endl;
                    networkManager.getWallet().sendMoney(receiver, amount, tx);
                    std::cout<<"Money is sent"<<std::endl;
                    blockchain.addTransaction(tx);
                    std::cout<<"Transaction is added"<<std::endl;
                    networkManager.broadcastTransaction(tx);
                    std::cout<<"Transaction is broadcasted"<<std::endl;
                } else {
                    clearScreen();
                    std::cout << "Address: " << networkManager.getWallet().getAddress() << std::endl;
                    std::cout << "Balance: " << networkManager.getWallet().getBalance() << std::endl;
                }
                break;
            case 5:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    std::cout << "Address: " << networkManager.getWallet().getAddress() << std::endl;
                    std::cout << "Balance: " << networkManager.getWallet().getBalance() << std::endl;
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

    networkManager.stop();
    return 0;
}
