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
- Configure mining difficulty via UI
- Networking with peer-to-peer connections
- Persistent storage using LevelDB
- Support for two types of nodes:
  - Full Nodes: Complete blockchain functionality with mining capabilities
  - Wallet Nodes: Transaction creation and wallet management without mining
- Persistent wallet storage with automatic loading/saving of wallet files
- Native cryptocurrency ($CLST) with complete tokenomics
- Mining reward halving mechanism similar to Bitcoin
- Interactive blockchain explorer with address and transaction lookup
- Dynamic mining difficulty adjustment

## New Features

### Native Cryptocurrency ($CLST)
- The blockchain now features a native cryptocurrency named $CLST
- Complete tokenomics with supply management and transaction history
- Balance tracking across all addresses with appropriate formatting
- UI displays of balances, transactions, and total supply in $CLST

### Mining Reward Halving
- Block rewards automatically halve every 30 days
- Initial mining reward: 50 $CLST
- Current reward calculation based on time elapsed since genesis block
- UI displays of current reward, number of halvings occurred, and days until next halving
- Minimum reward floor of 0.01 $CLST to ensure continued mining incentives

### Adjustable Mining Difficulty
- Mining difficulty can be changed through the user interface (1-8)
- Higher difficulty requires more computational work to mine blocks
- UI displays estimated mining time based on current difficulty
- Prevents runaway inflation by maintaining appropriate block generation times

### Enhanced Blockchain Explorer
- Improved blockchain explorer with detailed statistics
- Top address balances display
- Wallet transaction history with pending and confirmed transactions
- Complete blockchain overview including supply information
## Screenshots

### Blockchain View
![Blockchain View](/web/blockchain-ui/images/blockchain.jpg)

### Create Transaction
![Create Transaction](/web/blockchain-ui/images/create_tx.jpg)

### Dashboard
![Dashboard](/web/blockchain-ui/images/dashboard.jpg)

### Explorer
![Explorer](/web/blockchain-ui/images/explorer.jpg)

### Mempool
![Mempool](/web/blockchain-ui/images/mempool.jpg)

### Mine Block
![Mine Block](/web/blockchain-ui/images/mineblock.jpg)

### Peer Connections
![Peer Connections](/web/blockchain-ui/images/peers.jpg)

### Synchronization
![Synchronization](/web/blockchain-ui/images/synchronize.jpg)

### Transaction History
![Transaction History](/web/blockchain-ui/images/tx_history.jpg)

### Wallet View
![Wallet View](/web/blockchain-ui/images/wallet.jpg)

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

## Testing the Longest Chain Algorithm

The blockchain implements the "longest chain rule" based on accumulated proof of work. To test this functionality:

1. **Start separate node groups**:
   ```bash
   # Start first node
   ./blockchain_node --port 8000 --type full --difficulty 4
   
   # Start second node (don't connect it to first yet)
   ./blockchain_node --port 9000 --type full --difficulty 4
   ```

2. **Create chains of different lengths**:
   - In first node (port 8000), mine several blocks (3-4)
   - In second node (port 9000), mine 1-2 blocks

3. **Connect the nodes**:
   - From either node, choose the option to connect to the other node

4. **Observe chain synchronization**:
   - The node with the shorter chain should adopt the longer chain
   - Check logs to see chain replacement process

5. **Advanced test - Different Difficulties**:
   ```bash
   # Start first node with higher difficulty
   ./blockchain_node --port 8000 --type full --difficulty 6
   
   # Start second node with lower difficulty  
   ./blockchain_node --port 9000 --type full --difficulty 3
   ```
   - Mine more blocks on second node (due to lower difficulty)
   - Mine fewer blocks on first node
   - Connect them and observe whether the chain with more total work (sum of 2^difficulty) is chosen
   
## Database Persistence

The blockchain uses LevelDB for persistent storage of all blocks and transactions:

1. **Database Location**:
   - Each node stores data in a separate directory based on host:port
   - Default location: `./Storage_127.0.0.1_PORT/`

2. **Clean Start**:
   - To start with a clean blockchain: `./blockchain_node --clean`
   - Or for multiple nodes: `./run_network.bat --clean`

3. **Testing Persistence**:
   - Start nodes and mine blocks
   - Exit all nodes
   - Restart the nodes without the `--clean` flag
   - Verify that the blockchain state is preserved

4. **Database Verification**:
   - The system automatically verifies database integrity on startup
   - Detects and attempts to repair corrupted entries 

## Wallet Persistence

The blockchain maintains wallet persistence across sessions:

1. **Wallet File Location**:
   - Each node stores its wallet in a separate file based on host:port
   - Default location: `./wallets/127_0_0_1_PORT.ini`

2. **Wallet File Format**:
   - INI-style format containing:
     - Wallet address
     - Public key
     - Private key (securely stored)
     - Last known balance

3. **Automatic Management**:
   - On first run, a new wallet is generated and saved
   - On subsequent runs, the existing wallet is loaded
   - Balance is synchronized with the blockchain database

4. **Node-Specific Wallets**:
   - Each node maintains its own unique wallet
   - This allows testing transfers between nodes on the same machine

## Testing the Longest Chain Algorithm

The blockchain implements the "longest chain rule" based on accumulated proof of work. To test this functionality:

1. **Start separate node groups**:
   ```bash
   # Start first node
   ./blockchain_node --port 8000 --type full --difficulty 4
   
   # Start second node (don't connect it to first yet)
   ./blockchain_node --port 9000 --type full --difficulty 4
   ```

2. **Create chains of different lengths**:
   - In first node (port 8000), mine several blocks (3-4)
   - In second node (port 9000), mine 1-2 blocks

3. **Connect the nodes**:
   - From either node, choose the option to connect to the other node

4. **Observe chain synchronization**:
   - The node with the shorter chain should adopt the longer chain
   - Check logs to see chain replacement process

5. **Advanced test - Different Difficulties**:
   ```bash
   # Start first node with higher difficulty
   ./blockchain_node --port 8000 --type full --difficulty 6
   
   # Start second node with lower difficulty  
   ./blockchain_node --port 9000 --type full --difficulty 3
   ```
   - Mine more blocks on second node (due to lower difficulty)
   - Mine fewer blocks on first node
   - Connect them and observe whether the chain with more total work (sum of 2^difficulty) is chosen
   
## Database Persistence

The blockchain uses LevelDB for persistent storage of all blocks and transactions:

1. **Database Location**:
   - Each node stores data in a separate directory based on host:port
   - Default location: `./Storage_127.0.0.1_PORT/`

2. **Clean Start**:
   - To start with a clean blockchain: `./blockchain_node --clean`
   - Or for multiple nodes: `./run_network.bat --clean`

3. **Testing Persistence**:
   - Start nodes and mine blocks
   - Exit all nodes
   - Restart the nodes without the `--clean` flag
   - Verify that the blockchain state is preserved

4. **Database Verification**:
   - The system automatically verifies database integrity on startup
   - Detects and attempts to repair corrupted entries 
