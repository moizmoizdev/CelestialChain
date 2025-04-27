cmake_minimum_required(VERSION 3.14)
project(BlockchainDemo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(QT NAMES Qt6 COMPONENTS Gui Qml Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui Qml Quick REQUIRED)

# Find Boost
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS system thread)

# Add OpenSSL
find_package(OpenSSL REQUIRED)

# Find or add LevelDB
# If you have LevelDB in a custom location, set LEVELDB_ROOT
set(LEVELDB_ROOT "D:/msys2/mingw64" CACHE PATH "Path to LevelDB installation")
set(LEVELDB_INCLUDE_DIR "${LEVELDB_ROOT}/include")
set(LEVELDB_LIBRARY_DIR "${LEVELDB_ROOT}/lib")

# Source files
set(SOURCES
    NodeApp.cpp
    NetworkNode.cpp
    Blockchain.cpp
    Block.cpp
    Transaction.cpp
    wallet.cpp
    sha.cpp
    crypto_utils.cpp
    BlockchainDB.cpp
    balanceMapping.cpp
    explorer.cpp
    UiController.cpp
)

# Qt resource files
set(QML_RESOURCES
    qml.qrc
)

add_executable(BlockchainDemo
    ${SOURCES}
    ${QML_RESOURCES}
)

# Include directories
target_include_directories(BlockchainDemo PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${LEVELDB_INCLUDE_DIR}
    "D:/Distributed BlockChain"  # Add your project root directory
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link libraries
target_link_libraries(BlockchainDemo PRIVATE
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    "${LEVELDB_LIBRARY_DIR}/libleveldb.a"
    ws2_32
    mswsock
    wsock32
)

# If using Qt provided by MSYS2, you might need to add this
if(MINGW)
    target_link_libraries(BlockchainDemo PRIVATE
        pthread
    )
endif() 