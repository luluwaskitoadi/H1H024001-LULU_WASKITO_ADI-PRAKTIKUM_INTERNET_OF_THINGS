#include <WiFi.h>

// Kredensial untuk terhubung ke WiFi rumah (mode Station)
const char* sta_ssid     = "NAMA_WIFI_RUMAH";
const char* sta_password = "PASSWORD_WIFI_RUMAH";

// Kredensial untuk Access Point yang dibuat ESP32 sendiri
const char* ap_ssid     = "ESP32_AccessPoint";
const char* ap_password = "12345678"; // minimal 8 karakter

void setup() {
  Serial.begin(115200);

  // Set mode WiFi menjadi gabungan Access Point + Station
  WiFi.mode(WIFI_AP_STA);

  // Aktifkan Access Point terlebih dahulu
  WiFi.softAP(ap_ssid, ap_password);
  IPAddress apIP = WiFi.softAPIP();
  Serial.println("Access Point aktif!");
  Serial.print("AP SSID       : ");
  Serial.println(ap_ssid);
  Serial.print("AP IP Address : ");
  Serial.println(apIP);

  // Sambil itu, hubungkan juga ke WiFi rumah sebagai Station
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Menghubungkan ke WiFi rumah");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Berhasil terhubung ke WiFi rumah!");
  Serial.print("STA IP Address : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Tampilkan jumlah perangkat yang terhubung ke AP setiap 5 detik
  int jumlahClient = WiFi.softAPgetStationNum();
  Serial.print("Jumlah perangkat terhubung ke AP: ");
  Serial.println(jumlahClient);

  // Cek juga status koneksi Station tetap aktif
  Serial.print("Status koneksi ke WiFi rumah: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Terhubung" : "Terputus");

  delay(5000);
}
