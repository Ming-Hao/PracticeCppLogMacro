#!/bin/bash

set -euo pipefail

# Get the directory path of the script
SCRIPT_DIR=$(dirname "$0")

# Step 1: Remove build/ if it exists
if [ -d "$SCRIPT_DIR/build" ]; then
    echo "Deleting existing build directory..."
    rm -rf "$SCRIPT_DIR/build"
fi

# Step 2: Create build directory
mkdir "$SCRIPT_DIR/build"

# Step 3: Run cmake to configure in Release mode
echo "[Step] Configuring CMake..."
cmake -S "$SCRIPT_DIR" -B "$SCRIPT_DIR/build" \
    -DLOGUTIL_USE_STRUCTURED_LOGGING=ON \
    -DLOGUTIL_SHORTEN_PATH=OFF \
    -DLOGUTIL_PATH_DEPTH=3 \
    -DLOGUTIL_VERBOSE_LEVEL=1 \
    -DLOGUTIL_ENABLE_WARN=ON \
    -DLOGUTIL_ENABLE_ERROR=OFF \
    -DLOGUTIL_INCLUDE_THREAD_ID=OFF \
    -DLOGUTIL_INCLUDE_TIMESTAMP=OFF

echo "[Step] Building..."
cmake --build "$SCRIPT_DIR/build" --config Release -- -j

# Step 4: Run the executable
echo "Path: $SCRIPT_DIR/build/myapp"
if [ -f "$SCRIPT_DIR/build/myapp" ]; then
    echo "Running myapp..."
    "$SCRIPT_DIR/build/myapp"
else
    echo "myapp not found"
    exit 1
fi