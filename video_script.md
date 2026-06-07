# Script Video Demo Capstone — ESP32 IoT Thermal Management System
**Target durasi:** 12-15 menit
**Resolusi:** 1080p minimal
**Format:** Screen recording + camera hp (handheld ok)

---

## Pre-production Checklist

Sebelum mulai syuting, siapkan:
- [ ] ESP32 + sensor BME280 + L298N + fan sudah terangkai dan **running**
- [ ] HP untuk rekam video (bisa juga laptop webcam untuk screen recording)
- [ ] Tripod atau sandar HP agar stabil
- [ ] Lampu tambahan (jika gelap)
- [ ] Software: `idf.py monitor`, browser (untuk dashboard), Google Sheets terbuka
- [ ] Heat gun / hair dryer (untuk uji safety override)
- [ ] Jari (untuk uji sentuh sensor)

---

## SCENE 1: Opening (0:00 - 1:00)

**Visual:**
- Tampak keseluruhan hardware di meja (ESP32, sensor, L298N, fan, button)
- Judul overlay: "ESP32 IoT Thermal Management System — [Nama Anda]"

**Narasi:**
> "Selamat datang. Pada video ini saya akan mendemonstrasikan proyek capstone Embedded System 1: sistem manajemen termal berbasis ESP32 dengan monitoring cloud. Sistem ini membaca suhu via sensor digital, mengontrol kipas secara otomatis atau manual, menyimpan log, dan mengunggah data ke Google Sheets untuk monitoring jarak jauh."

**Mapping ke evaluation point:** 1 (kelayakan problem)

---

## SCENE 2: Hardware Overview (1:00 - 3:00)

**Visual:**
- Close-up setiap komponen satu per satu
- Tunjukkan label/nama komponen (sticky note ok)
- Wide shot semua komponen terhubung

**Sequence shot:**
1. ESP32 DevKit (tampak atas)
2. Sensor BME280/BMP280 (tampak modul + pin SDA/SCL/VCC/GND)
3. Driver L298N (tampak board, label ENA/IN1/IN2/VS/+5V)
4. Fan DC 12V (tampak label spec)
5. 2 push button
6. Battery 3S Li-ion + BMS
7. Kabel jumper dan wiring

**Narasi (sambil nunjuk):**
> "Komponen utama: ESP32 sebagai mikrokontroler, sensor BME280 untuk pembacaan suhu digital via I2C, L298N motor driver untuk mengontrol kipas 12V dengan PWM, dua push button untuk input lokal, dan baterai 3S Li-ion sebagai power supply."

**Mapping ke evaluation point:** 2 (ketepatgunaan komponen), 8 (konstruksi hardware)

---

## SCENE 3: System Architecture (3:00 - 4:30)

**Visual:**
- Screen recording: buka `architecture.pdf` section 3.1 (diagram blok)
- ATAU whiteboard/gambar tangan diagram

**Tunjukkan:**
- Diagram blok: Sensor → ESP32 → L298N → Fan
- Data plane vs control plane
- Power plane

**Narasi:**
> "Arsitektur sistem terbagi menjadi tiga plane. Data plane: sensor BME280 mengirim data via I2C ke ESP32, diproses, dan PWM dikirim ke L298N untuk mengatur kecepatan kipas. Control plane: pengguna dapat mengontrol via web dashboard REST API atau tombol fisik. Power plane: baterai 12V memberi daya ke L298N yang juga menghasilkan 5V untuk ESP32."

**Mapping ke evaluation point:** 3 (arsitektur & interkoneksi)

---

## SCENE 4: Pin Selection (4:30 - 5:30)

**Visual:**
- Screen recording `config.h` di editor
- Highlight baris `BMP280_SDA_GPIO 21`, `L298N_ENA_GPIO 16`, dll
- Tunjukkan tabel pin di PDF

**Narasi:**
> "Pemilihan pin mempertimbangkan beberapa faktor. GPIO21 dan 22 untuk I2C karena merupakan default I2C0 ESP32. GPIO16 untuk PWM karena PWM-capable dan bukan strapping pin. GPIO4 dan 5 untuk tombol karena RTC-capable sehingga bisa wake dari deep sleep di masa depan."

