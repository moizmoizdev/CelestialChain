CC = g++

BOOST_PATH = D:\algo\Socket Distributed Programming\boost_1_87_0# Add OpenSSL include path
OPENSSL_LIB_PATH = D:\Distributed BlockChain\vcpkg\installed\x64-windows\lib
OPENSSL_PATH = D:\Distributed BlockChain\vcpkg\installed\x64-windows\include
# Add LevelDB paths from MinGW installation
LEVELDB_PATH = D:\msys2\mingw64\include
LEVELDB_LIB_PATH = D:\msys2\mingw64\lib
LEVELDB_STATIC = D:\msys2\mingw64\lib\libleveldb.a

CFLAGS = -std=c++17 -Wall -I"$(BOOST_PATH)" -I"$(OPENSSL_PATH)" -I"$(LEVELDB_PATH)"
# If you have built Boost libraries, uncomment and update this
# LDFLAGS = -L"$(BOOST_PATH)/stage/lib" -lboost_system -lboost_thread -pthread
# If you haven't built Boost libraries yet, use header-only mode with Windows socket libraries
LDFLAGS = -L"$(OPENSSL_LIB_PATH)" -L"$(LEVELDB_LIB_PATH)" -pthread -lws2_32 -lmswsock -lwsock32 -lcrypto
# TARGET = blockchain_demo
TARGET_NODE = blockchain_node

# Source files for the node application
NODE_SRCS = NodeApp.cpp NetworkNode.cpp Blockchain.cpp Block.cpp Transaction.cpp wallet.cpp sha.cpp crypto_utils.cpp BlockchainDB.cpp balanceMapping.cpp explorer.cpp

# Object files
NODE_OBJS = $(NODE_SRCS:.cpp=.o)

all: $(TARGET_NODE)

$(TARGET_NODE): $(NODE_OBJS)
	$(CC) -o $(TARGET_NODE) $(NODE_OBJS) $(LDFLAGS) $(LEVELDB_STATIC)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(NODE_OBJS) $(TARGET_NODE).exe

run-node: $(TARGET_NODE)
	./$(TARGET_NODE)

run-full-node: $(TARGET_NODE)
	./$(TARGET_NODE) --type full --port 8000

run-wallet-node: $(TARGET_NODE)
	./$(TARGET_NODE) --type wallet --port 8001

.PHONY: all clean run-node run-full-node run-wallet-node