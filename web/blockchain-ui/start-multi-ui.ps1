# PowerShell script to launch multiple UI instances for the blockchain application

# Define the ports and configurations
$instances = @(
    @{Port = 3000; Name = "Blockchain UI - Full Node"; ApiPort = 8080},
    @{Port = 3001; Name = "Blockchain UI - Wallet Node"; ApiPort = 8090},
    @{Port = 3002; Name = "Blockchain UI - Full Node 2"; ApiPort = 8081}
)

Write-Host "Starting multiple blockchain UI instances..." -ForegroundColor Green
Write-Host "Each instance will open in a separate terminal window." -ForegroundColor Yellow
Write-Host "Close individual windows to stop specific instances." -ForegroundColor Yellow

# Function to create necessary files for each instance
function Create-InstanceFiles {
    param (
        [int]$Port,
        [string]$Name,
        [int]$ApiPort
    )
    
    # Create .env file
    $envContent = @"
PORT=$Port
REACT_APP_API_PORT=$ApiPort
REACT_APP_NODE_NAME=$Name
"@
    
    Set-Content -Path "$PSScriptRoot\.env.$Port" -Value $envContent
    Write-Host "Created .env.$Port file with PORT=$Port and API_PORT=$ApiPort"
    
    # Create proxy configuration
    $proxyContent = @"
const { createProxyMiddleware } = require('http-proxy-middleware');

module.exports = function(app) {
  app.use(
    '/api',
    createProxyMiddleware({
      target: 'http://localhost:$ApiPort',
      changeOrigin: true,
      pathRewrite: {
        '^/api': '/api', // no rewrite needed, but it's explicit
      },
    })
  );
};
"@
    
    $proxyPath = "$PSScriptRoot\src\setupProxy.js"
    Set-Content -Path $proxyPath -Value $proxyContent
    Write-Host "Updated proxy configuration for port $Port -> API port $ApiPort"
    
    # Return the command to start this instance
    return "cd $PSScriptRoot; set PORT=$Port; set REACT_APP_API_PORT=$ApiPort; set REACT_APP_NODE_NAME='$Name'; set BROWSER=none; npm start"
}

# Start each instance in a separate PowerShell window
foreach ($instance in $instances) {
    $command = Create-InstanceFiles -Port $instance.Port -Name $instance.Name -ApiPort $instance.ApiPort
    
    # Start PowerShell window with the command
    Write-Host "Starting $($instance.Name) on port $($instance.Port) connecting to API on port $($instance.ApiPort)" -ForegroundColor Cyan
    
    # Using start-process to open a new PowerShell window
    Start-Process powershell.exe -ArgumentList "-NoExit", "-Command", "cd '$PSScriptRoot'; `$env:PORT='$($instance.Port)'; `$env:REACT_APP_API_PORT='$($instance.ApiPort)'; `$env:REACT_APP_NODE_NAME='$($instance.Name)'; `$env:BROWSER='none'; npm start"
    
    # Wait a moment before starting the next instance to avoid port conflicts
    Start-Sleep -Seconds 3
}

Write-Host "All instances have been started." -ForegroundColor Green
Write-Host "Access the UIs at: " -ForegroundColor Green
foreach ($instance in $instances) {
    Write-Host "  http://localhost:$($instance.Port) - $($instance.Name)" -ForegroundColor Cyan
} 