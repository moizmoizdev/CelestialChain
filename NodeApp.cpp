#include <iostream>
#include <string>
#include <limits>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm> // Add for sort and min functions
#include "NetworkNode.h"
#include "BlockchainDB.h"
#include "Blockchain.h"
#include "wallet.h"
#include "Types.h"
#include "balanceMapping.h"
#include "explorer.h"
#include <stdexcept>
#include <direct.h> // For _mkdir on Windows
using namespace std;
// Helper function to return foldername from ip+port
string fileNameFromHost(const string& input) {
    string result = input;
    for (size_t i = 0; i < result.size(); ++i) {
        if (result[i] == '.' || result[i] == ':' || result[i] == '/' || result[i] == '\\') {
            result[i] = '_';
        }
    }
    return result;
}

void clearScreen() {
    system("cls");
}

// Create directory if it doesn't exist
bool createDirectory(const string& path) {
#ifdef _WIN32
    int result = _mkdir(path.c_str());
#else
    int result = mkdir(path.c_str(), 0755);
#endif
    return result == 0 || errno == EEXIST;
}

void printFullNodeMenu() {
    cout << "\n-------- Blockchain Node --------" << endl;
    cout << "1. View blockchain" << endl;
    cout << "2. View mempool" << endl;
    cout << "3. Mine block" << endl;
    cout << "4. Create transaction" << endl;
    cout << "5. View wallet" << endl;
    cout << "6. Connect to peer" << endl;
    cout << "7. Request blockchain from peers" << endl;
    cout << "8. View connected peers" << endl;
    cout << "9. View blockchain statistics" << endl;
    cout << "10. Visit Explorer" << endl;
    cout << "0. Exit" << endl;
    cout << "====================================" << endl;
    cout << "Enter your choice: ";
}

void printWalletNodeMenu() {
    cout << "\n--------- Blockchain Wallet --------" << endl;
    cout << "1. View blockchain" << endl;
    cout << "2. View mempool" << endl;
    cout << "3. Create transaction" << endl;
    cout << "4. View wallet" << endl;
    cout << "5. Connect to peer" << endl;
    cout << "6. Request blockchain from peers" << endl;
    cout << "7. View connected peers" << endl;
    cout << "8. View blockchain statistics" << endl;
    cout << "9. Visit Explorer" << endl;
    cout << "0. Exit" << endl;
    cout << "-----------------------------------" << endl;
    cout << "Enter your choice: ";
}

