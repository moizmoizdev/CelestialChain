@echo off
echo Testing Blockchain Database Persistence

REM Check if the executable exists
if not exist "blockchain_node.exe" (
    echo Error: blockchain_node.exe not found.
    echo Please compile the project first with: make
    exit /b 1
)

echo This script will help you test blockchain database persistence.
echo.
echo Test steps:
echo 1. Start a node with a clean blockchain
echo 2. Mine several blocks and create transactions
echo 3. Exit the node
echo 4. Restart the node without the clean flag
echo 5. Verify that your blockchain state is preserved
echo.
echo Press any key to start a node with a clean blockchain...
pause > nul

REM Start with a clean blockchain
echo Starting node on port 8888 (Clean start)...
start "Blockchain Node 8888 (Clean Start)" cmd /k "blockchain_node.exe --type full --port 8888 --host 127.0.0.1 --clean"

echo.
echo Instructions:
echo 1. Mine several blocks (option 3)
echo 2. Create some transactions (option 4)
echo 3. View the blockchain (option 1) and take note of its state
echo 4. Exit the node (option 0)
echo 5. Then press any key to continue with the test...
pause > nul

echo Starting node on port 8888 again (Should load existing data)...
start "Blockchain Node 8888 (Persistence Test)" cmd /k "blockchain_node.exe --type full --port 8888 --host 127.0.0.1"

echo.
echo Verify that the blockchain has loaded with your previous state.
echo The blockchain should contain all the blocks you mined previously.
echo.
echo Test completed. Press Ctrl+C to exit the node. 