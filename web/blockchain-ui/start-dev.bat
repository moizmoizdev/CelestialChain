@echo off
echo Starting Blockchain UI Development Server...
echo.
echo NOTE: If you're using PowerShell, please use the PowerShell script instead:
echo       .\start-dev.ps1
echo.
echo If you encounter errors with && in commands, please use PowerShell instead.
echo.

:: Check if the blockchain API server is running
ping -n 1 localhost:8081 >nul 2>nul
if errorlevel 1 (
    echo WARNING: Blockchain API at localhost:8081 is not responding!
    echo The UI will use fallback mock data until the API is available.
    echo To run the blockchain node, open another terminal and run the node executable.
    echo.
)

echo Starting npm development server...
npm start 