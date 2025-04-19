@echo off
echo Starting Blockchain Demo

REM Check if the executable exists
if not exist "blockchain_demo.exe" (
    echo Error: blockchain_demo.exe not found.
    echo Please compile the project first with: g++ -std=c++11 -Wall main.cpp Blockchain.cpp Block.cpp Transaction.cpp wallet.cpp sha.cpp -o blockchain_demo
    exit /b 1
)

REM 
start "Blockchain Demo 1" cmd /k "blockchain_node.exe --type full --port 8000"

REM 
start "Blockchain Demo 2" cmd /k "blockchain_node.exe --type full --port 8001"

REM 
start "Blockchain Demo 3" cmd /k "blockchain_node.exe --type full --port 8005"

REM 
start "Blockchain Demo 4" cmd /k "blockchain_node.exe --type full --port 8010"

echo Started two blockchain demos.
echo You can interact with each separately. 