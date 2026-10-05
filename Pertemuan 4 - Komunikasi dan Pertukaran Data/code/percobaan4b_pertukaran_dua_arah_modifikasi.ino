#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* topicData           = "unsoed/tk245004/kelompokAnda/data";
const char* topicPerintahLED    = "unsoed/tk245004/kelompokAnda/perintah";        // aktuator 1: LED
const char* topicPerintahBuzzer = "unsoed/tk245004/kelompokAnda/perintah_buzzer"; // aktuator 2: buzzer (baru)

#define DHTPIN 4
#define DHTTYPE DHT11
const int ledPin = 26;
const int buzzerPin = 27; // pin baru untuk buzzer

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (unsigned int i = 0; i < length; i++) pesan += (char)payload[i];

  JsonDocument doc;
  if (deserializeJson(doc, pesan)) return; // abaikan jika parsing gagal

  const char* perintah = doc["perintah"];
  String topicStr = String(topic); // ubah topic (char*) jadi String agar mudah dibandingkan

  // Bedakan aksi berdasarkan topic mana pesan diterima
  if (topicStr == topicPerintahLED) {
    digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("Perintah LED diterima -> ");
    Serial.println(perintah);
  } else if (topicStr == topicPerintahBuzzer) {
    digitalWrite(buzzerPin, String(perintah) == "ON" ? HIGH : LOW);
    Serial.print("Perintah Buzzer diterima -> ");
    Serial.println(perintah);
  } else {
    Serial.print("Pesan dari topic tidak dikenali: ");
    Serial.println(topicStr);
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // Subscribe ke kedua topic perintah sekaligus setelah terhubung
      client.subscribe(topicPerintahLED);
      client.subscribe(topicPerintahBuzzer);
      Serial.println("Terhubung dan subscribe ke topic LED & Buzzer");
    } else {
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT); // inisialisasi pin buzzer sebagai output
  dht.begin();

  hubungkanWiFi();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) hubungkanMQTT();
  client.loop();

  if (millis() - waktuTerakhirPublish > intervalPublish) {
    waktuTerakhirPublish = millis();

    float suhu = dht.readTemperature();
    if (!isnan(suhu)) {
      JsonDocument doc;
      doc["suhu"] = suhu;
      char buffer[128];
      serializeJson(doc, buffer);
      client.publish(topicData, buffer);
      Serial.print("Data terkirim: ");
      Serial.println(buffer);
    }
  }
}
