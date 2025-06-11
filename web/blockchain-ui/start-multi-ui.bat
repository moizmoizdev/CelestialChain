@echo off
echo Starting multiple blockchain UI instances...
echo Each instance will open in a separate command window.
echo Close individual windows to stop specific instances.
echo.

REM Create setupProxy.js for the first instance
echo const { createProxyMiddleware } = require('http-proxy-middleware'); > "src\setupProxy.js"
echo. >> "src\setupProxy.js"
echo module.exports = function(app) { >> "src\setupProxy.js"
echo   app.use( >> "src\setupProxy.js"
echo     '/api', >> "src\setupProxy.js"
echo     createProxyMiddleware({ >> "src\setupProxy.js"
echo       target: 'http://localhost:8080', >> "src\setupProxy.js"
echo       changeOrigin: true, >> "src\setupProxy.js"
echo       pathRewrite: { >> "src\setupProxy.js"
echo         '^/api': '/api', >> "src\setupProxy.js"
echo       }, >> "src\setupProxy.js"
echo       onProxyRes: function(proxyRes, req, res) { >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Origin'] = '*'; >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'; >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'; >> "src\setupProxy.js"
echo       } >> "src\setupProxy.js"
echo     }) >> "src\setupProxy.js"
echo   ); >> "src\setupProxy.js"
echo }; >> "src\setupProxy.js"

REM Start the first instance (Full Node)
echo Starting Blockchain UI - Full Node on port 3000 (API port: 8080)...
start "Blockchain UI - Full Node" cmd /k "set PORT=3000 && set REACT_APP_API_PORT=8080 && set REACT_APP_NODE_NAME=Blockchain UI - Full Node && set BROWSER=none && npm start"

REM Wait a bit to avoid port conflicts
timeout /t 5 /nobreak > nul

REM Create setupProxy.js for the second instance
echo const { createProxyMiddleware } = require('http-proxy-middleware'); > "src\setupProxy.js"
echo. >> "src\setupProxy.js"
echo module.exports = function(app) { >> "src\setupProxy.js"
echo   app.use( >> "src\setupProxy.js"
echo     '/api', >> "src\setupProxy.js"
echo     createProxyMiddleware({ >> "src\setupProxy.js"
echo       target: 'http://localhost:8090', >> "src\setupProxy.js"
echo       changeOrigin: true, >> "src\setupProxy.js"
echo       pathRewrite: { >> "src\setupProxy.js"
echo         '^/api': '/api', >> "src\setupProxy.js"
echo       }, >> "src\setupProxy.js"
echo       onProxyRes: function(proxyRes, req, res) { >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Origin'] = '*'; >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'; >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'; >> "src\setupProxy.js"
echo       } >> "src\setupProxy.js"
echo     }) >> "src\setupProxy.js"
echo   ); >> "src\setupProxy.js"
echo }; >> "src\setupProxy.js"

REM Start the second instance (Wallet Node)
echo Starting Blockchain UI - Wallet Node on port 3001 (API port: 8090)...
start "Blockchain UI - Wallet Node" cmd /k "set PORT=3001 && set REACT_APP_API_PORT=8090 && set REACT_APP_NODE_NAME=Blockchain UI - Wallet Node && set BROWSER=none && npm start"

REM Wait a bit to avoid port conflicts
timeout /t 5 /nobreak > nul

REM Create setupProxy.js for the third instance
echo const { createProxyMiddleware } = require('http-proxy-middleware'); > "src\setupProxy.js"
echo. >> "src\setupProxy.js"
echo module.exports = function(app) { >> "src\setupProxy.js"
echo   app.use( >> "src\setupProxy.js"
echo     '/api', >> "src\setupProxy.js"
echo     createProxyMiddleware({ >> "src\setupProxy.js"
echo       target: 'http://localhost:8081', >> "src\setupProxy.js"
echo       changeOrigin: true, >> "src\setupProxy.js"
echo       pathRewrite: { >> "src\setupProxy.js"
echo         '^/api': '/api', >> "src\setupProxy.js"
echo       }, >> "src\setupProxy.js"
echo       onProxyRes: function(proxyRes, req, res) { >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Origin'] = '*'; >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS'; >> "src\setupProxy.js"
echo         proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization'; >> "src\setupProxy.js"
echo       } >> "src\setupProxy.js"
echo     }) >> "src\setupProxy.js"
echo   ); >> "src\setupProxy.js"
echo }; >> "src\setupProxy.js"

REM Start the third instance (Full Node 2)
echo Starting Blockchain UI - Full Node 2 on port 3002 (API port: 8081)...
start "Blockchain UI - Full Node 2" cmd /k "set PORT=3002 && set REACT_APP_API_PORT=8081 && set REACT_APP_NODE_NAME=Blockchain UI - Full Node 2 && set BROWSER=none && npm start"

echo.
echo All instances have been started.
echo Access the UIs at:
echo   http://localhost:3000 - Blockchain UI - Full Node
echo   http://localhost:3001 - Blockchain UI - Wallet Node
echo   http://localhost:3002 - Blockchain UI - Full Node 2
echo.
echo You can close this window now. 