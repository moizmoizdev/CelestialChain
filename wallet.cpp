#include "wallet.h"
#include "crypto_utils.h"
#include "Transaction.h"
#include "Blockchain.h"
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>

// Helper function to split string by delimiter (local to wallet.cpp)
namespace wallet_internal {
    std::vector<std::string> splitString(const std::string& str, char delim) {
        std::vector<std::string> parts;
        std::stringstream ss(str);
        std::string part;
        while (std::getline(ss, part, delim)) {
            parts.push_back(part);
        }
        return parts;
    }
}

Wallet::Wallet() : balance(100.0), db(nullptr) {
    generateKeyPair();
    saveToIniFile();
}

Wallet::Wallet(BlockchainDB* database) : balance(100.0), db(database) {
    generateKeyPair();
    saveToIniFile();
}

Wallet::~Wallet() {
    // Free the EC key pair
    if (key_pair) {
        EC_KEY_free(key_pair);
        key_pair = nullptr;
    }
}

std::string Wallet::getPublicKeyHex() const {
    if (!key_pair) {
        std::cerr << "ERROR: Wallet has no key pair" << std::endl;
        return "";
    }
    
    std::string result = ::getPublicKeyHex(key_pair);
    if (result.empty()) {
        std::cerr << "ERROR: Failed to get public key hex" << std::endl;
    }
    return result;
}

std::string Wallet::getAddress() const {
    return address;
}

double Wallet::getBalance() const {
    return balance;
}

std::string Wallet::signMessage(const std::string& message) const {
    if (!key_pair) {
        std::cerr << "ERROR: Cannot sign message - wallet has no key pair" << std::endl;
        return "";
    }
    
    std::string signature = ::signMessage(key_pair, message);
    if (signature.empty()) {
        std::cerr << "ERROR: Failed to sign message" << std::endl;
    } else {
        std::cout << "DEBUG - Message signed successfully" << std::endl;
    }
    return signature;
}

bool Wallet::verifySignature(const std::string& message, const std::string& signature, const std::string& publicKeyOrAddress) {
    std::cout << "DEBUG - Static verify signature called with:" << std::endl;
    std::cout << "  Message: " << message << std::endl;
    std::cout << "  Signature: " << (signature.length() > 20 ? signature.substr(0, 20) + "..." : signature) << std::endl;
    std::cout << "  Public Key/Address: " << publicKeyOrAddress << std::endl;

    // Call the global verification function
    bool result = ::verifySignature(message, signature, publicKeyOrAddress);
    std::cout << "Signature verification result: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    return result;
}

bool Wallet::sendMoney(double amount, const std::string& receiverAddress, Transaction& transaction) {
    if (amount <= 0) {
        std::cerr << "ERROR: Cannot send non-positive amount" << std::endl;
        return false;
    }
    
    if (amount > balance) {
        std::cerr << "ERROR: Insufficient funds. Balance: " << balance << ", Trying to send: " << amount << std::endl;
        return false;
    }
    
    std::cout << "\n===== Creating Transaction =====" << std::endl;
    std::cout << "  Sender address: " << address << std::endl;
    std::cout << "  Receiver address: " << receiverAddress << std::endl;
    std::cout << "  Amount: " << amount << std::endl;
    
    transaction = Transaction(address, receiverAddress, amount);
    std::cout << "  Transaction hash: " << transaction.hash << std::endl;
    
    transaction.sign(*this);
    
    // Update balance and save to INI file
    balance -= amount;
    saveToIniFile();
    
    std::cout << "  Transaction created and signed successfully" << std::endl;
    std::cout << "  New balance: " << balance << std::endl;
    std::cout << "===== Transaction Complete =====" << std::endl;
    
    return true;
}

void Wallet::receiveMoney(double amount) {
    if (amount <= 0) {
        std::cerr << "ERROR: Cannot receive non-positive amount" << std::endl;
        return;
    }
    
    balance += amount;
    saveToIniFile();
    
    std::cout << "Received " << amount << " coins. New balance: " << balance << std::endl;
}

// Implement private methods

void Wallet::generateKeyPair() {
    // Generate a new EC key pair
    key_pair = generateECKeyPair();
    if (!key_pair) {
        std::cerr << "ERROR: Failed to generate EC key pair" << std::endl;
        return;
    }
    
    // Get the public key in hex format
    publicKey = ::getPublicKeyHex(key_pair);
    
    // Derive the wallet address from the public key
    address = ::deriveAddressFromPublicKey(publicKey);
    
    std::cout << "Created new wallet with address: " << address << std::endl;
    std::cout << "DEBUG - Public Key: " << publicKey << std::endl;
    std::cout << "DEBUG - Derived Address: " << address << std::endl;
}

