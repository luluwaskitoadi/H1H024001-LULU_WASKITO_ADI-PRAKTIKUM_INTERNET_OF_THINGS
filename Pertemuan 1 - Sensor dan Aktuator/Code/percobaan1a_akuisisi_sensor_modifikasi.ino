#include <DHT.h>

#define DHTPIN 4         // pin data DHT11 terhubung ke GPIO 4
#define DHTTYPE DHT11     // tipe sensor yang digunakan
#define JUMLAH_SAMPEL 5   // jumlah pembacaan yang akan dirata-rata

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();  // inisialisasi sensor DHT22
  Serial.println("Memulai akuisisi data sensor DHT11 (rata-rata 5 sampel)...");
}

void loop() {
  float totalSuhu = 0;       // akumulator total suhu
  float totalKelembaban = 0; // akumulator total kelembaban
  int sampelValid = 0;       // penghitung sampel yang berhasil dibaca

  // Ambil 5 kali pembacaan berturut-turut
  for (int i = 0; i < JUMLAH_SAMPEL; i++) {
    float suhu = dht.readTemperature();
    float kelembaban = dht.readHumidity();

    // Hanya jumlahkan data yang valid (bukan NaN)
    if (!isnan(suhu) && !isnan(kelembaban)) {
      totalSuhu += suhu;
      totalKelembaban += kelembaban;
      sampelValid++; // tambah penghitung sampel valid
    } else {
      Serial.println("Sampel gagal dibaca, dilewati.");
    }

    delay(2000); // jeda wajib 2 detik antar pembacaan DHT22
  }

  // Hitung rata-rata hanya jika ada sampel valid, untuk mencegah pembagian dengan nol
  if (sampelValid > 0) {
    float rataSuhu = totalSuhu / sampelValid;
    float rataKelembaban = totalKelembaban / sampelValid;

    Serial.print("Rata-rata Suhu: ");
    Serial.print(rataSuhu);
    Serial.print(" °C, Rata-rata Kelembaban: ");
    Serial.print(rataKelembaban);
    Serial.println(" %");
  } else {
    Serial.println("Semua sampel gagal dibaca, tidak dapat menghitung rata-rata.");
  }
}
