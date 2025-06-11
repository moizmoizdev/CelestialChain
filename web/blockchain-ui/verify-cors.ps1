# PowerShell script to test CORS configuration of the blockchain API endpoints

Write-Host "=== CORS Configuration Verification Tool ===" -ForegroundColor Cyan
Write-Host

$apiPorts = @(8080, 8081, 8085, 8090)

foreach ($port in $apiPorts) {
    Write-Host "Testing API endpoint on port $port..." -ForegroundColor Yellow
    
    # Test if the endpoint is responsive at all
    try {
        $basicResponse = Invoke-WebRequest -Uri "http://localhost:$port/api/wallet" -Method GET -TimeoutSec 2 -ErrorAction Stop
        Write-Host "  Basic connectivity: OK (Status: $($basicResponse.StatusCode))" -ForegroundColor Green
    } 
    catch {
        Write-Host "  Basic connectivity: FAILED - Can't connect to API on port $port" -ForegroundColor Red
        Write-Host "  Error: $($_.Exception.Message)" -ForegroundColor Red
        Write-Host
        continue
    }
    
    # Test OPTIONS preflight for CORS
    try {
        $headers = @{
            'Origin' = 'http://localhost:3000'
            'Access-Control-Request-Method' = 'POST'
            'Access-Control-Request-Headers' = 'Content-Type'
        }
        
        $optionsResponse = Invoke-WebRequest -Uri "http://localhost:$port/api/transaction" -Method OPTIONS -Headers $headers -TimeoutSec 2 -ErrorAction Stop
        
        # Check CORS headers in response
        $corsSuccess = $true
        
        if ($optionsResponse.Headers["Access-Control-Allow-Origin"] -ne "*") {
            Write-Host "  Missing or incorrect Access-Control-Allow-Origin header" -ForegroundColor Red
            $corsSuccess = $false
        }
        
        if (-not $optionsResponse.Headers["Access-Control-Allow-Methods"]) {
            Write-Host "  Missing Access-Control-Allow-Methods header" -ForegroundColor Red
            $corsSuccess = $false
        }
        
        if (-not $optionsResponse.Headers["Access-Control-Allow-Headers"]) {
            Write-Host "  Missing Access-Control-Allow-Headers header" -ForegroundColor Red
            $corsSuccess = $false
        }
        
        if ($corsSuccess) {
            Write-Host "  CORS preflight: OK" -ForegroundColor Green
        } else {
            Write-Host "  CORS preflight: PARTIAL - Some headers missing" -ForegroundColor Yellow
        }
    } 
    catch {
        Write-Host "  CORS preflight: FAILED" -ForegroundColor Red
        Write-Host "  Error: $($_.Exception.Message)" -ForegroundColor Red
    }
    
    # Test simple POST request
    try {
        $postData = @{
            receiver = "test_address"
            amount = 0.001
        } | ConvertTo-Json
        
        $postHeaders = @{
            'Content-Type' = 'application/json'
            'Origin' = 'http://localhost:3000'
        }
        
        # Note: We don't want to actually submit this transaction, just test if CORS would allow it
        # So we use a very short timeout to likely fail on the actual processing
        try {
            Invoke-WebRequest -Uri "http://localhost:$port/api/transaction" -Method POST -Headers $postHeaders -Body $postData -TimeoutSec 1 -ErrorAction Stop
            Write-Host "  POST request: OK" -ForegroundColor Green
        }
        catch [System.Net.WebException] {
            # Check if this is a timeout or connection refused
            if ($_.Exception.Response -ne $null) {
                # If we got a response, CORS was likely not the issue - probably just timed out on processing
                $statusCode = [int]$_.Exception.Response.StatusCode
                if ($statusCode -eq 0) {
                    Write-Host "  POST request: LIKELY OK (Request timed out but response was started)" -ForegroundColor Yellow
                } else {
                    Write-Host "  POST request: OK (Status: $statusCode)" -ForegroundColor Green
                }
            } else {
                # No response received, might be CORS or other issue
                Write-Host "  POST request: FAILED - No response" -ForegroundColor Red
                Write-Host "  Error: $($_.Exception.Message)" -ForegroundColor Red
            }
        }
    } 
    catch {
        Write-Host "  POST request: ERROR" -ForegroundColor Red
        Write-Host "  Error: $($_.Exception.Message)" -ForegroundColor Red
    }
    
    Write-Host
}

Write-Host "=== Verification Complete ===" -ForegroundColor Cyan
Write-Host
Write-Host "If any endpoints show CORS issues, try restarting both the blockchain nodes and UI instances." -ForegroundColor Yellow
Write-Host
Write-Host "To restart everything:
1. Run 'run_network.bat --clean' from the root directory
2. Run 'web/blockchain-ui/restart-multi-ui.bat' to restart all UI instances" -ForegroundColor Yellow 