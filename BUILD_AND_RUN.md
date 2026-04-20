# LINA Desktop - Complete Build & Run Guide

## Overview
LINA (Learning Interconnected Network Analyzer) adalah aplikasi desktop defensif berbasis AI yang menganalisis metadata jaringan lokal secara pasif. Aplikasi ini menggabungkan:
- **GUI Desktop:** Qt 6 (C++)
- **Backend AI:** Python dengan llama-cpp-python (model GGUF lokal)
- **Network Collector:** Passive read-only data collection
- **Language Support:** Indonesian (ID), English (EN), Arabic (AR) dengan RTL

## Prerequisites

### System Requirements
- **OS:** Linux (Ubuntu 20.04+)
- **RAM:** 4GB minimum (8GB+ recommended for AI model)
- **Disk:** 2GB free space

### Required Packages
```bash
# Qt6 Development
sudo apt-get update
sudo apt-get install -y \
    qt6-base-dev \
    qt6-tools-dev \
    libqt6core6 \
    libqt6gui6 \
    libqt6widgets6

# Build tools
sudo apt-get install -y \
    cmake \
    make \
    g++ \
    pkg-config

# JSON library
sudo apt-get install -y libjsoncpp-dev

# Python
sudo apt-get install -y python3 python3-venv python3-pip
```

## Quick Start

### Option 1: Automated Build
```bash
cd /path/to/LINA
chmod +x build.sh
./build.sh
```

This will:
1. Check all dependencies
2. Create Python virtual environment
3. Install Python packages
4. Build C++ application
5. Verify binary is ready

### Option 2: Manual Build

#### 1. Setup Python Environment
```bash
cd /path/to/LINA
python3 -m venv .venv
source .venv/bin/activate
pip install -r desktop_py/requirements.txt
```

#### 2. Build C++
```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..
```

#### 3. Run Application
```bash
source .venv/bin/activate
./run_lina.sh
```

## Usage

### Starting LINA Desktop
```bash
# Ensure Python env is activated
source .venv/bin/activate

# Run the application
./run_lina.sh
```

The GUI will:
1. Load with LINA icon (from resources/)
2. Display in English by default
3. Show START button ready to collect network data

### Using LINA

1. **Start Collection**
   - Click "▶ START COLLECTION" button
   - LINA passively collects network metadata (no packet inspection)
   - Status changes to "🟢 Active (Collecting data)"

2. **View Auto-Opening Message**
   - LINA automatically sends opening message when data is collected
   - Shows summary: network count, device count, signal strength
   - Displays in the Chat section on the right

3. **Ask Questions**
   - Type question in input field at bottom right
   - Click "📤 SEND MESSAGE" button
   - LINA responds based on collected network data (not hallucinatory)
   - If data insufficient, LINA explicitly says so

4. **Change Language**
   - Dropdown at top right: "🇮🇩 Indonesian" / "🇬🇧 English" / "🇸🇦 العربية"
   - UI updates in real-time
   - Arabic (ar_SA) enables RTL layout automatically

### Available Data
LINA collects (read-only):
- **Networks:** WiFi SSIDs, signal strength, security type
- **Devices:** Local IP addresses, MAC addresses, interface names
- **Protocols:** Traffic volume summary, TCP/UDP statistics
- **Interfaces:** Network interface information and statistics

## Testing Without GUI

To test the backend (collector, normalizer, AI) without requiring a display:
```bash
source .venv/bin/activate
python3 test_backend.py
```

This will verify:
- ✓ Network data collection works
- ✓ Data normalization works
- ✓ AI model loads and can generate responses
- ✓ Opening message and Q&A functionality

## File Structure

