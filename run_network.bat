@echo off
echo Starting Blockchain Demo Network

REM Check if the executable exists
if not exist "blockchain_node.exe" (
    echo Error: blockchain_node.exe not found.
    echo Please compile the project first with: make
    exit /b 1
)

REM Check if we should start with clean databases
set CLEAN_FLAG=
if "%1"=="--clean" set CLEAN_FLAG=--clean
if "%1"=="--fresh" set CLEAN_FLAG=--clean

if defined CLEAN_FLAG (
    echo NOTICE: Starting with fresh blockchain data (ignoring existing database)
) else (
    echo Using existing blockchain data if available
)

echo.
echo Starting node on port 8000 (Full Node) with API on port 8080...
start "Blockchain Node 8000" cmd /k "blockchain_node.exe --host 127.0.0.1 --port 8000 --type full --api-port 8080 %CLEAN_FLAG%"

REM Wait for the first node to start up
timeout /t 3 /nobreak

echo Starting node on port 8001 (Full Node) with API on port 8081...
start "Blockchain Node 8001" cmd /k "blockchain_node.exe --host 127.0.0.1 --port 8001 --type full --api-port 8081 %CLEAN_FLAG%"

REM Wait before starting the next node
timeout /t 4 /nobreak

echo Starting node on port 8005 (Full Node) with API on port 8085...
start "Blockchain Node 8005" cmd /k "blockchain_node.exe --host 127.0.0.1 --port 8005 --type full --api-port 8085 %CLEAN_FLAG%"

REM Wait before starting the next node
timeout /t 5 /nobreak

echo Starting node on port 8010 (Wallet Node) with API on port 8090...
start "Blockchain Node 8010" cmd /k "blockchain_node.exe --host 127.0.0.1 --port 8010 --type wallet --api-port 8090 %CLEAN_FLAG%"

echo.
echo All blockchain nodes have been started.
echo You can interact with each separately in their own console window.
echo Each node has a different port (8000, 8001, 8005, 8010).
echo Each node has a corresponding API port (8080, 8081, 8085, 8090).
echo.
echo Press Ctrl+C in any window to exit that node.
echo.
echo Note: Run with 'run_network.bat --clean' to start with fresh databases. 