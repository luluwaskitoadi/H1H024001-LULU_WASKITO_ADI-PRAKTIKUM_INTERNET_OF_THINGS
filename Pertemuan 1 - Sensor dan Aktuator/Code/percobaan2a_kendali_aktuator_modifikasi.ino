#include <DHT.h>

#define DHTPIN 4          // pin data DHT22 terhubung ke GPIO 4
#define DHTTYPE DHT22
#define RELAYPIN 26        // pin kendali relay/LED indikator

DHT dht(DHTPIN, DHTTYPE);

const float suhuNyala = 30.0;  // ambang batas suhu untuk menyalakan aktuator (°C)
const float suhuMati  = 28.0;  // ambang batas suhu untuk mematikan aktuator (°C)

bool statusAktuator = false;   // status aktuator saat ini (false = OFF, true = ON)

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAYPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW); // pastikan aktuator mati di awal
}

void loop() {
  float suhu = dht.readTemperature();

  if (isnan(suhu)) {
    Serial.println("Gagal membaca data sensor!");
  } else {
    Serial.print("Suhu: ");
    Serial.print(suhu);
    Serial.print(" °C -> ");

    // Logika histerisis: gunakan status sebelumnya untuk menentukan aksi
    if (!statusAktuator && suhu > suhuNyala) {
      // Aktuator sedang OFF dan suhu melebihi batas atas -> nyalakan
      statusAktuator = true;
    } else if (statusAktuator && suhu < suhuMati) {
      // Aktuator sedang ON dan suhu turun di bawah batas bawah -> matikan
      statusAktuator = false;
    }
    // Jika suhu berada di antara suhuMati dan suhuNyala, status TIDAK berubah
    // (inilah "zona aman" histerisis yang mencegah chattering)

    digitalWrite(RELAYPIN, statusAktuator ? HIGH : LOW);
    Serial.println(statusAktuator ? "Aktuator: ON" : "Aktuator: OFF");
  }

  delay(2000); // jeda pembacaan setiap 2 detik
}
