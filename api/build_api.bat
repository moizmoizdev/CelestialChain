@echo off
REM Build script for CelestialChain API with Crow

REM Ensure we have a clean build directory
if exist build rmdir /s /q build
mkdir build
cd build

REM Run CMake to generate build files
cmake .. -G "MinGW Makefiles"

REM Build the project
cmake --build . --config Release

REM Copy the executable to the parent directory
copy CelestialChainAPI.exe ..

cd ..

echo Build complete!
echo.
echo To run the API server: CelestialChainAPI.exe [port]
echo Default port is 8080 if not specified 