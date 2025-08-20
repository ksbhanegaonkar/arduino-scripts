#include <SPI.h>
#include <LoRa.h>

#define TRIG_PIN 4
#define ECHO_PIN 3

#define NSS 10
#define RST 9
#define DIO0 2

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Sender - Ultrasonic Distance");

  // Setup LoRa pins
  LoRa.setPins(NSS, RST, DIO0);

  if (!LoRa.begin(433E6)) {   // Change frequency if needed
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Setup done, starting to send distance data...");
}

void loop() {
  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2;  // cm

  // Print locally
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Send via LoRa
  LoRa.beginPacket();
  LoRa.print("Distance: ");
  LoRa.print(distance);
  LoRa.println(" cm");
  LoRa.endPacket();

  delay(600000);  
}
