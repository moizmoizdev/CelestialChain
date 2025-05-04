@echo off
echo Starting CelestialChain Blockchain Environment

REM Start the CORS proxy in its own window
echo Starting CORS proxy...
start "CORS Proxy" cmd /c "cd web\blockchain-ui && start-proxy.bat"

REM Wait for the proxy to start
timeout /t 3 /nobreak

REM Start the blockchain network
echo Starting blockchain nodes...
call run_network.bat %*

echo.
echo All services are running.
echo - Blockchain nodes are running on ports 8000, 8001, 8005, 8010
echo - APIs are available on ports 8080, 8081, 8085, 8090
echo - CORS proxy is running on port 8000
echo.
echo You can now access the blockchain UI at:
echo - http://localhost:3000 (if started separately with 'cd web/blockchain-ui && npm start')
echo.
echo Press Ctrl+C in the individual windows to stop each service. 