#include <SPI.h>
#include <LoRa.h>
#include <TM1637Display.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// --- LoRa Pins ---
#define LORA_SS   0   // D8 = GPIO15
#define LORA_RST  2   // D4 = GPIO2
#define LORA_DIO0 4   // D3 = GPIO0

// --- TM1637 Display Pins ---
#define CLK 16   // D0 = GPIO16
#define DIO 5    // D1 = GPIO5

TM1637Display display(CLK, DIO);

// --- Wi-Fi Credentials ---
const char* ssid = "Test123";
const char* password = "Welcome@01";

// --- Endpoint URL ---
const String postURL = "http://nirman-aura.mysocietyhub.in/api/tank/reading";

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // --- Setup TM1637 Display ---
  display.setBrightness(0x01);  // low brightness
  display.showNumberDec(0);

  // --- Connect to Wi-Fi ---
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());

  // --- Setup LoRa ---
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(433E6)) { // adjust frequency if needed
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Receiver Ready!");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.print("Received: ");
    Serial.println(received);

    int distance = -1;
    int idx = received.indexOf("Distance:");
    if (idx != -1) {
      int cmIdx = received.indexOf("cm", idx);
      if (cmIdx != -1) {
        String numStr = received.substring(idx + 9, cmIdx);
        numStr.trim();
        distance = numStr.toInt();
      }
    }

      // --- Upload to server using POST ---
      if (WiFi.status() == WL_CONNECTED) {
        WiFiClient wifiClient;   // <<-- add this line
        HTTPClient http;
        http.begin(wifiClient, postURL);   // <<-- updated API
        http.addHeader("Content-Type", "application/json");

        // Build JSON body manually
        String jsonBody = "{\"tankId\":\"NA001\",\"waterLevel\":" + String(distance) + "}";

        int httpCode = http.POST(jsonBody);
        if (httpCode > 0) {
          Serial.print("HTTP Response code: ");
          Serial.println(httpCode);
          String payload = http.getString();
          Serial.println("Response: " + payload);
        } else {
          Serial.print("Error on HTTP POST: ");
          Serial.println(http.errorToString(httpCode));
        }
        http.end();
      } else {
        Serial.println("WiFi not connected. Cannot upload.");
      }
    }
  }

