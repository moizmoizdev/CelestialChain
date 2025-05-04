# PowerShell script to check blockchain API connectivity

Write-Host "Checking blockchain API connectivity..."

# Try different ports
$ports = 8081, 8080, 8085, 8090, 8095

foreach ($port in $ports) {
    $url = "http://localhost:$port/api/statistics"
    Write-Host "Trying $url..."
    
    try {
        $response = Invoke-WebRequest -Uri $url -Method GET -UseBasicParsing -TimeoutSec 5
        
        if ($response.StatusCode -eq 200) {
            Write-Host "SUCCESS: Connected to API on port $port!"
            Write-Host "Response status: $($response.StatusCode)"
            Write-Host "Response content: $($response.Content)"
            Write-Host ""
            Write-Host "Using this port in .env file..."
            
            # Create or update .env file with the working port
            Set-Content -Path ".\.env" -Value "REACT_APP_API_URL=http://localhost:$port/api"
            Write-Host "Created .env file with REACT_APP_API_URL=http://localhost:$port/api"
            
            # Open the file to verify
            if (Test-Path ".\.env") {
                Write-Host "Content of .env file:"
                Get-Content ".\.env"
            }
            
            # Success - exit the loop
            break
        }
    }
    catch {
        Write-Host "Failed to connect to port $port. Error: $_"
    }
}

Write-Host ""
Write-Host "API check complete. If no successful connection was found, check that your blockchain node is running."
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown") 