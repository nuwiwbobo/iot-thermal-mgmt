# Script Video Demo — Kelompok 8 Embedded System 1 — Project 01
## ESP32 IoT Thermal Management System

**Target durasi:** 14-16 menit
**Format rekaman:** Audio narasi direkam terpisah dengan mic, lalu disinkronkan dengan video saat editing
**Catatan:** Baca narasi dengan tempo natural, tidak perlu cepat. Beri jeda 1-2 detik antar paragraf.

---

## Pre-recording Checklist

Sebelum mulai syuting video:
- [ ] Hardware sudah terangkai dan **running normal**
- [ ] ESP32 sudah terkoneksi ke WiFi `Infinix SMART 6` (cek di monitor)
- [ ] Buka tab browser: dashboard `http://192.168.97.XXX/`
- [ ] Buka tab Google Sheets: spreadsheet "Thermal Log"
- [ ] Siapkan korek api (untuk Scene 8)
- [ ] Siapkan HP/laptop untuk rekam video (1080p, mode landscape)
- [ ] Siapkan mic (bisa clip-on HP atau bawaan)
- [ ] Lokasi syuting terang, background tidak mengganggu

---

## SCENE 1: Perkenalan (1:00 - 1:30)

**Visual:**
- Tampak keseluruhan hardware di meja, posisi wide shot
- Text overlay muncul satu per satu:
  - "Kelompok 8"
  - "Sistem Embedded 1 — Project 01"
  - "ESP32 IoT Thermal Management System"
  - "[Nama Anda]" / "[Nama Anggota Kelompok]"

**Narasi:**
> "Assalamualaikum Wr. Wb. Perkenalkan, kami dari Kelompok 8 kelas Sistem Embedded 1. Pada video ini kami akan mendemonstrasikan proyek akhir kami yang berjudul ESP32 IoT Thermal Management System. Sistem ini berfungsi untuk memantau suhu dan mengontrol kipas secara otomatis, dengan kemampuan monitoring melalui web dashboard dan Google Sheets secara real-time. Demonstrasi akan mencakup pengenalan hardware, proses boot, pengoperasian dashboard dan kontrol tombol, hingga uji responsivitas sensor. Semoga video ini dapat memberikan gambaran yang jelas tentang proyek kami. Wassalamualaikum Wr. Wb."

**Catatan:** Jika presentasi dalam bahasa Indonesia formal/semi-formal, sesuaikan dengan gaya Anda. Bisa juga "Selamat pagi/siang"代替 salam.

---

## SCENE 2: Hardware Overview (1:30 - 4:00)

**Visual:**
- Wide shot semua komponen di meja
- Zoom in satu per satu setiap komponen (5-10 detik each)
- Beri label/nama sticky note di tiap komponen

### Komponen 1: ESP32 DevKit
**Visual:** Close-up ESP32, tunjukkan label pin

**Narasi:**
> "Komponen pertama adalah ESP32 DevKit, yang merupakan otak dari seluruh sistem. ESP32 memiliki dual-core processor dengan WiFi dan Bluetooth terintegrasi, sehingga cocok untuk aplikasi IoT seperti proyek kami. Mikrokontroler ini menjalankan semua logika kontrol, membaca sensor, menggerakkan aktuator, dan berkomunikasi dengan cloud."

### Komponen 2: Sensor BME280
**Visual:** Close-up modul sensor, tunjukkan chip dan pin

**Narasi:**
> "Komponen kedua adalah sensor BME280, sebuah sensor digital empat-dalam-satu yang dapat mengukur suhu, tekanan, kelembapan, dan ketinggian. Dalam proyek ini, kami hanya menggunakan fungsi pengukuran suhu. Sensor ini berkomunikasi dengan ESP32 melalui protokol I2C, hanya membutuhkan dua kabel yaitu SDA dan SCL, sehingga hemat pin. Akurasinya mencapai satu derajat Celsius, lebih dari cukup untuk aplikasi thermal management."

### Komponen 3: L298N Motor Driver
**Visual:** Close-up L298N, tunjukkan label ENA, IN1, IN2, VS, +5V

