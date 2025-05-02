cmake_minimum_required(VERSION 3.16)
project(BlockchainDemo LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt paths explicitly
set(QT_ROOT "D:/QT/installation")
set(QT_VERSION_PATH "${QT_ROOT}/6.9.0")
set(QT_COMPILER_PATH "${QT_VERSION_PATH}/mingw_64")
set(Qt6_DIR "${QT_COMPILER_PATH}/lib/cmake/Qt6")

# Add Qt installation to prefix path
list(APPEND CMAKE_PREFIX_PATH "${QT_COMPILER_PATH}")

# Find Qt packages
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Find Boost - use header-only mode if libraries aren't found
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ROOT "D:/algo/Socket Distributed Programming/boost_1_87_0")
set(Boost_INCLUDE_DIR "${BOOST_ROOT}")
# Try to find Boost with components
find_package(Boost COMPONENTS system thread)
# If we couldn't find the compiled libraries, use header-only mode
if(NOT Boost_FOUND)
    message(STATUS "Could not find Boost compiled libraries, using header-only mode")
    find_package(Boost REQUIRED)
    # Define our own variables that would normally be set by find_package
    if(Boost_FOUND)
        set(Boost_LIBRARIES "")
        set(Boost_SYSTEM_LIBRARY "")
        set(Boost_THREAD_LIBRARY "")
        # Define BOOST_ALL_NO_LIB to prevent auto-linking
        add_definitions(-DBOOST_ALL_NO_LIB)
        # Define these to use header-only version
        add_definitions(-DBOOST_SYSTEM_NO_LIB)
        add_definitions(-DBOOST_THREAD_NO_LIB)
        add_definitions(-DBOOST_ASIO_HEADER_ONLY)
    endif()
endif()

# Add OpenSSL with explicit paths
set(OPENSSL_ROOT_DIR "D:/Distributed BlockChain/vcpkg/installed/x64-windows")
set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")
find_package(OpenSSL REQUIRED)

# Find or add LevelDB
# If you have LevelDB in a custom location, set LEVELDB_ROOT
set(LEVELDB_ROOT "D:/msys2/mingw64" CACHE PATH "Path to LevelDB installation")
set(LEVELDB_INCLUDE_DIR "${LEVELDB_ROOT}/include")
set(LEVELDB_LIBRARY_DIR "${LEVELDB_ROOT}/lib")

# Set Crow path
set(CROW_INCLUDE_DIR "D:/Distributed BlockChain/vcpkg/installed/x64-windows/include")

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
)

# Don't include UiController.cpp if it doesn't exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/UiController.cpp")
    list(APPEND SOURCES UiController.cpp)
endif()

# Qt resource files - only include if file exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc")
    set(QML_RESOURCES qml.qrc)
else()
    set(QML_RESOURCES "")
endif()

# Build the API library
add_subdirectory(api)

add_executable(BlockchainDemo
    ${SOURCES}
    ${QML_RESOURCES}
)

# Include directories
target_include_directories(BlockchainDemo PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${LEVELDB_INCLUDE_DIR}
    ${CROW_INCLUDE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Link libraries
target_link_libraries(BlockchainDemo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    ${Boost_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    "${LEVELDB_LIBRARY_DIR}/libleveldb.a"
    ws2_32
    mswsock
    wsock32
    CelestialChainAPI
)

# If using Qt provided by MSYS2, you might need to add this
if(MINGW)
    target_link_libraries(BlockchainDemo PRIVATE
        pthread
    )
endif() 