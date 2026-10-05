<div align="center">

<img src="https://raw.githubusercontent.com/BangAguse/Gambar/main/LINA.png" alt="LINA — Learning Interconnected Network Analyzer" width="720">

### Learning Interconnected Network Analyzer

**Pahami jaringan lokal dengan ringkasan dan laporan yang mudah dibaca.**

[![Rilis terbaru](https://img.shields.io/badge/Unduh-Rilis%20terbaru-147D83?style=for-the-badge&logo=github)](https://github.com/BangAguse/LINA/releases/latest)

[🇮🇩 Bahasa Indonesia](#bahasa-indonesia) &nbsp;·&nbsp; [🇬🇧 English](#english)

</div>

---

## Bahasa Indonesia

LINA adalah aplikasi desktop untuk meninjau metadata jaringan lokal dan membuat laporan defensif. Pengumpulan dan pemrosesan data berlangsung di perangkat; LINA tidak menggunakan layanan cloud atau model AI.

### Unduh LINA

**[⬇ Buka rilis LINA terbaru](https://github.com/BangAguse/LINA/releases/latest)** untuk memilih installer yang sesuai dengan sistem operasi Anda.

| Sistem operasi | Unduhan | Langkah ringkas |
|---|---|---|
| Windows 64-bit | Arsip `.zip` di bagian **Assets** pada rilis terbaru | Ekstrak ZIP, lalu jalankan installer `.exe` di dalamnya. |
| Linux 64-bit | Arsip `.tar.xz` di bagian **Assets** pada rilis terbaru | Ekstrak arsip, lalu ikuti petunjuk di bawah atau catatan yang disertakan. |

> **Tips:** Pilih file dari bagian **Assets** pada halaman rilis. Jangan mengunduh **Source code**; itu bukan installer aplikasi.

<details>
<summary><strong>Petunjuk instalasi Windows</strong></summary>

1. Unduh arsip Windows `.zip` dari **Assets** rilis terbaru.
2. Klik kanan arsip, lalu pilih **Extract All** atau aplikasi ekstraksi ZIP pilihan Anda.
3. Buka folder hasil ekstraksi dan jalankan file installer `.exe`.
4. Ikuti petunjuk installer sampai selesai, lalu buka LINA dari Start Menu.

Nmap dan Npcap tidak disertakan di dalam paket LINA. Keduanya hanya diperlukan untuk fitur jaringan tambahan:

- [Nmap](https://nmap.org/download.html) diperlukan untuk penemuan perangkat di Wi-Fi lokal.
- [Npcap](https://npcap.com/#download) diperlukan untuk pembaca paket langsung.

LINA akan menampilkan halaman persiapan Windows jika komponen tersebut belum terdeteksi. Mulai ulang aplikasi setelah memasang Npcap. Jika konfigurasi Npcap membatasi akses, jalankan LINA sebagai administrator saat menggunakan pembaca paket.

</details>

<details>
<summary><strong>Petunjuk instalasi Linux</strong></summary>

1. Unduh arsip Linux `.tar.xz` dari **Assets** rilis terbaru.
2. Ekstrak arsip melalui file manager, atau jalankan:

   ```bash
   mkdir -p "$HOME/Downloads/lina"
   tar -xf "$HOME/Downloads/NAMA_ARSIP.tar.xz" -C "$HOME/Downloads/lina"
   ```

   Ganti `NAMA_ARSIP.tar.xz` dengan nama file yang Anda unduh.
3. Jika hasil ekstraksi berisi file `.deb`, pasang dengan:

   ```bash
   sudo apt install "$HOME/Downloads/lina/"*.deb
   ```

4. Buka LINA dari menu aplikasi.

Penemuan perangkat Wi-Fi di Linux memerlukan NetworkManager, `ip`, dan Nmap. Pembaca paket memerlukan izin capture paket dari sistem operasi.

</details>

### Fitur utama

- **22 laporan lokal** untuk ikhtisar jaringan, Wi-Fi, antarmuka, perangkat teramati, kualitas data, dan rekomendasi defensif.
- **Penemuan perangkat Wi-Fi** secara eksplisit hanya pada subnet IPv4 Wi-Fi yang sedang terhubung. Subnet yang lebih besar dari `/24` ditolak; fitur ini tidak memindai port.
- **Pembaca paket langsung** pada antarmuka yang dipilih, dengan penyaring tampilan, ringkasan paket, detail protokol Scapy, dan tampilan heksadesimal/ASCII.
- **Ekspor dan buka ulang capture** dalam format LINA `.linacap`.
- **Antarmuka Bahasa Indonesia dan English.**

### Privasi dan batasan

Sebagian besar pengumpulan metadata bersifat pasif dan dilakukan secara lokal. Penemuan perangkat Wi-Fi adalah tindakan eksplisit dan terbatas pada subnet Wi-Fi yang sedang tersambung. Perangkat yang tidur atau tidak merespons ARP mungkin tidak terdeteksi.

Pembaca paket dapat menyimpan isi lalu lintas jaringan. File capture mungkin berisi informasi sensitif, jadi tangkap hanya lalu lintas yang Anda miliki atau berwenang pantau, dan simpan file dengan aman. LINA tidak menyusun ulang aliran TCP dan tidak menyediakan semua dissector Wireshark.

LINA memakai komponen pihak ketiga, termasuk Scapy, PySide6, Nmap, dan Npcap untuk fitur terkait. Setiap komponen tetap tunduk pada ketentuan lisensinya masing-masing.

### Bantuan dan rilis

- **Unduhan:** [Rilis LINA terbaru](https://github.com/BangAguse/LINA/releases/latest)
- **Riwayat versi:** [Semua rilis](https://github.com/BangAguse/LINA/releases)
- **Masalah atau pertanyaan:** [Hubungi pengembang](mailto:muhammadagustriananda@gmail.com)

[Kembali ke pilihan bahasa ↑](#lina)

---

## English

LINA is a desktop application for reviewing local network metadata and generating defensive reports. Data collection and processing stay on your device; LINA does not use cloud services or AI models.

### Download LINA

**[⬇ Open the latest LINA release](https://github.com/BangAguse/LINA/releases/latest)** and choose the package for your operating system.

| Operating system | Download | Quick steps |
|---|---|---|
| Windows 64-bit | The `.zip` archive under **Assets** in the latest release | Extract the ZIP, then run the `.exe` installer inside. |
| Linux 64-bit | The `.tar.xz` archive under **Assets** in the latest release | Extract the archive, then follow the instructions below or any included notes. |

> **Tip:** Choose a file under **Assets** on the release page. Do not download **Source code**; it is not an application installer.

<details>
<summary><strong>Windows installation</strong></summary>

1. Download the Windows `.zip` archive from **Assets** in the latest release.
2. Right-click the archive and choose **Extract All**, or use your preferred ZIP utility.
3. Open the extracted folder and run the `.exe` installer.
4. Complete the installer, then launch LINA from the Start Menu.

Nmap and Npcap are not bundled with LINA. They are only needed for optional network features:

- [Nmap](https://nmap.org/download.html) is required for Wi-Fi device discovery.
- [Npcap](https://npcap.com/#download) is required for live packet capture.

LINA shows a Windows setup page if either component is missing. Restart the application after installing Npcap. If Npcap is configured to restrict access, run LINA as an administrator when using the packet reader.

</details>

<details>
<summary><strong>Linux installation</strong></summary>

1. Download the Linux `.tar.xz` archive from **Assets** in the latest release.
2. Extract it using your file manager, or run:

   ```bash
   mkdir -p "$HOME/Downloads/lina"
   tar -xf "$HOME/Downloads/ARCHIVE_NAME.tar.xz" -C "$HOME/Downloads/lina"
   ```

   Replace `ARCHIVE_NAME.tar.xz` with the name of the downloaded file.
3. If the extracted files include a `.deb` package, install it with:

   ```bash
   sudo apt install "$HOME/Downloads/lina/"*.deb
   ```

4. Launch LINA from your applications menu.

Wi-Fi device discovery on Linux requires NetworkManager, `ip`, and Nmap. Packet reading requires packet-capture permissions from the operating system.

</details>

### Key features

- **22 local reports** covering network overview, Wi-Fi, interfaces, observed devices, data quality, and defensive recommendations.
- **Wi-Fi device discovery** is explicitly limited to the connected Wi-Fi IPv4 subnet. Networks larger than `/24` are refused; the feature does not scan ports.
- **Live packet reader** on a user-selected interface, with display filters, packet summaries, Scapy protocol details, and a hex/ASCII view.
- **Save and reopen captures** in LINA's `.linacap` format.
- **Indonesian and English** user interfaces.

### Privacy and limitations

Most metadata collection is passive and happens locally. Wi-Fi device discovery is an explicit action limited to the currently connected Wi-Fi subnet. Sleeping devices or devices that do not respond to ARP may not be detected.

The packet reader can save network traffic contents. Capture files may contain sensitive information: capture only traffic you own or are authorized to monitor, and store capture files securely. LINA does not reassemble TCP streams or provide every Wireshark dissector.

LINA uses third-party components, including Scapy, PySide6, Nmap, and Npcap for related features. Each component remains subject to its own license terms.

### Help and releases

- **Downloads:** [Latest LINA release](https://github.com/BangAguse/LINA/releases/latest)
- **Version history:** [All releases](https://github.com/BangAguse/LINA/releases)
- **Questions:** [Contact the developer](mailto:muhammadagustriananda@gmail.com)

[Back to language choices ↑](#lina)
