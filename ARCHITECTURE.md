# LINA (Learning Interconnected Network Analyzer)
## Dokumentasi Arsitektur Lengkap

---

## 1. RINGKASAN PROYEK

**LINA** adalah aplikasi defensive, read-only, berbasis AI lokal yang menganalisis metadata jaringan sekitar secara pasif.

**Konsep Inti:**
- User menekan START
- Aplikasi mengumpulkan metadata jaringan (passive observation)
- Data dinormalisasi & disanitasi
- Disubmit ke AI lokal (mistrallite.Q2_K.gguf)
- User mengajukan pertanyaan
- AI menjawab berdasarkan data yang dikumpulkan

**Platform:**
- Desktop (Windows/Linux/macOS)

---

## 2. ARSITEKTUR SISTEM

```
┌─────────────────────────────────────────────────────────┐
│                    UI LAYER                             │
├──────────────────────────────────────────────────────────┤
│                   Desktop UI (Qt C++)                    │
└──────────────────────────────────────────────────────────┘
                       ↓
┌─────────────────────────────────────────────────────────┐
│              CORE BUSINESS LOGIC LAYER                   │
├──────────────────┬──────────────┬──────────────────────┤
│ Network          │ Data          │ AI Engine            │
│ Collector        │ Normalizer    │ (llama.cpp)         │
│                  │               │                      │
│ • WiFi scan      │ • Validation  │ • Load GGUF model   │
│ • Devices scan   │ • Sanitize    │ • Process queries   │
│ • Signal strength│ • Format data │ • Constrain output  │
└──────────────────┴──────────────┴──────────────────────┘
                       ↓
┌─────────────────────────────────────────────────────────┐
│              UTILITIES & SUPPORT                         │
├──────────────────┬──────────────┬──────────────────────┤
│ Language         │ Configuration │ Security            │
│ Manager          │ Manager       │ Module              │
│                  │               │                      │
│ • i18n (3 langs) │ • JSON config │ • Validation        │
│ • RTL support    │ • Settings    │ • Rate limiting     │
└──────────────────┴──────────────┴──────────────────────┘
```

---

## 3. KOMPONEN DETAIL

### 3.1 Network Collector (Core Engine)
**Fungsi:** Mengumpulkan metadata jaringan secara pasif

**Fitur:**
- Scanning WiFi networks (SSID, BSSID, signal strength)
- Scanning nearby Bluetooth devices
- Detecting connected devices (via ARP scanning - passive)
- DNS cache analysis
- Network interface information

**Output:**
```json
{
  "timestamp": "2026-01-31T10:30:00Z",
  "networks": [
    {
      "ssid": "Network1",
      "bssid": "AA:BB:CC:DD:EE:FF",
      "signal": -45,
      "frequency": 2400,
      "security": "WPA2"
    }
  ],
  "devices": [
    {
      "mac": "11:22:33:44:55:66",
      "ip": "192.168.1.100",
      "hostname": "device-name",
      "type": "smartphone"
    }
  ]
}
```

### 3.2 Data Normalizer (Core Engine)
**Fungsi:** Membersihkan, validasi, dan format data

**Proses:**
1. Validasi struktur JSON
2. Sanitasi string (remove special chars)
3. Normalisasi IP addresses
4. MAC address formatting
5. Signal strength normalization
6. Remove duplicate entries
7. Add metadata (collection timestamp, source)

### 3.3 AI Engine (llama.cpp Integration)
**Model:** mistrallite.Q2_K.gguf

**System Prompt:**
```
Anda adalah LINA, asisten AI jaringan yang defensive dan read-only.
Peran Anda:
- Menjawab pertanyaan tentang jaringan lokal HANYA berdasarkan data yang disediakan
- Tidak pernah membuat asumsi di luar data
- Jika data tidak cukup, katakan "Data tidak tersedia"
- Fokus pada keamanan dan privasi
- Tidak pernah memberikan saran untuk exploit atau attack
- Gunakan bahasa yang professional dan educational

Data jaringan saat ini:
{COLLECTED_DATA}

Pertanyaan user:
{USER_QUESTION}
```

