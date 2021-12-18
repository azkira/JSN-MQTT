#include "DHT.h"
#include <SoftwareSerial.h>
#define DHTPIN 2

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11 // DHT 11
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321
//#define DHTTYPE DHT21 // DHT 21 (AM2301)

SoftwareSerial espSerial(5, 6);
DHT dht(DHTPIN, DHTTYPE);

String str;

int sensorPin = A0;  // pin sensor
int powerPin = 8;

void setup() {
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  Serial.begin(9600);
  espSerial.begin(9600);
  dht.begin();
}

void loop() {
  delay(5000);
  float t = dht.readTemperature();
  if (isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
  int nilaiHum = bacaSensorHum();
  String data = String(t)+"#"+String(nilaiHum);
  Serial.println(data);
  espSerial.println(data);
}

int bacaSensorHum() {
  digitalWrite(powerPin, HIGH);
  delay(5000);
  int nilaiSensor = analogRead(sensorPin);
  digitalWrite(powerPin, LOW);
  delay(5000);
  return 1023 - nilaiSensor;
}
