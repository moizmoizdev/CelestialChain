cmake_minimum_required(VERSION 3.16)
project(CelestialChainAPI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Boost in header-only mode
add_definitions(-DBOOST_ALL_NO_LIB)
add_definitions(-DBOOST_SYSTEM_NO_LIB)
add_definitions(-DBOOST_THREAD_NO_LIB)
add_definitions(-DBOOST_ASIO_HEADER_ONLY)

# Define the boost include path explicitly
set(BOOST_PATH "D:/algo/Socket Distributed Programming/boost_1_87_0")

# Find OpenSSL
set(OPENSSL_ROOT_DIR "D:/Distributed BlockChain/vcpkg/installed/x64-windows")
find_package(OpenSSL REQUIRED)

# Find or add LevelDB
# If you have LevelDB in a custom location, set LEVELDB_ROOT
set(LEVELDB_ROOT "D:/msys2/mingw64" CACHE PATH "Path to LevelDB installation")
set(LEVELDB_INCLUDE_DIR "${LEVELDB_ROOT}/include")
set(LEVELDB_LIBRARY_DIR "${LEVELDB_ROOT}/lib")

# Set Crow path - use the one from vcpkg
set(CROW_INCLUDE_DIR "D:/Distributed BlockChain/vcpkg/installed/x64-windows/include")

# API source files
set(API_SOURCES
    CelestialChainAPI.cpp
)

# Create a static library for the API component
add_library(CelestialChainAPI STATIC ${API_SOURCES})

# Include directories
target_include_directories(CelestialChainAPI PUBLIC
    ${BOOST_PATH}
    ${OPENSSL_INCLUDE_DIR}
    ${LEVELDB_INCLUDE_DIR}
    ${CROW_INCLUDE_DIR}
    ".."  # Include parent directory for project headers
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Add Crow compilation flags and definitions
target_compile_definitions(CelestialChainAPI PRIVATE
    CROW_ENABLE_SSL
    CROW_ROUTE_ENABLE_SSL
    CROW_JSON_NO_ERROR_CHECK
)

# Link libraries - don't link Boost as we're using header-only
target_link_libraries(CelestialChainAPI PRIVATE
    ${OPENSSL_LIBRARIES}
    "${LEVELDB_LIBRARY_DIR}/libleveldb.a"
    ws2_32
    mswsock
    wsock32
)

# If using MinGW, add pthread
if(MINGW)
    target_link_libraries(CelestialChainAPI PRIVATE
        pthread
    )
endif()

# Install target
install(TARGETS CelestialChainAPI
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
) 