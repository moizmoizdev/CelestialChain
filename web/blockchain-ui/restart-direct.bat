@echo off
echo Restarting React app with direct API connections...

:: Kill any existing instances on port 3000
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3000') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
)

:: Delete the localStorage data to reset node connections
echo Cleaning browser localStorage data...
echo Please manually clear your browser's localStorage data for localhost:3000 or open in incognito mode.
echo.

:: Start the React app
echo Starting React app with direct API connections...
set BROWSER=none
npm start 