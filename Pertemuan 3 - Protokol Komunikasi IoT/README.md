# Pertemuan 3 Protokol Komunikasi IoT

## Deskripsi Singkat

Praktikum ini membahas dua protokol komunikasi yang umum digunakan pada sistem IoT berbasis ESP32: HTTP dan MQTT, dengan data yang dipertukarkan diformat menggunakan JSON. Percobaan 3A berfokus pada pengiriman data dari ESP32 ke server menggunakan protokol HTTP metode POST, sedangkan Percobaan 3B berfokus pada pertukaran data dari ESP32 ke broker MQTT menggunakan pola publish-subscribe.

## Alat dan Bahan

- Board ESP32 DevKit
- Kabel USB (Micro-USB/USB-C sesuai board)
- Laptop/PC dengan Arduino IDE (sudah terpasang board manager ESP32, pustaka ArduinoJson, dan PubSubClient)
- Jaringan WiFi yang terhubung ke internet
- Aplikasi client MQTT (MQTT Explorer/HiveMQ WebSocket Client) untuk verifikasi data
- Broker MQTT publik `broker.hivemq.com` (port 1883) dan endpoint uji HTTP `httpbin.org/post`

## Library / Dependencies

- `WiFi.h`pustaka bawaan board manager ESP32, digunakan untuk koneksi jaringan WiFi.
- `HTTPClient.h`pustaka bawaan board manager ESP32, digunakan untuk mengirim request HTTP (Percobaan 3A).
- `PubSubClient.h` (by Nick O'Leary)digunakan untuk komunikasi MQTT (Percobaan 3B). Instal melalui Arduino IDE Library Manager.
- `ArduinoJson.h` (by Benoit Blanchon)digunakan untuk membuat dan mem-parsing data JSON. Instal melalui Arduino IDE Library Manager.

---

## Percobaan 3A: Komunikasi Data Menggunakan HTTP

- Code original: [`code/percobaan3a_komunikasi_http.ino`](./code/percobaan3a_komunikasi_http.ino)
- Code modifikasi: [`code/percobaan3a_komunikasi_http_modifikasi.ino`](./code/percobaan3a_komunikasi_http_modifikasi.ino)

### Tujuan

Memahami dan mengimplementasikan pengiriman data dari ESP32 ke server menggunakan protokol HTTP dengan metode POST dalam format JSON.

### Penjelasan Code

- `WiFi.begin(ssid, password)`menghubungkan ESP32 ke jaringan WiFi yang tersedia, ditunggu hingga status `WL_CONNECTED`.
- `HTTPClient http; http.begin(serverUrl)`menyiapkan objek HTTP client dan menentukan endpoint tujuan (`https://httpbin.org/post`).
- `http.addHeader("Content-Type", "application/json")`memberi tahu server bahwa data yang dikirim berformat JSON.
- `JsonDocument doc; doc["suhu"] = 28.5; doc["kelembaban"] = 65.0`membuat objek JSON berisi data sensor.
- `serializeJson(doc, requestBody)`mengubah objek JSON menjadi teks (string) agar dapat dikirimkan sebagai body request.
- `http.POST(requestBody)`mengirim data ke server menggunakan metode POST, hasilnya berupa kode response HTTP.
- Jika `httpResponseCode > 0`, program mencetak kode response dan isi balasan server (`http.getString()`); jika gagal, dicetak kode error.
- `http.end()`menutup koneksi HTTP setelah proses selesai, dan `delay(10000)` mengatur pengiriman data setiap 10 detik.

### Penjelasan Code Modifikasi (tambah data waktu)

- Ditambahkan satu baris `doc["waktu_ms"] = millis();` sebelum proses `serializeJson()`.
- `millis()` mengembalikan jumlah milidetik sejak ESP32 dinyalakan/di-reset, sehingga setiap data yang dikirim kini memuat timestamp relatif, berguna untuk mengurutkan atau menghitung interval antar pengiriman data di sisi penerima/server.

### Jawaban Pertanyaan Praktikum

**1) Flowchart proses pengiriman data melalui HTTP POST**

Alur program: inisialisasi Serial dan koneksi WiFi di `setup()` → di dalam `loop()`, program memeriksa apakah WiFi terhubung → jika terhubung, siapkan objek `HTTPClient` dan tentukan URL endpoint (`http.begin(serverUrl)`) → tambahkan header `Content-Type: application/json` → buat objek JSON berisi data suhu dan kelembaban menggunakan `JsonDocument` → serialisasi objek JSON menjadi string (`serializeJson`) → kirim data melalui `http.POST(requestBody)` → periksa `httpResponseCode`: jika lebih besar dari 0, cetak kode response dan isi balasan server (`http.getString()`); jika tidak, cetak pesan gagal beserta kode error → tutup koneksi dengan `http.end()` → jika WiFi tidak terhubung, langsung lompat tanpa memproses apa pun → tunggu 10 detik (`delay(10000)`) sebelum mengulang proses dari awal.

