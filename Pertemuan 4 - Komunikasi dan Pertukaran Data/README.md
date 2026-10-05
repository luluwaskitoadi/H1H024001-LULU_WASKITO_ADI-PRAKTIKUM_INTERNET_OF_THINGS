# Pertemuan 4 — Komunikasi dan Pertukaran Data (Publish dan Subscribe)

## Deskripsi Singkat

Praktikum ini membahas komunikasi data dua arah (bidirectional) pada sistem IoT berbasis ESP8266. Percobaan 4A berfokus pada mekanisme subscribe dan deserialisasi data JSON untuk mengendalikan aktuator (LED) berdasarkan perintah yang diterima dari broker MQTT. Percobaan 4B berfokus pada integrasi publish dan subscribe secara bersamaan (full duplex) — ESP8266 mempublikasikan data sensor suhu secara berkala sekaligus tetap dapat menerima perintah kendali, menggunakan pendekatan non-blocking dengan `millis()`.

## Alat dan Bahan

- Board ESP8266 DevKit
- Sensor DHT11
- LED (sebagai simulasi aktuator) dan Resistor 220 Ω
- Breadboard dan Kabel Jumper
- Kabel USB (Micro-USB/USB-C sesuai board)
- Laptop/PC dengan Arduino IDE (board manager ESP8266, pustaka DHT sensor library, PubSubClient, dan ArduinoJson)
- Jaringan WiFi yang terhubung ke internet
- Aplikasi client MQTT (MQTT Explorer/HiveMQ WebSocket Client) untuk mengirim perintah kendali dan memantau data
- Broker MQTT publik `broker.hivemq.com` (port 1883)

## Library / Dependencies

- `ESP8266WiFi.h` — pustaka bawaan board manager ESP8266, digunakan untuk koneksi jaringan WiFi.
- `PubSubClient.h` (by Nick O'Leary) — digunakan untuk komunikasi MQTT (publish & subscribe).
- `ArduinoJson.h` (by Benoit Blanchon) — digunakan untuk serialisasi dan deserialisasi data JSON.
- `DHT.h` (DHT sensor library by Adafruit) — digunakan untuk membaca data suhu dari sensor DHT11 (Percobaan 4B).

---

## Percobaan 4A: Subscribe dan Deserialisasi Data JSON untuk Kendali Aktuator

- Code Percobaan: [`code/percobaan4a_subscribe_json.ino`](./code/percobaan4a_subscribe_json.ino)
- Code modifikasi: [`code/percobaan4a_subscribe_json_modifikasi.ino`](./code/percobaan4a_subscribe_json_modifikasi.ino)

### Tujuan

Memahami dan mengimplementasikan mekanisme subscribe pada MQTT beserta proses deserialisasi data JSON untuk mengendalikan aktuator berdasarkan perintah yang diterima.

### Konfigurasi Pin

| No | Komponen | Pin ESP8266 |
|----|----------|-------------|
| 1 | LED (anoda, via resistor 220 Ω) | GPIO 26 |
| 2 | LED (katoda) | GND |

### Penjelasan Code

- `callback(topic, payload, length)` — fungsi yang dipanggil otomatis setiap kali ada pesan baru masuk pada topic yang di-subscribe; pesan mentah (byte array) digabung menjadi `String`.
- `deserializeJson(doc, pesan)` — mengurai teks JSON yang diterima menjadi objek `JsonDocument` yang dapat diakses nilainya; jika gagal (format tidak valid), `error` akan bernilai true dan program mencetak pesan error lalu `return`.
- `doc["perintah"]` — mengambil nilai dari key `"perintah"` pada JSON yang diterima, lalu dibandingkan dengan `"ON"`/`"OFF"` untuk menentukan status LED.
- `client.subscribe(topicPerintah)` — dipanggil di dalam `hubungkanMQTT()` setelah koneksi ke broker berhasil, mendaftarkan ESP8266 untuk menerima pesan dari `topicPerintah`.
- `client.setCallback(callback)` — mendaftarkan fungsi callback di `setup()`, agar dijalankan setiap ada pesan masuk.
- `client.loop()` — dipanggil terus-menerus di `loop()` agar ESP8266 tetap responsif memeriksa pesan masuk dan menjaga koneksi broker tetap aktif.

### Penjelasan Code Modifikasi (kendali intensitas PWM)

- JSON yang diterima kini bisa memuat key tambahan `"intensitas"`, misalnya `{"perintah": "ON", "intensitas": 200}`.
- `int intensitas = doc["intensitas"] | 255;` — mengambil nilai intensitas dari JSON; jika key tidak ada, otomatis menggunakan nilai default 255 (kecerahan penuh).
- `constrain(intensitas, 0, 255)` — membatasi nilai intensitas agar tetap dalam rentang valid PWM (0–255).
- `analogWrite(ledPin, intensitas)` — menyalakan LED dengan kecerahan sesuai nilai intensitas yang diterima, menggantikan `digitalWrite()` yang hanya bisa ON/OFF penuh.

### Jawaban Pertanyaan Praktikum

**1) Flowchart proses penerimaan dan pemrosesan pesan pada fungsi callback**

