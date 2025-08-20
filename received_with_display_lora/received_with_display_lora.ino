#include <SPI.h>
#include <LoRa.h>
#include <TM1637Display.h>

// --- LoRa Pins ---
#define LORA_SS   0   // D8 = GPIO15
#define LORA_RST  2    // D4 = GPIO2
#define LORA_DIO0 4    // D3 = GPIO0

// --- TM1637 Display Pins ---
#define CLK 16   // RX pin = GPIO3
#define DIO 5  // D8 pin = GPIO15

TM1637Display display(CLK, DIO);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // --- Setup TM1637 Display ---
  display.setBrightness(0x01);  // max brightness

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

    // Only display if a valid number was found
    if (distance >= 0) {
      display.showNumberDec(distance, false);
      Serial.print("Displayed: ");
      Serial.println(distance);
    }
  }
}