*(Diagram alur visual dilampirkan sebagai gambar terpisahlihat bagian Dokumentasi.)*

**2) Fungsi `http.addHeader("Content-Type", "application/json")`**

Fungsi ini menambahkan header HTTP bernama `Content-Type` dengan nilai `application/json` pada request yang akan dikirim. Header ini memberi tahu server bahwa isi (body) dari request tersebut berformat JSON, sehingga server dapat memproses/mem-parsing data dengan benar sesuai formatnya, bukan memperlakukannya sebagai teks biasa atau format lain seperti `application/x-www-form-urlencoded`.

**3) Arti kode response HTTP 200 dan contoh kode response lain**

Kode response HTTP **200 (OK)** menandakan bahwa request berhasil diproses oleh server tanpa masalah, dan server memberikan balasan sesuai yang diharapkandalam kasus ini, httpbin.org berhasil menerima data JSON dan mengembalikannya (echo) sebagai bukti. Contoh kode response lain: **404 (Not Found)**endpoint yang dituju tidak ditemukan di server; **500 (Internal Server Error)**terjadi kesalahan pada sisi server saat memproses request.

**4) Modifikasi program: menambahkan data waktu (millis())**

Lihat file: [`code/percobaan3a_komunikasi_http_modifikasi.ino`](./code/percobaan3a_komunikasi_http_modifikasi.ino)

penjelasan tiap baris kode tambahan sudah dicantumkan di bagian **Penjelasan Code Modifikasi** di atas.

---

## Percobaan 3B: Komunikasi MQTT

- Code: [`code/percobaan3b_komunikasi_mqtt.ino`](./code/percobaan3b_komunikasi_mqtt.ino)

### Tujuan

Memahami dan mengimplementasikan pertukaran data dari ESP32 ke broker MQTT menggunakan pola publish-subscribe dengan format data JSON.

### Parameter Konfigurasi

| No | Parameter | Nilai |
|----|-----------|-------|
| 1 | Broker MQTT | broker.hivemq.com |
| 2 | Port | 1883 |
| 3 | Topic | unsoed/tk245005/H1H024001/sensor |

### Penjelasan Code

- `hubungkanWiFi()`fungsi terpisah untuk menghubungkan ESP32 ke jaringan WiFi, dipanggil sekali di `setup()`.
- `client.setServer(mqttServer, mqttPort)`mengatur alamat dan port broker MQTT yang dituju.
- `hubungkanMQTT()`fungsi yang mencoba menghubungkan ESP32 ke broker MQTT menggunakan client ID unik (dibuat dari angka acak), dan akan mengulang setiap 2 detik jika gagal.
- `client.loop()`dipanggil pada setiap iterasi `loop()` untuk menjaga koneksi MQTT tetap aktif dan memproses pesan masuk/keluar.
- `JsonDocument doc; serializeJson(doc, buffer)`membuat dan mengubah data sensor menjadi format JSON dalam bentuk buffer teks.
- `client.publish(mqttTopic, buffer)`mempublikasikan data JSON ke topic MQTT yang telah ditentukan.
- `delay(5000)`mengatur agar data dipublikasikan setiap 5 detik.

### Jawaban Pertanyaan Praktikum

**1) Fungsi topic pada MQTT dan alasan perlu dibuat unik**

Topic berfungsi sebagai alamat/label pengelompokan data yang digunakan broker untuk menentukan ke subscriber mana suatu pesan harus diteruskan. Topic perlu dibuat unik (menyertakan nama kelompok/NIM) karena broker publik seperti `broker.hivemq.com` digunakan bersama oleh banyak pengguna secara bersamaan; jika tidak unik, data dari kelompok lain berpotensi tercampur atau menyulitkan proses verifikasi.

**2) Fungsi perintah `client.loop()` yang dipanggil pada setiap iterasi loop()**

Fungsi ini menjaga koneksi ke broker MQTT tetap aktif dengan menangani mekanisme *keep-alive*, serta memproses pesan masuk (jika ESP32 subscribe ke suatu topic) maupun pesan yang sedang dalam antrean. Jika tidak dipanggil secara berkala, koneksi MQTT berisiko terputus karena broker menganggap client tidak lagi aktif (timeout keep-alive).

**3) Apa yang terjadi apabila koneksi ke broker MQTT terputus di tengah program berjalan?**