Alur: pesan masuk pada topic yang di-subscribe → fungsi `callback()` terpanggil otomatis dengan parameter topic, payload, dan length → payload (byte array) digabung menjadi string pesan → pesan dicetak ke Serial Monitor → `deserializeJson()` mencoba mengurai string menjadi objek JSON → jika gagal (error), cetak pesan error dan `return` (hentikan proses) → jika berhasil, ambil nilai `doc["perintah"]` → bandingkan dengan `"ON"`/`"OFF"` → set `digitalWrite(ledPin, HIGH/LOW)` sesuai hasil perbandingan.

*(Diagram alur visual dapat dilampirkan sebagai gambar terpisah — lihat bagian Dokumentasi.)*

**2) Apa yang terjadi apabila pesan yang dipublikasikan bukan format JSON yang valid?**

`deserializeJson()` akan mengembalikan objek `DeserializationError` yang bernilai true (terjadi error). Program akan mencetak pesan `"Gagal parsing JSON: ..."` beserta jenis errornya ke Serial Monitor, lalu langsung `return` dari fungsi `callback()` tanpa memproses perintah lebih lanjut — sehingga status LED tidak berubah sama sekali.

**3) Mengapa fungsi client.subscribe() dipanggil di dalam hubungkanMQTT(), bukan di dalam setup()?**

Karena `client.subscribe()` hanya akan berhasil jika koneksi ke broker MQTT sudah benar-benar terbentuk (`client.connected()` bernilai true). Jika dipanggil langsung di `setup()` sebelum koneksi MQTT berhasil dibuat, perintah subscribe akan gagal karena belum ada koneksi aktif ke broker. Dengan menempatkannya di dalam `hubungkanMQTT()` (tepat setelah `client.connect()` berhasil), subscribe dijamin selalu dilakukan ulang setiap kali koneksi baru terbentuk, termasuk saat reconnect setelah koneksi sempat terputus.

**4) Modifikasi program: kendali intensitas LED menggunakan PWM**

Lihat file: [`code/percobaan4a_subscribe_json_modifikasi.ino`](./code/percobaan4a_subscribe_json_modifikasi.ino)

penjelasan tiap baris kode tambahan sudah dicantumkan di bagian **Penjelasan Code Modifikasi** di atas.

---

## Percobaan 4B: Pertukaran Data Dua Arah (Publish dan Subscribe Secara Bersamaan)

- Code Percobaan: [`code/percobaan4b_pertukaran_dua_arah.ino`](./code/percobaan4b_pertukaran_dua_arah.ino)
- Code modifikasi: [`code/percobaan4b_pertukaran_dua_arah_modifikasi.ino`](./code/percobaan4b_pertukaran_dua_arah_modifikasi.ino)

### Tujuan

Mengimplementasikan sistem IoT yang dapat mempublikasikan data sensor dan menerima perintah kendali secara bersamaan (full duplex), sebagai integrasi antara mekanisme publish (Modul 3) dan subscribe (Percobaan 4A).

### Konfigurasi Pin

| No | Komponen | Pin ESP8266 |
|----|----------|-------------|
| 1 | DHT11 — VCC | 3.3V |
| 2 | DHT11 — DATA | GPIO 4 |
| 3 | DHT11 — GND | GND |
| 4 | LED (anoda, via resistor 220 Ω) | GPIO 26 |
| 5 | LED (katoda) | GND |
| 6 *(modifikasi)* | Buzzer (+) | GPIO 27 |
| 7 *(modifikasi)* | Buzzer (−) | GND |

