#if 0

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

void printSensorValues (void *parameters) {
  while(true) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature(); // Default is Celsius
    // Check if readings failed
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      humidity = 0.0;
      temperature = 0.0;
    }
    Serial.println("Temperature = " + String(temperature) + "\t\tHumidity = " + String(humidity) );

    Serial.println("Soil moisture sensor value = " + String(analogRead(soilMoistureSensorPin)));
    
    Serial.println("ldr sensor value = " + String(analogRead(ldrPin)));

    Serial.println("\n");
    vTaskDelay(2000/portTICK_PERIOD_MS);//read dht every 1 to 2 secs
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  // set pinModes for DIGITAL inputs and output devices
  pinMode(buzzerPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(tungstenBulbPin, OUTPUT);
  xTaskCreate(
    printSensorValues,  //FUNCTION TO RUN
    "Print Sensor Values",  //NAMING MY TASK
    2048, //STACK SIZE FOR MY TASK 
    NULL,  // I HAVE NO PARAMETERS
    2,  // PRIORITY OF 2 
    NULL);  // NO TASK HANDLE
}

// /*
void loop() {
  for(int count = 1; count <= 3; count++) {
    digitalWrite(buzzerPin, HIGH);
    vTaskDelay(displayDelay/portTICK_PERIOD_MS);
    digitalWrite(buzzerPin, LOW);
    vTaskDelay(displayDelay/portTICK_PERIOD_MS);
  }


  for(int count = 1; count <= 3; count++) {
    digitalWrite(lightPin, HIGH);
    vTaskDelay(displayDelay/portTICK_PERIOD_MS);
    digitalWrite(lightPin, LOW);
    vTaskDelay(displayDelay/portTICK_PERIOD_MS);
  }

  for(int count = 1; count <= 3; count++) {
    digitalWrite(tungstenBulbPin, HIGH);
    vTaskDelay(displayDelay/portTICK_PERIOD_MS);
    digitalWrite(tungstenBulbPin, LOW);
    vTaskDelay(displayDelay/portTICK_PERIOD_MS);
  }

  const int testDelay = 5000; 
  // TEST PUMP
    digitalWrite(pumpPin, HIGH);
    delay(testDelay);
    digitalWrite(pumpPin, LOW);

  // Test FAN
    digitalWrite(fanPin, HIGH);
    delay(testDelay);
    digitalWrite(fanPin, LOW);  
}
//  */

#endif