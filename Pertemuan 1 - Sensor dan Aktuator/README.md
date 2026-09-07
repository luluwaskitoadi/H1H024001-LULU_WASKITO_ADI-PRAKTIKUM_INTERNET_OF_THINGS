# Pertemuan 1  Sensor dan Aktuator

## Deskripsi Singkat

Praktikum ini membahas dua konsep dasar sistem IoT berbasis ESP32: akuisisi data sensor dan kendali aktuator otomatis. Percobaan 1A berfokus pada pembacaan data suhu dan kelembaban menggunakan sensor DHT11, sedangkan Percobaan 2A berfokus pada kendali aktuator (relay/LED) secara otomatis berdasarkan data suhu yang diperoleh, termasuk penerapan logika histerisis untuk mencegah chattering.

## Alat dan Bahan

- Board ESP32 DevKit
- Sensor DHT11
- Modul Relay 1 channel
- Breadboard dan kabel jumper
- Kabel USB (Micro-USB/USB-C)
- Laptop/PC dengan Arduino IDE (board manager ESP32 + pustaka DHT sensor library)

## Library / Dependencies

- `DHT.h` (DHT sensor library)  digunakan untuk komunikasi dan pembacaan data dari sensor DHT11. Instal melalui Arduino IDE Library Manager, cari "DHT sensor library" (by Adafruit).
- Board manager **ESP32**  perlu ditambahkan di Arduino IDE melalui Preferences > Additional Board Manager URLs.

---

## Percobaan 1A: Akuisisi Data Sensor DHT11

Code Percobaan : [Code/percobaan1a_akuisisi_sensor.ino](./Code/percobaan1a_akuisisi_sensor.ino)
Code modifikasi : [Code/percobaan1a_akuisisi_sensor_modifikasi.ino](./Code/percobaan1a_akuisisi_sensor_modifikasi.ino)

### Tujuan

Memahami dan mengimplementasikan proses akuisisi data sensor suhu dan kelembaban menggunakan sensor DHT11 pada ESP32.

### Konfigurasi Pin

| No | Komponen | Pin ESP32 |
|----|----------|-----------|
| 1 | VCC DHT11 | 3.3V |
| 2 | DATA DHT11 | GPIO 4 |
| 3 | GND DHT11 | GND |

### Penjelasan Code Original

- `DHTPIN 4`  pin GPIO ESP32 yang terhubung ke pin data DHT11.
- `dht.begin()`  inisialisasi komunikasi dengan sensor, dipanggil sekali di `setup()`.
- `dht.readTemperature()` / `dht.readHumidity()`  membaca nilai suhu (°C) dan kelembaban relatif (%) dari sensor.
- `isnan()`  memeriksa apakah hasil pembacaan valid; mencegah data gagal ditampilkan.
- `delay(2000)`  jeda 2 detik antar pembacaan sesuai batas minimal sampling DHT11.

### Penjelasan Code Modifikasi (rata-rata 5 sampel)

- Loop `for` mengambil 5 sampel berturut-turut (dengan delay 2 detik antar sampel), lalu menghitung rata-rata hanya dari sampel yang valid  mencegah pembagian dengan nol jika semua sampel gagal.
- Variabel `sampelValid` menghitung jumlah pembacaan yang berhasil, dipakai sebagai pembagi saat menghitung rata-rata.

### Jawaban Pertanyaan Praktikum

**1) Flowchart proses akuisisi data sensor DHT11**

Alur program: inisialisasi Serial dan `dht.begin()` sekali di `setup()` → di dalam `loop()`, sistem membaca data suhu & kelembaban → memvalidasi data dengan `isnan()` → jika gagal, cetak pesan error; jika berhasil, cetak nilai suhu & kelembaban → tunggu (`delay(2000)`) → kembali mengulang proses dari awal secara terus-menerus.

*(Diagram alur/flowchart visual dilampirkan sebagai gambar terpisah  lihat bagian Dokumentasi.)*

**2) Fungsi perintah `isnan()`**

`isnan()` (is Not a Number) berfungsi untuk memeriksa apakah nilai hasil pembacaan sensor merupakan angka yang valid atau tidak. Pembacaan sensor DHT11 kadang gagal (misalnya karena gangguan sinyal, kabel data longgar, atau waktu pembacaan terlalu cepat), sehingga fungsi `readTemperature()`/`readHumidity()` akan mengembalikan nilai `NaN`. Dengan memeriksa `isnan()`, program dapat mendeteksi kegagalan tersebut dan mencegah data yang tidak valid ditampilkan atau digunakan untuk logika kendali aktuator selanjutnya.

**3) Alasan diperlukan delay minimal ±2 detik antar pembacaan**