### Penjelasan Code

- `topicData` dan `topicPerintah` — dua topic terpisah: satu untuk ESP8266 publish data suhu, satu lagi untuk ESP8266 subscribe menerima perintah kendali.
- `waktuTerakhirPublish` dan `intervalPublish` — digunakan bersama `millis()` untuk menjadwalkan publish data suhu setiap 5 detik **tanpa** memakai `delay()` yang bersifat blocking.
- Di dalam `loop()`: `client.loop()` dipanggil terus-menerus (memproses pesan masuk kapan saja), sedangkan blok `if (millis() - waktuTerakhirPublish > intervalPublish)` hanya dieksekusi ketika sudah lewat 5 detik sejak publish terakhir — kedua proses ini berjalan "bersamaan" tanpa saling memblokir satu sama lain.
- `dht.readTemperature()` dan validasi `!isnan(suhu)` — memastikan hanya data suhu valid yang dikemas jadi JSON dan di-publish.

### Penjelasan Code Modifikasi (tambah aktuator kedua/buzzer)

- `topicPerintahBuzzer` ditambahkan sebagai topic kedua yang terpisah dari `topicPerintahLED`, sehingga masing-masing aktuator punya "alamat" perintahnya sendiri.
- `buzzerPin` (GPIO 27) diatur sebagai output tambahan di `setup()`.
- Di dalam `callback()`, parameter `topic` diubah menjadi `String` agar bisa dibandingkan dengan `==`, lalu program memeriksa topic asal pesan untuk menentukan aktuator mana yang dikendalikan (LED atau buzzer) — satu fungsi callback menangani dua topic sekaligus.
- `hubungkanMQTT()` kini memanggil dua `client.subscribe()` (untuk topic LED dan topic buzzer) sekaligus setelah koneksi berhasil.

### Jawaban Pertanyaan Praktikum

**1) Mengapa penggunaan delay() yang lama sebaiknya dihindari pada program yang menggabungkan proses publish dan subscribe secara bersamaan?**

Karena `delay()` bersifat blocking — selama delay berjalan, seluruh program berhenti total, termasuk `client.loop()` yang bertugas memeriksa dan memproses pesan masuk dari broker. Jika ada perintah kendali masuk saat program sedang "tertahan" di dalam `delay()`, ESP8266 tidak akan bisa merespons sampai delay tersebut selesai, sehingga sistem terasa lambat/tidak real-time dan berpotensi kehilangan keep-alive ke broker (risiko disconnect).

**2) Jelaskan cara kerja mekanisme non-blocking menggunakan fungsi millis() pada program di atas!**

`millis()` mengembalikan jumlah milidetik sejak ESP8266 menyala, tanpa menghentikan eksekusi program. Setiap iterasi `loop()`, program membandingkan selisih antara waktu sekarang (`millis()`) dengan waktu publish terakhir (`waktuTerakhirPublish`); jika selisihnya sudah melebihi `intervalPublish` (5000 ms), barulah blok publish dijalankan dan `waktuTerakhirPublish` diperbarui ke waktu saat ini. Di luar kondisi tersebut, `loop()` tetap berjalan cepat dan terus memanggil `client.loop()`, sehingga proses penerimaan pesan (subscribe) tidak pernah tertunda oleh jadwal publish.

**3) Apa yang akan terjadi apabila fungsi client.loop() jarang dipanggil (misalnya hanya sekali setiap 10 detik)?**

ESP8266 akan menjadi kurang responsif terhadap pesan masuk — perintah kendali yang dikirim akan tertunda hingga 10 detik sebelum diproses, alih-alih segera. Lebih buruk lagi, broker MQTT dapat menganggap client tidak lagi aktif (timeout keep-alive) karena `client.loop()` juga bertugas menjaga koneksi tetap hidup, sehingga koneksi ke broker berisiko terputus jika fungsi ini dipanggil terlalu jarang.

**4) Modifikasi program: tambah topic perintah baru untuk aktuator kedua (buzzer)**

Lihat file: [`code/percobaan4b_pertukaran_dua_arah_modifikasi.ino`](./code/percobaan4b_pertukaran_dua_arah_modifikasi.ino) 

