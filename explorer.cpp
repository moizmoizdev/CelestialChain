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
    if (!blockchain || !db || !balanceMap) {
        std::cout << "ERROR: Explorer requires valid blockchain, database, and balance mapping" << std::endl;
        return;
    }
    
    int choice;
    do {
        std::cout << "\n========== Blockchain Explorer ==========\n" << std::endl;
        std::cout << "1. View Blockchain Summary" << std::endl;
        std::cout << "2. View Block Details" << std::endl;
        std::cout << "3. View Transaction Details" << std::endl;
        std::cout << "4. Search Address" << std::endl;
        std::cout << "5. View Top Addresses by Balance" << std::endl;
        std::cout << "6. View Latest Transactions" << std::endl;
        std::cout << "0. Return to Main Menu" << std::endl;
        std::cout << "\nEnter your choice: ";
        
        std::cin >> choice;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }
        
        std::string input;
        
        switch (choice) {
            case 1:
                // View Blockchain Summary
                std::cout << "\n---------- Blockchain Summary ----------" << std::endl;
                std::cout << "Blockchain Height: " << getBlockCount() << " blocks" << std::endl;
                std::cout << "Total Transactions: " << getTransactionCount() << std::endl;
                std::cout << "Total Supply: " << blockchain->getTotalSupply() << " coins" << std::endl;
                std::cout << "Current Mining Reward: " << blockchain->getCurrentMiningReward() << " coins" << std::endl;
                std::cout << "Number of Unique Addresses: " << balanceMap->getAllBalances().size() << std::endl;
                
                // Display latest block info
                if (blockchain->getChainSize() > 0) {
                    const Block& latestBlock = blockchain->getLatestBlock();
                    std::cout << "\nLatest Block:" << std::endl;
                    std::cout << "  Block #" << latestBlock.blockNumber << std::endl;
                    std::cout << "  Hash: " << latestBlock.hash << std::endl;
                    std::cout << "  Timestamp: " << std::asctime(std::localtime(&latestBlock.timestamp));
                    std::cout << "  Transactions: " << latestBlock.transactions.size() << std::endl;
                }
                break;
                
            case 2:
                explorerClearScreen();
                cout << "Enter block number: ";
                cin >> input;
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                try {
                    displayBlockDetails(std::stoi(input));
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
                
            case 3:
                explorerClearScreen();
                cout << "Enter transaction hash: ";
                getline(cin, input);
                try {
                    displayTransactionDetails(input);
                } catch (const exception& e) {
                    cout << "Error: " << e.what() << endl;
                }
                break;
                
            case 4:
                explorerClearScreen();
                cout << "Enter address: ";
                getline(cin, input);
                try {
                    displayAddressDetails(input);
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
                
            case 6:
                explorerClearScreen();
                displayLatestBlocks();
                break;
                
            case 0:
                break;
                
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
        
        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 0);
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
    cout << "Balance: " << getAddressBalance(address) << " $CLST" << endl;
    
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
