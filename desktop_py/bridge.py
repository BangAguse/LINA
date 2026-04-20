#!/usr/bin/env python3
"""
Bridge process for integrating LINA Python backend with C++ Qt app.
Protocol: JSON Lines on stdout/stderr and stdin.
Commands (stdin JSON):
  {"cmd":"collect"}
  {"cmd":"query","question":"..."}
Responses (stdout JSON):
  {"event":"collected","data":{...}}
  {"event":"ai_opening","text":"..."}
  {"event":"ai_response","text":"...","question":"..."}
  {"event":"error","message":"..."}

The bridge is headless and safe: collector is passive and AI uses local GGUF with fallback.
"""
import sys
import json
import threading
from pathlib import Path

from ai_engine import AIEngine
from collector import collect_network_metadata
from normalizer import normalize_network_data

BASE = Path(__file__).resolve().parents[1]
MODEL_PATH = str(BASE / 'models' / 'mistrallite.Q2_K.gguf')

ai = AIEngine(model_path=MODEL_PATH)
_latest_data = None


def send(obj):
    try:
        sys.stdout.write(json.dumps(obj, ensure_ascii=False) + "\n")
        sys.stdout.flush()
    except Exception:
        pass


def handle_collect():
    global _latest_data
    try:
        raw = collect_network_metadata()
        norm = normalize_network_data(raw)
        _latest_data = norm
        # Send collected event
        send({"event": "collected", "data": norm})
        # Build opening message and AI insight
        opening = []
        opening.append("LINA: Data siap dianalisis.")
        opening.append(f"- Jaringan terdeteksi: {len(norm.get('networks',[]))}")
        opening.append(f"- Perangkat terdeteksi: {len(norm.get('devices',[]))}")
        if norm.get('networks'):
            strongest = max(norm['networks'], key=lambda n: n.get('signal_strength', -999))
            opening.append(f"- Contoh jaringan terkuat: {strongest.get('ssid','(hidden)')} ({strongest.get('signal_strength')} dBm)")
        opening_text = "\n".join(opening)
        send({"event": "ai_opening", "text": opening_text})
        # Ask AI for an initial insight (non-hallucinating)
        try:
            ai_text = ai.query("Berikan ringkasan dan insight berdasarkan data yang diberikan.", norm)
            send({"event":"ai_response","text":ai_text, "question":"auto_summary"})
        except Exception as e:
            send({"event":"error","message": str(e)})
    except Exception as e:
        send({"event":"error","message": str(e)})


def handle_query(question: str):
    global _latest_data
    if _latest_data is None:
        send({"event":"ai_response","text":"Data tidak tersedia untuk menjawab pertanyaan ini.", "question": question})
        return
    try:
        ai_text = ai.query(question, _latest_data)
        send({"event":"ai_response","text": ai_text, "question": question})
    except Exception as e:
        send({"event":"error","message": str(e)})


def repl():
    # Read lines from stdin
    while True:
        line = sys.stdin.readline()
        if not line:
            break
        line = line.strip()
        if not line:
            continue
        try:
            obj = json.loads(line)
        except Exception as e:
            # Silently skip invalid JSON instead of sending error event
            # (could be partial reads or junk from subprocess output)
            print(f"[Bridge] Skipped invalid json: {e}", file=sys.stderr)
            continue
        cmd = obj.get('cmd')
        if cmd == 'collect':
            t = threading.Thread(target=handle_collect, daemon=True)
            t.start()
        elif cmd == 'query':
            q = obj.get('question','')
            t = threading.Thread(target=handle_query, args=(q,), daemon=True)
            t.start()
        else:
            send({"event":"error","message": f"unknown command: {cmd}"})


if __name__ == '__main__':
    repl()
