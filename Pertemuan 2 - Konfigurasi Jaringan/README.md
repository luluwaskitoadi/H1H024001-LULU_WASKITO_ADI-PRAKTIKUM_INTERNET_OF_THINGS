# Pertemuan 2 Konfigurasi Jaringan

## Deskripsi Singkat

Praktikum ini membahas konfigurasi jaringan nirkabel (WiFi) pada ESP32 menggunakan pustaka WiFi.h. Percobaan 2A berfokus pada konfigurasi ESP32 sebagai Station (STA) yang terhubung ke jaringan WiFi yang sudah ada, sedangkan Percobaan 2B berfokus pada konfigurasi ESP32 sebagai Access Point (AP) yang menyediakan jaringannya sendiri agar dapat diakses oleh perangkat lain.

## Alat dan Bahan

- Board ESP32 DevKit
- Kabel USB (Micro-USB/USB-C sesuai board)
- Laptop/PC dengan Arduino IDE (sudah terpasang board manager ESP32)
- Jaringan WiFi (router/hotspot smartphone) beserta SSID dan password
- Smartphone/laptop untuk menguji koneksi ke Access Point ESP32
- LED (1 buah) dan Resistor 220 Ω (opsional, untuk indikator status koneksi)

## Library / Dependencies

- `WiFi.h`  pustaka bawaan board manager ESP32 pada Arduino IDE, digunakan untuk mengatur dan memonitor koneksi jaringan (mode Station, Access Point, maupun AP+STA). Tidak perlu instalasi terpisah, otomatis tersedia setelah board manager ESP32 terpasang.

---

## Percobaan 2A: Konfigurasi Mode Station (STA)

- Code Percobaan:[`code/percobaan2a_konfigurasi_sta.ino`](./code/percobaan2a_konfigurasi_sta.ino)
- Code modifikasi:[`code/percobaan2a_konfigurasi_sta_modifikasi.ino`](./code/percobaan2a_konfigurasi_sta_modifikasi.ino)

> **Catatan keamanan:** nilai `ssid` dan `password` dapat diseuaikan dengan nama wifi dan password yang anda ketahui
### Tujuan

Memahami dan mengimplementasikan konfigurasi ESP32 pada mode Station (STA) agar dapat terhubung ke jaringan WiFi yang sudah tersedia.

### Konfigurasi Rangkaian

LED indikator (opsional) dihubungkan ke pin GPIO 2 melalui resistor 220 Ω, kaki LED lainnya dihubungkan ke GND.

### Penjelasan Code

- `WiFi.mode(WIFI_STA)`  menetapkan mode operasi WiFi ESP32 menjadi Station (klien).
- `WiFi.begin(ssid, password)`  memulai proses koneksi ke jaringan WiFi yang dituju.
- `while (WiFi.status() != WL_CONNECTED)`  loop yang menunggu hingga status koneksi berhasil, mencetak "." setiap 500 ms sebagai indikator progres.
- Setelah terhubung, `WiFi.localIP()`, `WiFi.macAddress()`, dan `WiFi.RSSI()` dipanggil untuk menampilkan IP Address, MAC Address, dan kekuatan sinyal ke Serial Monitor, lalu LED indikator dinyalakan.
- Di dalam `loop()`, status koneksi dicek ulang setiap 5 detik; jika terputus, LED dimatikan dan pesan "Status: Terputus" dicetak.

### Penjelasan Code Modifikasi (reconnect otomatis)

- Saat status koneksi terdeteksi `!= WL_CONNECTED` di dalam `loop()`, program memanggil `WiFi.disconnect()` lalu `WiFi.begin()` ulang untuk memicu proses reconnect.
- Ditambahkan batas waktu tunggu menggunakan `millis() - waktuMulai < 10000` (maksimal ~10 detik) agar program tidak menggantung tanpa batas seperti pada koneksi awal.
- Jika reconnect berhasil dalam batas waktu tersebut, LED dinyalakan kembali; jika gagal, program tetap lanjut ke siklus `loop()` berikutnya dan akan mencoba lagi otomatis 5 detik kemudian.

