# CelestialChain

CelestialChain is a complete blockchain system implemented in C++ with a modern React-based UI. It demonstrates the core principles of blockchain technology including networking, wallet management, mining, and real-time interactions across distributed nodes. This project is ideal for educational exploration, blockchain prototyping, and real-world decentralized architecture simulation.

---

## Core Features

### Blockchain Backend (C++)
- Create and broadcast transactions with sender, receiver, and amount
- Wallet management with public/private key generation and address derivation
- Full and Wallet Node support:
  - Full Nodes can mine blocks and validate chain integrity
  - Wallet Nodes handle transaction creation and balance tracking
- Mining with configurable difficulty and reward halving every 30 days (like Bitcoin)
- Persistent data storage using LevelDB for blockchain and wallets
- Peer-to-peer networking using Boost.Asio
- Mempool for pending transactions
- Longest chain rule validation
- Interactive CLI-based blockchain explorer

### Native Cryptocurrency: $CLST
- Tokenomics built-in with reward schedule
- Real-time balance tracking
- Explorer UI showing top holders and total supply

### Blockchain Explorer UI (React)
- Dashboard with network stats and mining status
- Real-time mempool view
- Wallet interface with transaction history
- Send transactions from wallet
- Peer network management
- Blockchain viewer: blocks, transactions, balances
- Dynamic node switching and multi-node monitoring
- Adjustable mining difficulty and sync status view

---
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

## Backend Requirements

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

## Running the UI Based Application

### Single Instance

To run a single instance of the UI connecting to a default blockchain node (localhost:8080):

```
npm start
```

This will start the application at http://localhost:3000.

### Multiple Instances

The application supports connecting to multiple blockchain nodes simultaneously by running multiple UI instances. This is useful for:

- Monitoring different nodes in your network
- Using both full nodes and wallet nodes
- Testing peer-to-peer communication

#### Using PS Script (Windows)

1. Run the PS script to launch multiple UI instances:

```
.\start-multi-ui
```

This will:
- Open separate terminal windows for each UI instance
- Configure each instance to connect to a different blockchain node API
- Make each UI instance available at a different port

#### Using Node.js Script

1. Run the Node.js script with an optional port parameter:

```
node run-instances.js [port]
```

Without specifying a port, it will run the first configured instance.

For example:
- `node run-instances.js 3000` - Starts a UI instance on port 3000 connecting to the API on port 8080
- `node run-instances.js 3001` - Starts a UI instance on port 3001 connecting to the API on port 8090

#### Manually Configuring Instances

You can also manually configure each instance by:

1. Creating a `.env.{port}` file with the appropriate configuration
2. Updating the `setupProxy.js` file to point to the desired API port
3. Starting the application with specific environment variables:

```
PORT=3001 REACT_APP_API_PORT=8090 npm start
```

## Node Connectivity

The UI allows you to:
- Switch between different nodes within the same UI instance using the node selector in the header
- Add, edit, and remove node connections
- Test connections to verify node availability

By default, the following nodes are configured:
- Local Node 1 (http://localhost:8080/api)
- Local Node 2 (http://localhost:8081/api)
- Local Node 3 (http://localhost:8085/api)
- Local Wallet (http://localhost:8090/api)

## Development

This project was bootstrapped with [Create React App](https://github.com/facebook/create-react-app).

### Available Scripts

- `npm start` - Runs the app in development mode
- `npm test` - Launches the test runner
- `npm run build` - Builds the app for production