**Narasi:**
> "Komponen ketiga adalah L298N motor driver. Komponen ini berfungsi sebagai penghubung antara ESP32 yang berlogika 3.3 volt dengan motor DC 12 volt. L298N menerima sinyal PWM dari ESP32 pada pin ENA untuk mengatur kecepatan kipas, sementara pin IN1 dan IN2 mengatur arah putaran. Dalam proyek ini, kipas hanya berputar satu arah, sehingga IN1 selalu HIGH dan IN2 selalu LOW. L298N juga memiliki regulator 5 volt internal yang kami gunakan untuk mencatu ESP32, sehingga tidak perlu power supply terpisah."

### Komponen 4: Kipas DC 12V
**Visual:** Close-up fan, tunjukkan label tegangan

**Narasi:**
> "Komponen keempat adalah kipas DC 12 volt, yang merupakan aktuator dari sistem ini. Saat menerima PWM duty cycle rendah, kipas berputar pelan; saat duty cycle tinggi, kipas berputar kencang. Kipas ini berfungsi membuang panas dari sistem, menjaga suhu tetap dalam rentang yang diinginkan."

### Komponen 5: Tombol Push Button
**Visual:** Tunjukkan 2 tombol di breadboard

**Narasi:**
> "Komponen kelima adalah dua push button. Tombol pertama adalah tombol mode, digunakan untuk berpindah antara mode otomatis dan mode manual. Tombol kedua adalah tombol value, digunakan untuk menyesuaikan nilai setpoint di mode otomatis, atau kecepatan kipas di mode manual. Tekan singkat menyesuaikan setengah derajat, tekan lama menyesuaikan sepuluh derajat."

### Komponen 6: Baterai 3S Li-ion + BMS
**Visual:** Tunjukkan baterai dan BMS

**Narasi:**
> "Komponen terakhir adalah baterai lithium-ion tiga sel atau 3S dengan Battery Management System. Baterai ini menyediakan tegangan nominal 11.1 volt, yang masuk ke pin VS pada L298N. BMS melindungi baterai dari over-charge, over-discharge, dan korsleting. Tegangan 12 volt ini menggerakkan kipas, sekaligus memberi daya pada ESP32 melalui regulator 5 volt di L298N."

**Visual:** Wide shot lagi semua komponen terhubung

**Narasi:**
> "Semua komponen ini terhubung dan bekerja bersama sebagai satu sistem terintegrasi."

---

## SCENE 4: Pemilihan Pin (4:00 - 5:00)

**Visual:**
- Screen recording `config.h` di text editor
- Tunjukkan baris-baris pin definition
- Highlight satu per satu

**Narasi:**
> "Sekarang mari kita lihat konfigurasi pin ESP32. Pada baris pertama, GPIO 21 dan 22 digunakan untuk SDA dan SCL sensor I2C. Pemilihan pin ini karena GPIO 21 dan 22 adalah pin default untuk I2C0 ESP32, dengan dukungan hardware pull-up. Untuk PWM, kami menggunakan GPIO 16 pada pin ENA L298N. GPIO 16 dipilih karena PWM-capable dan bukan strapping pin, sehingga tidak mengganggu proses boot. Untuk tombol, GPIO 4 dan 5 digunakan. Keduanya RTC-capable, artinya bisa wake dari deep sleep, dan juga bukan strapping pin."

---

## SCENE 6: Boot & Operasi Langsung (5:00 - 7:30)

**Visual:**
- Screen recording `idf.py -p /dev/ttyUSB0 monitor`
- TUNGGU 15-20 detik untuk capture full boot sequence

**Sequence yang akan muncul di monitor:**

1. **Boot loader messages** (5:00-5:20)
   ```
   ESP-IDF v5.4 2nd stage bootloader
   Partition Table: ...
   ```
   **Narasi:**
   > "Saat ESP32 dinyalakan, bootloader ESP-IDF versi 5.4 akan dimuat terlebih dahulu. Bootloader menampilkan tabel partisi flash: ada partisi untuk data NVS, PHY, aplikasi utama, dan SPIFFS untuk file system."

