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

int getDistance() {
  // Send ultrasonic pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return -1; // invalid reading
  return duration * 0.034 / 2;  // cm
}

int getCleanAverage() {
  int readings[20];
  int validCount = 0;

  // Collect 20 readings
  for (int i = 0; i < 20; i++) {
    int d = getDistance();
    if (d > 0) {
      readings[validCount++] = d;
    }
    delay(1000);
  }

  if (validCount == 0) return -1;

  // Step 1: Calculate initial average
  long sum = 0;
  for (int i = 0; i < validCount; i++) {
    sum += readings[i];
  }
  float avg = (float)sum / validCount;

  // Step 2: Discard readings too far from average (±5 cm)
  sum = 0;
  int goodCount = 0;
  for (int i = 0; i < validCount; i++) {
    if (abs(readings[i] - avg) <= 5) {
      sum += readings[i];
      goodCount++;
    }
  }

  if (goodCount == 0) return -1;

  // Step 3: Return refined average
  return sum / goodCount;
}

void loop() {


  int cleanAvg = 0;

  do{
    cleanAvg = getCleanAverage();
  }while(cleanAvg == -1);

  // Print locally
  Serial.print("Filtered Distance: ");
  Serial.print(cleanAvg);
  Serial.println(" cm");

  delay(5000);
  // Send via LoRa
  LoRa.beginPacket();
  LoRa.print("Distance: ");
  LoRa.print(cleanAvg);
  LoRa.println(" cm");
  LoRa.endPacket();

  delay(1800000);  
}
