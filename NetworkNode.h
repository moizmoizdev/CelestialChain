#ifndef NETWORK_NODE_H
#define NETWORK_NODE_H

// Define this to keep the global placeholders (_1, _2, etc.)
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <vector>
#include <set>
#include <mutex>
#include <thread>
#include "Blockchain.h"
#include "wallet.h"
#include "Types.h"

// Forward declarations
class NetworkManager;
class NetworkMessage;

// Types of messages that can be sent over the network
enum class MessageType {
    HANDSHAKE,        // Initial connection message
    TRANSACTION,      // New transaction
    BLOCK,            // New block mined
    CHAIN_REQUEST,    // Request for the blockchain
    CHAIN_RESPONSE,   // Response with blockchain data
    PEER_LIST,        // List of known peers
    PING,             // Ping message to check if a node is alive
    PONG              // Response to a ping
};

// Class to represent a message sent over the network
class NetworkMessage {
public:
    MessageType type;
    std::string sender;
    std::string data;
    
    NetworkMessage(MessageType type, const std::string& sender, const std::string& data)
        : type(type), sender(sender), data(data) {}
    
    // Serialize the message to a string
    std::string serialize() const;
    
    // Deserialize a string to a NetworkMessage
    static NetworkMessage deserialize(const std::string& serialized);
};

// Represents a peer in the network
struct Peer {
    std::string address;
    int port;
    NodeType type;
    std::string id;
    
    Peer(const std::string& address, int port, NodeType type, const std::string& id)
        : address(address), port(port), type(type), id(id) {}
    
    bool operator==(const Peer& other) const {
        return address == other.address && port == other.port;
    }
    
    bool operator<(const Peer& other) const {
        if (address != other.address) {
            return address < other.address;
        }
        return port < other.port;
    }
};

// Class to handle a connection to a peer
class Connection : public boost::enable_shared_from_this<Connection> {
public:
    typedef boost::shared_ptr<Connection> pointer;
    
    static pointer create(boost::asio::io_context& io_context, NetworkManager* manager) {
        return pointer(new Connection(io_context, manager));
    }
    
    boost::asio::ip::tcp::socket& socket() {
        return socket_;
    }
    
    void start();
    void send(const NetworkMessage& message);
    
private:
    Connection(boost::asio::io_context& io_context, NetworkManager* manager);
    
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    
    boost::asio::ip::tcp::socket socket_;
    NetworkManager* manager_;
    boost::array<char, 1024> buffer_;
    std::string message_buffer_;
};

// Class to manage the network functionality
class NetworkManager {
public:
    // Constructor now takes a reference to an external wallet
    NetworkManager(Blockchain& blockchain, Wallet& wallet, const std::string& host, int port, NodeType type);
    ~NetworkManager();
    
    // Start the network services
    void start();
    
    // Stop the network services
    void stop();
    
    // Connect to a peer
    bool connectToPeer(const std::string& address, int port);
    
    // Broadcast a transaction to all peers
    void broadcastTransaction(const Transaction& transaction);
    
    // Broadcast a newly mined block to all peers
    void broadcastBlock(const Block& block);
    
    // Request the blockchain from peers
    void requestBlockchain();
    
    // Handle an incoming message
    void handleMessage(Connection::pointer connection, const NetworkMessage& message);
    
    // Get node type
    NodeType getNodeType() const {
        return nodeType;
    }
    
    // Set node type
    void setNodeType(NodeType type) {
        nodeType = type;
    }
    
    // Get the blockchain
    Blockchain& getBlockchain() {
        return blockchain;
    }
    
    // Get the wallet
    Wallet& getWallet() {
        return wallet;
    }
    
    // Get the list of connected peers
    std::vector<Peer> getConnectedPeers() const;
    
private:
    // Start accepting incoming connections
    void startAccept();
    
    // Handle a new connection
    void handleAccept(Connection::pointer new_connection, const boost::system::error_code& error);
    
    // Handle a peer connection
    void handlePeerConnection(Connection::pointer connection);
    
    Blockchain& blockchain;
    Wallet& wallet;          // Reference to an external wallet
    NodeType nodeType;
    std::string host;
    int port;
    std::string nodeId;
    
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    
    std::set<Peer> peers;
    std::vector<Connection::pointer> connections;
    
    mutable std::mutex peers_mutex;
    mutable std::mutex connections_mutex;
    
    bool running;
    std::thread service_thread;
};

#endif // NETWORK_NODE_H 