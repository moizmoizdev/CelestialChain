# PowerShell script to check CORS configuration of the blockchain API
Write-Host "Checking CORS configuration for the Blockchain API..." -ForegroundColor Cyan

# Default API URL
$apiUrl = "http://localhost:8081/api"

# Perform an OPTIONS request to test CORS preflight
Write-Host "`nTesting OPTIONS request to /api/transaction endpoint..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "$apiUrl/transaction" -Method OPTIONS -Headers @{
        "Origin" = "http://localhost:3000"
        "Access-Control-Request-Method" = "POST"
        "Access-Control-Request-Headers" = "Content-Type"
    } -TimeoutSec 5 -ErrorAction Stop

    Write-Host "Response Status: $($response.StatusCode)" -ForegroundColor Green
    Write-Host "Response Headers:" -ForegroundColor Green
    $response.Headers | Format-Table -AutoSize
    
    # Check for CORS headers
    $corsHeaders = @(
        "Access-Control-Allow-Origin", 
        "Access-Control-Allow-Methods", 
        "Access-Control-Allow-Headers"
    )
    
    $corsSuccess = $true
    foreach ($header in $corsHeaders) {
        if ($response.Headers[$header]) {
            Write-Host "$header : $($response.Headers[$header])" -ForegroundColor Green
        } else {
            Write-Host "$header : MISSING" -ForegroundColor Red
            $corsSuccess = $false
        }
    }
    
    if ($corsSuccess) {
        Write-Host "`nCORS is properly configured!" -ForegroundColor Green
    } else {
        Write-Host "`nCORS configuration is incomplete. Some headers are missing." -ForegroundColor Red
    }
} catch {
    Write-Host "Error performing OPTIONS request: $_" -ForegroundColor Red
    
    if ($_.Exception.Response) {
        Write-Host "Status Code: $($_.Exception.Response.StatusCode)" -ForegroundColor Red
    }
    
    Write-Host "`nCORS appears to be misconfigured. The API server isn't responding correctly to preflight requests." -ForegroundColor Red
    Write-Host "Try running the API server with the updated configuration." -ForegroundColor Yellow
}

Write-Host "`nTrying a basic GET request to /api/wallet endpoint..." -ForegroundColor Yellow
try {
    $walletResponse = Invoke-WebRequest -Uri "$apiUrl/wallet" -Method GET -Headers @{
        "Origin" = "http://localhost:3000"
    } -TimeoutSec 5 -ErrorAction Stop
    
    Write-Host "Response Status: $($walletResponse.StatusCode)" -ForegroundColor Green
    $walletJson = $walletResponse.Content | ConvertFrom-Json
    Write-Host "Wallet Address: $($walletJson.address)" -ForegroundColor Green
    Write-Host "Wallet Balance: $($walletJson.balance)" -ForegroundColor Green
} catch {
    Write-Host "Error making GET request: $_" -ForegroundColor Red
} 