### Jawaban Pertanyaan Praktikum

**1) Flowchart proses koneksi ESP32 ke jaringan WiFi**

Alur program: inisialisasi Serial dan pin LED di `setup()` → set `WiFi.mode(WIFI_STA)` → panggil `WiFi.begin(ssid, password)` untuk memulai koneksi → masuk ke loop `while (WiFi.status() != WL_CONNECTED)` yang terus mengecek status koneksi setiap 500 ms (mencetak "." sebagai indikator progres) → setelah status `WL_CONNECTED` tercapai, keluar dari loop `while` dan mencetak IP Address, MAC Address, serta RSSI ke Serial Monitor, lalu menyalakan LED indikator → program masuk ke `loop()` utama yang mengecek status koneksi setiap 5 detik secara berulang, mencetak "Terhubung" atau "Terputus" (mematikan LED jika terputus).

*(Diagram alur visual dilampirkan sebagai gambar terpisah  lihat bagian Dokumentasi.)*

**2) Fungsi perintah `WiFi.mode(WIFI_STA)`**

Fungsi ini menetapkan mode operasi WiFi ESP32 menjadi **Station (STA)**, yaitu mode di mana ESP32 bertindak sebagai klien yang akan terhubung ke jaringan WiFi yang sudah ada (seperti router atau hotspot), bukan menjadi penyedia jaringan sendiri. Pemanggilan fungsi ini penting dilakukan sebelum `WiFi.begin()` agar ESP32 tahu peran apa yang harus dijalankan saat mencoba melakukan koneksi.

**3) Apa yang terjadi apabila SSID atau password yang dimasukkan salah?**

Jika SSID atau password yang dimasukkan salah, ESP32 tidak akan pernah berhasil mendapatkan status `WL_CONNECTED` dari access point tujuan. Akibatnya, program akan terjebak tanpa batas waktu (infinite loop) di dalam `while (WiFi.status() != WL_CONNECTED)`, terus-menerus mencetak "." di Serial Monitor tanpa pernah lanjut ke baris kode setelahnya (menampilkan IP Address, MAC Address, RSSI, dan menyalakan LED). Program akan tampak "menggantung" karena tidak ada mekanisme timeout pada kode original ini.

**4) Modifikasi program: reconnect otomatis apabila koneksi terputus**

Lihat file: [`code/percobaan2a_konfigurasi_sta_modifikasi.ino`](./code/percobaan2a_konfigurasi_sta_modifikasi.ino)  

penjelasan tiap baris kode tambahan sudah dicantumkan di bagian **Penjelasan Code Modifikasi** di atas.

---

## Percobaan 2B: Konfigurasi Mode Access Point (AP)

- Code Percobaan :[`code/percobaan2b_konfigurasi_ap.ino`](./code/percobaan2b_konfigurasi_ap.ino)
- Code modifikasi:[`code/percobaan2b_konfigurasi_ap_modifikasi.ino`](./code/percobaan2b_konfigurasi_ap_modifikasi.ino)

### Tujuan

Memahami dan mengimplementasikan konfigurasi ESP32 sebagai Access Point (AP) yang dapat diakses langsung oleh perangkat lain.

### Parameter Konfigurasi Access Point

| No | Parameter | Nilai |
|----|-----------|-------|
| 1 | SSID Access Point | ESP32_AccessPoint |
| 2 | Password | 12345678 |
| 3 | IP Address default | 192.168.4.1 |

### Penjelasan Code Original

- `WiFi.mode(WIFI_AP)`  menetapkan mode operasi WiFi ESP32 menjadi Access Point.
- `WiFi.softAP(ap_ssid, ap_password)`  mengaktifkan ESP32 sebagai Access Point dengan SSID dan password yang ditentukan.
- `WiFi.softAPIP()`  mengambil alamat IP dari Access Point yang dibuat (default 192.168.4.1), lalu ditampilkan ke Serial Monitor.
- Di dalam `loop()`, `WiFi.softAPgetStationNum()` dipanggil setiap 5 detik untuk memantau jumlah perangkat yang sedang terhubung ke Access Point.

