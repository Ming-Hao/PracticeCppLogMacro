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
    -DLOGUTIL_PATH_DEPTH=3 ^
    -DLOGUTIL_VERBOSE_LEVEL=1 ^
    -DLOGUTIL_ENABLE_WARN=ON ^
    -DLOGUTIL_ENABLE_ERROR=OFF ^
    -DLOGUTIL_INCLUDE_THREAD_ID=OFF ^
    -DLOGUTIL_INCLUDE_TIMESTAMP=OFF

if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

cmake --build "%SCRIPT_DIR%build" --config Release -- /m

if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

rem Step 4: Run the executable
echo Path: "%SCRIPT_DIR%build\Release\myapp.exe"
if exist "%SCRIPT_DIR%build\Release\myapp.exe" (
    echo Running myapp...
    "%SCRIPT_DIR%build\Release\myapp.exe"
) else (
    echo myapp.exe not found
    exit /b 1
)

endlocal