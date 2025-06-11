@echo off
echo ==== Restarting Blockchain Multi-UI Setup ====
echo This script will kill any existing UI instances and restart them

echo.
echo Step 1: Killing any existing React development servers...

REM Kill any processes using ports 3000-3003 (React dev servers)
echo Checking port 3000...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3000') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    )
)

echo Checking port 3001...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3001') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    )
)

echo Checking port 3002...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3002') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    )
)

echo Checking port 3003...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3003') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    )
)

echo.
echo Step 2: Waiting 5 seconds for ports to be released...
timeout /t 5 /nobreak > nul

echo.
echo Step 3: Creating the dynamic setupProxy.js file...
echo.

REM Use PowerShell to ensure proper template literals
powershell -Command "Set-Content -Path 'src\setupProxy.js' -Value 'const { createProxyMiddleware } = require(''http-proxy-middleware'');

// Map UI ports to API ports
const portMapping = {
  3000: 8080,  // UI port 3000 -> API port 8080
  3001: 8090,  // UI port 3001 -> API port 8090
  3002: 8081,  // UI port 3002 -> API port 8081
  3003: 8085   // UI port 3003 -> API port 8085
};

module.exports = function(app) {
  // Get the current UI port from environment variable
  const currentPort = parseInt(process.env.PORT || ''3000'', 10);
  
  // Determine which API port to use based on the UI port
  const apiPort = portMapping[currentPort] || 8080; // Default to 8080 if no mapping
  
  console.log(`Setting up proxy from UI port ${currentPort} to API port ${apiPort}`);
  
  app.use(
    ''/api'',
    createProxyMiddleware({
      target: `http://localhost:${apiPort}`,
      changeOrigin: true,
      pathRewrite: {
        ''^/api'': ''/api'',
      },
      onProxyReq: function(proxyReq, req, res) {
        // Add CORS headers to proxy requests
        proxyReq.setHeader(''Access-Control-Allow-Origin'', ''*'');
        proxyReq.setHeader(''Access-Control-Allow-Methods'', ''GET, POST, PUT, DELETE, OPTIONS'');
        proxyReq.setHeader(''Access-Control-Allow-Headers'', ''Content-Type, Authorization'');
        
        // Log the request for debugging
        console.log(`Proxying ${req.method} ${req.path} to http://localhost:${apiPort}${req.path}`);
      },
      onProxyRes: function(proxyRes, req, res) {
        // Add CORS headers to proxy responses
        proxyRes.headers[''Access-Control-Allow-Origin''] = ''*'';
        proxyRes.headers[''Access-Control-Allow-Methods''] = ''GET, POST, PUT, DELETE, OPTIONS'';
        proxyRes.headers[''Access-Control-Allow-Headers''] = ''Content-Type, Authorization'';
      },
      // Handle proxy errors
      onError: function(err, req, res) {
        console.error(''Proxy error:'', err);
        res.writeHead(500, {
          ''Content-Type'': ''text/plain'',
          ''Access-Control-Allow-Origin'': ''*'',
          ''Access-Control-Allow-Methods'': ''GET, POST, PUT, DELETE, OPTIONS'',
          ''Access-Control-Allow-Headers'': ''Content-Type, Authorization''
        });
        res.end(`Proxy error: ${err.message}`);
      }
    })
  );
  
  // Add a special handler for OPTIONS requests
  app.use(''/api/*'', (req, res, next) => {
    if (req.method === ''OPTIONS'') {
      res.header(''Access-Control-Allow-Origin'', ''*'');
      res.header(''Access-Control-Allow-Methods'', ''GET, POST, PUT, DELETE, OPTIONS'');
      res.header(''Access-Control-Allow-Headers'', ''Content-Type, Authorization'');
      res.status(204).send();
      return;
    }
    next();
  });
};'"

echo Step 4: Starting multiple blockchain UI instances...
echo Each instance will open in a separate command window.
echo.

REM Start the first instance (Full Node)
echo Starting Blockchain UI - Full Node on port 3000 (API port: 8080)...
start "Blockchain UI - Full Node" cmd /k "set PORT=3000 && set REACT_APP_API_PORT=8080 && set REACT_APP_NODE_NAME=Blockchain UI - Full Node && set BROWSER=none && npm start"

REM Wait a bit to avoid port conflicts
timeout /t 5 /nobreak > nul

REM Start the second instance (Wallet Node)
echo Starting Blockchain UI - Wallet Node on port 3001 (API port: 8090)...
start "Blockchain UI - Wallet Node" cmd /k "set PORT=3001 && set REACT_APP_API_PORT=8090 && set REACT_APP_NODE_NAME=Blockchain UI - Wallet Node && set BROWSER=none && npm start"

REM Wait a bit to avoid port conflicts
timeout /t 5 /nobreak > nul

REM Start the third instance (Full Node 2)
echo Starting Blockchain UI - Full Node 2 on port 3002 (API port: 8081)...
start "Blockchain UI - Full Node 2" cmd /k "set PORT=3002 && set REACT_APP_API_PORT=8081 && set REACT_APP_NODE_NAME=Blockchain UI - Full Node 2 && set BROWSER=none && npm start"

REM Wait a bit to avoid port conflicts
timeout /t 5 /nobreak > nul

REM Start the fourth instance (Full Node 3)
echo Starting Blockchain UI - Full Node 3 on port 3003 (API port: 8085)...
start "Blockchain UI - Full Node 3" cmd /k "set PORT=3003 && set REACT_APP_API_PORT=8085 && set REACT_APP_NODE_NAME=Blockchain UI - Full Node 3 && set BROWSER=none && npm start"

echo.
echo All instances have been started.
echo Access the UIs at:
echo   http://localhost:3000 - Blockchain UI - Full Node (API port: 8080)
echo   http://localhost:3001 - Blockchain UI - Wallet Node (API port: 8090)
echo   http://localhost:3002 - Blockchain UI - Full Node 2 (API port: 8081)
echo   http://localhost:3003 - Blockchain UI - Full Node 3 (API port: 8085)
echo.
echo You can close this window now. 