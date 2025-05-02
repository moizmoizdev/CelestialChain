@echo off
echo ===== Celestial Chain Blockchain Build Script =====
echo.

REM Set paths
set BOOST_PATH=D:\algo\Socket Distributed Programming\boost_1_87_0
set OPENSSL_PATH=D:\Distributed BlockChain\vcpkg\installed\x64-windows
set LEVELDB_PATH=D:\msys2\mingw64
set CROW_PATH=D:\Distributed BlockChain\vcpkg\installed\x64-windows\include

REM Check paths exist
if not exist "%BOOST_PATH%" (
    echo ERROR: Boost path not found at %BOOST_PATH%
    echo Please update the BOOST_PATH variable in this script
    exit /b 1
)

echo Building CelestialChain API component...
cd api
if exist build rmdir /s /q build
mkdir build
cd build

REM Run CMake for API component
cmake .. -G "MinGW Makefiles" ^
    -DBOOST_PATH="%BOOST_PATH%" ^
    -DOPENSSL_ROOT_DIR="%OPENSSL_PATH%" ^
    -DLEVELDB_ROOT="%LEVELDB_PATH%" ^
    -DCROW_INCLUDE_DIR="%CROW_PATH%"

REM Build the API library
cmake --build . --config Release

cd ..\..
echo API component built successfully!
echo.

echo Building main blockchain application...
echo.

REM Compile the main application with g++
g++ -std=c++17 -Wall ^
    -I"%BOOST_PATH%" ^
    -I"%OPENSSL_PATH%\include" ^
    -I"%LEVELDB_PATH%\include" ^
    -I"%CROW_PATH%" ^
    NodeApp.cpp ^
    NetworkNode.cpp ^
    Blockchain.cpp ^
    Block.cpp ^
    Transaction.cpp ^
    wallet.cpp ^
    sha.cpp ^
    crypto_utils.cpp ^
    BlockchainDB.cpp ^
    balanceMapping.cpp ^
    explorer.cpp ^
    api\CelestialChainAPI.cpp ^
    -L"%OPENSSL_PATH%\lib" ^
    -L"%LEVELDB_PATH%\lib" ^
    -lpthread -lws2_32 -lmswsock -lwsock32 -lcrypto ^
    "%LEVELDB_PATH%\lib\libleveldb.a" ^
    -o blockchain_node.exe

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed!
    exit /b 1
)

echo.
echo ===== Build completed successfully! =====
echo.
echo To run the blockchain node with API:
echo   blockchain_node.exe --host 127.0.0.1 --port 8000 --type full --api-port 8080
echo.
echo The API will be available at http://localhost:8080/api/ 