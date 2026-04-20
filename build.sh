#!/bin/bash
# LINA Build and Run Script
# Builds C++ desktop app and sets up Python environment

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo "=========================================="
echo "LINA - Build and Setup Script"
echo "=========================================="

# Check dependencies
echo ""
echo "[1/5] Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo "✗ cmake not found. Install: sudo apt-get install cmake"
    exit 1
fi
echo "✓ cmake found: $(cmake --version | head -1)"

if ! command -v make &> /dev/null; then
    echo "✗ make not found. Install: sudo apt-get install make"
    exit 1
fi
echo "✓ make found"

if ! command -v python3 &> /dev/null; then
    echo "✗ python3 not found. Install: sudo apt-get install python3"
    exit 1
fi
echo "✓ python3 found: $(python3 --version)"

# Setup Python environment
echo ""
echo "[2/5] Setting up Python environment..."

if [ ! -d ".venv" ]; then
    python3 -m venv .venv
    echo "✓ Virtual environment created"
fi

source .venv/bin/activate
echo "✓ Virtual environment activated"

echo "✓ Installing Python dependencies..."
pip install -q -r desktop_py/requirements.txt
echo "✓ Dependencies installed"

# Build C++
echo ""
echo "[3/5] Configuring C++ build..."

if [ ! -d "build" ]; then
    mkdir -p build
fi

cd build
cmake .. > /dev/null 2>&1 || (echo "✗ CMake configuration failed"; exit 1)
echo "✓ CMake configuration complete"

echo ""
echo "[4/5] Compiling C++ application..."
make -j$(nproc) > /dev/null 2>&1 || (echo "✗ Build failed"; exit 1)
echo "✓ Build complete"

cd ..

# Verify binary
echo ""
echo "[5/5] Verifying build..."

if [ -f "build/desktop/LINA" ]; then
    echo "✓ LINA binary ready: $(ls -lh build/desktop/LINA | awk '{print $5}')"
else
    echo "✗ LINA binary not found"
    exit 1
fi

echo ""
echo "=========================================="
echo "✓ Build complete!"
echo "=========================================="
echo ""
echo "To run LINA Desktop GUI:"
echo "  source .venv/bin/activate"
echo "  ./run_lina.sh"
echo ""
echo "To test backend only (no GUI):"
echo "  source .venv/bin/activate"
echo "  python3 test_backend.py"
echo ""
