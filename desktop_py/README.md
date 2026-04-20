LINA - Desktop Python MVP

How to run (desktop MVP):

1. Create a virtualenv and activate it (recommended):

```bash
python3 -m venv .venv
source .venv/bin/activate
```

2. Install requirements:

```bash
pip install -r desktop_py/requirements.txt
```

3. Run the desktop app:

```bash
python desktop_py/main.py
```

Notes:
- The app uses the local GGUF model at `models/mistrallite.Q2_K.gguf` if `llama-cpp-python` is installed and the model can be loaded.
- If the model or runtime is not available, LINA will fallback to a deterministic, data-grounded summary engine (safe fallback).
- The collector is passive and read-only: it reads `ip neigh`, `nmcli` (if available), and `/proc/net/dev`. It does NOT perform active probing or packet inspection.
- UI supports Indonesian, English and Arabic (RTL).

Security & Ethics:
- LINA is strictly defensive and read-only. It never inspects packet payloads or performs network attacks.
