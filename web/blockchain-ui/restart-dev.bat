@echo off
echo Restarting React development server...

:: Check for processes on port 3000 and kill them
echo Checking for processes using port 3000...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3000') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    ) else (
        echo Failed to kill process %%a
    )
)

:: Run the API check script first
echo Running API check...
powershell -ExecutionPolicy Bypass -File .\check-api.ps1

:: Wait 1 second
timeout /t 1 /nobreak > nul

:: Start the development server
echo Starting development server...
npm start 