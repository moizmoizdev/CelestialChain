@echo off
echo ==== Blockchain Node + UI Starter with CORS Fix ====
echo This script will start the blockchain node and UI with proper CORS configuration

REM Step 1: Create the setupProxy.js file for the UI
echo.
echo Step 1: Creating setupProxy.js with CORS configuration...

set SETUP_PROXY_PATH=web\blockchain-ui\src\setupProxy.js

REM Create the setupProxy.js file
echo const { createProxyMiddleware } = require('http-proxy-middleware'); > "%SETUP_PROXY_PATH%"
echo. >> "%SETUP_PROXY_PATH%"
echo module.exports = function(app) { >> "%SETUP_PROXY_PATH%"
echo   app.use( >> "%SETUP_PROXY_PATH%"
echo     '/api', >> "%SETUP_PROXY_PATH%"
echo     createProxyMiddleware({ >> "%SETUP_PROXY_PATH%"
echo       target: 'http://localhost:8081', >> "%SETUP_PROXY_PATH%"
echo       changeOrigin: true, >> "%SETUP_PROXY_PATH%"
echo       pathRewrite: { >> "%SETUP_PROXY_PATH%"
echo         '^/api': '/api', >> "%SETUP_PROXY_PATH%"
echo       }, >> "%SETUP_PROXY_PATH%"
echo       onProxyRes: function(proxyRes, req, res) { >> "%SETUP_PROXY_PATH%"
echo         // Add CORS headers to every response >> "%SETUP_PROXY_PATH%"
echo         proxyRes.headers['Access-Control-Allow-Origin'] = '*'; >> "%SETUP_PROXY_PATH%"
echo         proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'; >> "%SETUP_PROXY_PATH%"
echo         proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'; >> "%SETUP_PROXY_PATH%"
echo       } >> "%SETUP_PROXY_PATH%"
echo     }) >> "%SETUP_PROXY_PATH%"
echo   ); >> "%SETUP_PROXY_PATH%"
echo }; >> "%SETUP_PROXY_PATH%"

echo Created setupProxy.js successfully

REM Step 2: Start the blockchain node
echo.
echo Step 2: Starting the blockchain node on port 8001 with API on port 8081...
start "Blockchain Node 8001" powershell -Command "blockchain_node.exe --host 127.0.0.1 --port 8001 --type full --api-port 8081"

REM Wait a bit for the node to start
echo Waiting 5 seconds for the node to initialize...
timeout /t 5 /nobreak

REM Step 3: Start the React UI
echo.
echo Step 3: Starting the React UI on port 3000...

REM Change to the UI directory and start the app
cd web\blockchain-ui
set PORT=3000
set REACT_APP_API_PORT=8081
set BROWSER=none
npm start

REM Return to the original directory
cd ..\..

echo.
echo All processes have been started. Close this window to stop them. 