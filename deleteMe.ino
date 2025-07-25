#if 0

#include "DHT.h"

#define DHTPIN 4         // GPIO4
#define DHTTYPE DHT11    // DHT 11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println("DHT11 sensor reading...");
  dht.begin();
}

void loop() {
  delay(2000);  // Wait a few seconds between readings

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(); // Default is Celsius

  // Check if readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
}

#endif