Pengecekan `if (!client.connected())` pada `loop()` akan bernilai benar (true), sehingga program otomatis memanggil kembali `hubungkanMQTT()` untuk mencoba menyambungkan ulang setiap 2 detik hingga berhasil. Selama proses reconnect berlangsung, publikasi data akan tertunda atau gagal, karena `client.publish()` tidak akan berhasil mengirim data selama koneksi belum pulih.

---

## Pertanyaan Analisis

**1) Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!**

Pada Percobaan 3A, ESP32 berhasil mengirimkan data suhu dan kelembaban dalam format JSON ke httpbin.org/post melalui HTTP POST setiap 10 detik, dengan kode response 200 dan server berhasil meng-echo kembali data yang dikirim. Pada Percobaan 3B, ESP32 berhasil terhubung ke broker broker.hivemq.com dan mempublikasikan data JSON ke topic yang ditentukan setiap 5 detik, yang berhasil diverifikasi muncul di aplikasi MQTT Explorer.

**2) Bandingkan besar overhead data dan pola komunikasi antara protokol HTTP dan MQTT berdasarkan hasil percobaan yang telah dilakukan!**

HTTP menggunakan pola request-response, di mana setiap pengiriman data memerlukan header yang relatif besar (Content-Type, Content-Length, Host, User-Agent, dsb) meskipun payload JSON-nya kecil, sehingga overhead per pengiriman cenderung besar. Sebaliknya, MQTT menggunakan pola publish-subscribe dengan koneksi yang tetap terjaga; overhead besar hanya terjadi sekali di awal saat CONNECT, sedangkan tiap publish setelahnya overhead-nya jauh lebih kecil, sehingga total data yang dikirim MQTT jauh lebih hemat dibandingkan HTTP untuk jumlah pesan yang sama.

**3) Untuk skenario pengiriman data sensor secara terus-menerus setiap beberapa detik dalam jangka waktu lama, protokol manakah yang lebih sesuai?**

MQTT lebih sesuai, karena dirancang khusus untuk komunikasi data kontinu dengan koneksi yang tetap terjaga, tidak perlu membuka-menutup koneksi berulang kali seperti HTTP. Overhead komunikasi MQTT yang kecil juga membuatnya lebih hemat bandwidth dan daya, sangat penting untuk perangkat IoT dengan sumber daya terbatas yang mengirim data secara berkala dalam jangka waktu lama.

**4) Bagaimana peran format JSON dalam mendukung interoperabilitas data antara perangkat IoT dan berbagai platform/aplikasi yang berbeda?**

JSON berperan sebagai format data standar yang ringan, berbasis teks, dan tidak bergantung pada bahasa pemrograman maupun platform tertentu, sehingga data ESP32 mudah dibaca dan diproses oleh berbagai sistem penerima (server, dashboard web, aplikasi mobile, platform cloud IoT). Struktur key-value pada JSON juga memudahkan penambahan/perubahan data tanpa mengubah keseluruhan protokol komunikasi, sehingga mendukung interoperabilitas antar berbagai perangkat dan aplikasi dalam ekosistem IoT.

---

## Kesimpulan

1. ESP32 dapat mengirimkan data sensor ke server menggunakan protokol HTTP metode POST, dengan data diformat sebagai JSON menggunakan pustaka ArduinoJson dan dikirim melalui pustaka HTTPClient.
2. ESP32 dapat mempublikasikan data sensor ke broker MQTT publik menggunakan pola publish-subscribe dengan pustaka PubSubClient, dan data dapat diverifikasi melalui aplikasi client MQTT yang subscribe ke topic yang sama.
3. HTTP memiliki overhead komunikasi yang lebih besar dibandingkan MQTT karena setiap request membawa header yang relatif besar, sedangkan MQTT lebih hemat karena hanya memerlukan overhead besar sekali saat proses koneksi awal.
4. MQTT lebih cocok untuk pengiriman data kontinu jangka panjang karena koneksinya tetap terjaga (persistent) dan hemat bandwidth/daya, sedangkan HTTP lebih sesuai untuk komunikasi yang sifatnya sesekali atau memicu aksi spesifik di server.
5. Topic pada MQTT perlu dibuat unik ketika menggunakan broker publik agar data tidak tercampur dengan pengguna lain, dan fungsi `client.loop()` penting dipanggil secara berkala untuk menjaga koneksi tetap aktif.
6. Format JSON berperan penting dalam mendukung interoperabilitas data IoT, karena bersifat ringan, mudah dibaca, dan dapat diproses oleh berbagai platform maupun bahasa pemrograman yang berbeda.
