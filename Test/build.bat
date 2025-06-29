@echo off
setlocal

rem Get the directory path of the batch script
set "SCRIPT_DIR=%~dp0"
rem Step 1: Remove build/ if it exists
if exist "%SCRIPT_DIR%build" (
    echo Deleting existing build directory...
    rmdir /s /q "%SCRIPT_DIR%build"
)

rem Step 2: Create build directory
mkdir "%SCRIPT_DIR%build"

rem Step 3: Run cmake to configure in Release mode
::cmake .. -DCMAKE_BUILD_TYPE=Release -DLOGUTIL_ENABLE_LOG=OFF
cmake -S %SCRIPT_DIR% -B %SCRIPT_DIR%build ^
    -DLOGUTIL_USE_STRUCTURED_LOGGING=ON ^
    -DLOGUTIL_SHORTEN_PATH=OFF ^
    -DLOGUTIL_PATH_DEPTH=3

cmake --build "%SCRIPT_DIR%build" --config Release -- /m

rem Step 4: Run the executable
echo Path: "%SCRIPT_DIR%build\Release\myapp.exe"
if exist "%SCRIPT_DIR%build\Release\myapp.exe" (
    echo Running myapp...
    "%SCRIPT_DIR%build\Release\myapp.exe"
) else (
    echo myapp.exe not found
)

endlocal