// Versi mock: sensor DHT22 diganti mock function (data suhu simulasi).
// Logika moving average, minimum, dan maksimum sama persis dengan versi original.

const int N = 5;       // jumlah sampel moving average
float buffer[N];       // menyimpan N pembacaan terakhir
int indeks = 0;        // posisi penulisan berikutnya pada buffer
int jumlah = 0;        // jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

// Mock function pengganti dht.readTemperature()
float bacaSuhuMock() {
  // 1 dari 15 pembacaan dibuat gagal (NaN) untuk menguji validasi
  if (random(0, 15) == 0) return NAN;

  // Suhu dasar 28 °C dengan derau acak sekitar ±1 °C
  float suhu = 28.0 + (random(-100, 101) / 100.0);

  // 1 dari 10 pembacaan dibuat lonjakan (meniru gangguan/hembusan napas)
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

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0)); // seed acak agar data tiap run berbeda
}

void loop() {
  float suhu = bacaSuhuMock();

  if (isnan(suhu)) {
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu);

    if (suhu < suhuMin) suhuMin = suhu;
    if (suhu > suhuMax) suhuMax = suhu;

    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.println(suhuMax);
  }

  delay(2000);
}