2. **App initialization** (5:20-5:50)
   ```
   app_main: Calling app_main()
   sensor: BME280 found (ID=0x60)
   sensor: Calibration: T1=28067 T2=26309 T3=50
   sensor: Sensor ready
   cloud: Cloud module initialized (URL configured)
   ```
   **Narasi:**
   > "Setelah bootloader, aplikasi utama dijalankan. Sensor BME280 berhasil terdeteksi dengan chip ID 0x60. ID ini mengonfirmasi bahwa sensor terhubung dengan benar. Firmware kemudian membaca data kalibrasi unik dari sensor — setiap BME280出厂 sudah dikalibrasi individual di pabrik, dan koefisien ini disimpan di register 0x88 hingga 0xA1. Modul cloud juga diinisialisasi, dengan URL Apps Script yang sudah dikonfigurasi."

3. **WiFi connection** (5:50-6:30)
   ```
   wifi:mode : sta (68:fe:71:0b:65:8c) + softAP
   network: STA: trying to connect to SSID='Infinix SMART 6'
   wifi:connected with Infinix SMART 6
   network: Got IP: 192.168.97.XXX
   network: AP started: ThermalCtrl-AP
   network: HTTP server started
   ```
   **Narasi:**
   > "Setelah sensor siap, ESP32 mencoba koneksi WiFi. Mode AP+STA diaktifkan: STA untuk koneksi ke jaringan rumah, AP sebagai fallback. Dalam demo ini, ESP32 berhasil terhubung ke hotspot smartphone dengan SSID Infinix SMART 6. Alamat IP yang didapat adalah 192.168.97.xxx dari DHCP. AP ThermalCtrl-AP juga tetap aktif di alamat 192.168.4.1 sebagai backup. HTTP server kemudian dijalankan untuk melayani dashboard web."

4. **Live operation** (6:30-7:30)
   ```
   thermal: Temp: 35.3°C  Fan: 53%  Mode: AUTO  Setpoint: 30.0°C
   ```
   Tunggu 30 detik, tunjukkan beberapa baris log
   **Narasi:**
   > "Dan inilah output sistem saat berjalan. Setiap dua detik, firmware menampilkan pembacaan suhu saat ini, persentase kipas, mode operasi, dan setpoint. Terlihat suhu 35.3 derajat dengan kipas di 53 persen, mode otomatis, setpoint 30 derajat. Karena suhu melebihi setpoint, kipas berputar untuk mendinginkan sistem. Ini adalah logika kontrol proporsional sederhana: semakin besar selisih antara suhu aktual dan setpoint, semakin kencang kipas berputar."

---

## SCENE 7: Dashboard Web + Cloud Upload (7:30 - 11:00)

**Visual:**
- Split screen: kiri = browser dashboard, kanan = Google Sheet
- ATAU alternating: dashboard → switch ke sheet → balik ke dashboard

### Sub-scene 7a: Dashboard
**Visual:** Browser di `http://192.168.97.XXX/`

**Narasi:**
> "Sekarang mari kita akses dashboard web. Di browser HP atau laptop, kita ketik alamat IP ESP32, yaitu 192.168.97.xxx. Tunggu sebentar, dan terlihat empat kartu informasi di bagian atas: suhu saat ini 35.3 derajat, kecepatan kipas 53 persen, mode AUTO, dan setpoint 30.0 derajat. Di bawahnya ada dua grafik real-time: grafik biru menunjukkan perubahan suhu dalam 60 detik terakhir, grafik ungu menunjukkan kecepatan kipas. Di bawah lagi, ada slider untuk mengatur setpoint dan kecepatan kipas secara manual."

### Sub-scene 7b: Kontrol via dashboard
**Visual:** Interaksi dengan dashboard — geser slider, klik tombol

**Narasi:**
> "Mari kita coba kontrol via dashboard. Saya akan geser slider setpoint ke 32 derajat. Lihat, setpoint di header berubah, dan kipas otomatis menyesuaikan. Sekarang saya klik tombol Toggle Mode, mode berubah ke MANUAL dan ditunjukkan dengan badge ungu. Di mode manual, saya bisa langsung mengatur kecepatan kipas dengan slider Fan. Saya geser ke 80 persen, dan terdengar kipas berputar lebih kencang. Sistem merespons dengan latency kurang dari satu detik."

### Sub-scene 7c: API endpoint
**Visual:** Buka tab baru, akses `/api/status`

**Narasi:**
> "Dashboard ini juga menyediakan REST API untuk integrasi. Endpoint /api/status mengembalikan data sistem dalam format JSON. Terlihat field temperature, fan, mode, setpoint, status WiFi, uptime, dan yang penting, status cloud upload dengan jumlah upload berhasil, gagal, dan usia upload terakhir dalam detik."

