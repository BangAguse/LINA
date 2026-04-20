import sys
import json
import os
import threading
from pathlib import Path

from PySide6.QtWidgets import (QApplication, QWidget, QMainWindow, QLabel, QPushButton,
                               QVBoxLayout, QHBoxLayout, QTextEdit, QComboBox)
from PySide6.QtGui import QIcon
from PySide6.QtCore import Qt, QObject, Signal

from ai_engine import AIEngine
from collector import collect_network_metadata
from normalizer import normalize_network_data


class CollectorWorker(QObject):
    data_ready = Signal(dict)

    def __init__(self):
        super().__init__()
        self._running = False

    def start(self):
        # Run collection in background thread
        self._running = True
        t = threading.Thread(target=self._collect_once, daemon=True)
        t.start()

    def _collect_once(self):
        data = collect_network_metadata()
        self.data_ready.emit(data)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        base = Path(__file__).resolve().parents[1]
        self.icon_path = str(base / 'icon.svg') if (base / 'icon.svg').exists() else None

        self.setWindowTitle('LINA - Learning Interconnected Network Analyzer')
        if self.icon_path:
            self.setWindowIcon(QIcon(self.icon_path))

        # Core components
        # Check if API mode is enabled via environment variable
        use_api_mode = os.environ.get('LINA_USE_API') == '1'
        api_key = os.environ.get('LINA_API_KEY', '')
        
        if use_api_mode and api_key:
            # Use API mode
            self.ai = AIEngine(api_key=api_key, use_api=True)
            self.ai_mode = "API (OpenAI)"
        else:
            # Use local model mode (default)
            self.ai = AIEngine(model_path=str(base / 'models' / 'mistrallite.Q2_K.gguf'), use_api=False)
            self.ai_mode = "Local Model"
        
        self.collector = CollectorWorker()
        self.collector.data_ready.connect(self.on_data_ready)

        # UI
        self._build_ui()

        # State
        self.latest_data = None

    def _build_ui(self):
        central = QWidget()
        layout = QVBoxLayout(central)

        # Top row: title + language
        top = QHBoxLayout()
        self.title_label = QLabel('LINA')
        top.addWidget(self.title_label)
        top.addStretch()
        self.lang_selector = QComboBox()
        self.lang_selector.addItem('🇮🇩 Indonesian', 'id_ID')
        self.lang_selector.addItem('🇬🇧 English', 'en_US')
        self.lang_selector.addItem('🇸🇦 العربية', 'ar_SA')
        self.lang_selector.currentIndexChanged.connect(self.on_language_changed)
        top.addWidget(self.lang_selector)
        layout.addLayout(top)

        # Control row
        ctrl = QHBoxLayout()
        self.start_btn = QPushButton('▶ START')
        self.start_btn.clicked.connect(self.on_start)
        self.stop_btn = QPushButton('⏹ STOP')
        self.stop_btn.setEnabled(False)
        self.stop_btn.clicked.connect(self.on_stop)
        ctrl.addWidget(self.start_btn)
        ctrl.addWidget(self.stop_btn)
        layout.addLayout(ctrl)

        # Live log
        self.live_log = QTextEdit()
        self.live_log.setReadOnly(True)
        self.live_log.setMaximumHeight(180)
        layout.addWidget(self.live_log)

        # Chat
        self.chat = QTextEdit()
        self.chat.setReadOnly(True)
        layout.addWidget(self.chat)

        # Input
        self.input = QTextEdit()
        self.input.setMaximumHeight(100)
        layout.addWidget(self.input)

        send_row = QHBoxLayout()
        self.send_btn = QPushButton('📤 SEND')
        self.send_btn.clicked.connect(self.on_send)
        send_row.addStretch()
        send_row.addWidget(self.send_btn)
        layout.addLayout(send_row)

        self.setCentralWidget(central)
        # Load default language
        self.load_language('en_US')

    def on_start(self):
        self.live_log.append(f'[*] Starting passive collection...')
        self.live_log.append(f'[*] AI Mode: {self.ai_mode}')
        self.start_btn.setEnabled(False)
        self.stop_btn.setEnabled(True)
        # Start single-shot collection (passive)
        self.collector.start()

    def on_stop(self):
        self.live_log.append('[*] Stopping collection (no-op for passive collector)')
        self.start_btn.setEnabled(True)
        self.stop_btn.setEnabled(False)

    def on_data_ready(self, raw_data: dict):
        self.live_log.append('[*] Data collected')
        normalized = normalize_network_data(raw_data)
        self.latest_data = normalized
        # Auto-send opening message from LINA summarizing collected data
        opening = self._build_opening_message(normalized)
        # Query AI in background thread so UI stays responsive
        t = threading.Thread(target=self._ask_ai_and_display, args=("", normalized, opening), daemon=True)
        t.start()

    def _build_opening_message(self, normalized: dict) -> str:
        # Build a short human-friendly summary to show immediately
        nets = normalized.get('networks', [])
        devs = normalized.get('devices', [])
        parts = ["LINA: Data siap dianalisis."]
        parts.append(f"- Jaringan terdeteksi: {len(nets)}")
        parts.append(f"- Perangkat terdeteksi: {len(devs)}")
        if nets:
            strongest = max(nets, key=lambda n: n.get('signal_strength', -999))
            parts.append(f"- Contoh jaringan terkuat: {strongest.get('ssid','(hidden)')} ({strongest.get('signal_strength')} dBm)")
        return "\n".join(parts)

    def _ask_ai_and_display(self, question: str, normalized: dict, immediate_text: str = None):
        # Show opening message immediately
        if immediate_text:
            self._append_chat(immediate_text)
        # Build initial question to AI if question provided, otherwise a passive summary query
        user_q = question if question else "Berikan ringkasan dan insight berdasarkan data yang diberikan."
        ai_resp = self.ai.query(user_q, normalized)
        self._append_chat(str(ai_resp))

    def _append_chat(self, text: str):
        # Append text in main thread
        def _append():
            self.chat.append(text + "\n")
        QApplication.instance().postEvent(self.chat, _CallableEvent(_append))

    def on_send(self):
        question = self.input.toPlainText().strip()
        if not question:
            return
        self._append_chat("→ You:\n" + question)
        self.input.clear()
        # Ask AI in background
        t = threading.Thread(target=self._ask_ai_and_display, args=(question, self.latest_data or {}, None), daemon=True)
        t.start()

    def on_language_changed(self, idx):
        key = self.lang_selector.itemData(idx)
        self.load_language(key)

    def load_language(self, locale_key: str):
        base = Path(__file__).resolve().parents[1]
        lang_file = base / 'desktop_py' / 'lang' / (locale_key + '.json')
        if not lang_file.exists():
            return
        with open(lang_file, 'r', encoding='utf-8') as f:
            obj = json.load(f)
        self.title_label.setText(obj.get('app_name','LINA'))
        self.start_btn.setText(obj.get('btn_start','▶ START'))
        self.stop_btn.setText(obj.get('btn_stop','⏹ STOP'))
        self.send_btn.setText(obj.get('btn_send','📤 SEND'))
        self.live_log.setPlaceholderText(obj.get('chat_placeholder',''))
        self.input.setPlaceholderText(obj.get('input_placeholder',''))
        # RTL support
        if locale_key.startswith('ar'):
            QApplication.instance().setLayoutDirection(Qt.RightToLeft)
        else:
            QApplication.instance().setLayoutDirection(Qt.LeftToRight)


# Helper to post a callable to Qt event loop via custom event
from PySide6.QtCore import QEvent
class _CallableEvent(QEvent):
    EVENT_TYPE = QEvent.Type(QEvent.registerEventType())
    def __init__(self, fn):
        super().__init__(self.EVENT_TYPE)
        self.fn = fn

    def call(self):
        try:
            self.fn()
        except Exception as e:
            print('Event call error', e)


# Patch QTextEdit to handle our callable event
from PySide6.QtWidgets import QTextEdit
old_event = QTextEdit.event

def _patched_event(self, ev):
    if isinstance(ev, _CallableEvent):
        ev.call()
        return True
    return old_event(self, ev)

QTextEdit.event = _patched_event


def main():
    app = QApplication(sys.argv)
    w = MainWindow()
    w.resize(1000, 700)
    w.show()
    sys.exit(app.exec())

if __name__ == '__main__':
    main()
