CC = g++

BOOST_PATH = D:\algo\Socket Distributed Programming\boost_1_87_0
MINGW_PREFIX = D:\msys2\mingw64
OPENSSL_LIB_PATH = $(MINGW_PREFIX)/lib
OPENSSL_PATH = $(MINGW_PREFIX)/include
LEVELDB_INCLUDE = $(MINGW_PREFIX)/include
LEVELDB_LIB = $(MINGW_PREFIX)/lib
PATH := $(MINGW_PREFIX)/bin:$(PATH)

# Compiler flags
CFLAGS = -std=c++17 -Wall -I"$(BOOST_PATH)" -I"$(OPENSSL_PATH)" -I"$(LEVELDB_INCLUDE)" -I.

# Library flags
OPENSSL_LIBS = -L"$(OPENSSL_LIB_PATH)" -lssl -lcrypto
LEVELDB_LIBS = -L"$(LEVELDB_LIB)" -lleveldb -lsnappy
WIN_LIBS = -lws2_32 -lmswsock -lwsock32 -lshlwapi -lcrypt32 -lsecur32 -liphlpapi

# Combine all libraries in correct order
LDFLAGS = $(LEVELDB_LIBS) $(OPENSSL_LIBS) $(WIN_LIBS) -static-libgcc -static-libstdc++

TARGET_NODE = blockchain_node

# Source files for the node application
NODE_SRCS = NodeApp.cpp NetworkNode.cpp BlockchainDB.cpp Blockchain.cpp Block.cpp Transaction.cpp wallet.cpp sha.cpp crypto_utils.cpp

# Object files
NODE_OBJS = $(NODE_SRCS:.cpp=.o)

all: $(TARGET_NODE)

$(TARGET_NODE): $(NODE_OBJS)
	$(CC) -o $(TARGET_NODE) $(NODE_OBJS) $(LDFLAGS)

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