**Mapping ke evaluation point:** 4 (pertimbangan pin/interface)

---

## SCENE 5: Realistic Constraints (5:30 - 6:30)

**Visual:**
- Tabel constraints (sudah ada di `architecture.pdf`)
- ATAU visual demonstrasi dari masing-masing

**Sequence shot/visual:**
- Brownout: kapasitor 470µF di 5V
- Inrush current: tunjukkan fan start (mungkin sedikit jerky)
- Safety override: pegang heat gun ke sensor
- I2C noise: tahan motor manual
- WiFi disconnect: cabut WiFi sebentar

**Narasi:**
> "Berbagai constraints ditangani: kapasitor bulk 470µF mencegah brownout saat fan start. Safety override: jika suhu >60°C, kipas otomatis ke 100%. Internal pull-up ESP32 untuk I2C cukup untuk breadboard, eksternal 4.7kΩ untuk produksi. WiFi reconnect otomatis setiap 30 detik. Data dual-buffer: RAM ring + LittleFS untuk防止数据丢失."

**Mapping ke evaluation point:** 5 (multi-realistic constraint)

---

## SCENE 6: Boot & Live Operation (6:30 - 8:00)

**Visual:**
- Screen recording `idf.py -p /dev/ttyUSB0 monitor`
- TUNGGU 15 detik untuk capture full boot

**Tunjukkan di monitor:**
```
BME280 found (ID=0x60)
STA: trying to connect to SSID='Infinix SMART 6'
Got IP: 192.168.97.XXX
HTTP server started
Temp: XX.X°C Fan: XX% Mode: AUTO Setpoint: 30.0°C
```

**Narasi:**
> "Saat booting, sensor BME280 terdeteksi dengan chip ID 0x60. Firmware mencoba koneksi ke WiFi rumah. Setelah berhasil, mendapat IP dari DHCP. HTTP server berjalan di port 80. Setiap 2 detik, monitor menampilkan suhu, fan, mode, dan setpoint saat ini."

**Mapping ke evaluation point:** 9 (kelayakan eksperimen)

---

## SCENE 7: Web Dashboard Demo (8:00 - 10:00)

**Visual:**
- Screen recording browser di HP/laptop
- Buka `http://192.168.97.XXX/` (IP dari monitor)

**Sequence shot:**
1. Halaman utama muncul (4 cards: Temperature, Fan Speed, Mode, Setpoint)
2. Tunggu 5 detik — angka berubah, grafik bergerak
3. Klik "Toggle Mode" → mode berubah MANUAL
4. Geser slider Fan ke 80% → kipas makin kencang
5. Geser slider Setpoint ke 25°C
6. Buka `/api/status` di tab baru → tunjukkan JSON

**Narasi:**
> "Dashboard web menampilkan data real-time. Empat kartu informasi utama di header. Dua grafik live: suhu dan fan dalam 60 detik terakhir. Tabel log di bawah. Slider untuk mengatur setpoint dan fan secara manual. Endpoint API juga tersedia untuk integrasi."

---

## SCENE 8: Sensor Test (10:00 - 11:30)

**Visual:**
- Kamera zoom ke sensor BME280
- Sentuh sensor dengan jari selama 10 detik
- Monitor serial di background

**Sequence:**
1. Suhu awal: ~35°C
2. Sentuh sensor: suhu naik 1-2°C dalam 5-10 detik
3. Lepaskan: suhu turun kembali
4. (Opsional) Heat gun ke sensor: suhu naik cepat → fan ke 100%

**Narasi:**
> "Mari kita uji responsivitas sensor. Saat saya sentuh sensor dengan jari, suhu naik sekitar 1-2°C. Sistem merespons dengan menyesuaikan kecepatan kipas. Saat saya lepaskan, suhu turun kembali ke ambient. Untuk uji safety override, kita gunakan heat gun..."

**Mapping ke evaluation point:** 8 (konstruksi/operasi), 9 (kelayakan)

---

## SCENE 9: Button Control (11:30 - 12:30)

