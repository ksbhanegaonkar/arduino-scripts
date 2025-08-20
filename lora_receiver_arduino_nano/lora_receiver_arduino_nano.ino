#include <SPI.h>
#include <LoRa.h>

#define NSS 10
#define RST 9
#define DIO0 2

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver - Arduino Nano");

  // Set LoRa pins
  LoRa.setPins(NSS, RST, DIO0);

  // Initialize at 433 MHz (or 868E6 / 915E6 depending on your module)
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa init success, waiting for messages...");
}

void loop() {
  // Try to parse a packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Received packet: ");

    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    Serial.print("  | RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}