**Constraints:**
- Max tokens: 500
- Temperature: 0.3 (deterministic)
- Top-p: 0.9
- Timeout: 30 detik

### 3.4 Language Manager
**Bahasa Support:**
1. Indonesian (id_ID)
2. English (en_US)
3. Arabic (ar_SA) - dengan RTL layout

**Struktur File Lokalisasi:**
```
languages/
├── id_ID.json
├── en_US.json
└── ar_SA.json
```

---

## 4. DESKTOP UI (Qt C++)

### Layout Wireframe

```
┌─────────────────────────────────────────────────────────┐
│ 🔷 LINA        Settings | 🌐 Indonesian ▼              │
├──────────────────────────────────────────────────────────┤
│                                                          │
│ LEFT PANEL (300px)        │    RIGHT PANEL (flex)       │
│ ─────────────────────────────────────────────────────   │
│                           │                             │
│ Status: 🟢 Active         │   Chat with AI LINA        │
│                           │                             │
│ ┌─────────────────────┐   │   ┌──────────────────────┐ │
│ │   [START] [STOP]    │   │   │ Previous responses   │ │
│ └─────────────────────┘   │   │ ...                  │ │
│                           │   │                      │ │
│ Live Metadata Log:        │   │                      │ │
│ ┌─────────────────────┐   │   └──────────────────────┘ │
│ │ 10:30:15 WiFi scan  │   │                             │
│ │ 10:30:20 Device +1  │   │ ┌──────────────────────┐   │
│ │ 10:30:25 DNS cache  │   │ │ Type your question.. │   │
│ │ ...                 │   │ │              [Send]  │   │
│ └─────────────────────┘   │ └──────────────────────┘   │
│                           │                             │
│ Network Summary:          │                             │
│ • Networks: 4             │                             │
│ • Devices: 12             │                             │
│ • Signal avg: -50dBm      │                             │
│                           │                             │
└──────────────────────────────────────────────────────────┘
```

### Desktop Komponen:
- **Top Bar:** Logo, App name, Language selector, Settings
- **Status Indicator:** Connected/Scanning/Idle
- **START/STOP Buttons:** Control metadata collection
- **Live Log:** Real-time events
- **Network Summary:** Statistics
- **Chat Interface:** Q&A dengan AI LINA

---

## 4. DESKTOP UI (Qt C++)

### Layout Wireframe

```
┌─────────────────────────────────────────────────────────┐
│ 🔷 LINA        Settings | 🌐 Indonesian ▼              │
├──────────────────────────────────────────────────────────┤
│                                                          │
│ LEFT PANEL (300px)        │    RIGHT PANEL (flex)       │
│ ─────────────────────────────────────────────────────   │
│                           │                             │
│ Status: 🟢 Active         │   Chat with AI LINA        │
│                           │                             │
│ ┌─────────────────────┐   │   ┌──────────────────────┐ │
│ │   [START] [STOP]    │   │   │ Previous responses   │ │
│ └─────────────────────┘   │   │ ...                  │ │
│                           │   │                      │ │
│ Live Metadata Log:        │   │                      │ │
│ ┌─────────────────────┐   │   └──────────────────────┘ │
│ │ 10:30:15 WiFi scan  │   │                             │
│ │ 10:30:20 Device +1  │   │ ┌──────────────────────┐   │
│ │ 10:30:25 DNS cache  │   │ │ Type your question.. │   │
│ │ ...                 │   │ │              [Send]  │   │
│ └─────────────────────┘   │ └──────────────────────┘   │
│                           │                             │
│ Network Summary:          │                             │
│ • Networks: 4             │                             │
│ • Devices: 12             │                             │
│ • Signal avg: -50dBm      │                             │
│                           │                             │
└──────────────────────────────────────────────────────────┘
```

