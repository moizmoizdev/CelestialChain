# PowerShell script to check all blockchain API endpoints
Write-Host "Checking all blockchain API endpoints..." -ForegroundColor Cyan

$baseUrl = "http://localhost:8081/api"
$endpoints = @(
    "/statistics",
    "/blockchain",
    "/explorer/latest-blocks?limit=5",
    "/mempool",
    "/wallet",
    "/peers",
    "/explorer/block/1",
    "/transactions/latest?limit=5"
)

foreach ($endpoint in $endpoints) {
    $url = "$baseUrl$endpoint"
    Write-Host "Testing endpoint: $url" -ForegroundColor Yellow
    
    try {
        $response = Invoke-WebRequest -Uri $url -Method GET -UseBasicParsing -TimeoutSec 10
        
        if ($response.StatusCode -eq 200) {
            Write-Host "SUCCESS: Endpoint working (200 OK)" -ForegroundColor Green
            Write-Host "Response Content:" -ForegroundColor Gray
            Write-Host "$($response.Content)" -ForegroundColor White
        } else {
            Write-Host "WARNING: Endpoint returned status code $($response.StatusCode)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "ERROR: Failed to connect to endpoint" -ForegroundColor Red
        Write-Host "Error Message: $($_.Exception.Message)" -ForegroundColor Red
    }
    
    Write-Host "-----------------------------------------" -ForegroundColor Gray
}

# Display summary
Write-Host "`nEndpoint Check Summary" -ForegroundColor Cyan
Write-Host "-----------------------------------------" -ForegroundColor Gray
Write-Host "Base URL: $baseUrl" -ForegroundColor White
Write-Host "Total Endpoints Checked: $($endpoints.Count)" -ForegroundColor White
Write-Host "`nIf any endpoints failed, check that:" -ForegroundColor Yellow
Write-Host "1. Your blockchain node is running" -ForegroundColor Yellow
Write-Host "2. The API server is running on port 8081" -ForegroundColor Yellow
Write-Host "3. The CORS settings allow requests from your React app" -ForegroundColor Yellow 