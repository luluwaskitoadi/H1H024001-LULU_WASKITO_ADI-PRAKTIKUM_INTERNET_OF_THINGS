#include <WiFi.h>

const char* ap_ssid = "ESP32_AccessPoint";
const char* ap_password = "12345678"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);

  // Set mode WiFi menjadi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress apIP = WiFi.softAPIP();
  Serial.println("Access Point aktif!");
  Serial.print("SSID : ");
  Serial.println(ap_ssid);
  Serial.print("IP Address : ");
  Serial.println(apIP);
  Serial.println();
  Serial.println("Waktu(s) | Jumlah Client");
}

void loop() {
  // Hitung waktu berjalan sejak ESP32 nyala (dalam detik)
  unsigned long waktu = millis() / 1000;

  // Menampilkan jumlah perangkat yang terhubung setiap 5 detik
  int jumlahClient = WiFi.softAPgetStationNum();

  Serial.print(waktu);
  Serial.print("s        | ");
  Serial.println(jumlahClient);

  delay(5000);
}
