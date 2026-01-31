<div align="center">

  <!-- Banner -->
  <img src="banner.png" alt="LINA Banner" width="100%" />

  <h1>Learning Interconnected Network Analyzer</h1>
  <h2>“Aku bikin LINA (Learning Interconnected Network Analyzer) terinspirasi dari ibuku, Herlina Ningsi. Karena beliau pelindungku di dunia nyata, software ini aku dedikasikan untuk jadi pelindung di dunia digital.” 🤍🛡️</h2>

  <p>
    AI lokal berbasis GUI untuk memahami jaringan di sekitar perangkat secara aman, pasif, dan manusiawi.
  </p>

  <!-- Badges -->
  <p>
    <img src="https://img.shields.io/badge/AI-Local_GGUF-blue" />
    <img src="https://img.shields.io/badge/Security-Defensive_Only-green" />
    <img src="https://img.shields.io/badge/GUI-Desktop-orange" />
  </p>

  <!-- Author -->
  <p>
    Dibuat oleh <a href="https://github.com/BangAguse"><strong>@BangAguse</strong></a>
  </p>

</div>

<hr/>

## 🧠 Apa itu LINA?

<b>LINA (Learning Interconnected Network Analyzer)</b> adalah aplikasi desktop berbasis AI lokal yang membantu pengguna memahami apa yang terjadi di jaringan di sekitar perangkat mereka.  
Pengguna cukup menekan tombol <b>START</b>, lalu LINA akan mengamati aktivitas jaringan secara pasif dan menjelaskannya dengan bahasa yang mudah dipahami.

<hr/>

## 🎥 Cara Kerja (Visual)

<div align="center">

  <img src="demo-start.gif" alt="Start Scanning" width="80%" />
  <p><i>User membuka aplikasi dan menekan START</i></p>

  <img src="demo-analysis.gif" alt="AI Analysis" width="80%" />
  <p><i>LINA menganalisis dan menjelaskan data jaringan</i></p>

</div>

<hr/>

## 🔁 Alur Kerja LINA

<ul>
  <li>User membuka aplikasi GUI</li>
  <li>Memilih bahasa (Indonesia / English / العربية)</li>
  <li>Menekan tombol <b>START</b></li>
  <li>LINA mengumpulkan metadata jaringan secara pasif</li>
  <li>Data dinormalisasi & disanitasi</li>
  <li>Data disetor ke AI lokal (GGUF)</li>
  <li>User bertanya, LINA menjawab berdasarkan data</li>
</ul>

<hr/>

## 📊 Visualisasi Konsep

<div align="center">

  <!-- Grafik Konsep (SVG sederhana, bisa diganti nanti) -->
  <svg width="600" height="220">
    <rect x="10" y="60" width="150" height="60" fill="#1f2937"/>
    <rect x="220" y="60" width="150" height="60" fill="#2563eb"/>
    <rect x="430" y="60" width="150" height="60" fill="#059669"/>

    <text x="35" y="95" fill="white">Network Data</text>
    <text x="250" y="95" fill="white">LINA AI</text>
    <text x="455" y="95" fill="white">Human Insight</text>

    <line x1="160" y1="90" x2="220" y2="90" stroke="black" />
    <line x1="370" y1="90" x2="430" y2="90" stroke="black" />
  </svg>

  <p><i>Dari data mentah → AI lokal → pemahaman manusia</i></p>

</div>

<hr/>

## 🛡️ Prinsip Keamanan & Etika

<ul>
  <li>✅ Read-only & passive observation</li>
  <li>❌ Tidak ada payload inspection</li>
  <li>❌ Tidak ada eksploitasi</li>
  <li>❌ Tidak ada brute force</li>
  <li>❌ Tidak ada cloud / API eksternal</li>
</ul>

<hr/>

## 🧩 Teknologi

<ul>
  <li>Local LLM (GGUF) – <code>mistrallite.Q2_K.gguf</code></li>
  <li>Desktop GUI Application</li>
  <li>Offline-first architecture</li>
  <li>Multi-language UI (ID / EN / AR RTL)</li>
</ul>

<hr/>

## 📄 Lisensi

Proyek ini dilisensikan di bawah ketentuan yang dijelaskan pada file  
👉 <a href="LICENSE"><b>LICENSE</b></a>

<hr/>

<div align="center">
  <b>LINA bukan alat untuk menyerang.</b><br/>
  LINA adalah alat untuk <b>memahami</b>.
</div>