### Penjelasan Code Modifikasi (mode AP+STA)

- `WiFi.mode(WIFI_AP_STA)` mengaktifkan kedua mode sekaligus dalam satu waktu, berbeda dari kode original yang hanya menggunakan `WIFI_AP`.
- `WiFi.softAP()` tetap dipanggil untuk mengaktifkan Access Point ESP32 sendiri, sementara `WiFi.begin()` dipanggil terpisah untuk menyambungkan ESP32 sebagai Station ke jaringan WiFi rumah.
- Karena kedua mode berjalan bersamaan, `loop()` memantau kedua sisi: jumlah client yang terhubung ke Access Point ESP32, sekaligus status koneksi ESP32 sebagai klien ke jaringan rumah.

### Jawaban Pertanyaan Praktikum

**1) Mengapa alamat IP default Access Point pada ESP32 umumnya bernilai 192.168.4.1?**

Alamat 192.168.4.1 merupakan nilai default yang sudah ditetapkan di dalam pustaka WiFi/SDK ESP32 (Espressif) untuk mode Soft Access Point, sebagai bagian dari rentang alamat IP privat kelas C (192.168.x.x) yang lazim digunakan untuk jaringan lokal. Nilai ini bisa diubah secara manual menggunakan fungsi `WiFi.softAPConfig()`, namun secara default SDK sudah menetapkan 192.168.4.1 sebagai gateway/IP dari Access Point tersebut.

**2) Apa perbedaan mendasar antara mode Station dan mode Access Point pada ESP32?**

Perbedaan mendasarnya terletak pada peran ESP32 dalam jaringan. Pada mode Station, ESP32 berperan sebagai **klien** yang bergabung ke jaringan WiFi yang sudah ada (seperti router), dan alamat IP-nya diberikan oleh router tersebut melalui DHCP. Pada mode Access Point, ESP32 berperan sebagai **penyedia jaringan** itu sendiri  ESP32 membuat SSID-nya sendiri, mengatur alamat IP untuk perangkat yang terhubung (default gateway 192.168.4.1), dan tidak bergantung pada router eksternal.

**3) Risiko keamanan apabila password Access Point tidak diberikan atau terlalu sederhana**

Jika Access Point dibuat tanpa password (open network) atau menggunakan password yang terlalu sederhana, maka: siapa pun dalam jangkauan sinyal dapat terhubung tanpa otorisasi; data yang dikirim berisiko disadap (man-in-the-middle); jika Access Point digunakan untuk provisioning, pihak tak dikenal berpotensi mengambil alih konfigurasi perangkat; dan ESP32 bisa kehabisan slot koneksi akibat perangkat asing yang ikut terhubung, mengganggu fungsi utama sistem IoT.

**4) Modifikasi program: mode AP+STA**

Lihat file: [`code/percobaan2b_konfigurasi_ap_modifikasi.ino`](./code/percobaan2b_konfigurasi_ap_modifikasi.ino)  

penjelasan inti perubahan sudah dicantumkan di bagian **Penjelasan Code Modifikasi** di atas.

---

## Pertanyaan Analisis

**1) Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!**

*(Isi bagian ini sesuai data pengamatan asli praktikummu, misalnya: Pada Percobaan 2A, ESP32 berhasil terhubung ke jaringan WiFi yang ditentukan dan menampilkan IP Address, MAC Address, serta RSSI di Serial Monitor. Pada Percobaan 2B, ESP32 berhasil membuat Access Point dengan SSID "ESP32_AccessPoint" dan dapat diakses oleh smartphone/laptop, dengan jumlah client yang terpantau bertambah di Serial Monitor saat ada perangkat yang terhubung.)*

