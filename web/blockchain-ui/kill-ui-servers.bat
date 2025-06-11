@echo off
echo Looking for React development servers...

REM Kill any processes using ports 3000-3009 (React dev servers)
echo Checking port 3000...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3000') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    )
)

echo Checking port 3001...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3001') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    )
)

echo Checking port 3002...
for /f "tokens=5" %%a in ('netstat -ano ^| findstr :3002') do (
    echo Found process: %%a
    taskkill /F /PID %%a 2>nul
    if %errorlevel% equ 0 (
        echo Process %%a killed successfully
    )
)

REM Try to kill any node processes that might be React servers
echo.
echo Looking for node processes...
tasklist /fi "imagename eq node.exe" | find "node.exe" > nul
if %errorlevel% equ 0 (
    echo Found node processes, attempting to kill them...
    taskkill /F /IM node.exe 2>nul
    if %errorlevel% equ 0 (
        echo All node processes killed successfully
    ) else (
        echo Some node processes could not be killed
    )
) else (
    echo No node processes found
)

echo.
echo Process cleanup complete!
echo You can now safely start the UI again

REM Optional pause to see results
pause 