```
LINA/
├── build/                          # Build output
│   ├── desktop/LINA        # Main executable
│   ├── icon.svg/icon.ico          # Application icons
│   ├── models/                     # AI model (GGUF)
│   ├── languages/                  # Language files
│   └── desktop_py/                 # Python backend
│       ├── bridge.py               # IPC bridge
│       ├── ai_engine.py            # AI wrapper
│       ├── collector.py            # Network collector
│       └── normalizer.py           # Data normalizer
├── desktop/                        # C++ Desktop GUI source
│   └── src/
│       ├── main.cpp
│       ├── main_window.cpp/h       # GUI implementation
│       └── qml_bridge.cpp/h
├── desktop_py/                     # Python backend source
│   ├── bridge.py                   # JSON-lines IPC protocol
│   ├── ai_engine.py                # llama-cpp-python wrapper
│   ├── collector.py                # Passive network metadata
│   ├── normalizer.py               # Data validation/normalization
│   ├── lang/                       # Language files
│   │   ├── en_US.json
│   │   ├── id_ID.json
│   │   └── ar_SA.json
│   └── requirements.txt
├── core/                           # C++ Core engine
│   └── src/
│       ├── network_collector.cpp/h
│       ├── data_normalizer.cpp/h
│       ├── ai_engine.cpp/h
│       └── language_manager.cpp/h
├── models/                         # AI Model
│   └── mistrallite.Q2_K.gguf      # Local GGUF model
├── resources/                      # Resources
│   ├── icon.svg
│   └── icon.ico
├── build.sh                        # Automated build script
├── run_lina.sh                     # Application launcher
├── test_backend.py                 # Backend test suite
└── CMakeLists.txt
```

## Troubleshooting

### Issue: "python3: command not found"
**Solution:** Install Python 3
```bash
sudo apt-get install python3 python3-venv python3-pip
```

### Issue: "qt6-config: command not found" or Qt6 not found
**Solution:** Install Qt6 development
```bash
sudo apt-get install qt6-base-dev qt6-tools-dev
```

### Issue: "libjsoncpp.so not found"
**Solution:** Install jsoncpp library
```bash
sudo apt-get install libjsoncpp-dev
```

### Issue: GUI doesn't appear
**Possible causes & solutions:**
1. **No display (running remotely):** Use `python3 test_backend.py` instead
2. **Display server issue:** Ensure `DISPLAY` is set (usually `:0` or `:1`)
3. **Library conflicts (Snap):** Application launcher handles this automatically

### Issue: Bridge process won't start
**Debug steps:**
```bash
# Test bridge directly
python3 -c "from desktop_py.bridge import *; print('Bridge imports OK')"

# Test with manual input
printf '{"cmd":"collect"}\n' | python3 desktop_py/bridge.py
```

### Issue: AI model not loading
**Possible causes:**
1. Model file missing: Check `build/models/mistrallite.Q2_K.gguf` exists
2. llama-cpp-python not installed: Run `pip install llama-cpp-python`
3. Model file corrupt: Verify with `ls -lh build/models/`

**Fallback:** If model cannot load, AI will use safe deterministic fallback that only summarizes collected data (no hallucination).

## Security & Ethics

LINA is a **defensive, read-only** application:
- ✓ **Passive observation only** - No active probing
- ✓ **No packet payload inspection** - Only metadata
- ✓ **No HTTPS decryption** - Respects encryption
- ✓ **No brute force** - Educational tool only
- ✓ **No credential harvesting** - Focuses on network topology
- ✓ **Local AI only** - No cloud upload, all processing local

## Performance Notes

- **First startup:** AI model loading may take 30-60 seconds (first time only)
- **Data collection:** ~2-5 seconds (depends on network size)
- **AI response time:** 5-15 seconds (depends on question complexity)
- **Memory usage:** ~500MB base + model size (varies by model)

## Advanced Usage

### Custom AI Model
To use a different GGUF model:
1. Place model in `build/models/yourmodel.gguf`
2. Update `desktop/src/main_window.cpp` line ~24:
   ```cpp
   ai_engine = std::make_unique<AIEngine>("models/yourmodel.gguf");
   ```
3. Rebuild: `cd build && make LINA`

### Custom Language
To add a new language (e.g., Spanish):
1. Create `desktop_py/lang/es_ES.json` with translations
2. Update language selector in `desktop/src/main_window.cpp` (~125):
   ```cpp
   language_selector->addItem("🇪🇸 Español", "es_ES");
   ```
3. Rebuild and restart

## Support & Documentation

- **Architecture:** See `ARCHITECTURE.md`
- **Completion Report:** See `COMPLETION_REPORT.md`
- **Implementation Notes:** See `IMPLEMENTATION_NOTES.md`
- **Deliverables:** See `DELIVERABLES.md`

## License & Attribution

LINA uses:
- **Qt 6:** LGPL 3.0+
- **jsoncpp:** MIT
- **llama.cpp:** MIT
- **Mistral 7B Lite GGUF:** Model-specific license

Refer to respective projects for license details.

---

**Last Updated:** 2026-02-01  
**Status:** Ready for Production  
**Version:** 1.0.0 MVP
