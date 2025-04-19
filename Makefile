CC = g++
# Update this path to where your boost_1_87_0 folder is located
BOOST_PATH = D:\Apps\boost_1_88_0
# Add OpenSSL include path
OPENSSL_PATH = D:\Apps\OpenSSL\include
OPENSSL_LIB_PATH = D:\Apps\OpenSSL\lib

CFLAGS = -std=c++11 -Wall -I"$(BOOST_PATH)" -I"$(OPENSSL_PATH)"
# If you have built Boost libraries, uncomment and update this
# LDFLAGS = -L"$(BOOST_PATH)/stage/lib" -lboost_system -lboost_thread -pthread
# If you haven't built Boost libraries yet, use header-only mode with Windows socket libraries
LDFLAGS = -L"$(OPENSSL_LIB_PATH)" -pthread -lws2_32 -lmswsock -lwsock32 -lcrypto
TARGET = blockchain_demo
TARGET_NODE = blockchain_node

# Source files for the demo application
SRCS = main.cpp Blockchain.cpp Block.cpp Transaction.cpp wallet.cpp sha.cpp NetworkNode.cpp crypto_utils.cpp

# Source files for the node application
NODE_SRCS = NodeApp.cpp NetworkNode.cpp Blockchain.cpp Block.cpp Transaction.cpp wallet.cpp sha.cpp crypto_utils.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)
NODE_OBJS = $(NODE_SRCS:.cpp=.o)

all: $(TARGET) $(TARGET_NODE)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(TARGET_NODE): $(NODE_OBJS)
	$(CC) -o $(TARGET_NODE) $(NODE_OBJS) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del $(OBJS) $(NODE_OBJS) $(TARGET).exe $(TARGET_NODE).exe

run: $(TARGET)
	./$(TARGET)

run-node: $(TARGET_NODE)
	./$(TARGET_NODE)

run-full-node: $(TARGET_NODE)
	./$(TARGET_NODE) --type full --port 8000

run-wallet-node: $(TARGET_NODE)
	./$(TARGET_NODE) --type wallet --port 8001

.PHONY: all clean run run-node run-full-node run-wallet-node 