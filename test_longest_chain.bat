@echo off
echo Testing Blockchain Longest Chain Algorithm

REM Check if the executable exists
if not exist "blockchain_node.exe" (
    echo Error: blockchain_node.exe not found.
    echo Please compile the project first with: make
    exit /b 1
)

echo This script will start two separate blockchain nodes for testing the longest chain algorithm.
echo You'll need to manually mine blocks on each node and then connect them to test chain synchronization.
echo.
echo Instructions:
echo 1. On the first node, mine 3-4 blocks (option 3)
echo 2. On the second node, mine 1-2 blocks (option 3)
echo 3. From either node, connect to the other node (option 6)
echo 4. Observe the logs to see chain synchronization
echo.
echo Press any key to start the test...
pause > nul

REM Clean start for both nodes
echo Starting node on port 8080 (Longer Chain Node)...
start "Blockchain Node 8080" cmd /k "blockchain_node.exe --type full --port 8080 --host 127.0.0.1 --clean"

echo.
echo Starting node on port 9090 (Shorter Chain Node)...
start "Blockchain Node 9090" cmd /k "blockchain_node.exe --type full --port 9090 --host 127.0.0.1 --clean"

echo.
echo Both test nodes have been started.
echo - Node 1: 127.0.0.1:8080 (Longer Chain Node)
echo - Node 2: 127.0.0.1:9090 (Shorter Chain Node)
echo.
echo Follow the instructions above to test the longest chain algorithm.
echo Press Ctrl+C in any window to exit that node.