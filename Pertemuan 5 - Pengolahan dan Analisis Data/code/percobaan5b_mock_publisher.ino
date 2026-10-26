#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* topicData = "unsoed/tk245004/kelompokAnda/data"; // harus sama dengan TOPIC di Python

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000; // publish setiap 5 detik (non-blocking)

// Mock function pengganti dht.readTemperature()
float bacaSuhuMock() {
  // Suhu dasar 28 °C dengan derau acak sekitar ±0.5 °C
  float suhu = 28.0 + (random(-50, 51) / 100.0);

  // Sekitar 1 dari 10 data dibuat lonjakan (meniru sumber panas / gangguan)
  if (random(0, 10) == 0) {
    float lonjakan = 3.0 + (random(0, 201) / 100.0); // 3.0 s.d. 5.0
    suhu += (random(0, 2) == 0) ? lonjakan : -lonjakan;
  }
  return suhu;
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke broker MQTT...");
    String clientId = "ESP32Mock-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("berhasil terhubung!");
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop();

  // Publish data secara berkala tanpa delay() yang memblokir
  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = bacaSuhuMock();

    JsonDocument doc;
    doc["suhu"] = round(suhu * 100) / 100.0; // dibulatkan 2 desimal
    char buffer[64];
    serializeJson(doc, buffer);

    client.publish(topicData, buffer);
    Serial.print("Data terkirim: ");
    Serial.println(buffer);
  }
}
