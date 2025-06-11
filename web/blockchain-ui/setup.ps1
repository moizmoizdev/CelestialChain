# Setup script for blockchain-ui multi-node support

Write-Host "Setting up blockchain UI with multi-node support..." -ForegroundColor Green

# Check if Node.js is installed
try {
    $nodeVersion = node -v
    Write-Host "Node.js version: $nodeVersion" -ForegroundColor Cyan
} catch {
    Write-Host "Error: Node.js is not installed or not in PATH" -ForegroundColor Red
    Write-Host "Please install Node.js from https://nodejs.org/" -ForegroundColor Red
    exit 1
}

# Install dependencies
Write-Host "Installing project dependencies..." -ForegroundColor Yellow
npm install

# Install additional dependencies needed for multi-node support
Write-Host "Installing additional dependencies for multi-node support..." -ForegroundColor Yellow
npm install --save http-proxy-middleware

# Set up setupProxy.js file
$proxyJsPath = "$PSScriptRoot\src\setupProxy.js"
$proxyJsContent = @"
const { createProxyMiddleware } = require('http-proxy-middleware');

module.exports = function(app) {
  app.use(
    '/api',
    createProxyMiddleware({
      target: 'http://localhost:8080',
      changeOrigin: true,
      pathRewrite: {
        '^/api': '/api',
      },
    })
  );
};
"@

# Create the setupProxy.js file
Set-Content -Path $proxyJsPath -Value $proxyJsContent
Write-Host "Created setupProxy.js file" -ForegroundColor Green

# Ensure scripts are executable
Write-Host "Setting up execution scripts..." -ForegroundColor Yellow

# Create a simple launcher script
$launcherPath = "$PSScriptRoot\launcher.ps1"
$launcherContent = @"
param (
    [string]`$script = "start-multi-ui.ps1"
)

# Execute the specified script
Write-Host "Launching `$script..." -ForegroundColor Green
& "`$PSScriptRoot\`$script"
"@

Set-Content -Path $launcherPath -Value $launcherContent
Write-Host "Created launcher script" -ForegroundColor Green

# Instructions
Write-Host "`nSetup complete!" -ForegroundColor Green
Write-Host "`nTo run the blockchain UI with multiple nodes:"
Write-Host "  1. Start your blockchain nodes (make sure you have different API ports configured)" -ForegroundColor Cyan
Write-Host "  2. Run .\start-multi-ui.ps1 to launch multiple UI instances" -ForegroundColor Cyan
Write-Host "  or" -ForegroundColor Yellow
Write-Host "  3. Run .\start-multi-ui.bat if you prefer cmd windows" -ForegroundColor Cyan
Write-Host "`nTo run a single instance with node selection capabilities:" -ForegroundColor Yellow
Write-Host "  npm start" -ForegroundColor Cyan
Write-Host "`nFor more information, see the README.md file." -ForegroundColor Yellow 