Sensor DHT11 memiliki keterbatasan pada kecepatan sampling internalnya  datasheet DHT11 merekomendasikan interval pembacaan minimal 2 detik antar pengukuran. Jika pembacaan dilakukan lebih cepat dari itu, sensor belum sempat menyelesaikan siklus pengukuran internalnya (elemen kapasitif kelembaban dan termistor memerlukan waktu stabilisasi), sehingga hasil pembacaan berikutnya berisiko tidak akurat atau menghasilkan nilai `NaN`. Delay ini juga mencegah pembebanan komunikasi berlebih pada jalur data single-wire sensor.

**4) Modifikasi program: rata-rata dari 5 kali pembacaan**

Lihat file: [Code/percobaan1a_akuisisi_sensor_modifikasi.ino](./Code/percobaan1a_akuisisi_sensor_modifikasi.ino) penjelasan tiap baris kode tambahan sudah dicantumkan di bagian Penjelasan Code Modifikasi di atas.

---

## Percobaan 2A: Kendali Aktuator Relay Berdasarkan Data Sensor

Code Percobaan : [Code/percobaan2a_kendali_aktuator.ino](./Code/percobaan2a_kendali_aktuator.ino)
Code modifikasi : [Code/percobaan2a_kendali_aktuator_modifikasi.ino](./Code/percobaan2a_kendali_aktuator_modifikasi.ino)

### Tujuan

Memahami dan mengimplementasikan kendali aktuator (relay) secara otomatis berdasarkan data suhu yang diakuisisi dari sensor DHT11.

### Konfigurasi Pin

| No | Komponen | Pin ESP32 |
|----|----------|-----------|
| 1 | IN Relay / Anoda LED | GPIO 26 |
| 2 | VCC Relay | 5V (VIN) |
| 3 | GND Relay / Katoda LED | GND |

### Penjelasan Code Original

- `suhuThreshold` (30°C)  satu ambang batas tunggal untuk ON dan OFF aktuator.
- `if (suhu > suhuThreshold)`  jika suhu melebihi threshold, relay/LED dinyalakan (`HIGH`); jika tidak, dimatikan (`LOW`).
- `digitalWrite(RELAYPIN, ...)`  mengeksekusi status aktuator ke pin fisik.

### Penjelasan Code Modifikasi (histerisis 2 ambang batas)

- `suhuNyala` (30°C) dan `suhuMati` (28°C)  dua ambang batas terpisah, membentuk zona histerisis.
- `statusAktuator`  variabel state yang menyimpan kondisi aktuator saat ini, sehingga keputusan ON/OFF tidak hanya bergantung pada nilai suhu sesaat, tetapi juga status sebelumnya.
- Percabangan `if/else if`: aktuator hanya menyala jika sebelumnya OFF **dan** suhu melebihi `suhuNyala`; hanya mati jika sebelumnya ON **dan** suhu turun di bawah `suhuMati`. Di antara kedua batas tersebut, status tidak berubah (zona aman anti-chattering).

### Jawaban Pertanyaan Praktikum

**1) Mengapa diperlukan nilai ambang batas (threshold) dalam sistem kendali aktuator berbasis sensor?**

Threshold diperlukan sebagai acuan keputusan otomatis bagi sistem untuk menentukan kapan aktuator harus aktif atau nonaktif, tanpa memerlukan intervensi manual. Tanpa threshold, mikrokontroler tidak memiliki patokan nilai untuk membedakan kondisi "normal" dan "perlu direspons" dari data sensor yang bersifat kontinu. Threshold juga memungkinkan sistem bekerja secara konsisten dan dapat diprediksi, sesuai kebutuhan aplikasi (misalnya suhu ruangan, kelembaban tanah, dsb).

**2) Apa yang terjadi jika `suhuThreshold` diturunkan menjadi sangat rendah, misalnya 20.0?**

Jika threshold diturunkan menjadi 20.0°C, aktuator (relay/LED) akan hampir selalu dalam kondisi ON, karena suhu ruangan pada umumnya (~25–30°C) sudah berada di atas 20°C. Akibatnya:
- Relay akan sering aktif meskipun kondisi sebenarnya tidak memerlukan respons tersebut (false trigger).
- Jika ini diterapkan pada perangkat nyata (bukan simulasi LED), hal ini bisa memperpendek umur relay karena sering switching, serta boros energi karena aktuator menyala terus-menerus.
- Sistem menjadi tidak sensitif terhadap perubahan suhu yang sebenarnya ingin dideteksi, karena kondisi ON sudah tercapai bahkan pada suhu normal.

**3) Perbedaan kendali kondisi tunggal dan kendali dengan histerisis (dua ambang batas)**

