#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <functional>
#include "Transaction.h"

class Block {
public:
    int blockNumber;
    time_t timestamp;
    std::vector<Transaction> transactions;
    std::string previousHash;
    std::string hash;
    int nonce;
    int difficulty;

    Block(int blockNumber, std::vector<Transaction> txs, std::string prevHash, int diff);
   
    std::string calculateHash() const;
    std::string mineBlock();

private:
    std::string simpleHash(const std::string& str) const;
};

#endif 