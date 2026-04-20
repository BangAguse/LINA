#!/bin/bash
# LINA Desktop Build Script

set -e

echo "=========================================="
echo "LINA - Desktop Application Build Script"
echo "=========================================="

# Check dependencies
echo "[1/5] Checking dependencies..."
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found. Please install CMake."
    exit 1
fi

if ! pkg-config --exists Qt6Core 2>/dev/null; then
    echo "WARNING: Qt6 not found. Install with:"
    echo "  Ubuntu: sudo apt-get install qt6-base-dev"
    echo "  macOS: brew install qt"
fi

# Create build directory
echo "[2/5] Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "[3/5] Configuring with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
echo "[4/5] Building application..."
make -j$(nproc 2>/dev/null || echo 1)

# Check result
echo "[5/5] Build complete!"
if [ -f desktop/LINA ] || [ -f desktop/Release/LINA.exe ]; then
    echo ""
    echo "=========================================="
    echo "✅ BUILD SUCCESS!"
    echo "=========================================="
    echo ""
    if [ -f desktop/LINA ]; then
        echo "Run with: ./build/desktop/LINA"
    else
        echo "Run with: .\\build\\desktop\\Release\\LINA.exe"
    fi
else
    echo ""
    echo "=========================================="
    echo "❌ BUILD FAILED"
    echo "=========================================="
    exit 1
fi
