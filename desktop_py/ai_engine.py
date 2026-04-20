import json
import threading
import os
import sys

try:
    # Silence llama-cpp-python progress output
    os.environ['PYTHONUNBUFFERED'] = '1'
    from llama_cpp import Llama
    LLAMA_AVAILABLE = True
except Exception:
    LLAMA_AVAILABLE = False

try:
    import openai
    OPENAI_AVAILABLE = True
except Exception:
    OPENAI_AVAILABLE = False

class AIEngine:
    def __init__(self, model_path="models/mistrallite.Q2_K.gguf", api_key=None, use_api=False):
        """
        Initialize AIEngine with support for both local models and OpenAI API
        
        Args:
            model_path: Path to local GGUF model (default: mistrallite.Q2_K.gguf)
            api_key: OpenAI API key (optional, for API mode)
            use_api: If True, use OpenAI API instead of local model
        """
        self.model_path = model_path
        self.api_key = api_key
        self.use_api = use_api and OPENAI_AVAILABLE
        self._llm = None
        self._ready = False
        self.max_tokens = 512
        self.temperature = 0.2
        self.top_p = 0.9
        
        if self.use_api and self.api_key:
            # API mode
            try:
                openai.api_key = self.api_key
                self._ready = True
            except Exception as e:
                self._ready = False
                self.use_api = False
        elif LLAMA_AVAILABLE and not self.use_api:
            # Local model mode
            try:
                # Load model lazily in background
                t = threading.Thread(target=self._init_llm, daemon=True)
                t.start()
            except Exception:
                self._ready = False

    def _init_llm(self):
        try:
            # Suppress verbose llama-cpp output by redirecting stderr
            old_stderr = sys.stderr
            sys.stderr = open(os.devnull, 'w')
            try:
                self._llm = Llama(model_path=self.model_path, verbose=False)
                self._ready = True
                sys.stderr = old_stderr  # Restore stderr
                # Silently loaded (don't print to stdout, interferes with JSON-lines protocol)
            finally:
                sys.stderr = old_stderr
        except Exception as e:
            sys.stderr = old_stderr if 'old_stderr' in locals() else sys.stderr
            # Silently fail
            self._llm = None
            self._ready = False

    def _query_api(self, full_prompt: str, network_data: dict = None) -> str:
        """Query OpenAI API (compatible with openai >= 0.27.0)"""
        try:
            # Check if openai is configured properly
            if not self.api_key:
                return self._fallback_answer("", network_data or {})
            
            # Check if MOCK mode is enabled for testing (when API key invalid)
            if os.environ.get('LINA_API_MOCK_MODE') == '1':
                return self._mock_api_response(full_prompt, network_data)
            
            # OpenAI 2.x format with client
            try:
                from openai import OpenAI
                client = OpenAI(api_key=self.api_key)
                response = client.chat.completions.create(
                    model="gpt-3.5-turbo",
                    messages=[
                        {"role": "system", "content": "Anda adalah LINA (Learning Interconnected Network Analyzer), asisten AI yang membantu analisis jaringan dan menjawab pertanyaan networking dengan profesional dan ramah. PENTING: Analisis data jaringan yang diberikan dengan detail, berikan rekomendasi keamanan yang spesifik."},
                        {"role": "user", "content": full_prompt}
                    ],
                    temperature=self.temperature,
                    max_tokens=self.max_tokens,
                    top_p=self.top_p
                )
                return response.choices[0].message.content.strip()
            except ImportError:
                # Fallback untuk openai < 2.0
                import openai
                openai.api_key = self.api_key
                response = openai.ChatCompletion.create(
                    model="gpt-3.5-turbo",
                    messages=[
                        {"role": "system", "content": "Anda adalah LINA (Learning Interconnected Network Analyzer), asisten AI yang membantu analisis jaringan dan menjawab pertanyaan networking dengan profesional dan ramah. PENTING: Analisis data jaringan yang diberikan dengan detail, berikan rekomendasi keamanan yang spesifik."},
                        {"role": "user", "content": full_prompt}
                    ],
                    temperature=self.temperature,
                    max_tokens=self.max_tokens,
                    top_p=self.top_p
                )
                if response and 'choices' in response and len(response['choices']) > 0:
                    return response['choices'][0]['message']['content'].strip()
                return self._fallback_answer("", network_data or {})
        except Exception as e:
            # If API fails (auth error, rate limit, etc), try mock mode
            if os.environ.get('LINA_AUTO_MOCK_ON_ERROR') == '1':
                return self._mock_api_response(full_prompt, network_data)
            # Otherwise fallback gracefully
            return self._fallback_answer("", network_data or {})

    def is_ready(self):
        return self._ready or (not LLAMA_AVAILABLE)

    def build_system_prompt(self, collected_data_str):
        """Build system prompt for intelligent conversation"""
        system_prompt = (
            "Anda adalah LINA (Learning Interconnected Network Analyzer), asisten AI yang:"
            "\n\n1. NETWORK ANALYSIS MODE:"
            "\n   - Analisis jaringan & perangkat berdasarkan data terkumpul"
            "\n   - Berikan insight tentang keamanan, performa, & topologi"
            "\n   - Gunakan data lokal untuk menjawab pertanyaan spesifik\n"
            "\n2. GENERAL CONVERSATION MODE:"
            "\n   - Jawab pertanyaan umum tentang networking, teknologi, & security"
            "\n   - Bergabung dengan data lokal saat relevan"
            "\n   - Jadilah helpful & conversational\n"
            "\n3. SAFETY RULES:"
            "\n   - Jangan rekomendasikan aktivitas ilegal atau berbahaya"
            "\n   - Jangan bantu hacking atau exploitation"
            "\n   - Berikan resiko warnings jika diperlukan\n"
            "\n4. PERSONALITY:"
            "\n   - Profesional tapi ramah & approachable"
            "\n   - Gunakan Bahasa Indonesia atau English sesuai konteks"
            "\n   - Jika data tidak tersedia, tawarkan context lain yang berguna\n"
            "\nDATA JARINGAN LOKAL YANG TERKUMPUL:\n"
        )
        system_prompt += collected_data_str
        system_prompt += "\n-- AKHIR DATA LOKAL --\n\n"
        system_prompt += "Sekarang siap menjawab pertanyaan user. Gunakan data lokal jika relevan, tapi juga bisa expand dengan general knowledge untuk conversation lebih engaging.\n"
        return system_prompt

    def query(self, question: str, network_data: dict) -> str:
        # Build prompt
        data_str = json.dumps(network_data, indent=2, ensure_ascii=False)
        system_prompt = self.build_system_prompt(data_str)
        full_prompt = system_prompt + "\nPertanyaan: " + question + "\nJawaban:" 

        # If using API mode
        if self.use_api and self.api_key:
            return self._query_api(full_prompt, network_data)

        # Local model mode (existing logic)
        # Wait a bit for model to load if still loading in background
        if LLAMA_AVAILABLE and self._llm is None:
            import time
            for _ in range(30):  # wait max 30 seconds for model to load
                if self._llm is not None:
                    break
                time.sleep(0.1)

        # If llama isn't available or model not loaded after wait, fallback to smart answer
        if not LLAMA_AVAILABLE or self._llm is None:
            return self._fallback_answer(question, network_data)

        try:
            # Use create_completion (llama-cpp-python API)
            resp = self._llm.create_completion(prompt=full_prompt, max_tokens=self.max_tokens, temperature=self.temperature)
            # Extract text (llama-cpp-python returns dict with 'choices')
            if isinstance(resp, dict) and 'choices' in resp and len(resp['choices'])>0:
                text = resp['choices'][0].get('text', '').strip()
            else:
                text = str(resp)
            # Ensure response references data or says insufficient
            if not self._references_data(text):
                return self._fallback_answer(question, network_data)
            return text
        except Exception as e:
            # Silently fail to fallback (no print to stdout as it interferes with IPC protocol)
            return self._fallback_answer(question, network_data)

    def _references_data(self, text: str) -> bool:
        # Very simple heuristic: require mention of keywords or explicit data
        markers = ["jaringan", "perangkat", "MAC", "IP", "SSID", "terdeteksi", "signal", "volume"]
        t_low = text.lower()
        return any(m.lower() in t_low for m in markers)

    def _mock_api_response(self, full_prompt: str, network_data: dict = None) -> str:
        """
        Mock API response for testing without valid API key
        Simulates intelligent LINA responses using local analysis logic
        This is used when API fails or LINA_API_MOCK_MODE environment variable is set
        """
        if not network_data:
            network_data = {}
        
        networks = network_data.get('networks', [])
        devices = network_data.get('devices', [])
        
        # If no data, return helpful message
        if not networks and not devices:
            return "LINA (Mode Demo): Tidak ada data jaringan untuk dianalisis. Jalankan scan dengan menekan tombol START terlebih dahulu, kemudian saya akan memberikan analisis detail mengenai WiFi, perangkat, dan rekomendasi keamanan Anda."
        
        response = "LINA (Mode Demo/API): "
        
        # Analyze and provide insights
        response += f"Saya mendeteksi aktivitas jaringan di area Anda. Berikut analisisnya:\n\n"
        
        if networks:
            response += f"**📡 Jaringan WiFi:** {len(networks)} jaringan aktif\n"
            
            # Categorize by security
            wpa3 = [n for n in networks if 'WPA3' in n.get('security', '')]
            wpa2 = [n for n in networks if 'WPA2' in n.get('security', '') and 'WPA3' not in n.get('security', '')]
            open_nets = [n for n in networks if n.get('security', '').upper() == 'OPEN']
            
            if wpa3:
                response += f"  ✓ {len(wpa3)} dengan WPA3 (Sangat aman)\n"
            if wpa2:
                response += f"  ~ {len(wpa2)} dengan WPA2 (Cukup aman)\n"
            if open_nets:
                response += f"  ✗ {len(open_nets)} tanpa enkripsi (BERBAHAYA!)\n"
            
            # Signal analysis
            if networks:
                avg_signal = sum(n.get('signal_strength', -100) for n in networks) / len(networks)
                response += f"\n**📊 Kualitas Signal:** Rata-rata {int(avg_signal)}dBm\n"
                if avg_signal > -50:
                    response += "  ✓ Signal sangat kuat - koneksi stabil\n"
                elif avg_signal > -70:
                    response += "  ~ Signal cukup baik - performa normal\n"
                else:
                    response += "  ⚠️  Signal lemah - pertimbangkan relokasi router\n"
        
        if devices:
            response += f"\n**🖥️  Perangkat Terhubung:** {len(devices)} perangkat\n"
            # Show sample devices (max 3)
            for i, dev in enumerate(devices[:3], 1):
                ip = dev.get('ip', '?')
                response += f"  {i}. {ip}\n"
            if len(devices) > 3:
                response += f"  ... dan {len(devices)-3} perangkat lainnya\n"
        
        # Security recommendations
        response += f"\n**🔒 Rekomendasi Keamanan:**\n"
        if open_nets and networks:
            response += f"  ⚠️  PENTING: {len(open_nets)} jaringan tanpa enkripsi terdeteksi!\n"
            response += f"     Aktifkan WPA2/WPA3 segera untuk melindungi data Anda.\n"
        response += f"  • Gunakan password WiFi 12+ karakter (huruf, angka, simbol)\n"
        response += f"  • Update router firmware secara berkala\n"
        response += f"  • Nonaktifkan WPS dan remote management\n"
        response += f"  • Monitor perangkat yang terhubung secara rutin\n"
        
        response += f"\n💡 Catatan: Response ini adalah simulasi demo. Untuk analisis penuh dengan AI real-time, gunakan API key OpenAI yang valid."
        
        return response

    def _fallback_answer(self, question: str, network_data: dict) -> str:
        """
        Intelligent fallback response that handles:
        1. Network-specific questions (using collected data)
        2. General networking/tech knowledge questions
        3. Casual conversation
        """
        
        nets = network_data.get('networks', [])
        devs = network_data.get('devices', [])
        q_lower = question.lower()
        
        # === GENERAL KNOWLEDGE RESPONSES (CHECK FIRST) ===
        general_responses = {
            'apa itu': 'LINA: Saya bisa jelaskan! Networking adalah proses menghubungkan perangkat untuk berbagi data dan resource. Dalam konteks lokal, saya menganalisis WiFi, perangkat, dan konektivitas di area sekitar Anda.',
            'keamanan': 'LINA: Keamanan jaringan sangat penting! Tips: 1) Gunakan password WiFi yang kuat, 2) Update router secara berkala, 3) Aktifkan enkripsi (WPA2/WPA3), 4) Monitor perangkat yang terhubung. Saya bisa scan jaringan Anda untuk lebih detail!',
            'password': 'LINA: Password WiFi yang baik harus: minimal 12 karakter, kombinasi huruf/angka/simbol, tidak mudah ditebak. Ganti password setiap 6 bulan untuk keamanan lebih baik.',
            'router': 'LINA: Router adalah pusat jaringan Anda. Tips maintenance: 1) Reboot sebulan sekali, 2) Update firmware, 3) Posisikan di tempat sentral, 4) Jauh dari microwave dan perangkat lain. Mau saya scan sinyal router Anda?',
            'vpn': 'LINA: VPN (Virtual Private Network) mengenkripsi traffic internet Anda. Gunakan VPN untuk: keamanan WiFi publik, privasi online, akses konten geografis. Pilih VPN terpercaya dengan no-log policy.',
            'malware': 'LINA: Malware adalah software berbahaya. Pencegahan: 1) Install antivirus update, 2) Jangan klik link mencurigakan, 3) Download dari sumber resmi, 4) Aktifkan firewall. Saya fokus pada keamanan jaringan lokal Anda!',
            'hacking': 'LINA: Hacking adalah aktivitas ilegal. Saya tidak akan membantu aktivitas ilegal. Tapi saya bisa bantu Anda PROTEKSI jaringan dari hacker dengan analisis keamanan WiFi dan perangkat Anda!',
            'bandwidth': 'LINA: Bandwidth adalah kecepatan maksimal data jaringan Anda. Untuk optimal: 1) Kurangi perangkat terhubung, 2) Jangan streaming multiple devices, 3) Gunakan 5GHz jika tersedia, 4) Upgrade paket ISP jika perlu.',
            'dns': 'LINA: DNS (Domain Name System) menerjemahkan URL ke IP address. Tips: gunakan DNS publik seperti 8.8.8.8 atau 1.1.1.1 untuk kecepatan lebih baik. Saya fokus analisis local network, tapi DNS penting untuk internet speed!',
            'ip': 'LINA: IP address mengidentifikasi perangkat di jaringan. Private IPs (192.168.x.x, 10.x.x.x) untuk jaringan lokal. Public IP untuk internet. Saya scan dan tampilkan IP perangkat Anda yang terdeteksi!',
            'mac': 'LINA: MAC address identifikasi unik perangkat hardware. Berbeda dari IP, MAC hanya bekerja di jaringan lokal. Saya capture dan tampilkan MAC address perangkat di area Anda!',
            'hello': 'LINA: Halo! 👋 Saya LINA, asisten AI jaringan Anda. Saya bisa:\n• Scan & analisis WiFi di sekitar\n• Identifikasi perangkat terhubung\n• Monitor signal strength\n• Jawab pertanyaan networking\nKlik START untuk scan jaringan, atau tanya saya apapun tentang networking!',
            'hai': 'LINA: Hai! 👋 Ada yang bisa saya bantu? Saya bisa analisis WiFi, perangkat, signal strength, dan jawab pertanyaan networking. Apa yang ingin Anda ketahui?',
            'halo': 'LINA: Halo! Selamat datang di LINA. Saya siap membantu Anda memahami jaringan lokal. Tanya saya apapun atau klik START untuk mulai scan!',
            'siapa': 'LINA: Saya LINA (Learning Interconnected Network Analyzer). Saya AI lokal yang menganalisis jaringan dan perangkat sekitar Anda. Saya tidak kirim data ke cloud, semuanya lokal dan private. Mau mulai scan?',
            'bantuan': 'LINA: Berikut yang bisa saya bantu:\n📡 Analisis WiFi & signal strength\n🖥️  Scan perangkat terhubung\n🔒 Tips keamanan jaringan\n💡 Jawab pertanyaan networking\n\nKlik START untuk collect data, terus tanya saya apapun!',
            'help': 'LINA: I can help you with:\n📡 WiFi analysis & signal strength\n🖥️  Device detection\n🔒 Network security tips\n💡 Networking questions\n\nClick START to scan, then ask me anything!',
        }
        
        # Check general knowledge first (before network questions)
        for keyword, response_text in general_responses.items():
            if keyword.lower() in q_lower:
                return response_text
        
        # === NETWORK-SPECIFIC RESPONSES ===
        # Helper: Check if question is network-related
        network_keywords = ['jaringan', 'ssid', 'wifi', 'perangkat', 'device', 'ip', 'mac', 'signal', 'sinyal', 'kuat', 'internet', 'connection', 'connected', 'network']
        is_network_q = any(kw in q_lower for kw in network_keywords)
        
        if is_network_q:
            response = "LINA: "
            
            # Jaringan WiFi questions
            if any(k in q_lower for k in ['jaringan', 'ssid', 'wifi', 'network']):
                if nets:
                    response += f"Saya mendeteksi {len(nets)} jaringan WiFi di area Anda:\n"
                    for i, net in enumerate(nets, 1):
                        ssid = net.get('ssid', '(hidden)')
                        signal = net.get('signal_strength', '?')
                        security = net.get('security', '?')
                        response += f"  {i}. SSID: {ssid}\n"
                        response += f"     Signal: {signal} dBm | Security: {security}\n"
                    response += "\nAnda memiliki jaringan yang cukup kuat untuk koneksi stabil. Pastikan menggunakan password yang aman!"
                else:
                    response += "Tidak ada jaringan WiFi yang terdeteksi di sekitar. Periksa apakah WiFi Anda aktif atau coba scan ulang."
            
            # Perangkat/Device questions
            elif any(k in q_lower for k in ['perangkat', 'device', 'terhubung', 'connected']):
                if devs:
                    response += f"Ada {len(devs)} perangkat terdeteksi di jaringan Anda:\n"
                    for i, dev in enumerate(devs, 1):
                        ip = dev.get('ip', '?')
                        mac = dev.get('mac', '?')
                        iface = dev.get('iface', '?')
                        response += f"  {i}. IP: {ip}\n"
                        response += f"     MAC: {mac} ({iface})\n"
                    response += "\nPastikan Anda mengenali semua perangkat ini. Jika ada yang tidak dikenal, ada kemungkinan intrusi!"
                else:
                    response += "Tidak ada perangkat lain yang terdeteksi. Perangkat Anda mungkin terisolasi atau tidak semua perangkat broadcast IP mereka."
            
            # Signal strength questions
            elif any(k in q_lower for k in ['signal', 'sinyal', 'kuat', 'strength']):
                if nets:
                    strongest = max(nets, key=lambda n: n.get('signal_strength', -999))
                    weakest = min(nets, key=lambda n: n.get('signal_strength', 999))
                    avg_signal = sum(n.get('signal_strength', 0) for n in nets) / len(nets) if nets else 0
                    response += f"Analisis signal WiFi Anda:\n"
                    response += f"  Terukuat: {strongest.get('ssid')} ({strongest.get('signal_strength')} dBm)\n"
                    response += f"  Terlemah: {weakest.get('ssid')} ({weakest.get('signal_strength')} dBm)\n"
                    response += f"  Rata-rata: {int(avg_signal)} dBm\n\n"
                    if strongest.get('signal_strength', -100) > -50:
                        response += "✓ Signal Anda sangat kuat! Koneksi seharusnya stabil dan cepat."
                    elif strongest.get('signal_strength', -100) > -70:
                        response += "~ Signal cukup baik. Anda mungkin perlu posisikan lebih dekat ke router untuk performa optimal."
                    else:
                        response += "⚠ Signal lemah. Coba pindahkan router atau ganti lokasi untuk koneksi lebih stabil."
                else:
                    response += "Tidak ada jaringan untuk dianalisis. Lakukan scan jaringan terlebih dahulu dengan klik START."
            
            else:
                # Generic network data summary
                response = f"LINA: Berdasarkan scan jaringan saya:\n"
                if nets:
                    response += f"• {len(nets)} jaringan WiFi aktif\n"
                if devs:
                    response += f"• {len(devs)} perangkat terhubung\n"
                response += "\nTanya saya tentang jaringan, perangkat, atau signal strength untuk analisis lebih detail!"
            
            return response
        
        # === FALLBACK RESPONSE ===
        # If nothing matched, give helpful generic response
        return (
            f"LINA: Pertanyaan menarik! Saya spesialis dalam analisis jaringan lokal Anda.\n\n"
            f"Jika tentang jaringan/WiFi/perangkat: Saya bisa analisis detail berdasarkan data scan.\n"
            f"Jika pertanyaan umum: Saya tahu banyak tentang networking, keamanan, dan tech!\n\n"
            f"Tips: Klik START untuk scan jaringan terbaru, terus tanya saya apa yang ingin Anda ketahui. "
            f"Saya siap membantu! 😊"
        )

