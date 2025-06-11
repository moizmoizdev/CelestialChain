# PowerShell script to check CORS configuration for all blockchain API instances

Write-Host "Checking CORS configuration for multiple Blockchain API instances..." -ForegroundColor Cyan

# API ports to check
$apiPorts = @(8080, 8081, 8085, 8090)

foreach ($port in $apiPorts) {
    $apiUrl = "http://localhost:$port/api"
    
    Write-Host "`n=====================================================" -ForegroundColor Yellow
    Write-Host "Testing API at port $port" -ForegroundColor Yellow
    Write-Host "=====================================================" -ForegroundColor Yellow
    
    # Test the wallet endpoint first to see if the API is even running
    Write-Host "`nTesting /api/wallet endpoint..." -ForegroundColor Yellow
    try {
        $walletResponse = Invoke-WebRequest -Uri "$apiUrl/wallet" -Method GET -Headers @{
            "Origin" = "http://localhost:3000"
        } -TimeoutSec 2 -ErrorAction Stop
        
        Write-Host "API on port $port is running!" -ForegroundColor Green
        Write-Host "Response Status: $($walletResponse.StatusCode)" -ForegroundColor Green
        
        # Now test CORS with OPTIONS request
        Write-Host "`nTesting OPTIONS request for CORS preflight..." -ForegroundColor Yellow
        try {
            $response = Invoke-WebRequest -Uri "$apiUrl/transaction" -Method OPTIONS -Headers @{
                "Origin" = "http://localhost:3000"
                "Access-Control-Request-Method" = "POST"
                "Access-Control-Request-Headers" = "Content-Type"
            } -TimeoutSec 2 -ErrorAction Stop

            Write-Host "OPTIONS request succeeded" -ForegroundColor Green
            
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
                Write-Host "CORS is properly configured on port $port!" -ForegroundColor Green
            } else {
                Write-Host "CORS configuration is incomplete on port $port." -ForegroundColor Red
            }
        } catch {
            Write-Host "CORS preflight request failed on port $port" -ForegroundColor Red
            Write-Host "Error: $_" -ForegroundColor Red
        }
    } catch {
        Write-Host "API on port $port is not responding" -ForegroundColor Red
    }
}

Write-Host "`n=====================================================" -ForegroundColor Yellow
Write-Host "API Check Complete" -ForegroundColor Yellow
Write-Host "=====================================================" -ForegroundColor Yellow 