### Sub-scene 7d: Cloud upload ke Google Sheets
**Visual:** Switch ke tab Google Sheets, TUNGGU 10-15 detik

**Narasi:**
> "Sekarang yang paling menarik: integrasi cloud. Setiap 5 detik, ESP32 mengirim data sensor terbaru ke Google Sheets via Google Apps Script. Saya buka spreadsheet Thermal Log di tab lain. Perhatikan, di bagian bawah tabel, baris-baris baru terus ditambahkan secara otomatis. Setiap baris berisi timestamp, nomor sampel, suhu, kecepatan kipas, mode, dan setpoint. Dengan interval 5 detik, dalam satu menit muncul 12 baris."

**Visual:** Highlight baris terakhir beberapa kali untuk tunjukkan update

**Narasi:**
> "Ini adalah implementasi nyata dari monitoring IoT: data historis otomatis terakumulatif, dapat diakses dari mana saja selama ada internet, tanpa perlu download manual. Untuk capstone, ini memenuhi poin evaluasi tentang konsep penyimpanan data, time correlation, dan pengolahan data menjadi informasi."

---

## SCENE 8: Tes Sensor dengan Korek Api (11:00 - 12:30)

**Visual:**
- Zoom ke sensor BME280
- Suhu awal ditunjukkan (~35°C di monitor)
- Korek api dinyalakan dan diarahkan ke sensor dari jarak ~5 cm

**Sequence:**

1. **Kondisi awal** (11:00-11:15)
   - Tunjukkan suhu di monitor/dashboard: ~35°C
   **Narasi:**
   > "Sekarang kita uji responsivitas sensor. Suhu awal terlihat 35.3 derajat. Perhatikan baik-baik grafik."

2. **Pemanasan** (11:15-11:50)
   - Nyalakan korek api, arahkan ke sensor
   - Suhu di monitor naik perlahan
   - Kipas otomatis naik
   **Narasi:**
   > "Saya nyalakan korek api dan arahkan ke sensor dari jarak sekitar lima sentimeter. Api korek memiliki suhu sekitar 700 derajat Celcius, namun karena sensor hanya menerima radiasi termal dari jarak jauh, suhu yang terbaca hanya naik moderat. Perhatikan monitor: suhu naik dari 35 ke 38, lalu 40, 42 derajat. Kipas secara otomatis berputar lebih kencang — terlihat dari nilai fan yang naik dari 53 ke 65, lalu 80 persen. Ini adalah respons kontrol proporsional sistem."

3. **Pendinginan** (11:50-12:15)
   - Matikan korek api
   - Suhu turun perlahan
   - Kipas turun
   **Narasi:**
   > "Sekarang saya matikan korek api. Suhu mulai turun: 40, 38, 36 derajat. Kipas也跟着 turun dari 80 ke 60, lalu kembali ke 50 persen. Sistem menunjukkan histeresis: ia tidak langsung berubah drastis, melainkan menyesuaikan secara bertahap. Ini mencegah osilasi yang dapat merusak motor."

4. **Konfirmasi safety** (12:15-12:30)
   - Tunjukkan suhu maksimum tercapai ~45-50°C, **tidak capai 60°C**
   **Narasi:**
   > "Dalam pengujian ini, suhu maksimum hanya mencapai sekitar 45 derajat, tidak sampai 60 derajat yang merupakan ambang batas safety override. Untuk memicu safety override, diperlukan sumber panas yang lebih kuat seperti heat gun atau api yang lebih dekat. Namun sistem sudah merespons dengan benar untuk pemanasan moderat."

---

## SCENE 9: Kontrol Tombol Fisik (12:30 - 13:30)

**Visual:**
- Kamera ke 2 push button di breadboard
- Monitor serial di background menampilkan perubahan

### Sub-scene 9a: Mode toggle
**Visual:** Tekan tombol mode (GPIO 4)

**Narasi:**
> "Sekarang kita uji kontrol via tombol fisik. Tombol pertama adalah tombol mode. Saya tekan singkat. Perhatikan di dashboard: badge mode berubah dari AUTO biru ke MANUAL ungu. Tombol ini menggunakan internal pull-up ESP32 dan logic active-low, dengan software debounce 50 milidetik."

