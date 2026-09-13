#include <WiFi.h>

const char* ssid     = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

const int ledPin = 2;  // LED indikator status koneksi

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Set mode WiFi menjadi Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Jika berhasil terhubung
  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());
  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());

  digitalWrite(ledPin, HIGH);  // nyalakan LED sebagai indikator
}

void loop() {
  // Cek status koneksi setiap 5 detik
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Status: Terhubung");
  } else {
    // Koneksi terputus -> matikan LED dan coba reconnect otomatis
    digitalWrite(ledPin, LOW);
    Serial.println("Status: Terputus. Mencoba menghubungkan kembali...");

    WiFi.disconnect();          // pastikan koneksi lama benar-benar direset
    WiFi.begin(ssid, password); // mulai ulang proses koneksi

    // Tunggu hingga terhubung kembali, maksimal ~10 detik agar tidak menggantung selamanya
    unsigned long waktuMulai = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - waktuMulai < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Berhasil terhubung kembali!");
      digitalWrite(ledPin, HIGH); // nyalakan kembali LED indikator
    } else {
      Serial.println();
      Serial.println("Gagal reconnect, akan dicoba lagi pada siklus berikutnya.");
    }
  }
  delay(5000);
}
