# PowerShell script to fix CORS issues and start the blockchain node and UI

Write-Host "==== Blockchain Node + UI Starter with CORS Fix ====" -ForegroundColor Cyan
Write-Host "This script will start the blockchain node and UI with proper CORS configuration"

# Step 1: Create the setupProxy.js file for the UI
Write-Host "`nStep 1: Creating setupProxy.js with CORS configuration..." -ForegroundColor Yellow

$setupProxyPath = ".\web\blockchain-ui\src\setupProxy.js"
$setupProxyContent = @"
const { createProxyMiddleware } = require('http-proxy-middleware');

module.exports = function(app) {
  app.use(
    '/api',
    createProxyMiddleware({
      target: 'http://localhost:8081',
      changeOrigin: true,
      pathRewrite: {
        '^/api': '/api',
      },
      onProxyRes: function(proxyRes, req, res) {
        // Add CORS headers to every response
        proxyRes.headers['Access-Control-Allow-Origin'] = '*';
        proxyRes.headers['Access-Control-Allow-Methods'] = 'GET, POST, PUT, DELETE, OPTIONS';
        proxyRes.headers['Access-Control-Allow-Headers'] = 'Content-Type, Authorization';
      }
    })
  );
};
"@

# Create the directory if it doesn't exist
$setupProxyDir = Split-Path -Parent $setupProxyPath
if (-not (Test-Path $setupProxyDir)) {
    New-Item -ItemType Directory -Path $setupProxyDir -Force | Out-Null
}

# Write the setupProxy.js file
Set-Content -Path $setupProxyPath -Value $setupProxyContent -Force
Write-Host "Created setupProxy.js at $setupProxyPath" -ForegroundColor Green

# Step 2: Start the blockchain node
Write-Host "`nStep 2: Starting the blockchain node on port 8001 with API on port 8081..." -ForegroundColor Yellow
$nodeProcess = Start-Process -FilePath "powershell" -ArgumentList "-Command", ".\blockchain_node.exe --host 127.0.0.1 --port 8001 --type full --api-port 8081" -PassThru -WindowStyle Minimized

# Wait a bit for the node to start
Write-Host "Waiting 5 seconds for the node to initialize..." -ForegroundColor Yellow
Start-Sleep -Seconds 5

# Step 3: Start the React UI
Write-Host "`nStep 3: Starting the React UI on port 3000..." -ForegroundColor Yellow
$env:PORT = 3000
$env:REACT_APP_API_PORT = 8081
$env:BROWSER = "none"

# Change to the UI directory and start the app
Set-Location -Path ".\web\blockchain-ui"
npm start

# When the script is interrupted, clean up processes
$nodeProcess.Kill()

Write-Host "All processes have been stopped." -ForegroundColor Yellow 