**Visual:**
- Kamera ke 2 push button
- Tekan tombol mode (pendek)
- Tekan tombol value (pendek dan panjang)

**Sequence:**
1. Tekan mode button: mode toggle AUTO↔MANUAL (cek di dashboard)
2. Tekan value button (pendek): setpoint +0.5°C
3. Tekan value button (panjang, >500ms): setpoint +10°C
4. Mode MANUAL: tekan value untuk ubah fan speed

**Narasi:**
> "Tombol fisik juga berfungsi sebagai backup ketika WiFi mati. Tombol mode untuk toggle antara AUTO dan MANUAL. Tombol value: tekan singkat untuk adjust +0.5°C, tekan lama untuk adjust +10°C. Di mode MANUAL, tombol value mengatur fan speed langsung."

**Mapping ke evaluation point:** 5 (constraints), 9 (operasional)

---

## SCENE 10: Cloud Upload Demo (12:30 - 14:00)

**Visual:**
- Screen recording: buka Google Sheet "Thermal Log"
- TUNGGU 10-15 detik, tunjukkan baris baru masuk

**Tunjukkan:**
- Tabel di Sheet terisi otomatis
- Kolom: Timestamp, Sample, Temp, Fan, Mode, Setpoint
- Tunjukkan URL endpoint: `https://script.google.com/macros/.../exec`
- (Opsional) Buka editor Apps Script, tunjukkan kode `doPost`

**Narasi:**
> "Setiap 5 detik, ESP32 mengirim data ke Google Sheets via Apps Script. Tidak perlu download manual, data akumulatif otomatis. Bisa dibuka dari mana saja selama ada internet. Cocok untuk monitoring jarak jauh."

**Mapping ke evaluation point:** 6 (data log + time), 7 (pengolahan jadi informasi)

---

## SCENE 11: Data Analysis (14:00 - 15:00)

**Visual:**
- Screen recording: download CSV dari dashboard atau dari Sheet
- Buka di Excel/pandas/gnuplot
- Tunjukkan grafik suhu vs fan response

**Narasi:**
> "Data yang terakumulasi bisa dianalisis. Download sebagai CSV dari dashboard, atau langsung dari Google Sheet. Plot dengan gnuplot atau pandas. Bisa lihat: response time, overshoot, steady-state error, oscillations. Ini mengubah raw data menjadi informasi yang berguna untuk tuning parameter kontrol."

**Mapping ke evaluation point:** 7 (pengolahan jadi informasi)

---

## SCENE 12: Closing (15:00 - 15:30)

**Visual:**
- Wide shot hardware
- Tampilkan `architecture.pdf` di background

**Narasi:**
> "Demikian demonstrasi proyek ESP32 IoT Thermal Management System. Sistem ini menunjukkan integrasi sensor digital, kontrol aktuator, web interface, dan cloud monitoring dalam satu platform. Semua 9 evaluation points dari project guideline telah dipenuhi. Terima kasih."

---

## Editing Tips

1. **Speed up** bagian yang membosankan (build process, boot sequence) — 2-4x speed
2. **Tambah zoom-in** pada bagian penting (nomor IP, error message, grafik)
3. **Background music** volume rendah (10-20%)
4. **Subtitle** untuk kata-kata teknis
5. **Text overlay** untuk label komponen dan evaluation point
6. **Transition** antar scene: cut langsung atau fade 0.5s

## Equipment Minimum

- Kamera: HP 12MP+ (4K jika ada)
- Audio: microphone external (jika narasi现场) atau subtitle only
- Lighting: lampu meja atau natural light
- Editing: CapCut (HP) atau DaVinci Resolve (PC, gratis)

## Common Mistakes to Avoid

- ❌ Lupa merekam serial monitor saat boot
- ❌ Tidak menyiapkan Google Sheet sebelum syuting
- ❌ Membaca narasi (sounds robotic) — lebih baik hafal poin-poin, narasi natural
- ❌ Tidak menunjukkan data real-time (pre-recorded)
- ❌ Skip demonstrasi — caption tanpa bukti hardware
- ❌ Audio tidak jelas / noise
