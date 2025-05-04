@echo off
echo ===== BLOCKCHAIN SYSTEM CHECK =====
echo This script will verify that all components are running
echo.

echo Checking blockchain API endpoints...
powershell -ExecutionPolicy Bypass -Command "& {
    Write-Host 'Checking Blockchain Node APIs...' -ForegroundColor Cyan
    
    $apiPorts = @(8080, 8081, 8085, 8090)
    $runningCount = 0
    
    foreach ($port in $apiPorts) {
        $apiUrl = 'http://localhost:' + $port + '/api/wallet'
        Write-Host ('Testing API on port ' + $port + '...') -NoNewline
        
        try {
            $response = Invoke-WebRequest -Uri $apiUrl -Method GET -TimeoutSec 2 -ErrorAction Stop
            Write-Host ' RUNNING' -ForegroundColor Green
            $runningCount++
        } catch {
            Write-Host ' NOT RUNNING' -ForegroundColor Red
        }
    }
    
    Write-Host ''
    Write-Host ('API Status: ' + $runningCount + ' of 4 nodes running') -ForegroundColor Cyan
}"

echo.
echo Checking UI instances...
powershell -ExecutionPolicy Bypass -Command "& {
    Write-Host 'Checking UI Instances...' -ForegroundColor Cyan
    
    $uiPorts = @(3000, 3001, 3002)
    $runningCount = 0
    
    foreach ($port in $uiPorts) {
        Write-Host ('Testing UI on port ' + $port + '...') -NoNewline
        
        try {
            $processesUsingPort = Get-NetTCPConnection -LocalPort $port -ErrorAction SilentlyContinue | 
                                Select-Object -ExpandProperty OwningProcess
            
            if ($processesUsingPort) {
                Write-Host ' RUNNING' -ForegroundColor Green
                $runningCount++
            } else {
                Write-Host ' NOT RUNNING' -ForegroundColor Red
            }
        } catch {
            Write-Host ' NOT RUNNING' -ForegroundColor Red
        }
    }
    
    Write-Host ''
    Write-Host ('UI Status: ' + $runningCount + ' of 3 instances running') -ForegroundColor Cyan
}"

echo.
echo ===== SYSTEM CHECK COMPLETE =====
echo.
echo If any components are not running:
echo 1. Run start-full-system.bat to restart everything
echo 2. Or restart individual components:
echo   - run_network.bat (for blockchain nodes)
echo   - web\blockchain-ui\restart-multi-ui.bat (for UI instances)
echo.
pause 