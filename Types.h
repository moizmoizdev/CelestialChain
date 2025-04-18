#ifndef TYPES_H
#define TYPES_H

// Types of network nodes
enum class NodeType {
    WALLET_NODE,  // Simple wallet node (no mining)
    FULL_NODE     // Full node with mining capabilities
};

#endif // TYPES_H 