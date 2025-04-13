#include <iostream>
#include <string>
#include <limits>
#include "Blockchain.h"
#include "Transaction.h"
//#include "wallet.h"

void clearScreen() {
    system("cls");
}
using namespace std;
void printMenu() {
    cout << "\n--------------------\n";
    cout << "1. Add a new transaction to mempool\n";
    cout << "2. Mine a new block\n";
    cout << "3. View blockchain\n";
    cout << "4. View mempool\n";
    cout << "5. View specific block\n";
    cout << "6. Change mining difficulty\n";
    cout << "7. Validate blockchain\n";
    //cout << "8. Create wallet\n";
    //cout << "9. Send money\n";
    cout << "0. Exit\n";
    cout << "----------------------------\n";
    cout << "Enter your choice: ";
}

int main() {
    clearScreen();
    cout << "Welcome to Blockchain Demo\n";
    
    int difficulty = 4;
    cout << "Enter mining difficulty (1-6 recommended, higher = slower): ";
    cin >> difficulty;
    
    if (difficulty < 1) 
        difficulty = 1;
    if (difficulty > 6) 
        difficulty = 6;
    
    Blockchain blockchain(difficulty);
    cout << "Blockchain initialized with difficulty " << difficulty << endl;
    
    vector<Wallet> wallets;
    cout << "Creating default wallet......" << endl;
    wallets.push_back(Wallet());
    
    int choice;
    do {
        printMenu();
        cin >> choice;
        

        
        string transactionData;
        string sender, receiver;
        int amount;
        int blockIndex;
        int walletIndex;
        
        switch (choice) {
            case 1:
                clearScreen();
                cout << "Enter sender: ";
                cin >> sender;
                cout << "Enter receiver: ";
                cin >> receiver;
                cout << "Enter amount: ";
                cin >> amount;
                blockchain.addTransaction(Transaction(sender, receiver, amount));
                cout << "Transaction added to mempool." << endl;
                break;
               
            case 2: 
                clearScreen();
                cout << "Mining block..." << endl;
                blockchain.mineBlock(wallets);
                break;
                
            case 3: 
                clearScreen();
                cout << "Current Blockchain Status:\n" << endl;
                blockchain.printBlockchain();
                break;
                
            case 4: 
                clearScreen();
                cout << "Current Mempool Status:\n" << endl;
                blockchain.printMempool();
                break;
                
            case 5: 
                clearScreen();
                cout << "Enter block index: ";
                cin >> blockIndex;
                
                try {
                    Block block = blockchain.getBlock(blockIndex);
                    cout << "Block #" << block.blockNumber << endl;
                    cout << "Hash: " << block.hash << endl;
                    cout << "Previous Hash: " << block.previousHash << endl;
                    cout << "Nonce: " << block.nonce << endl;
                    cout << "Timestamp: " << block.timestamp << endl;
                    cout << "Transactions: " << block.transactions.size() << endl;
                    
                    for (const auto tx : block.transactions) {
                        cout << "    - " << tx.sender << " -> " << tx.receiver << ": " << tx.amount << endl;
                    }
                } catch (const out_of_range e) {
                    cout << "Invalid block index." << endl;
                }
                break;
                
            case 6: 
                clearScreen();
                cout << "Current difficulty: " << difficulty << endl;
                cout << "Enter new mining difficulty (1-6 recommended): ";
                cin >> difficulty;
                
                if (difficulty < 1) difficulty = 1;
                if (difficulty > 6) difficulty = 6;
                
                cout << "Mining difficulty changed to " << difficulty << endl;
                blockchain = Blockchain(difficulty); 
                cout << "Blockchain has been reset with new difficulty." << endl;
                break;
                
            case 7: 
                clearScreen();
                if (blockchain.isValidChain()) {
                    cout << "Blockchain is valid." << endl;
                } else {
                    cout << "WARNING!!!! Blockchain is NOT valid!" << endl;
                }
                break;
                
           /* case 8:
                clearScreen();
                cout << "Creating new wallet..." << endl;
                wallets.push_back(Wallet());
                cout << "Wallet #" << (wallets.size() - 1) << " created." << endl;
                break;*/
          
            case 0:
                cout << "Exiting Blockchain Demo. Goodbye!" << endl;
                break;
                
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
        
        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
            clearScreen();
        }
    } while (choice != 0);
    
    return 0;
} 