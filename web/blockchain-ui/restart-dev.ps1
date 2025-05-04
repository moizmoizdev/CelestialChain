# PowerShell script to restart the React development server

Write-Host "Restarting React development server..."

# Check if any process is using port 3000
$processesUsingPort = Get-NetTCPConnection -LocalPort 3000 -ErrorAction SilentlyContinue | 
                     Select-Object -ExpandProperty OwningProcess

if ($processesUsingPort) {
    Write-Host "Found processes using port 3000. Attempting to kill them..."
    
    foreach ($processId in $processesUsingPort) {
        try {
            $process = Get-Process -Id $processId -ErrorAction SilentlyContinue
            if ($process) {
                Write-Host "Killing process: $($process.Name) (PID: $processId)"
                Stop-Process -Id $processId -Force
                Write-Host "Process killed successfully."
            }
        }
        catch {
            Write-Host "Error killing process with ID $processId : $_"
        }
    }
} else {
    Write-Host "No processes found using port 3000."
}

# Run the API checker script first to ensure we use the right port
Write-Host "Checking API connectivity..."
& .\check-api.ps1

# Wait 1 second
Start-Sleep -Seconds 1

# Start the development server
Write-Host "`nStarting development server..."
npm start 