@echo off
echo Starting Blockchain UI with CORS configuration...
echo.

echo Creating setupProxy.js to handle CORS...

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

echo Proxy configuration created. Starting React development server...
echo.
echo This UI will connect to the node running on port 8081 (API)
echo.

powershell -Command "Set-Item -Path Env:PORT -Value 3000; Set-Item -Path Env:REACT_APP_API_PORT -Value 8081; Set-Item -Path Env:BROWSER -Value none; npm start" 