### Sub-scene 9b: Setpoint adjustment
**Visual:** Tekan tombol value (GPIO 5) — tekan singkat dan tahan

**Narasi:**
> "Tombol kedua adalah tombol value. Tekan singkat menyesuaikan setengah derajat. Tekan lama — tahan lebih dari 500 milidetik — menyesuaikan sepuluh derajat. Saya tekan singkat: setpoint naik dari 30 ke 30.5. Sekarang saya tekan lama: langsung naik ke 40.5. Ini memberikan kontrol kasar dan halus dalam satu tombol."

### Sub-scene 9c: Manual fan control
**Visual:** Di mode MANUAL, tekan value

**Narasi:**
> "Di mode manual, tombol value berfungsi berbeda: ia mengatur kecepatan kipas langsung. Saya tekan singkat: kipas naik satu persen. Tekan lama: kipas naik sepuluh persen. Ini adalah fallback ketika WiFi mati atau dashboard tidak bisa diakses."

---

## SCENE 11: Analisis Data (13:30 - 14:30)

**Visual:**
- Browser dashboard
- Klik "Download CSV"
- Buka file CSV di Excel/text editor
- Tunjukkan format data

**Narasi:**
> "Untuk analisis data lebih lanjut, sistem menyediakan download CSV. Dari dashboard, klik Download CSV dan file log.csv akan terdownload. Formatnya: nomor sampel, timestamp dalam milidetik, suhu, fan percent, mode, dan setpoint. File ini bisa dibuka di Excel, Google Sheets, atau dianalisis dengan Python pandas."

**Visual:** (Opsional) Tampilkan plot dari gnuplot atau matplotlib

**Narasi:**
> "Dari data historis ini, kita bisa ekstrak informasi seperti response time, overshoot, steady-state error, dan oscillations. Ini adalah transformasi dari raw data menjadi informasi yang berguna untuk tuning parameter kontrol atau audit kualitas."

---

## SCENE 12: Penutup (14:30 - 15:30)

**Visual:**
- Wide shot hardware dengan operator di belakangnya
- ATAU fade ke logo / closing card

**Narasi:**
> "Demikian demonstrasi proyek ESP32 IoT Thermal Management System oleh Kelompok 8 Sistem Embedded 1. Proyek ini menunjukkan integrasi sensor digital, kontrol aktuator, antarmuka web, dan cloud monitoring dalam satu platform embedded. Kami berharap proyek ini dapat menjadi referensi untuk aplikasi IoT serupa di masa depan. Terima kasih atas perhatiannya. Wassalamualaikum Wr. Wb."

**Visual:** End card dengan NIM, nama anggota, link repository GitHub

---

## Tips Perekaman Audio

1. **Rekam narasi terpisah** dengan mic di tempat tenang (bukan di dekat kipas karena bising)
2. **Bicara dengan tempo natural** — tidak perlu cepat, idealnya 130-150 kata/menit
3. **Beri jeda 1-2 detik** antar paragraf untuk sinkronisasi nanti
4. **Practice dulu** sebelum rekam: baca 2-3 kali supaya lancar
5. **Gunakan energi** — suara datar = video membosankan
6. **Korek api** akan menghasilkan suara klik — siap-siap pause narasi saat itu

## Tips Editing Pasca-Produksi

1. **Sinkronkan audio** dengan video di software editor (CapCut, DaVinci Resolve, Premiere)
2. **Speed-up** bagian yang membosankan: 2-4x untuk build process, jeda kosong
3. **Tambah text overlay** untuk: judul, IP address, evaluation point number, label komponen
4. **Highlight** nilai penting di monitor dengan kotak kuning/arrow
5. **Background music** volume 10-20%, instrumental only
6. **Export**: 1080p, H.264, bitrate 8-10 Mbps

## Catatan Penting

- **Demonstrasi harus real-time**, bukan pre-recorded/mock-up
- **Sensor test dengan korek api** aman karena tidak akan mencapai 60°C (sesuai catatan Anda)
- **Backup plan** kalau koneksi lambat: rekam di dekat ESP32, gunakan AP `ThermalCtrl-AP` di 192.168.4.1
- **Total durasi** yang ideal untuk capstone: 10-15 menit
