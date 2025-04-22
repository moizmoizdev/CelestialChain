#include "explorer.h"
#include <iostream>
#include <iomanip>
#include <limits>

using namespace std;

// Clear screen function for better UI
void explorerClearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

Explorer::Explorer(Blockchain* chain, BlockchainDB* database, BalanceMapping* balances)
    : blockchain(chain), db(database), balanceMap(balances) {
    if (!blockchain) {
        throw std::runtime_error("Explorer requires a valid blockchain instance");
    }
}

// Basic explorer menu
void Explorer::showExplorerMenu(const std::string& currentWalletAddress) const {
    bool running = true;
    while (running) {
        explorerClearScreen();
        cout << "===== Blockchain Explorer =====" << endl;
        cout << "1. View Latest Blocks" << endl;
        cout << "2. Search by Block Number" << endl;
        cout << "3. Search by Transaction Hash" << endl;
        cout << "4. Search by Address" << endl;
        
        // If we have a current wallet address, show as option 5
        if (!currentWalletAddress.empty()) {
            cout << "5. View Your Wallet Details" << endl;
        }
        
        cout << "0. Exit Explorer" << endl;
        cout << "===========================" << endl;
        cout << "Enter your choice: ";
        
        int choice;
        cin >> choice;
        
        // Clear input buffer
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        string searchQuery;
        size_t blockNumber;
        
        switch (choice) {
            case 1:
                explorerClearScreen();
                displayLatestBlocks();
                break;
                
            case 2:
                explorerClearScreen();
                cout << "Enter block number: ";
                cin >> blockNumber;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                try {
                    displayBlockDetails(blockNumber);
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
                
            case 3:
                explorerClearScreen();
                cout << "Enter transaction hash: ";
                getline(cin, searchQuery);
                try {
                    displayTransactionDetails(searchQuery);
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
                
            case 4:
                explorerClearScreen();
                cout << "Enter address: ";
                getline(cin, searchQuery);
                try {
                    displayAddressDetails(searchQuery);
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
                
            case 5:
                if (!currentWalletAddress.empty()) {
                    explorerClearScreen();
                    displayAddressDetails(currentWalletAddress);
                }
                break;
                
            case 0:
                running = false;
                break;
                
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
        
        if (running && choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
}

// Get balance for an address
double Explorer::getAddressBalance(const std::string& address) const {
    double balance = 0.0;
    if (balanceMap) {
        balanceMap->getBalance(address, balance);
    }
    return balance;
}

// Get a block by its number
Block Explorer::getBlockByNumber(size_t blockNumber) const {
    if (blockNumber >= blockchain->getChainSize()) {
        throw std::runtime_error("Block number out of range");
    }
    return blockchain->getBlock(blockNumber);
}

// Get the total number of blocks
size_t Explorer::getBlockCount() const {
    return blockchain->getChainSize();
}

// Get the total number of transactions
size_t Explorer::getTransactionCount() const {
    size_t count = 0;
    
    // Count confirmed transactions
    for (size_t i = 0; i < blockchain->getChainSize(); i++) {
        count += blockchain->getBlock(i).transactions.size();
    }
    
    // Add pending transactions
    count += blockchain->getMempool().size();
    
    return count;
}

// Display address details
void Explorer::displayAddressDetails(const std::string& address) const {
    cout << "===== Address Information =====" << endl;
    cout << "Address: " << address << endl;
    cout << "Balance: " << getAddressBalance(address) << endl;
    
    // Display transactions
    cout << "\nRecent Transactions:" << endl;
    bool foundTransactions = false;
    
    // Check mempool first for pending transactions
    cout << "\nPending Transactions:" << endl;
    for (const auto& tx : blockchain->getMempool()) {
        if (tx.sender == address || tx.receiver == address) {
            cout << "----------------------------" << endl;
            cout << "Hash: " << tx.hash.substr(0, 10) << "..." << endl;
            cout << "  " << (tx.sender == address ? "Sent to: " : "Received from: ")
                 << (tx.sender == address ? tx.receiver : tx.sender) << endl;
            cout << "  Amount: " << tx.amount << endl;
            cout << "  Status: Pending" << endl;
            foundTransactions = true;
        }
    }
    
    // Then check the blockchain for confirmed transactions
    cout << "\nConfirmed Transactions:" << endl;
    size_t displayCount = 0;
    const size_t MAX_DISPLAY = 10;
    
    // Scan the blockchain from newest to oldest
    for (int i = blockchain->getChainSize() - 1; i >= 0 && displayCount < MAX_DISPLAY; i--) {
        const Block& block = blockchain->getBlock(i);
        
        for (const auto& tx : block.transactions) {
            if (tx.sender == address || tx.receiver == address) {
                cout << "----------------------------" << endl;
                cout << "Hash: " << tx.hash.substr(0, 10) << "..." << endl;
                cout << "  " << (tx.sender == address ? "Sent to: " : "Received from: ")
                     << (tx.sender == address ? tx.receiver : tx.sender) << endl;
                cout << "  Amount: " << tx.amount << endl;
                cout << "  Status: Confirmed (Block #" << block.blockNumber << ")" << endl;
                
                displayCount++;
                foundTransactions = true;
                
                if (displayCount >= MAX_DISPLAY) break;
            }
        }
    }
    
    if (!foundTransactions) {
        cout << "No transactions found for this address." << endl;
    }
}

// Display transaction details
void Explorer::displayTransactionDetails(const std::string& txHash) const {
    bool found = false;
    
    // Check mempool first
    for (const auto& tx : blockchain->getMempool()) {
        if (tx.hash == txHash) {
            cout << "===== Transaction Information =====" << endl;
            cout << "Hash: " << tx.hash << endl;
            cout << "Sender: " << tx.sender << endl;
            cout << "Receiver: " << tx.receiver << endl;
            cout << "Amount: " << tx.amount << endl;
            cout << "Status: Pending" << endl;
            found = true;
            break;
        }
    }
    
    // Then check the blockchain
    if (!found) {
        for (size_t i = 0; i < blockchain->getChainSize(); i++) {
            const Block& block = blockchain->getBlock(i);
            for (const auto& tx : block.transactions) {
                if (tx.hash == txHash) {
                    cout << "===== Transaction Information =====" << endl;
                    cout << "Hash: " << tx.hash << endl;
                    cout << "Sender: " << tx.sender << endl;
                    cout << "Receiver: " << tx.receiver << endl;
                    cout << "Amount: " << tx.amount << endl;
                    cout << "Status: Confirmed" << endl;
                    cout << "Block: #" << block.blockNumber << endl;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
    }
    
    if (!found) {
        cout << "Transaction not found: " << txHash << endl;
    }
}

// Display block details
void Explorer::displayBlockDetails(size_t blockNumber) const {
    try {
        Block block = getBlockByNumber(blockNumber);
        
        cout << "===== Block Information =====" << endl;
        cout << "Block #: " << block.blockNumber << endl;
        cout << "Hash: " << block.hash << endl;
        cout << "Previous Hash: " << block.previousHash << endl;
        cout << "Timestamp: " << block.timestamp << endl;
        cout << "Nonce: " << block.nonce << endl;
        cout << "Difficulty: " << block.difficulty << endl;
        cout << "Transactions: " << block.transactions.size() << endl;
        
        if (!block.transactions.empty()) {
            cout << "\nTransaction List:" << endl;
            for (const auto& tx : block.transactions) {
                cout << "  " << tx.hash.substr(0, 10) << "... | " 
                     << tx.sender.substr(0, 10) << "... -> " 
                     << tx.receiver.substr(0, 10) << "... | " 
                     << tx.amount << endl;
            }
        }
    } catch (const std::exception& e) {
        cout << "Block not found: " << e.what() << endl;
    }
}

// Display latest blocks
void Explorer::displayLatestBlocks(size_t count) const {
    cout << "===== Latest Blocks =====" << endl;
    
    size_t chainSize = blockchain->getChainSize();
    size_t start = (chainSize > count) ? chainSize - count : 0;
    
    for (size_t i = chainSize - 1; i >= start; i--) {
        const Block& block = blockchain->getBlock(i);
        cout << "Block #" << block.blockNumber << " | Hash: " << block.hash.substr(0, 15) << "..." << endl;
        cout << "  Transactions: " << block.transactions.size() << " | Timestamp: " << block.timestamp << endl;
        cout << "----------------------------" << endl;
        
        if (i == 0) break; // Avoid underflow
    }
}
