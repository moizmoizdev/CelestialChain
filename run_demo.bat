@echo off
echo Starting Blockchain Demo

REM Check if the executables exist
if not exist "blockchain_demo.exe" (
    echo Error: blockchain_demo.exe not found.
    echo Please compile the project first by running compile.bat
    exit /b 1
)

REM Start the blockchain demo
start "Blockchain Demo" cmd /k "blockchain_demo.exe"

echo Started blockchain demo.
echo You can interact with it in the new window. 