#include "Transaction.h"
#include "sha.h"
#include "wallet.h"
#include "crypto_utils.h"
#include <iostream>
#include <sstream>
#include <ctime>

Transaction::Transaction(std::string sender, std::string receiver, double amount)
    : sender(sender), senderPublicKey(""), receiver(receiver), amount(amount), timestamp(time(nullptr)) {
    // Calculate the hash but leave signature empty
    hash = calculateHash();
}

Transaction::Transaction(std::string sender, std::string senderPublicKey, std::string receiver, 
                        double amount, std::string hash, std::string signature, unsigned long timestamp)
    : sender(sender), senderPublicKey(senderPublicKey), receiver(receiver), 
      amount(amount), hash(hash), signature(signature), timestamp(timestamp) {
    // Hash is provided, so we don't recalculate it
}

std::string Transaction::calculateHash() const {
    std::string data = sender + senderPublicKey + receiver + std::to_string(amount) + std::to_string(timestamp);
    return "0x" + computeSHA256(data); // Add 0x prefix to transaction hash
}

bool Transaction::verifyAddress() const {
    // Skip verification for the genesis transaction
    if (sender == "Genesis" && receiver == "Genesis") {
        return true;
    }
    
    // If public key is empty, we can't verify the address
    if (senderPublicKey.empty()) {
        std::cerr << "ERROR: Cannot verify address - sender public key is empty" << std::endl;
        return false;
    }
    
    // Derive the address from the provided public key
    std::string derivedAddress = deriveAddressFromPublicKey(senderPublicKey);
    
    // Check if the derived address matches the claimed address
    bool result = (derivedAddress == sender);
    
    if (!result) {
        std::cerr << "ERROR: Address verification failed!" << std::endl;
        std::cerr << "  Claimed address: " << sender << std::endl;
        std::cerr << "  Derived from public key: " << derivedAddress << std::endl;
    }
    
    return result;
}

bool Transaction::verifySignature() const {
    // Skip verification for the genesis transaction
    if (sender == "Genesis" && receiver == "Genesis") {
        return true;
    }
    
    // Check for empty signature
    if (signature.empty()) {
        std::cerr << "ERROR: Cannot verify empty signature" << std::endl;
        return false;
    }
    
    // Check for empty public key
    if (senderPublicKey.empty()) {
        std::cerr << "ERROR: Cannot verify signature - sender public key is empty" << std::endl;
        return false;
    }
    
    // Debug output for transaction verification
    std::cout << "\n===== Transaction Signature Verification =====" << std::endl;
    std::cout << "  Sender Address: " << sender << std::endl;
    std::cout << "  Sender Public Key: " << (senderPublicKey.length() > 20 ? senderPublicKey.substr(0, 20) + "..." : senderPublicKey) << std::endl;
    std::cout << "  Hash: " << hash << std::endl;
    std::cout << "  Signature: " << (signature.length() > 20 ? signature.substr(0, 20) + "..." : signature) << std::endl;
    
    // Verify using the public key (not the address)
    bool result = Wallet::verifySignature(hash, signature, senderPublicKey);
    std::cout << "===== Signature Verification " << (result ? "PASSED" : "FAILED") << " =====" << std::endl;
    return result;
}

bool Transaction::isValid() const {
    // Special case for genesis transaction
    if (sender == "Genesis" && receiver == "Genesis") {
        return true;
    }
    
    // Regular transaction validations
    if (sender.empty()) {
        std::cerr << "ERROR: Transaction has empty sender field" << std::endl;
        return false;
    }
    
    if (receiver.empty()) {
        std::cerr << "ERROR: Transaction has empty receiver field" << std::endl;
        return false;
    }
    
    if (amount <= 0) {
        std::cerr << "ERROR: Transaction has non-positive amount: " << amount << std::endl;
        return false;
    }
    
    // Verify the hash matches
    std::string expectedHash = calculateHash();
    if (hash != expectedHash) {
        std::cerr << "ERROR: Transaction hash mismatch." << std::endl;
        std::cerr << "  Expected: " << expectedHash << std::endl;
        std::cerr << "  Got: " << hash << std::endl;
        return false;
    }
    
    // Step 1: Verify that the public key matches the claimed sender address
    bool addressValid = verifyAddress();
    if (!addressValid) {
        std::cerr << "ERROR: Address verification failed" << std::endl;
        return false;
    }
    
    // Step 2: Verify the signature using the public key
    bool sigValid = verifySignature();
    if (!sigValid) {
        std::cerr << "ERROR: Signature verification failed" << std::endl;
        return false;
    }
    
    return true;
}

void Transaction::print() const {
    std::cout << "Transaction Details:" << std::endl;
    std::cout << "  Sender Address: " << sender << std::endl;
    if (!senderPublicKey.empty()) {
        std::cout << "  Sender Public Key: " << senderPublicKey.substr(0, 20) << "..." << std::endl;
    } else {
        std::cout << "  Sender Public Key: <none>" << std::endl;
    }
    std::cout << "  Receiver: " << receiver << std::endl;
    std::cout << "  Amount: " << amount << std::endl;
    std::cout << "  Hash: " << hash << std::endl;
    std::cout << "  Signature: " << (signature.empty() ? "unsigned" : 
                                  (signature.length() > 20 ? signature.substr(0, 20) + "..." : signature)) << std::endl;
    std::cout << "  Timestamp: " << timestamp << std::endl;
    std::cout << "  Valid: " << (isValid() ? "Yes" : "No") << std::endl;
}

void Transaction::sign(const Wallet& wallet) {
    // Add sender's public key to the transaction
    senderPublicKey = wallet.getPublicKeyHex();
    
    // Recalculate hash now that we have the public key
    hash = calculateHash();
    
    // Sign the transaction using the wallet's private key
    signature = wallet.signMessage(hash);
    std::cout << "Transaction signed with wallet " << wallet.getAddress() << std::endl;
}