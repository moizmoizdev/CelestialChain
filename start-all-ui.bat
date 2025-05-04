@echo off
echo Starting CelestialChain Full Environment

REM Start the blockchain network in the background
echo Starting blockchain nodes...
start "Blockchain Network" cmd /c "call run_network.bat %*"

REM Wait for nodes to start
echo Waiting for blockchain nodes to start...
timeout /t 10 /nobreak

REM Start the UI
echo Starting blockchain UI...
cd web\blockchain-ui
start "Blockchain UI" cmd /c "npm start"

echo.
echo All services are now running:
echo - Blockchain nodes on ports 8000, 8001, 8005, 8010
echo - APIs on ports 8080, 8081, 8085, 8090
echo - Web UI on http://localhost:3000
echo.
echo Press Ctrl+C in the individual windows to stop each service. 