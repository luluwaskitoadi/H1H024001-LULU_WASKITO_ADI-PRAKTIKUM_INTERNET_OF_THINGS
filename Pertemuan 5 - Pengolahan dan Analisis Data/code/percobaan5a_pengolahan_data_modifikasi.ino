// Modifikasi: menambahkan perhitungan simpangan baku dari data pada buffer.
// Sumber data memakai mock function (pengganti DHT22), sama seperti versi mock.

const int N = 5;       // jumlah sampel moving average
float buffer[N];       // menyimpan N pembacaan terakhir
int indeks = 0;        // posisi penulisan berikutnya pada buffer
int jumlah = 0;        // jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Mock function pengganti dht.readTemperature()
float bacaSuhuMock() {
  if (random(0, 15) == 0) return NAN;
  float suhu = 28.0 + (random(-100, 101) / 100.0);
  if (random(0, 10) == 0) suhu += 4.0;
  return suhu;
}

float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru;
  indeks = (indeks + 1) % N;
  if (jumlah < N) jumlah++;

  float total = 0;
  for (int i = 0; i < jumlah; i++) {
    total += buffer[i];
  }
  return total / jumlah;
}

// BARU: menghitung simpangan baku (sampel) dari data yang terisi pada buffer
float hitungSimpanganBaku(float rataRata) {
  if (jumlah < 2) return 0.0; // simpangan baku sampel butuh minimal 2 data

  float totalKuadrat = 0;
  for (int i = 0; i < jumlah; i++) {
    float selisih = buffer[i] - rataRata; // jarak tiap data dari rata-rata
    totalKuadrat += selisih * selisih;    // dikuadratkan agar selalu positif
  }
  return sqrt(totalKuadrat / (jumlah - 1)); // dibagi (n-1), sama dengan pandas .std()
}

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
}

void loop() {
  float suhu = bacaSuhuMock();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu);
    float std = hitungSimpanganBaku(suhuFilter); // BARU: hitung simpangan baku buffer

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Std: ");           // BARU: tampilkan simpangan baku
    Serial.print(std);
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.println(suhuMax);
  }

  delay(2000);
}