### Desktop Komponen:
- **Top Bar:** Logo, App name, Language selector, Settings
- **Status Indicator:** Connected/Scanning/Idle
- **START/STOP Buttons:** Control metadata collection
- **Live Log:** Real-time events
- **Network Summary:** Statistics
- **Chat Interface:** Q&A dengan AI LINA

---

## 5. SECURITY GUIDELINES

**Passive Collection HANYA:**
- ✅ WiFi networks (public broadcast)
- ✅ Device MACs (passive scan)
- ✅ Signal strength (passive)
- ✅ Device hostnames (from ARP responses)
- ✅ Network interfaces info

**KETAT DILARANG:**
- ❌ Packet sniffing/inspection
- ❌ Man-in-the-middle
- ❌ Brute force attempts
- ❌ Password harvesting
- ❌ Exploitation attempts
- ❌ Active probing (nmap-style)

---

## 6. ALUR USER

### Desktop
1. User membuka aplikasi → Welcome screen
2. Pilih bahasa (Indo/English/Arabic)
3. Main interface tampil
4. User klik START → data collection dimulai
5. Live log muncul
6. Network summary diupdate real-time
7. User bertanya di chat panel
8. AI LINA menjawab berdasarkan data

---

## 7. STRUKTUR PROYEK

```
LINA/
├── core/                           # Shared logic
│   ├── src/
│   │   ├── network_collector.cpp
│   │   ├── network_collector.h
│   │   ├── data_normalizer.cpp
│   │   ├── data_normalizer.h
│   │   ├── ai_engine.cpp
│   │   ├── ai_engine.h
│   │   ├── language_manager.cpp
│   │   └── language_manager.h
│   ├── languages/
│   │   ├── id_ID.json
│   │   ├── en_US.json
│   │   └── ar_SA.json
│   └── CMakeLists.txt
│
├── desktop/                        # Qt Desktop App
│   ├── src/
│   │   ├── main.cpp
│   │   ├── main_window.cpp
│   │   ├── main_window.h
│   │   ├── main_window.ui
│   │   ├── chat_widget.cpp
│   │   ├── chat_widget.h
│   │   └── status_widget.cpp
│   ├── resources/
│   │   └── resources.qrc
│   ├── CMakeLists.txt
│   └── LINA.pro
│
├── models/
│   └── mistrallite.Q2_K.gguf       # AI Model
│
├── resources/
│   └── icon.svg                     # App Logo
│
├── ARCHITECTURE.md                  # This file
├── README.md
└── CMakeLists.txt                   # Root build
```

---

## 8. TECHNOLOGY STACK

| Layer | Technology | Bahasa |
|-------|-----------|--------|
| **Core Engine** | CMake, STL | C++ |
| **AI Runtime** | llama.cpp bindings | C++/Go |
| **Desktop UI** | Qt 6 | C++ |
| **Config** | JSON | - |
| **i18n** | Custom JSON | - |

---

## 9. IMPLEMENTASI PHASES

### Phase 1: Core Engine ✅
- Network Collector
- Data Normalizer
- AI Engine initialization
- Language Manager

### Phase 2: Desktop Application ✅
- Qt UI implementation
- Integration dengan core
- Chat interface
- Real-time updates

### Phase 3: Integration & Testing ✅
- Cross-platform testing
- Model loading verification
- Language switching
- Error handling

### Phase 4: Deployment ✅
- Desktop packaging
- Documentation
- Release

---

## 10. NOTES PENTING

- **Tidak ada cloud:** Semua processing lokal
- **Tidak ada external APIs:** Hanya mistrallite.Q2_K.gguf
- **Defensive only:** Read-only, tidak ada modifications
- **Professional focus:** Educational tool, bukan "hacker"
- **Privacy first:** Data tidak disimpan/dikirim

---

Dokumen ini akan diupdate seiring implementasi.
