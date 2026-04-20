#!/bin/bash
# LINA Desktop Application Runner
# Handles environment setup and launches the Qt GUI with Python bridge

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Ensure Python 3 is available
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 not found. Please install Python 3."
    exit 1
fi

# Check if we're in the build directory or project root
if [ ! -f "desktop_py/bridge.py" ] && [ -d "../desktop_py" ]; then
    cd ..
fi

# Ensure all required files are in place
if [ ! -f "desktop_py/bridge.py" ]; then
    echo "Error: desktop_py/bridge.py not found. Please run from project root or build folder."
    exit 1
fi

echo "[LINA] Starting Desktop Application..."
echo "[LINA] Working directory: $(pwd)"
echo "[LINA] Python: $(python3 --version)"

# Prepare a clean environment to avoid snap/glibc conflicts while preserving display variables
# Preserve DISPLAY and XAUTHORITY to allow GUI rendering
SAVE_DISPLAY="$DISPLAY"
SAVE_XAUTHORITY="$XAUTHORITY"

# Unset environment variables that may interfere
unset LD_LIBRARY_PATH
unset SNAP
unset LD_PRELOAD

# Activate venv if present
if [ -f ".venv/bin/activate" ]; then
    # shellcheck disable=SC1091
    source .venv/bin/activate
    PYTHON_BIN="$VIRTUAL_ENV/bin/python3"
else
    PYTHON_BIN="$(command -v python3)"
fi

# Try to run the binary (renamed to LINA)
if [ -f "build/desktop/LINA" ]; then
    BIN="build/desktop/LINA"
elif [ -f "desktop/LINA" ]; then
    BIN="desktop/LINA"
elif [ -f "LINA" ]; then
    BIN="LINA"
else
    echo "Error: LINA binary not found. Please build first."
    echo "Run: cd build && cmake .. && make"
    exit 1
fi

# Restore minimal display env for GUI
export DISPLAY="$SAVE_DISPLAY"
if [ -n "$SAVE_XAUTHORITY" ]; then
    export XAUTHORITY="$SAVE_XAUTHORITY"
fi

# Launch in a cleaned environment while keeping PATH and DISPLAY
echo "[LINA] Launching: $BIN"
env -u SNAP -u LD_LIBRARY_PATH -u LD_PRELOAD PATH="$PATH" DISPLAY="$DISPLAY" XAUTHORITY="$XAUTHORITY" "$BIN" "$@"
