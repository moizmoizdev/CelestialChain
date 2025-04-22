@echo off
echo Testing Blockchain Longest Chain Algorithm with Different Difficulties

REM Check if the executable exists
if not exist "blockchain_node.exe" (
    echo Error: blockchain_node.exe not found.
    echo Please compile the project first with: make
    exit /b 1
)

echo This script will start two separate blockchain nodes with different difficulties.
echo You'll need to manually mine blocks on each node and then connect them to test work-based chain selection.
echo.
echo Instructions:
echo 1. On the high-difficulty node (port 8085), mine 1-2 blocks (option 3)
echo 2. On the low-difficulty node (port 9095), mine 3-5 blocks (option 3)
echo 3. From either node, connect to the other node (option 6)
echo 4. Observe the logs to see which chain is selected based on total work
echo.
echo Press any key to start the test...
pause > nul

REM Clean start for both nodes
echo Starting node on port 8085 (High Difficulty Node - difficulty 6)...
start "Blockchain Node 8085 (High Difficulty)" cmd /k "blockchain_node.exe --type full --port 8085 --host 127.0.0.1 --difficulty 6 --clean"

echo.
echo Starting node on port 9095 (Low Difficulty Node - difficulty 3)...
start "Blockchain Node 9095 (Low Difficulty)" cmd /k "blockchain_node.exe --type full --port 9095 --host 127.0.0.1 --difficulty 3 --clean"

echo.
echo Both test nodes have been started.
echo - Node 1: 127.0.0.1:8085 (High Difficulty Node: 6)
echo - Node 2: 127.0.0.1:9095 (Low Difficulty Node: 3)
echo.
echo Follow the instructions above to test the work-based chain selection algorithm.
echo Remember: The chain with more total work (sum of 2^difficulty for each block) should be selected.
echo Press Ctrl+C in any window to exit that node. 