**2) Bagaimana pengaruh kekuatan sinyal (RSSI) terhadap kestabilan koneksi WiFi pada perangkat IoT?**

Nilai RSSI yang lebih besar (lebih mendekati 0, misalnya -40 dBm) menunjukkan sinyal yang kuat dan koneksi yang cenderung lebih stabil dengan latensi rendah. Sebaliknya, nilai RSSI yang jauh dari 0 (misalnya -80 dBm ke bawah) menandakan sinyal lemah, yang dapat menyebabkan koneksi mudah terputus atau ESP32 gagal mempertahankan status `WL_CONNECTED`. Pada perangkat IoT, memantau RSSI penting untuk menentukan penempatan perangkat yang optimal terhadap access point.

**3) Bagaimana cara kerja ESP32 dalam membedakan peran sebagai klien (Station) dan sebagai penyedia jaringan (Access Point)?**

ESP32 membedakan perannya melalui pengaturan mode WiFi menggunakan fungsi `WiFi.mode()`. Saat mode diset ke `WIFI_STA`, ESP32 bertindak sebagai klien yang mencari dan bergabung ke access point lain menggunakan `WiFi.begin()`, sehingga menerima IP dari DHCP router tersebut. Saat mode diset ke `WIFI_AP`, ESP32 menjalankan fungsi seperti router kecil menggunakan `WiFi.softAP()`  menyediakan SSID sendiri, menjalankan DHCP server sendiri, dan menunggu perangkat lain terhubung. Ketika mode `WIFI_AP_STA` digunakan, kedua stack tersebut berjalan bersamaan.

**4) Bagaimana kombinasi mode Station dan Access Point (AP+STA) dapat dimanfaatkan dalam skenario nyata sistem IoT, misalnya pada proses konfigurasi awal perangkat (provisioning)?**

Kombinasi AP+STA banyak dimanfaatkan pada skenario provisioning perangkat IoT, di mana perangkat baru belum memiliki kredensial WiFi rumah tersimpan. Prosesnya: ESP32 pertama kali menyala dalam mode AP, memancarkan SSID sendiri, lalu pengguna menghubungkan smartphone ke SSID tersebut dan membuka halaman web konfigurasi (captive portal) untuk memasukkan SSID dan password WiFi rumah. Setelah kredensial disimpan, ESP32 menjalankan mode AP+STA untuk terhubung ke jaringan rumah sambil tetap mempertahankan Access Point-nya sebagai fallback jika koneksi gagal.

---

## Kesimpulan

1. ESP32 dapat dikonfigurasi ke dalam tiga mode operasi WiFi: Station (STA), Access Point (AP), dan gabungan AP+STA, sesuai kebutuhan aplikasi IoT.
2. Mode Station digunakan ketika ESP32 perlu terhubung ke jaringan yang sudah ada dan mengakses internet/server lokal, sedangkan mode Access Point digunakan ketika ESP32 perlu menjadi penyedia jaringan mandiri, misalnya untuk provisioning awal perangkat.
3. Pustaka WiFi.h menyediakan fungsi-fungsi penting seperti `WiFi.begin()`, `WiFi.status()`, `WiFi.localIP()`, `WiFi.macAddress()`, `WiFi.RSSI()`, `WiFi.softAP()`, dan `WiFi.softAPIP()` untuk mengatur dan memonitor koneksi jaringan pada ESP32.
4. Penambahan mekanisme reconnect otomatis dan batas waktu tunggu (timeout) penting diterapkan pada mode Station agar sistem lebih andal dan tidak menggantung ketika koneksi terputus.
5. Keamanan Access Point (penggunaan password yang kuat) penting diperhatikan untuk mencegah akses tidak sah maupun potensi penyusupan konfigurasi pada sistem IoT.
6. Mode AP+STA memungkinkan ESP32 menjalankan dua peran sekaligus, dan banyak dimanfaatkan pada skenario nyata seperti provisioning perangkat IoT maupun WiFi range extender.
