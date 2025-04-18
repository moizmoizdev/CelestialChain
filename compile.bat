@echo off
REM Set paths to the libraries
set BOOST_PATH=D:\algo\Socket Distributed Programming\boost_1_87_0
set OPENSSL_PATH=D:\Distributed BlockChain\vcpkg\installed\x64-windows

REM Compile the main blockchain demo
g++ -std=c++11 -Wall -I"%BOOST_PATH%" -I"%OPENSSL_PATH%\include" ^
    main.cpp Blockchain.cpp Block.cpp Transaction.cpp wallet.cpp sha.cpp NetworkNode.cpp crypto_utils.cpp ^
    -o blockchain_demo ^
    -L"%OPENSSL_PATH%\lib" -lws2_32 -lmswsock -lwsock32 -lcrypto -lssl

REM Compile the node application
g++ -std=c++11 -Wall -I"%BOOST_PATH%" -I"%OPENSSL_PATH%\include" ^
    NodeApp.cpp NetworkNode.cpp Blockchain.cpp Block.cpp Transaction.cpp wallet.cpp sha.cpp crypto_utils.cpp ^
    -o blockchain_node ^
    -L"%OPENSSL_PATH%\lib" -lws2_32 -lmswsock -lwsock32 -lcrypto -lssl

echo.
echo Compilation complete! If no errors were shown above, you can run:
echo blockchain_demo.exe - for the main application
echo blockchain_node.exe - for the network node 