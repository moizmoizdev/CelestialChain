#!/bin/bash
echo "Starting Blockchain Network Demo"

# Start a full node on port 8000
gnome-terminal --title="Full Node" -- ./blockchain_node --type full --port 8000 &

# Wait a moment for the first node to start
sleep 3

# Start a wallet node on port 8001
gnome-terminal --title="Wallet Node" -- ./blockchain_node --type wallet --port 8001 &

echo "Started network nodes."
echo "Use --host 127.0.0.1 --port 8000 to connect the wallet node to the full node." 