int main(int argc, char* argv[]) {
    clearScreen();
    cout << "Starting Blockchain Node Application" << endl;

    string host = "127.0.0.1";
    int port = 8000;
    NodeType nodeType = NodeType::FULL_NODE;
    int difficulty = 4;
    bool cleanStart = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--host" && i + 1 < argc) {
            host = argv[++i];
        } else if (arg == "--port" && i + 1 < argc) {
            port = stoi(argv[++i]);
        } else if (arg == "--type" && i + 1 < argc) {
            string type = argv[++i];
            if (type == "wallet") {
                nodeType = NodeType::WALLET_NODE;
            } else {
                nodeType = NodeType::FULL_NODE;
            }
        } else if (arg == "--difficulty" && i + 1 < argc) {
            difficulty = stoi(argv[++i]);
        } else if (arg == "--clean" || arg == "--fresh") {
            cleanStart = true;
        } else if (arg == "--help") {
            cout << "Usage: " << argv[0] << " [OPTIONS]\n";
            cout << "  --host HOST       Set the host address\n";
            cout << "  --port PORT       Set the port number\n";
            cout << "  --type TYPE       Set the node type (full or wallet)\n";
            cout << "  --difficulty DIFF Set the mining difficulty\n";
            cout << "  --clean           Start with a fresh blockchain (ignore existing database)\n";
            cout << "  --help            Display this help message\n";
            return 0;
        }
    }

    Blockchain blockchain(difficulty);
    
    string hostfilename = fileNameFromHost(host);
    string dbPath = "./Storage_" + hostfilename + "_" + to_string(port);
    
    if (!createDirectory(dbPath)) {
        cout << "Error: Could not create storage directory: " << dbPath << endl;
        return 1;
    }
    
    // Initializing Database
    cout << "Opening database at " << dbPath << "..." << endl;
    BlockchainDB db(dbPath);
    BlockchainDB* dbPtr = nullptr;
    BalanceMapping* balanceMapPtr = nullptr;
    
    if (!db.isOpen()) {
        cout << "Database error: " << db.getLastError() << endl;
        cout << "Do you want to continue without database? (y/n): ";
        char response;
        cin >> response;
        if (response != 'y' && response != 'Y') {
            cout << "Exiting application." << endl;
            return 1;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        // dbPtr remains nullptr - we'll operate without database
        cout << "Running without database functionality." << endl;
    } else {
        cout << "Database opened successfully." << endl;
        dbPtr = &db;  // Set the pointer to the valid database
        
        // Verify database integrity
        cout << "Verifying database integrity..." << endl;
        bool dbIntegrity = db.verifyDatabaseIntegrity(true);
        if (!dbIntegrity) {
            cout << "Database integrity issues were detected and attempt was made to repair." << endl;
        }
        
        // Initialize the balance mapping only if we have a valid database
        cout << "Initializing balance mapping..." << endl;
        BalanceMapping balanceMap(&db);
        balanceMapPtr = &balanceMap;
        
        // Initialize blockchain explorer
        cout << "Initializing blockchain explorer..." << endl;
        Explorer explorer(&blockchain, dbPtr, balanceMapPtr);
        
        // Connect database and balance mapping to blockchain
        blockchain.setDatabase(dbPtr);
        blockchain.setBalanceMapping(balanceMapPtr);
        
        if (!cleanStart) {
            // Load blockchain data from database
            cout << "Loading blockchain data from database..." << endl;
            try {
                blockchain.loadFromDatabase();
                cout << "Blockchain loaded with " << blockchain.getChainSize() << " blocks." << endl;
            } catch (const exception& e) {
                cout << "Error loading blockchain from database: " << e.what() << endl;
                cout << "Starting with fresh blockchain." << endl;
            }
        } else {
            cout << "Clean start requested. Starting with a fresh blockchain." << endl;
        }
    }
    
    cout << "Creating wallet for this node..." << endl;
    // Initialize wallet with host:port to load existing wallet or create a new one
    Wallet nodeWallet(host, port, dbPtr);
    cout << "Wallet address: " << nodeWallet.getAddress() << endl;
    
    // Initializing network manager with the blockchain and wallet
    NetworkManager networkManager(blockchain, nodeWallet, host, port, nodeType);

    // Start network services
    try {
        networkManager.start();
    } catch (const exception& e) {
        cout << "Failed to start network services: " << e.what() << endl;
        return 1;
    }

    // peers_discovery_list is to connect to a few known peers for peer discovery
    // later, it propagates when a network is established
    vector<pair<string,int>> peers_discovery_list = {
        {"127.0.0.1", 8000},
        {"127.0.0.1", 8001},
        {"127.0.0.1", 8002},
    };
    for (auto& [bhost, bport] : peers_discovery_list) {
        if (bhost == host && bport == port) 
        continue;
        if (networkManager.connectToPeer(bhost, bport)) {
            cout << "Bootstrapped from " << bhost << ':' << bport << endl;
        }
    }

    cout << "Network services started and bootstrapped. Press Enter to continue..." << endl;
    cin.get();

    vector<Wallet*> wallets = { &nodeWallet };
    if (nodeType == NodeType::FULL_NODE)
        {
            cout<<"Synchorizing requesting blockchain from peers..."<<endl;
            networkManager.requestBlockchain();
        }
    int choice;
    do {
        clearScreen();
        if (nodeType == NodeType::FULL_NODE)
        {
            printFullNodeMenu();
        }
        else
            printWalletNodeMenu();

        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        string peerAddress, receiver;
        int peerPort;double amount;

        switch (choice) {
            case 1:
                clearScreen();
                cout << blockchain.toString() << endl;
                break;
            case 2:
                clearScreen();
                blockchain.printMempool();
                break;
            case 3:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    blockchain.mineBlock(wallets, nodeType);
                    networkManager.broadcastBlock(blockchain.getLatestBlock());
                } else {
                    clearScreen();
                    cout << "Enter receiver: "; cin >> receiver;
                    cout << "Enter amount: ";   cin >> amount;
                    Transaction tx("", "", 0);
                    cout << "Transaction is created" << endl;
                    nodeWallet.sendMoney(amount, receiver, tx);
                    blockchain.addTransaction(tx);
                    networkManager.broadcastTransaction(tx);
                }
                break;
            case 4:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    cout << "Enter receiver: "; cin >> receiver;
                    cout << "Enter amount: ";   cin >> amount;
                    Transaction tx("", "", 0);
                    cout << "Transaction is created" << endl;
                    nodeWallet.sendMoney(amount, receiver, tx);
                    cout << "Money is sent" << endl;
                    blockchain.addTransaction(tx);
                    cout << "Transaction is added" << endl;
                    networkManager.broadcastTransaction(tx);
                    cout << "Transaction is broadcasted" << endl;
                } else {
                    clearScreen();
                    cout << "Address: " << nodeWallet.getAddress() << endl;
                    cout << "Balance: " << nodeWallet.getBalance() << endl;
                }
                break;
            case 5:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    cout << "Address: " << nodeWallet.getAddress() << endl;
                    cout << "Balance: " << nodeWallet.getBalance() << endl;
                } else {
                    clearScreen();
                    cout << "Peer address: "; cin >> peerAddress;
                    cout << "Peer port: ";   cin >> peerPort;
                    networkManager.connectToPeer(peerAddress, peerPort);
                }
                break;
            case 6:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    cout << "Peer address: "; cin >> peerAddress;
                    cout << "Peer port: ";   cin >> peerPort;
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
                } else if (nodeType == NodeType::WALLET_NODE) {
                    clearScreen();
                    auto peers = networkManager.getConnectedPeers();
                    for (auto& p : peers)
                        cout << p.id << " @ " << p.address << ":" << p.port << endl;
                }
                break;
            case 8:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    // Display blockchain statistics
                    if (dbPtr) {
                        Explorer explorer(&blockchain, dbPtr, balanceMapPtr);
                        
                        cout << "===== Blockchain Statistics =====" << endl;
                        cout << "Total Blocks: " << explorer.getBlockCount() << endl;
                        cout << "Total Transactions: " << explorer.getTransactionCount() << endl;
                        cout << "Unique Addresses: " << balanceMapPtr->getAllBalances().size() << endl;
                        cout << "Total Supply: " << blockchain.getTotalSupply() << endl;
                        
                        // Display wallet balance and transaction history
                        cout << "\n===== Your Wallet =====" << endl;
                        cout << "Address: " << nodeWallet.getAddress() << endl;
                        cout << "Balance: " << explorer.getAddressBalance(nodeWallet.getAddress()) << endl;
                        
                        // Display transactions directly
                        explorer.displayAddressDetails(nodeWallet.getAddress());
                    } else {
                        cout << "Explorer requires database connection to display statistics." << endl;
                    }
                }
                break;
            case 9:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    // Display blockchain statistics
                    if (dbPtr) {
                        Explorer explorer(&blockchain, dbPtr, balanceMapPtr);
                        
                        cout << "===== Blockchain Statistics =====" << endl;
                        cout << "Total Blocks: " << explorer.getBlockCount() << endl;
                        cout << "Total Transactions: " << explorer.getTransactionCount() << endl;
                        cout << "Unique Addresses: " << balanceMapPtr->getAllBalances().size() << endl;
                        cout << "Total Supply: " << blockchain.getTotalSupply() << endl;
                        
                        // Display top balances
                        auto balances = balanceMapPtr->getAllBalances();
                        // Convert to vector for sorting
                        vector<pair<string, double>> balanceList(balances.begin(), balances.end());
                        // Sort by balance (highest first)
                        sort(balanceList.begin(), balanceList.end(), 
                            [](const auto& a, const auto& b) { return a.second > b.second; });
                        
                        cout << "\n===== Top 5 Richest Addresses =====" << endl;
                        size_t count = min(balanceList.size(), size_t(5));
                        for (size_t i = 0; i < count; i++) {
                            cout << (i+1) << ". " << balanceList[i].first << ": " 
                                 << balanceList[i].second << endl;
                        }
                    } else {
                        cout << "Explorer requires database connection to display statistics." << endl;
                    }
                } else if (nodeType == NodeType::WALLET_NODE) {
                    clearScreen();
                    // Launch Explorer for wallet nodes
                    if (dbPtr) {
                        Explorer explorer(&blockchain, dbPtr, balanceMapPtr);
                        explorer.showExplorerMenu(nodeWallet.getAddress());
                    } else {
                        cout << "Explorer requires database connection." << endl;
                    }
                }
                break;
            case 10:
                if (nodeType == NodeType::FULL_NODE) {
                    clearScreen();
                    // Launch Explorer for full nodes
                    if (dbPtr) {
                        Explorer explorer(&blockchain, dbPtr, balanceMapPtr);
                        explorer.showExplorerMenu();
                    } else {
                        cout << "Explorer requires database connection." << endl;
                    }
                }
                break;
            case 0:
                cout << "Exiting. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice." << endl;
        }
        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    } while (choice != 0);

    // Clean shutdown
    networkManager.stop();

    return 0;
}


