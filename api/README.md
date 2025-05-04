# CelestialChain API

This is a REST API for the CelestialChain blockchain, built using the Crow C++ framework. It exposes all the functionality available in the command-line interface as HTTP endpoints.

## Overview

The API starts automatically when you run the CelestialChain node application, listening on port 8080 by default. You can configure the port by using the `--api-port` command-line argument.

```
./CelestialChain --api-port 9000
```

## Prerequisites

* C++17 compiler
* Boost libraries (system, thread)
* OpenSSL
* LevelDB
* Crow C++ framework

## Installation

### Install Crow

Using vcpkg (recommended):
```
vcpkg install crow
```

Or manually:
```
git clone https://github.com/CrowCpp/Crow.git
cd Crow
mkdir build && cd build
cmake ..
make
```

### Building the API

On Windows:
```
cd api
build_api.bat
```

On Linux/macOS:
```
cd api
mkdir build
cd build
cmake ..
make
cd ..
```

## API Endpoints

### Blockchain Operations

- **GET /api/blockchain** - View the entire blockchain
- **GET /api/mempool** - View transactions in the mempool
- **POST /api/mine** - Mine a new block
- **POST /api/transaction** - Create a new transaction
- **GET /api/wallet** - View wallet details
- **POST /api/peers/connect** - Connect to a peer
- **POST /api/blockchain/sync** - Request blockchain from peers
- **GET /api/peers** - View connected peers
- **GET /api/statistics** - View blockchain statistics
- **GET /api/explorer/block/:id** - View details of a specific block
- **GET /api/explorer/address/:address** - View details of a specific address
- **GET /api/explorer/transaction/:hash** - View details of a specific transaction
- **GET /api/difficulty** - Get current mining difficulty
- **POST /api/difficulty** - Change mining difficulty (full nodes only)

## Example Requests

### Create a transaction

```
POST /api/transaction
Content-Type: application/json

{
  "receiver": "0x4a9a6c48e9a1a9a5a1d5c1e8e7f9c1a2b3d4e5f6",
  "amount": 10.5
}
```

### Mine a block

```
POST /api/mine
```

### Connect to a peer

```
POST /api/peers/connect
Content-Type: application/json

{
  "address": "127.0.0.1",
  "port": 8001
}
```

## Response Format

Most responses are in JSON format. Example:

```json
{
  "address": "0x4a9a6c48e9a1a9a5a1d5c1e8e7f9c1a2b3d4e5f6",
  "balance": 125.75
}
```

## Error Handling

Error responses include a status code and an error message:

```json
{
  "error": "Transaction failed: Insufficient balance"
}
```

## Notes

- The API runs in a separate thread within the main application.
- CORS is enabled, allowing requests from any origin.
- You can use this API to build web interfaces or mobile apps that interact with your blockchain node. 