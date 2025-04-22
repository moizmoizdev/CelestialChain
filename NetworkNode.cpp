#include "NetworkNode.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <sstream>
#include <random>
#include <stdexcept>
#include <cmath>  

// NetworkMessage implementation
std::string NetworkMessage::serialize() const {
    std::stringstream ss;
    ss << static_cast<int>(type) << "|" << sender << "|" << data;
    return ss.str();
}

NetworkMessage NetworkMessage::deserialize(const std::string& serialized) {
    std::vector<std::string> parts;
    boost::split(parts, serialized, boost::is_any_of("|"));
    
    if (parts.size() < 3) {
        throw std::runtime_error("Invalid message format");
    }
    
    MessageType type = static_cast<MessageType>(std::stoi(parts[0]));
    std::string sender = parts[1];
    
    // Combine the rest of the parts as data (in case data contains the delimiter)
    std::string data;
    for (size_t i = 2; i < parts.size(); ++i) {
        if (i > 2) data += "|";
        data += parts[i];
    }
    
    return NetworkMessage(type, sender, data);
}

// Connection implementation
Connection::Connection(boost::asio::io_context& io_context, NetworkManager* manager)
    : socket_(io_context), manager_(manager) {
}

void Connection::start() {
    boost::asio::async_read(
        socket_,
        boost::asio::buffer(buffer_),
        boost::asio::transfer_at_least(1),
        boost::bind(
            &Connection::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

void Connection::send(const NetworkMessage& message) {
    std::string serialized = message.serialize() + "\n"; // Add newline as message delimiter
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(serialized),
        boost::bind(
            &Connection::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error
        )
    );
}

void Connection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        message_buffer_.append(buffer_.data(), bytes_transferred);
        
        // Check if we have a complete message (terminated by newline)
        size_t pos = message_buffer_.find('\n');
        while (pos != std::string::npos) {
            std::string message_str = message_buffer_.substr(0, pos);
            message_buffer_.erase(0, pos + 1);
            
            try {
                NetworkMessage message = NetworkMessage::deserialize(message_str);
                manager_->handleMessage(shared_from_this(), message);
            } catch (const std::exception& e) {
                std::cerr << "Error parsing message: " << e.what() << std::endl;
            }
            
            pos = message_buffer_.find('\n');
        }
        
        // Continue reading
        boost::asio::async_read(
            socket_,
            boost::asio::buffer(buffer_),
            boost::asio::transfer_at_least(1),
            boost::bind(
                &Connection::handle_read,
                shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    } else {
        // Connection closed or error occurred
        if (error != boost::asio::error::eof) {
            std::cerr << "Error: " << error.message() << std::endl;
        }
    }
}

void Connection::handle_write(const boost::system::error_code& error) {
    if (error) {
        std::cerr << "Error writing to socket: " << error.message() << std::endl;
    }
}

// NetworkManager implementation
NetworkManager::NetworkManager(Blockchain& blockchain, Wallet& wallet, const std::string& host, int port, NodeType type)
    : blockchain(blockchain),
      wallet(wallet),
      nodeType(type),
      host(host),
      port(port),
      acceptor(io_context, boost::asio::ip::tcp::endpoint(
          boost::asio::ip::make_address(host), port)),
      running(false) {
    
    // Generate a unique node ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1000, 9999);
    nodeId = "Node_" + std::to_string(dis(gen)) + "_" + std::to_string(time(nullptr));
    
    std::cout << "Node ID: " << nodeId << "   Type: "
              << (nodeType==NodeType::FULL_NODE?"Full":"Wallet")
              << " @ " << host << ":" << port << std::endl;
    std::cout << "Node wallet address: " << wallet.getAddress() << std::endl;
}

NetworkManager::~NetworkManager() {
    stop();
}

void NetworkManager::start() {
    if (running) return;
    
    running = true;
    
    // Start accepting incoming connections
    startAccept();
    
    // Start the io_context in a separate thread
    service_thread = std::thread([this]() {
        try {
            io_context.run();
        } catch (const std::exception& e) {
            std::cerr << "IO error: "<<e.what()<<std::endl;
        }
    });
    
    std::cout << "Network services started." << std::endl;
}

void NetworkManager::stop() {
    if (!running) return;
    
    std::cout << "Shutting down network manager..." << std::endl;
    
    running = false;
    
    // Close all connections first
    {
        std::lock_guard<std::mutex> lock(connections_mutex);
        std::cout << "Closing " << connections.size() << " peer connections..." << std::endl;
        // Close each connection socket
        for (auto& connection : connections) {
            try {
                boost::system::error_code ec;
                connection->socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                if (ec) {
                    std::cerr << "Error shutting down socket: " << ec.message() << std::endl;
                }
                connection->socket().close(ec);
                if (ec) {
                    std::cerr << "Error closing socket: " << ec.message() << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error during socket shutdown: " << e.what() << std::endl;
            }
        }
        connections.clear();
    }
    
    // Try to cancel any pending operations
    try {
        acceptor.close();
    } catch (const std::exception& e) {
        std::cerr << "Error closing acceptor: " << e.what() << std::endl;
    }
    
    // Stop the io_context
    try {
        io_context.stop();
    } catch (const std::exception& e) {
        std::cerr << "Error stopping IO context: " << e.what() << std::endl;
    }
    
    // Wait for the service thread to finish
    if (service_thread.joinable()) {
        std::cout << "Waiting for network thread to finish..." << std::endl;
        service_thread.join();
    }
    
    // Clear peers
    {
        std::lock_guard<std::mutex> lock(peers_mutex);
        peers.clear();
    }
    
    std::cout << "Network services stopped successfully." << std::endl;
}

void NetworkManager::startAccept() {
    Connection::pointer new_connection = Connection::create(io_context, this);
    
    acceptor.async_accept(
        new_connection->socket(),
        boost::bind(
            &NetworkManager::handleAccept,
            this,
            new_connection,
            boost::asio::placeholders::error
        )
    );
}

void NetworkManager::handleAccept(Connection::pointer new_connection, const boost::system::error_code& error) {
    if (!error) {
        // Successfully accepted a new connection
        std::cout << "Accepted new connection from " 
                  << new_connection->socket().remote_endpoint().address().to_string()
                  << ":" << new_connection->socket().remote_endpoint().port() << std::endl;
        
        // Start the connection
        new_connection->start();
        
        {
            std::lock_guard<std::mutex> lock(connections_mutex);
            connections.push_back(new_connection);
        }
        
        // Send a handshake message
        std::string type_str = (nodeType==NodeType::FULL_NODE?"FULL_NODE":"WALLET_NODE");
        std::string payload  = type_str + "|" + std::to_string(port);
        NetworkMessage hs(MessageType::HANDSHAKE, nodeId, payload);
        new_connection->send(hs);
    }
    
    // Continue accepting new connections
    startAccept();
}

bool NetworkManager::connectToPeer(const std::string& address, int peer_port) {
    try {
        // Check if we're already connected to this peer
        {
            std::lock_guard<std::mutex> lock(peers_mutex);
            for (const auto& peer : peers) {
                if (peer.address == address && peer.port == peer_port) {
                    std::cout << "Already connected to peer at " << address << ":" << peer_port << std::endl;
                    return true;
                }
            }
        }

        std::cout << "Attempting to connect to peer at " << address << ":" << peer_port << "..." << std::endl;
        
        boost::asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(address, std::to_string(peer_port));
        
        // Create a new connection
        Connection::pointer connection = Connection::create(io_context, this);
        
        // Connect to the peer
        boost::asio::connect(connection->socket(), endpoints);
        
        std::cout << "Connected to peer at " << address << ":" << peer_port << std::endl;
        
        // Start the connection
        connection->start();
        
        {
            std::lock_guard<std::mutex> lock(connections_mutex);
            connections.push_back(connection);
            std::cout << "Added peer to connections list. Total connections: " << connections.size() << std::endl;
        }
        
        // Send a handshake message
        std::string type_str = (nodeType==NodeType::FULL_NODE?"FULL_NODE":"WALLET_NODE");
        std::string payload  = type_str + "|" + std::to_string(port);
        NetworkMessage hs(MessageType::HANDSHAKE, nodeId, payload);
        connection->send(hs);
        std::cout << "Sent handshake message to peer" << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error connecting to peer at " << address << ":" << peer_port << ": " << e.what() << std::endl;
        return false;
    }
}

void NetworkManager::broadcastTransaction(const Transaction& transaction) {
    // Check if there are any connections
    std::lock_guard<std::mutex> lock(connections_mutex);
    if (connections.empty()) {
        std::cout << "No peers connected. Transaction will only be stored locally." << std::endl;
        return;
    }
    
    // Convert the transaction to a serialized string
    std::stringstream ss;
    ss << transaction.sender << "|"
       << transaction.senderPublicKey << "|"
       << transaction.receiver << "|"
       << transaction.amount << "|"
       << transaction.timestamp << "|"
       << transaction.hash << "|"
       << transaction.signature;
    
    std::string tx_data = ss.str();
    
    // Create a network message
    NetworkMessage msg(MessageType::TRANSACTION, nodeId, tx_data);
    
    // Broadcast to all connections
    for (auto& connection : connections) {
        connection->send(msg);
    }
    
    std::cout << "Broadcasted transaction to " << connections.size() << " peers." << std::endl;
}

void NetworkManager::broadcastBlock(const Block& block) {
    // Only full nodes should broadcast blocks
    if (nodeType != NodeType::FULL_NODE) {
        std::cerr << "Warning: Wallet nodes should not broadcast blocks." << std::endl;
        return;
    }
    
    // Check if there are any connections
    std::lock_guard<std::mutex> lock(connections_mutex);
    if (connections.empty()) {
        std::cout << "No peers connected. Block will only be stored locally." << std::endl;
        return;
    }
    
    // Serialize the block
    std::stringstream ss;
    ss << block.blockNumber << "|"
       << block.timestamp << "|"
       << block.previousHash << "|"
       << block.hash << "|"
       << block.nonce << "|"
       << block.difficulty << "|"
       << block.transactions.size();
    
    // Add each transaction to the serialized data
    for (const auto& tx : block.transactions) {
        ss << "|" << tx.sender << "|"
           << tx.senderPublicKey << "|"
           << tx.receiver << "|"
           << tx.amount << "|"
           << tx.timestamp << "|"
           << tx.hash << "|"
           << tx.signature;
    }
    
    std::string block_data = ss.str();
    
    // Create a network message
    NetworkMessage msg(MessageType::BLOCK, nodeId, block_data);
    
    // Broadcast to all connections
    for (auto& connection : connections) {
        connection->send(msg);
    }
    
    std::cout << "Broadcasted block #" << block.blockNumber << " to " << connections.size() << " peers." << std::endl;
}

void NetworkManager::requestBlockchain() {
    // Check if there are any connections
    std::lock_guard<std::mutex> lock(connections_mutex);
    if (connections.empty()) {
        std::cout << "No peers connected. Cannot request blockchain." << std::endl;
        return;
    }
    
    // Create a request message
    NetworkMessage msg(MessageType::CHAIN_REQUEST, nodeId, "");
    
    // Send to all connections
    for (auto& connection : connections) {
        connection->send(msg);
    }
    
    std::cout << "Requested blockchain from " << connections.size() << " peers." << std::endl;
}

void NetworkManager::handleMessage(Connection::pointer connection, const NetworkMessage& message) {
    std::cout << "Received message of type " << static_cast<int>(message.type) << " from " << message.sender << std::endl;
    
    switch (message.type) {
        case MessageType::HANDSHAKE: {
            // Add the peer to our peer list
            // split the payload
            std::vector<std::string> parts;
            boost::split(parts, message.data, boost::is_any_of("|"));

            // parts[0] == "FULL_NODE" or "WALLET_NODE"
            // parts[1] == listening port
            NodeType peer_type = (parts[0] == "FULL_NODE") ? NodeType::FULL_NODE : NodeType::WALLET_NODE;
            int peer_listen_port = std::stoi(parts[1]);

            std::string peer_address = connection->socket().remote_endpoint().address().to_string();
            Peer new_peer(peer_address, peer_listen_port, peer_type, message.sender);

            bool peer_already_known = false;
            {
                std::lock_guard<std::mutex> lock(peers_mutex);
                auto it = peers.find(new_peer);
                peer_already_known = (it != peers.end());
                if (!peer_already_known) {
                    peers.insert(new_peer);
                    std::cout << "Added new peer to peers list. Total peers: " << peers.size() << std::endl;
                }
            }
            
            std::cout << "Handshake completed with peer " << message.sender << " at " 
                      << peer_address << ":" << peer_listen_port << " (" 
                      << (peer_type == NodeType::FULL_NODE ? "Full Node" : "Wallet Node") << ")" 
                      << (peer_already_known ? " (already known)" : " (new peer)") << std::endl;
            
            // Send our peer list to the new peer
            std::stringstream ss;
            
            {
                std::lock_guard<std::mutex> lock(peers_mutex);
                ss << peers.size();
                
                for (const auto& peer : peers) {
                    ss << "|" << peer.address << "|" << peer.port << "|"
                       << (peer.type == NodeType::FULL_NODE ? "FULL_NODE" : "WALLET_NODE") << "|"
                       << peer.id;
                }
            }
            
            NetworkMessage peer_list_msg(MessageType::PEER_LIST, nodeId, ss.str());
            connection->send(peer_list_msg);
            std::cout << "Sent peer list to " << message.sender << " with " << peers.size() << " peers" << std::endl;
            break;
        }
        case MessageType::TRANSACTION: {
            // parse the transaction data
            std::vector<std::string> parts;
            boost::split(parts, message.data, boost::is_any_of("|"));
            if (parts.size() != 7) {
                std::cerr << "Invalid transaction data format" << std::endl;
                break;
            }
        
            // Construct the Transaction object
            // parts[0] = sender, parts[1] = senderPublicKey, parts[2] = receiver, parts[3] = amount
            // parts[4] = timestamp, parts[5] = hash, parts[6] = signature
            Transaction tx(
                parts[0],                    // sender address
                parts[1],                    // sender public key
                parts[2],                    // receiver
                std::stod(parts[3]),         // amount
                parts[5],                    // hash
                parts[6],                    // signature
                std::stoul(parts[4])         // timestamp
            );
        
            // 1) Validate the transaction
            if (!tx.isValid()) {
                std::cerr << "Received invalid transaction from " << message.sender << std::endl;
                break;
            }
        
            // 2) Deduplicate: if we've seen this hash before, do nothing
            {
                const auto& pool = blockchain.getMempool();
                bool seen = std::any_of(pool.begin(), pool.end(),
                    [&](const Transaction& old){ return old.hash == tx.hash; });
                if (seen) {
                    // already processed—drop it
                    break;
                }
            }
        
            // 3) Add it to our mempool
            blockchain.addTransaction(tx);
        
            // 4) Relay it on to all other peers (except the one who sent it)
            {
                std::lock_guard<std::mutex> lock(connections_mutex);
                for (auto& conn : connections) {
                    if (conn != connection) {
                        conn->send(message);
                    }
                }
            }
        
            std::cout << "Added and relayed transaction from "
                      << tx.sender << " to " << tx.receiver
                      << " for " << tx.amount << std::endl;
            break;
        }
        case MessageType::BLOCK: {
            // parse the block data
            std::vector<std::string> parts;
            boost::split(parts, message.data, boost::is_any_of("|"));
            if (parts.size() < 7) {
                std::cerr << "Invalid block data format" << std::endl;
                break;
            }
        
            int blockNumber = std::stoi(parts[0]);
            time_t timestamp = static_cast<time_t>(std::stoul(parts[1]));
            std::string previousHash = parts[2];
            std::string hash         = parts[3];
            int nonce      = std::stoi(parts[4]);
            int difficulty = std::stoi(parts[5]);
            int txCount    = std::stoi(parts[6]);
        
            // Check for enough data for all transactions
            size_t required_size = 7 + static_cast<size_t>(txCount) * 7;
            if (parts.size() < required_size) {
                std::cerr << "Invalid block data: missing transaction data" << std::endl;
                break;
            }
        
            // 1) De‑duplicate: skip if we already have this hash
            {
                const auto& chain = blockchain.getChain();
                auto it = std::find_if(chain.begin(), chain.end(),
                    [&](const Block& b){ return b.hash == hash; });
                if (it != chain.end()) {
                    // already added
                    break;
                }
            }
        
            // 2) Reconstruct the transactions
            std::vector<Transaction> transactions;
            for (int i = 0; i < txCount; ++i) {
                int idx = 7 + i * 7;
                Transaction tx(
                    parts[idx],                   // sender address
                    parts[idx+1],                 // sender public key
                    parts[idx+2],                 // receiver
                    std::stod(parts[idx+3]),      // amount
                    parts[idx+5],                 // hash
                    parts[idx+6],                 // signature
                    std::stoul(parts[idx+4])      // timestamp
                );
                transactions.push_back(tx);
            }
        
            // 3) Create a block stub and set its fields
            Block block(blockNumber, transactions, previousHash, difficulty);
            block.timestamp = timestamp;
            block.nonce     = nonce;
            block.hash      = hash;
        
            // 4) Add to our chain
            try{
                blockchain.addExistingBlock(block);  // or a new addExistingBlock method
            }
            catch(const std::exception& e){
                std::cerr << "Error adding block to chain: " << e.what() << std::endl;
                failedBlockCount++;
                if(failedBlockCount >= 5){
                    try{
                        requestBlockchain();
                        failedBlockCount = 0;
                    }
                    catch(const std::exception& e){
                        std::cerr << "Error requesting blockchain: " << e.what() << std::endl;
                    }
                }
            }
        
            // 5) Relay to other peers, except the sender
            {
                std::lock_guard<std::mutex> lock(connections_mutex);
                for (auto& conn : connections) {
                    if (conn != connection) {
                        conn->send(message);
                    }
                }
            }
        
            std::cout << "Added and relayed block #"
                      << blockNumber << " with "
                      << txCount << " transactions" << std::endl;
            break;
        } 
        case MessageType::CHAIN_REQUEST: {
            // Only full nodes should respond to blockchain requests
            if (nodeType != NodeType::FULL_NODE) {
                std::cerr << "Warning: Wallet node received blockchain request but cannot respond." << std::endl;
                break;
            }
            
            // Serialize the blockchain
            std::stringstream ss;
            const std::vector<Block>& chain = blockchain.getChain();
            ss << chain.size();
            for (const auto& block : chain) {
                ss << "|" << block.blockNumber << "|"
                   << block.timestamp << "|"
                   << block.previousHash << "|"
                   << block.hash << "|"
                   << block.nonce << "|"
                   << block.difficulty << "|"
                   << block.transactions.size();
                
                for (const auto& tx : block.transactions) {
                    ss << "|" << tx.sender << "|"
                       << tx.senderPublicKey << "|"
                       << tx.receiver << "|"
                       << tx.amount << "|"
                       << tx.timestamp << "|"
                       << tx.hash << "|"
                       << tx.signature;
                }
            }
            
            NetworkMessage response(MessageType::CHAIN_RESPONSE, nodeId, ss.str());
            connection->send(response);
            
            std::cout << "Sent blockchain (" << chain.size() << " blocks) to " << message.sender << std::endl;
            break;
        }
        case MessageType::CHAIN_RESPONSE: {
            // Parse the blockchain data
            std::vector<std::string> parts;
            boost::split(parts, message.data, boost::is_any_of("|"));
            
            if (parts.size() < 1) {
                std::cerr << "Invalid blockchain data format" << std::endl;
                break;
            }
            
            int blockCount = std::stoi(parts[0]);
            std::cout << "Received blockchain with " << blockCount << " blocks from " << message.sender << std::endl;
            
            // Implement the longest chain algorithm by:
            // 1. Parse the received blockchain
            // 2. Validate the received chain's integrity
            // 3. Compare with our current chain
            // 4. Replace our chain if the received one is valid and longer
            
            std::vector<Block> receivedChain;
            int currentPos = 1;
            bool chainValid = true;
            
            try {
            for (int i = 0; i < blockCount; i++) {
                if (currentPos + 6 >= parts.size()) {
                    std::cerr << "Invalid blockchain data: missing block data" << std::endl;
                        chainValid = false;
                    break;
                }
                
                int blockNumber = std::stoi(parts[currentPos++]);
                time_t timestamp = std::stoul(parts[currentPos++]);
                std::string previousHash = parts[currentPos++];
                std::string hash = parts[currentPos++];
                int nonce = std::stoi(parts[currentPos++]);
                int difficulty = std::stoi(parts[currentPos++]);
                int txCount = std::stoi(parts[currentPos++]);
                    
                    // Validate block number
                    if (i != blockNumber) {
                        std::cerr << "Block number mismatch: expected " << i << ", got " << blockNumber << std::endl;
                        chainValid = false;
                        break;
                    }
                
                std::vector<Transaction> transactions;
                
                for (int j = 0; j < txCount; j++) {
                    if (currentPos + 6 >= parts.size()) {
                        std::cerr << "Invalid blockchain data: missing transaction data" << std::endl;
                            chainValid = false;
                        break;
                    }
                    
                    std::string sender = parts[currentPos++];
                    std::string senderPublicKey = parts[currentPos++];
                    std::string receiver = parts[currentPos++];
                    double amount = std::stod(parts[currentPos++]);
                    unsigned long txTimestamp = std::stoul(parts[currentPos++]);
                    std::string txHash = parts[currentPos++];
                    std::string signature = parts[currentPos++];
                    
                    Transaction tx(sender, senderPublicKey, receiver, amount, txHash, signature, txTimestamp);
                        
                        // Validate transaction
                        if (!tx.isValid()) {
                            std::cerr << "Invalid transaction in block " << blockNumber << std::endl;
                            chainValid = false;
                            break;
                        }
                        
                    transactions.push_back(tx);
                }
                    
                    if (!chainValid) break;
                
                Block block(blockNumber, transactions, previousHash, difficulty);
                block.timestamp = timestamp;
                block.nonce = nonce;
                block.hash = hash;
                    
                    // Validate block hash
                    std::string calculatedHash = block.calculateHash();
                    if (blockNumber == 0) {
                        // Special handling for genesis block - use the hardcoded hash value
                        if (hash != "0x0000eb99d08f42f3c322b891f18212c85aa05365166964973a56d03e7da36f80") {
                            std::cerr << "Genesis block hash mismatch" << std::endl;
                            std::cerr << "Expected: 0x0000eb99d08f42f3c322b891f18212c85aa05365166964973a56d03e7da36f80" << std::endl;
                            std::cerr << "Got: " << hash << std::endl;
                            chainValid = false;
                            break;
                        }
                    } else if (hash != calculatedHash) {
                        // For non-genesis blocks, do the normal hash verification
                        std::cerr << "Block hash mismatch in block " << blockNumber << std::endl;
                        std::cerr << "Expected: " << calculatedHash << std::endl;
                        std::cerr << "Got: " << hash << std::endl;
                        chainValid = false;
                        break;
                    }
                    
                    // Validate chain links
                    if (i > 0) {
                        if (block.previousHash != receivedChain.back().hash) {
                            std::cerr << "Chain broken at block " << blockNumber << std::endl;
                            std::cerr << "Expected previous hash: " << receivedChain.back().hash << std::endl;
                            std::cerr << "Got: " << block.previousHash << std::endl;
                            chainValid = false;
                            break;
                        }
                    }
                
                receivedChain.push_back(block);
            }
            
                // Implement the longest chain algorithm
                if (chainValid && !receivedChain.empty()) {
                    // Check if the genesis block matches our genesis block
                    const auto& ourChain = blockchain.getChain();
                    
                    if (ourChain.empty()) {
                        std::cerr << "Our chain is empty. Cannot validate against genesis block." << std::endl;
                        break;
                    }
                    
                    if (receivedChain[0].hash != ourChain[0].hash) {
                        std::cerr << "Genesis block mismatch. Different blockchain network." << std::endl;
                        break;
                    }
                    
                    // Calculate total difficulty (work) for both chains
                    // In PoW blockchains, the chain with the most accumulated work is considered valid
                    // Total work is approximated as sum of 2^difficulty for each block
                    double ourTotalWork = 0;
                    double receivedTotalWork = 0;
                    
                    for (const auto& block : ourChain) {
                        // 2^difficulty approximates the amount of work needed
                        ourTotalWork += std::pow(2.0, block.difficulty);
                    }
                    
                for (const auto& block : receivedChain) {
                        receivedTotalWork += std::pow(2.0, block.difficulty);
                    }
                    
                    std::cout << "Our chain work: " << ourTotalWork << ", length: " << ourChain.size() << std::endl;
                    std::cout << "Received chain work: " << receivedTotalWork << ", length: " << receivedChain.size() << std::endl;
                    
                    // Compare chain work - implement the correct chain selection rule
                    if (receivedTotalWork > ourTotalWork) {
                        std::cout << "Received chain has more proof of work (" << receivedTotalWork 
                                  << ") than our chain (" << ourTotalWork << ")" << std::endl;
                        
                        // Clear our chain except genesis block
                        for (size_t i = 1; i < ourChain.size(); i++) {
                            // For any transactions in replaced blocks, add them back to mempool
                            // if they're not already in the new chain
                            for (const auto& tx : ourChain[i].transactions) {
                                bool txInNewChain = false;
                                for (size_t j = 1; j < receivedChain.size(); j++) {
                                    for (const auto& newTx : receivedChain[j].transactions) {
                                        if (tx.hash == newTx.hash) {
                                            txInNewChain = true;
                                            break;
                                        }
                                    }
                                    if (txInNewChain) break;
                                }
                                
                                if (!txInNewChain) {
                                    std::cout << "Re-adding transaction " << tx.hash << " to mempool" << std::endl;
                                    blockchain.addTransaction(tx);
                                }
                            }
                        }
                        
                        // Create a new blockchain with just the genesis block
                        Blockchain newChain(receivedChain[0].difficulty);
                        
                        // Add each block from the received chain (skip genesis which is already added)
                        for (size_t i = 1; i < receivedChain.size(); i++) {
                            try {
                                std::cout << "Adding block #" << i << " to our chain" << std::endl;
                                blockchain.addExistingBlock(receivedChain[i]);
                            } catch (const std::exception& e) {
                                std::cerr << "Error adding block #" << i << ": " << e.what() << std::endl;
                                chainValid = false;
                                break;
                            }
                        }
                        
                        if (chainValid) {
                            std::cout << "Chain replaced successfully with chain having more proof of work" << std::endl;
                        } else {
                            std::cerr << "Failed to replace chain - invalid blocks detected" << std::endl;
                        }
                    } else {
                        std::cout << "Our chain has more or equal proof of work. Keeping our chain." << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing received blockchain: " << e.what() << std::endl;
            }
            
            break;
        }
        
        case MessageType::PEER_LIST: {
            // Parse the peer list
            std::vector<std::string> parts;
            boost::split(parts, message.data, boost::is_any_of("|"));
            
            if (parts.size() < 1) {
                std::cerr << "Invalid peer list format" << std::endl;
                break;
            }
            
            int peerCount = std::stoi(parts[0]);
            std::cout << "Received list of " << peerCount << " peers from " << message.sender << std::endl;
            
            size_t required_size = 1 + static_cast<size_t>(peerCount) * 4;
            if (parts.size() < required_size) {
                std::cerr << "Invalid peer list: missing peer data" << std::endl;
                break;
            }
            
            for (int i = 0; i < peerCount; ++i) {
                int base_idx = 1 + i * 4;
                
                std::string peer_address = parts[base_idx];
                int peer_port = std::stoi(parts[base_idx + 1]);
                NodeType peer_type = (parts[base_idx + 2] == "FULL_NODE") ? NodeType::FULL_NODE : NodeType::WALLET_NODE;
                std::string peer_id = parts[base_idx + 3];
                
                // Don't connect to ourselves
                if (peer_address == host && peer_port == port) {
                    continue;
                }
                
                // Check if we already know this peer
                Peer new_peer(peer_address, peer_port, peer_type, peer_id);
                
                bool peer_exists = false;
                {
                    std::lock_guard<std::mutex> lock(peers_mutex);
                    peer_exists = (peers.find(new_peer) != peers.end());
                    if (!peer_exists) {
                        peers.insert(new_peer);
                    }
                }
                
                // Connect to the new peer if we don't already know it
                if (!peer_exists) {
                    connectToPeer(peer_address, peer_port);
                }
            }
            break;
        }
        
        case MessageType::PING: {
            // Respond with a PONG
            NetworkMessage pong(MessageType::PONG, nodeId, "");
            connection->send(pong);
            break;
        }
        
        case MessageType::PONG: {
            // Just log that we received a pong
            std::cout << "Received PONG from " << message.sender << std::endl;
            break;
        }
        
        default:
            std::cerr << "Unknown message type: " << static_cast<int>(message.type) << std::endl;
            break;
    }
}

std::vector<Peer> NetworkManager::getConnectedPeers() const {
    std::vector<Peer> connectedPeers;
    
    std::lock_guard<std::mutex> lock(peers_mutex);
    for (const auto& peer : peers) {
        connectedPeers.push_back(peer);
    }
    
    return connectedPeers;
} 