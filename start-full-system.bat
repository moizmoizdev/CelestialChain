@echo off
echo ===== BLOCKCHAIN SYSTEM STARTER =====
echo This script will start the complete blockchain system (nodes and UIs)
echo.

@REM REM Step 1: Kill any existing UI processes
@REM echo Step 1: Killing any existing React development servers...
@REM cd web\blockchain-ui
@REM call kill-ui-servers.bat > nul
@REM cd ..\..
@REM echo UI processes killed.

REM Step 2: Start the blockchain nodes
echo.
echo Step 2: Starting blockchain nodes...
echo Running all nodes with clean data...

REM Start the nodes with the run_network.bat script
call run_network.bat --clean

REM Wait for nodes to initialize
echo Waiting 10 seconds for nodes to initialize...
timeout /t 10 /nobreak > nul

REM Step 3: Start the UI instances
echo.
echo Step 3: Starting UI instances...
cd web\blockchain-ui
call restart-multi-ui.bat

echo.
echo ===== COMPLETE SYSTEM STARTED =====
echo To access the system:
echo - Node 1 (Full): http://localhost:3000 (API: 8080)
echo - Node 2 (Wallet): http://localhost:3001 (API: 8090)
echo - Node 3 (Full): http://localhost:3002 (API: 8081)
echo.
echo If you encounter CORS issues, try refreshing the browser.
echo. 