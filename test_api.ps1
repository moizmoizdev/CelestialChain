# CelestialChain API Test Script
Write-Host "Testing CelestialChain API Endpoints..." -ForegroundColor Cyan

$apiUrl = "http://localhost:8080/api"

# Helper function to make API calls
function Invoke-ApiCall {
    param(
        [string]$endpoint,
        [string]$method = "GET",
        [string]$body = ""
    )
    
    $url = "$apiUrl/$endpoint"
    Write-Host "`nCalling $method $url" -ForegroundColor Yellow
    
    try {
        if ($method -eq "GET") {
            $response = Invoke-RestMethod -Uri $url -Method $method
        } else {
            $response = Invoke-RestMethod -Uri $url -Method $method -Body $body -ContentType "application/json"
        }
        
        # Convert the response to formatted JSON for display
        $formattedJson = $response | ConvertTo-Json -Depth 10
        Write-Host "Response:" -ForegroundColor Green
        Write-Host $formattedJson
        
        return $response
    }
    catch {
        Write-Host "Error calling $url : $_" -ForegroundColor Red
        Write-Host $_.Exception.Response.StatusCode
        Write-Host $_.Exception.Response.StatusDescription
    }
}

# 1. Test blockchain endpoint
Invoke-ApiCall -endpoint "blockchain"

# 2. Test mempool endpoint
Invoke-ApiCall -endpoint "mempool"

# 3. Test wallet endpoint
$wallet = Invoke-ApiCall -endpoint "wallet"
$walletAddress = $wallet.address
Write-Host "Wallet address: $walletAddress" -ForegroundColor Cyan

# 4. Test statistics endpoint
Invoke-ApiCall -endpoint "statistics"

# 5. Test creating a transaction 
$txBody = @{
    receiver = "CE123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0"
    amount = 1.5
} | ConvertTo-Json

Invoke-ApiCall -endpoint "transaction" -method "POST" -body $txBody

# 6. Test mining endpoint
Invoke-ApiCall -endpoint "mine" -method "POST"

# 7. Check mempool after mining
Invoke-ApiCall -endpoint "mempool"

# 8. View block data
Invoke-ApiCall -endpoint "explorer/block/1"

# 9. View difficulty
Invoke-ApiCall -endpoint "difficulty"

# 10. Test connecting to another peer
$peerBody = @{
    address = "127.0.0.1"
    port = 9000
} | ConvertTo-Json

Invoke-ApiCall -endpoint "peers/connect" -method "POST" -body $peerBody

# 11. View connected peers
Invoke-ApiCall -endpoint "peers"

# 12. Test blockchain sync
Invoke-ApiCall -endpoint "blockchain/sync" -method "POST"

Write-Host "`nAPI Test Complete!" -ForegroundColor Cyan 