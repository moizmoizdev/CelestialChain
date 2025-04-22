# Blockchain Demo

A complete implementation of a blockchain in C++ that demonstrates the core concepts of blockchain technology, including networking and distributed nodes.

## Features

- Create transactions with sender, receiver, and amount information
- Manage wallets with private/public key pairs and address generation
- Send money between wallets and track balances
- Mine new blocks with pending transactions
- View the entire blockchain and its contents
- View transactions in the mempool
- Validate the integrity of the blockchain
- Configure mining difficulty
- Networking with peer-to-peer connections
- Persistent storage using LevelDB
- Persistent storage using LevelDB
- Support for two types of nodes:
  - Full Nodes: Complete blockchain functionality with mining capabilities
  - Wallet Nodes: Transaction creation and wallet management without mining

## Requirements

- C++17 compiler (for structured bindings)
- C++17 compiler (for structured bindings)
- Boost libraries (system, thread)
- pthread
- LevelDB
- OpenSSL
- LevelDB
- OpenSSL

## Building

```bash
# Install dependencies
# Install dependencies
# On Ubuntu:
sudo apt-get install libboost-all-dev libleveldb-dev libssl-dev
sudo apt-get install libboost-all-dev libleveldb-dev libssl-dev

# On Windows with MSYS2/MinGW:
pacman -S mingw-w64-x86_64-boost mingw-w64-x64-leveldb mingw-w64-x64-openssl
pacman -S mingw-w64-x86_64-boost mingw-w64-x64-leveldb mingw-w64-x64-openssl

# Compile the project
make

# Run the standalone demo
make run

# Run a full node (with mining capabilities)
make run-full-node

# Run a wallet node (without mining)
make run-wallet-node
```

## Running Custom Nodes

You can run nodes with custom settings using command line arguments:

```bash
./blockchain_node --host 127.0.0.1 --port 8000 --type full --difficulty 4
./blockchain_node --host 127.0.0.1 --port 8001 --type wallet
```

Available options:
- `--host HOST`: Specify the host address to bind to (default: 127.0.0.1)
- `--port PORT`: Specify the port to listen on (default: 8000)
- `--type TYPE`: Specify the node type (full or wallet, default: full)
- `--difficulty DIFF`: Set the mining difficulty (default: 4)

## Project Structure

- `main.cpp` - Contains the menu-driven interface for the standalone blockchain demo
- `NodeApp.cpp` - Contains the application for networked nodes (both full and wallet)
- `NetworkNode.h/cpp` - Implements the networking functionality using Boost.Asio
- `Blockchain.h/cpp` - Implements the blockchain data structure and related functionality
- `Block.h/cpp` - Implements a single block in the blockchain with mining capability
- `Transaction.h/cpp` - Implements a transaction model with sender, receiver, and amount
- `wallet.h/cpp` - Implements a wallet system for managing keys and balances
- `sha.h/cpp` - Provides hash functionality for the blockchain
- `BlockchainDB.h/cpp` - Implements persistent storage using LevelDB
- `BlockchainDB.h/cpp` - Implements persistent storage using LevelDB

## Networking Architecture

The blockchain uses a peer-to-peer network architecture where:

1. **Full Nodes**:
   - Maintain a complete copy of the blockchain
   - Validate transactions and blocks
   - Mine new blocks
   - Relay transactions and blocks to other nodes
   - Respond to blockchain requests

2. **Wallet Nodes**:
   - Maintain a lightweight copy of the blockchain
   - Create and broadcast transactions
   - Cannot mine new blocks
   - Request blockchain data from full nodes

Communication between nodes is done using a simple message protocol over TCP/IP. Message types include:
- Handshake messages for peer discovery
- Transaction broadcasts
- Block broadcasts
- Blockchain requests and responses
- Peer list exchanges

## How It Works

1. **Blockchain Structure**:
   - Each block contains a timestamp, transactions, previous hash, and current hash
   - Blocks are linked by including the previous block's hash
   - Transactions include sender, receiver, amount, and signature

2. **Mining**:
   - Only full nodes can mine blocks
   - Mining involves finding a hash with a specific number of leading zeros
   - Difficulty determines how many leading zeros are required
   - Once a block is mined, it's broadcast to all peers

3. **Transactions**:
   - Created by wallet or full nodes
   - Signed with the sender's private key
   - Broadcast to the network
   - Collected in the mempool until mined into a block

4. **Wallets**:
   - Generate private/public key pairs
   - Derive addresses from public keys
   - Sign transactions
   - Track balance based on blockchain transactions

5. **Networking**:
   - Nodes discover each other through peer exchange
   - New transactions and blocks are broadcast to all peers
   - Peers relay valid transactions and blocks to their peers
   - Nodes can request the blockchain from peers 

6. **Storage**:
   - Each node maintains its own database using LevelDB
   - Database files are stored in unique directories based on host:port combination
   - Blockchain state is persisted between executions
   - Automatic database directory creation

## Running Multiple Nodes

Each node uses a separate database directory based on its host and port combination. This allows multiple nodes to run simultaneously on the same machine without conflicts. The node application creates these directories automatically.

To start a network of nodes:
```bash
# Start the first full node on port 8000
./blockchain_node --port 8000 --type full

# In a separate terminal, start another full node on port 8001
./blockchain_node --port 8001 --type full

# In a third terminal, start a wallet node on port 8002
./blockchain_node --port 8002 --type wallet
```

Or use the batch script:
```bash
./run_network.bat
``` 