penjelasan inti perubahan sudah dicantumkan di bagian **Penjelasan Code Modifikasi** di atas.

---

## Pertanyaan Analisis

**1) Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!**

Pada Percobaan 4A, ESP8266 berhasil subscribe ke topic perintah dan LED merespons sesuai nilai "ON"/"OFF" yang diterima melalui deserialisasi JSON. Pada Percobaan 4B, ESP8266 berhasil mempublikasikan data suhu setiap 5 detik sekaligus tetap dapat menerima dan merespons perintah kendali LED secara real-time tanpa delay yang mengganggu, membuktikan sistem berjalan full duplex.

**2) Bandingkan mekanisme komunikasi satu arah (publish saja, seperti pada Modul Praktikum 3) dengan komunikasi dua arah (publish dan subscribe) yang diimplementasikan pada modul ini!**

Komunikasi satu arah (Modul 3) hanya memungkinkan ESP8266 mengirim data ke broker, tanpa kemampuan menerima perintah balik — cocok untuk sekadar monitoring. Komunikasi dua arah (Modul 4) menambahkan mekanisme subscribe dan callback, sehingga ESP8266 juga dapat menerima perintah dari luar (misalnya dari aplikasi/dashboard) dan meresponsnya secara real-time, menjadikan sistem lebih interaktif dan mendukung kendali jarak jauh (remote control), bukan sekadar pelaporan data.

**3) Mengapa pendekatan non-blocking (menggunakan millis()) lebih sesuai dibandingkan pendekatan blocking (menggunakan delay()) pada sistem IoT yang memerlukan komunikasi dua arah secara real-time?**

Karena `delay()` menghentikan seluruh eksekusi program selama durasi tertentu, termasuk menghentikan pemrosesan pesan masuk (`client.loop()`), sehingga sistem menjadi tidak responsif terhadap perintah yang datang di saat itu. Pendekatan non-blocking dengan `millis()` memungkinkan program tetap "berjalan" mengecek banyak kondisi sekaligus dalam satu iterasi `loop()` yang cepat, sehingga publish data berkala dan penerimaan perintah subscribe bisa berjalan seolah-olah bersamaan (konkuren), yang krusial untuk sistem IoT real-time.

**4) Berikan contoh penerapan komunikasi dan pertukaran data dua arah pada sistem IoT nyata, dan jelaskan manfaatnya dibandingkan sistem yang hanya satu arah!**

Contoh: sistem smart home yang memantau suhu ruangan (publish) sekaligus menerima perintah untuk menyalakan/mematikan AC dari aplikasi smartphone (subscribe). Dibandingkan sistem satu arah yang hanya bisa melaporkan data tanpa bisa dikendalikan balik, sistem dua arah memungkinkan pengguna merespons kondisi secara langsung dari jarak jauh — misalnya otomatis menyalakan AC saat suhu tinggi terdeteksi, atau pengguna mematikannya manual dari aplikasi — menjadikan sistem lebih adaptif, interaktif, dan benar-benar "pintar" dibandingkan sistem monitoring pasif.

---

## Kesimpulan

1. ESP8266 dapat melakukan subscribe ke suatu topic MQTT dan memproses pesan yang diterima melalui fungsi callback, termasuk melakukan deserialisasi data JSON menggunakan `deserializeJson()`.
2. Mekanisme callback memungkinkan ESP8266 merespons perintah kendali (seperti ON/OFF LED) secara otomatis dan real-time setiap kali pesan baru masuk pada topic yang di-subscribe.
3. Sistem IoT full duplex (publish dan subscribe bersamaan) dapat dicapai dengan menghindari `delay()` yang bersifat blocking, digantikan pendekatan non-blocking menggunakan `millis()`.
4. Fungsi `client.loop()` wajib dipanggil sesering mungkin agar ESP8266 tetap responsif menerima pesan dan menjaga koneksi broker tetap aktif.
5. Komunikasi dua arah menjadikan sistem IoT lebih interaktif dibandingkan sistem satu arah, karena perangkat tidak hanya melaporkan data tetapi juga dapat dikendalikan dari jarak jauh secara real-time.
6. Topic MQTT dapat dipisah berdasarkan fungsinya (data sensor vs. perintah kendali, bahkan per-aktuator) untuk mengorganisasikan komunikasi dalam sistem IoT yang lebih kompleks.
