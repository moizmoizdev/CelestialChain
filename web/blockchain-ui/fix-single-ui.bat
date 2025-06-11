@echo off
echo ==== Fix CORS for Single Blockchain UI Instance ====

if "%1"=="" (
  echo Usage: fix-single-ui.bat [API_PORT] [UI_PORT]
  echo Example: fix-single-ui.bat 8081 3002
  echo This will configure the UI on port 3002 to connect to the API on port 8081
  exit /b 1
)

set API_PORT=%1

if "%2"=="" (
  set UI_PORT=3000
) else (
  set UI_PORT=%2
)

echo Creating setupProxy.js with CORS configuration for API port %API_PORT%...

REM Create setupProxy.js for the specified API port
echo const { createProxyMiddleware } = require('http-proxy-middleware'); > "src\setupProxy.js"
echo. >> "src\setupProxy.js"
echo module.exports = function(app) { >> "src\setupProxy.js"
echo   app.use( >> "src\setupProxy.js"
echo     '/api', >> "src\setupProxy.js"
echo     createProxyMiddleware({ >> "src\setupProxy.js"
echo       target: 'http://localhost:%API_PORT%', >> "src\setupProxy.js"
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

echo Starting Blockchain UI on port %UI_PORT% connected to API on port %API_PORT%...
start "Blockchain UI - API Port %API_PORT%" cmd /k "set PORT=%UI_PORT% && set REACT_APP_API_PORT=%API_PORT% && set BROWSER=none && npm start"

echo.
echo UI has been started at http://localhost:%UI_PORT%
echo Connected to API at http://localhost:%API_PORT%/api
echo. 