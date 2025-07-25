#if 1

#include "DHT.h"

#define dhtPin 4         // GPIO4
#define dhtType DHT11    // DHT 11

#define displayDelay 1500

#define buzzerPin 13 // NOW WORKING
#define pumpPin 23 // NOW WORKING
#define lightPin 25 // NOW WORKING
#define fanPin 26 // NOW WORKINGs
#define tungstenBulbPin 27 // NOW WORKING
#define soilMoistureSensorPin 33 // NOW WORKING
#define ldrPin 32 // NOW WORKING


DHT dht(dhtPin, dhtType);

void setup() {
  Serial.begin(115200);
  dht.begin();
  // set pinModes for DIGITAL inputs and output devices
  pinMode(buzzerPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(tungstenBulbPin, OUTPUT);
}


float humidity = 0.0, temperature = 0.0;
void loop() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature(); // Default is Celsius
  // Check if readings failed
  humidity = isnan(humidity) ? 0.0 : humidity;
  temperature = isnan(temperature) ? 0.0 : temperature;

  Serial.println("Temperature = " + String(temperature) + "\t\tHumidity = " + String(humidity) );

  Serial.println("Soil moisture sensor value = " + String(analogRead(soilMoistureSensorPin)));
  
  Serial.println("ldr sensor value = " + String(analogRead(ldrPin)));

  Serial.println("\n");

  regulateGreenHouse();
  // vTaskDelay(2000/portTICK_PERIOD_MS);//read dht every 1 to 2 secs
}

void regulateGreenHouse() {
  // Temperature regulation using fan
  if (temperature > 30.0) {
    digitalWrite(fanPin, HIGH);
    Serial.println("Fan ON - Temperature too high");
  } else {
    digitalWrite(fanPin, LOW);
    Serial.println("Fan OFF - Temperature normal");
  }

  // Soil moisture regulation using pump
  int soilMoisture = analogRead(soilMoistureSensorPin);
  if (soilMoisture < 1000) {
    digitalWrite(pumpPin, HIGH); // dry
    Serial.println("Pump ON - Soil is dry");
  } else {
    digitalWrite(pumpPin, LOW);
    Serial.println("Pump OFF - Soil is moist");
  }

  // Light regulation using artificial light
  int lightLevel = analogRead(ldrPin);
  if (lightLevel < 1000) {
    digitalWrite(lightPin, HIGH); // dark
    Serial.println("Light ON - It's dark");
  } else {
    digitalWrite(lightPin, LOW);
    Serial.println("Light OFF - Environment is bright");
  }

  Serial.println("Greenhouse regulation complete.\n");
}




#endif