std::string Wallet::deriveAddress(const std::string& pubKey) const {
    return deriveAddressFromPublicKey(pubKey);
}

std::string Wallet::sign(const std::string& message) const {
    return signMessage(message);
}

// INI file operations
std::string Wallet::getIniFilePath() const {
    return "wallets/" + address + ".ini";
}

bool Wallet::saveToIniFile() const {
    // Create wallets directory if it doesn't exist
    std::filesystem::create_directories("wallets");
    
    std::ofstream file(getIniFilePath());
    if (!file.is_open()) {
        std::cerr << "Failed to open wallet file for writing: " << getIniFilePath() << std::endl;
        return false;
    }

    // Convert private key to PEM format using modern OpenSSL API
    EVP_PKEY* pkey = EVP_PKEY_new();
    if (!pkey) {
        std::cerr << "Failed to create EVP_PKEY" << std::endl;
        return false;
    }

    if (EVP_PKEY_set_type(pkey, EVP_PKEY_EC) != 1) {
        std::cerr << "Failed to set key type" << std::endl;
        EVP_PKEY_free(pkey);
        return false;
    }

    if (EVP_PKEY_set1_EC_KEY(pkey, key_pair) != 1) {
        std::cerr << "Failed to set EC key" << std::endl;
        EVP_PKEY_free(pkey);
        return false;
    }

    BIO* bio = BIO_new(BIO_s_mem());
    if (!bio || PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        std::cerr << "Failed to write private key to BIO" << std::endl;
        EVP_PKEY_free(pkey);
        if (bio) BIO_free(bio);
        return false;
    }

    char* data;
    long len = BIO_get_mem_data(bio, &data);
    std::string privKey(data, len);

    EVP_PKEY_free(pkey);
    BIO_free(bio);

    // Write wallet data
    file << "[wallet]\n";
    file << "private_key=" << privKey << "\n";
    file << "public_key=" << publicKey << "\n";
    file << "address=" << address << "\n";
    file << "balance=" << balance << "\n";

    file.close();
    std::cout << "Wallet saved to " << getIniFilePath() << std::endl;
    return true;
}

bool Wallet::loadFromIniFile(const std::string& walletAddress) {
    std::string filePath = "wallets/" + walletAddress + ".ini";
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open wallet file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    std::string section;
    std::string privKey;

    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';') continue;

        // Check for section header
        if (line[0] == '[') {
            section = line.substr(1, line.find(']') - 1);
            continue;
        }

        // Parse key-value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (section == "wallet") {
                if (key == "private_key") {
                    privKey = value;
                } else if (key == "public_key") {
                    publicKey = value;
                } else if (key == "address") {
                    address = value;
                } else if (key == "balance") {
                    balance = std::stod(value);
                }
            }
        }
    }

    file.close();

    // Load private key from PEM format using modern OpenSSL API
    if (!privKey.empty()) {
        BIO* bio = BIO_new_mem_buf(privKey.c_str(), privKey.length());
        if (!bio) {
            std::cerr << "Failed to create BIO" << std::endl;
            return false;
        }

        EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);

        if (!pkey) {
            std::cerr << "Failed to read private key" << std::endl;
            return false;
        }

        key_pair = EVP_PKEY_get1_EC_KEY(pkey);
        EVP_PKEY_free(pkey);

        if (!key_pair) {
            std::cerr << "Failed to get EC key from EVP_PKEY" << std::endl;
            return false;
        }
    }

    std::cout << "Wallet loaded from " << filePath << std::endl;
    return true;
}

void Wallet::setDatabase(BlockchainDB* database) {
    db = database;
}

bool Wallet::updateBalance() {
    return saveToIniFile();
}

std::vector<Transaction> Wallet::getTransactionHistory() const {
    std::vector<Transaction> history;
    if (!db) return history;
    
    // Get all transaction keys for this wallet
    auto keys = db->getAllKeys("tx:");
    
    for (const auto& key : keys) {
        std::string txData;
        if (db->get(key, txData)) {
            // Parse transaction data
            std::vector<std::string> parts = wallet_internal::splitString(txData, '|');
            
            if (parts.size() >= 7) {
                Transaction tx(
                    parts[0], // sender
                    parts[1], // senderPublicKey
                    parts[2], // receiver
                    std::stod(parts[3]), // amount
                    parts[5], // hash
                    parts[6], // signature
                    std::stoull(parts[4]) // timestamp
                );
                
                // Only add transactions where this wallet is sender or receiver
                if (tx.sender == address || tx.receiver == address) {
                    history.push_back(tx);
                }
            }
        }
    }
    
    return history;
}

void Wallet::synchronizeBalance(double newBalance) {
    // Update the in-memory balance to match database value
    balance = newBalance;
    std::cout << "Wallet " << address << " balance synchronized to " << balance << std::endl;
}