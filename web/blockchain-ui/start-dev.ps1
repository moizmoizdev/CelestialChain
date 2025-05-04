Write-Host "Starting Blockchain UI Development Server..." -ForegroundColor Green
Write-Host ""

# Change to the script's directory
Set-Location $PSScriptRoot

# Check if node and npm are installed
try {
    $nodeVersion = node -v
    $npmVersion = npm -v
    
    Write-Host "Node.js version: $nodeVersion"
    Write-Host "npm version: $npmVersion"
    Write-Host ""
} catch {
    Write-Host "Error: Node.js or npm is not installed or not in PATH." -ForegroundColor Red
    Write-Host "Please install Node.js from https://nodejs.org/" -ForegroundColor Red
    exit 1
}

# Check if blockchain API server is running
$apiAvailable = $false

try {
    $response = Invoke-WebRequest -Uri "http://localhost:8081/api/statistics" -Method GET -UseBasicParsing -TimeoutSec 2 -ErrorAction SilentlyContinue
    if ($response.StatusCode -eq 200) {
        Write-Host "Blockchain API is available at http://localhost:8081/api" -ForegroundColor Green
        $apiAvailable = $true
    }
} catch {
    Write-Host "Warning: Blockchain API not detected at http://localhost:8081/api" -ForegroundColor Yellow
}

if (-not $apiAvailable) {
    Write-Host "The React app will use fallback mock data until the API is available." -ForegroundColor Yellow
    Write-Host "To run the blockchain node, open another terminal and run the node executable." -ForegroundColor Yellow
    Write-Host ""
}

Write-Host "Starting npm development server..." -ForegroundColor Cyan
npm start 