Kendali kondisi tunggal menggunakan satu ambang batas yang sama untuk ON dan OFF, sehingga saat nilai sensor berosilasi di sekitar titik tersebut, aktuator mudah berganti status berulang kali (chattering). Kendali histerisis menggunakan dua ambang batas berbeda (misalnya ON di 30°C, OFF di 28°C), sehingga ada "jarak aman" yang mencegah aktuator berganti status terlalu sering, membuat komponen lebih awet meski programnya sedikit lebih kompleks. Karena itu, histerisis lebih disarankan untuk aplikasi nyata yang datanya selalu mengandung noise kecil.

**4) Modifikasi program: kendali dengan dua ambang batas (histerisis)**

Lihat file: [Code/percobaan2a_kendali_aktuator_modifikasi.ino](./Code/percobaan2a_kendali_aktuator_modifikasi.ino) penjelasan inti perubahan: variabel statusAktuator disimpan sebagai state agar keputusan ON/OFF tidak hanya bergantung pada nilai suhu saat itu saja, melainkan juga pada kondisi sebelumnya inilah yang membentuk zona histerisis antara suhuMati (28°C) dan suhuNyala (30°C).

---

## Pertanyaan Analisis

**1) Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan**

Pada percobaan 1A ESP32 berhasil membaca suhu dan kelembaban dari DHT11 dari beberapa kondisi di serial monitor. Pada Percobaan 2A aktuator menyala saat suhu melebihi threshold dan mati saat di bawahnya.

**2) Bagaimana pengaruh akurasi dan waktu tanggap (response time) sensor terhadap kecepatan reaksi aktuator pada sistem IoT?**

Akurasi sensor menentukan ketepatan keputusan kendali; DHT11 memiliki akurasi suhu ±0,5°C yang cukup untuk aplikasi umum. Waktu tanggapnya relatif lambat (siklus sampling minimal 2 detik), sehingga saat suhu berubah cepat, aktuator akan mengalami keterlambatan respons karena baru bereaksi pada siklus pembacaan berikutnya.

**3) Bagaimana cara kerja sistem dalam mengubah data sensor menjadi keputusan kendali aktuator (proses akuisisi hingga aktuasi)?**

Sensor DHT11 mengambil data suhu/kelembaban dan mengirimkannya sebagai sinyal digital ke ESP32, yang kemudian membacanya lewat pustaka DHT.h dan memvalidasinya dengan `isnan()`. Nilai valid dibandingkan dengan threshold untuk menentukan status aktuator, lalu ESP32 mengirim sinyal `digitalWrite()` ke relay untuk mengeksekusi aksi fisik, dan proses ini berulang terus-menerus.

**4) Bagaimana kombinasi antara akuisisi data sensor dan kendali aktuator dapat digunakan untuk membangun sistem IoT yang responsif terhadap perubahan kondisi lingkungan, misalnya pada sistem smart farming atau smart home?**

Kombinasi ini membentuk closed-loop control system yang memantau lingkungan dan merespons otomatis tanpa campur tangan manusia. Pada smart farming, sensor kelembaban tanah mengaktifkan pompa air otomatis; pada smart home, DHT11 mengaktifkan kipas/AC otomatis saat suhu melebihi ambang batas  keduanya meningkatkan efisiensi tanpa pengaturan manual.

---

## Kesimpulan

1. ESP32 dapat digunakan sebagai unit pemroses untuk melakukan akuisisi data sensor DHT11 (suhu dan kelembaban) secara real-time menggunakan pustaka DHT.h, dengan fungsi `isnan()` sebagai validasi untuk mendeteksi kegagalan pembacaan sensor.
2. Delay minimal 2 detik antar pembacaan diperlukan karena keterbatasan waktu sampling internal sensor DHT11, sehingga pembacaan yang terlalu cepat berisiko menghasilkan data tidak valid.
3. Aktuator (disimulasikan dengan relay/LED) dapat dikendalikan secara otomatis oleh ESP32 berdasarkan hasil akuisisi data sensor, dengan membandingkan nilai terbaca terhadap ambang batas (threshold) yang ditentukan dalam program.
4. Penggunaan histerisis (dua ambang batas) terbukti lebih baik dibandingkan kondisi tunggal (satu ambang batas) karena mampu mencegah chattering/flip-flop pada aktuator akibat fluktuasi kecil nilai sensor, sekaligus memperpanjang umur komponen.
5. Akurasi dan waktu tanggap sensor berpengaruh langsung terhadap kecepatan dan ketepatan reaksi aktuator, sehingga pemilihan sensor perlu disesuaikan dengan kebutuhan respons sistem.
6. Prinsip akuisisi data sensor dan kendali aktuator yang dipelajari pada praktikum ini merupakan dasar dari sistem IoT closed-loop yang dapat diterapkan pada berbagai aplikasi nyata, seperti smart farming dan smart home.

---
