#include <iostream>
#include <string>
#include <limits>
#include "Blockchain.h"
#include "Transaction.h"
#include "wallet.h"
#include "NetworkNode.h"
#include <stdexcept>

#define HOST "127.0.0.1"
#define PORT  8081

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else   
    system("clear");
#endif
}

void printMenu() {
    std::cout << "\n========== Blockchain Demo ==========\n";
    std::cout << "1. Add a new transaction to mempool\n";
    std::cout << "2. Mine a new block\n";
    std::cout << "3. View blockchain\n";
    std::cout << "4. View mempool\n";
    std::cout << "5. View specific block\n";
    std::cout << "6. Change mining difficulty\n";
    std::cout << "7. Validate blockchain\n";
    std::cout << "8. Create wallet\n";
    std::cout << "9. Send money\n";
    std::cout << "0. Exit\n";
    std::cout << "====================================\n";
    std::cout << "Enter your choice: ";
}

int main() {
    clearScreen();
    
    std::cout << "Welcome to Blockchain Demo\n";
    
    int difficulty = 4;
    std::cout << "Enter mining difficulty (1-6 recommended, higher = slower): ";
    std::cin >> difficulty;
    
    // Ensure difficulty is within reasonable limits
    if (difficulty < 1) difficulty = 1;
    if (difficulty > 6) difficulty = 6;
    
    Blockchain blockchain(difficulty);
    std::cout << "Blockchain initialized with difficulty " << difficulty << std::endl;
    
    // Create a default wallet list
    std::vector<Wallet*> walletPointers;
    std::cout << "Creating default wallet..." << std::endl;
    Wallet* wallet = new Wallet();
    walletPointers.push_back(wallet);
    
    // Create wallet list for UI access (non-pointer version)
    std::vector<Wallet> wallets;
    wallets.push_back(*wallet);
    
    std::cout << "Wallet address: " << wallet->getAddress() << std::endl;
    std::cout << "Public key: " << wallet->getPublicKeyHex() << std::endl;
    
    // NodeType for networking
    NodeType nodetype = NodeType::FULL_NODE;
    
    int choice;
    do {
        printMenu();
        std::cin >> choice;
        
        // Handle invalid input
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = -1;
        }
        
        std::string transactionData;
        std::string sender, receiver;
        int amount;
        int blockIndex;
        int walletIndex;
        
        switch (choice) {
            case 1: // Add transaction
                clearScreen();
                std::cout << "Enter sender: ";
                std::cin >> sender;
                std::cout << "Enter receiver: ";
                std::cin >> receiver;
                std::cout << "Enter amount: ";
                std::cin >> amount;
                blockchain.addTransaction(Transaction(sender, receiver, amount));
                std::cout << "Transaction added to mempool." << std::endl;
                break;
                
            case 2: // Mine block
                clearScreen();
                std::cout << "Mining block..." << std::endl;
                try {
                    Block minedblock = blockchain.mineBlock(walletPointers);
                    
                    // Pass the first wallet to NetworkManager
                    NetworkManager networkmanager(blockchain, *wallet, HOST, PORT, nodetype);
                    networkmanager.broadcastBlock(minedblock);
                }
                catch(const std::exception& err){
                    std::cout << err.what();
                }
                break;
                
            case 3: // View blockchain
                clearScreen();
                std::cout << "Current Blockchain Status:\n" << std::endl;
                blockchain.printBlockchain();
                break;
                
            case 4: // View mempool
                clearScreen();
                std::cout << "Current Mempool Status:\n" << std::endl;
                blockchain.printMempool();
                break;
                
            case 5: // View specific block
                clearScreen();
                std::cout << "Enter block index: ";
                std::cin >> blockIndex;
                
                try {
                    const Block& block = blockchain.getBlock(blockIndex);
                    std::cout << "Block #" << block.blockNumber << std::endl;
                    std::cout << "  Hash: " << block.hash << std::endl;
                    std::cout << "  Previous Hash: " << block.previousHash << std::endl;
                    std::cout << "  Nonce: " << block.nonce << std::endl;
                    std::cout << "  Timestamp: " << block.timestamp << std::endl;
                    std::cout << "  Transactions: " << block.transactions.size() << std::endl;
                    
                    for (const auto& tx : block.transactions) {
                        std::cout << "    - " << tx.sender << " -> " << tx.receiver << ": " << tx.amount << std::endl;
                    }
                } catch (const std::out_of_range& e) {
                    std::cout << "Invalid block index." << std::endl;
                }
                break;
                
            case 6: // Change mining difficulty
                clearScreen();
                std::cout << "Current difficulty: " << difficulty << std::endl;
                std::cout << "Enter new mining difficulty (1-6 recommended): ";
                std::cin >> difficulty;
                
                // Ensure difficulty is within reasonable limits
                if (difficulty < 1) difficulty = 1;
                if (difficulty > 6) difficulty = 6;
                
                std::cout << "Mining difficulty changed to " << difficulty << std::endl;
                blockchain = Blockchain(difficulty); // Restart blockchain with new difficulty
                std::cout << "Blockchain has been reset with new difficulty." << std::endl;
                break;
                
            case 7: // Validate blockchain
                clearScreen();
                if (blockchain.isValidChain()) {
                    std::cout << "Blockchain is valid." << std::endl;
                } else {
                    std::cout << "WARNING: Blockchain is NOT valid!" << std::endl;
                }
                break;
                
            case 8: // Create wallet
                clearScreen();
                std::cout << "Creating new wallet..." << std::endl;
                try {
                    Wallet* newWallet = new Wallet();
                    walletPointers.push_back(newWallet);
                    wallets.push_back(*newWallet);
                    std::cout << "Wallet #" << (wallets.size() - 1) << " created." << std::endl;
                    std::cout << "Wallet address: " << newWallet->getAddress() << std::endl;
                }
                catch(const std::exception& e){
                    std::cout << "Error creating wallet: " << e.what() << std::endl;
                }
                break;
                
            case 9: // Send money
                clearScreen();
                if (wallets.empty()) {
                    std::cout << "No wallets available. Create a wallet first." << std::endl;
                    break;
                }
                
                std::cout << "Available wallets:" << std::endl;
                for (size_t i = 0; i < wallets.size(); i++) {
                    std::cout << i << ": " << wallets[i].getAddress() << " (Balance: " << wallets[i].getBalance() << ")" << std::endl;
                }
                
                std::cout << "Select sender wallet index: ";
                std::cin >> walletIndex;
                
                if (walletIndex < 0 || walletIndex >= static_cast<int>(wallets.size())) {
                    std::cout << "Invalid wallet index." << std::endl;
                    break;
                }
                
                std::cout << "Select a recipient option:" << std::endl;
                std::cout << "1. Send to an existing wallet" << std::endl;
                std::cout << "2. Send to a custom address" << std::endl;
                int recipientOption;
                std::cin >> recipientOption;
                
                if (recipientOption == 1) {
                    std::cout << "Available recipient wallets:" << std::endl;
                    for (size_t i = 0; i < wallets.size(); i++) {
                        if (i != static_cast<size_t>(walletIndex)) {
                            std::cout << i << ": " << wallets[i].getAddress() << std::endl;
                        }
                    }
                    
                    int recipientIndex;
                    std::cout << "Select recipient wallet index: ";
                    std::cin >> recipientIndex;
                    
                    if (recipientIndex < 0 || recipientIndex >= static_cast<int>(wallets.size()) || recipientIndex == walletIndex) {
                        std::cout << "Invalid recipient index." << std::endl;
                        break;
                    }
                    
                    receiver = wallets[recipientIndex].getAddress();
                } else {
                    std::cout << "Enter receiver address: ";
                    std::cin >> receiver;
                }
                
                std::cout << "Enter amount: ";
                std::cin >> amount;
                
                if (amount <= 0) {
                    std::cout << "Amount must be positive." << std::endl;
                    break;
                }
                
                {
                    Transaction tx("", "", 0); // Placeholder transaction
                    walletPointers[walletIndex]->sendMoney(receiver, amount, tx);
                    blockchain.addTransaction(tx);
                    std::cout << "Transaction created and added to mempool." << std::endl;
                }
                break;
                
            case 0: // Exit
                std::cout << "Exiting Blockchain Demo. Goodbye!" << std::endl;
                // Clean up dynamically allocated wallets
                for (auto& w : walletPointers) {
                    delete w;
                }
                break;
                
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore();
            std::cin.get();
            clearScreen();
        }
    } while (choice != 0);
    
    return 0;
} 