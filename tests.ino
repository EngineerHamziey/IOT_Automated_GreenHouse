#if 1
#define displayDelay 1500

#define buzzerPin 13 // DONE
#define pumpPin 23 // DONE
#define lightPin 25 // DONE
#define fanPin 26 // DONE
#define tungstenBulbPin 27 // DONE
#define humiditySensorPin 32 // DONE
#define soilMoistureSensorPin 33 // DONE
#define ldrPin 34 // DONE


void setup() {
  // set pinModes for DIGITAL inputs and output devices
  pinMode(buzzerPin, OUTPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(tungstenBulbPin, OUTPUT);

  Serial.begin(115200);

}

void printSensorValues () {
  Serial.println("Humidity sensor value = " + String(humiditySensorPin));

  Serial.println("Soil moisture sensor value = " + String(soilMoistureSensorPin));

  Serial.println("ldr sensor value = " + String(ldrPin));
  Serial.println("\n\n");
}

void loop() {
  printSensorValues();
  for(int count = 1; count <= 3; count++) {
    digitalWrite(buzzerPin, HIGH);
    printSensorValues();
    delay(displayDelay);
    printSensorValues();
    digitalWrite(buzzerPin, LOW);
    printSensorValues();
    delay(displayDelay);
  }


  for(int count = 1; count <= 3; count++) {
    digitalWrite(pumpPin, HIGH);
    printSensorValues();
    delay(displayDelay);
    printSensorValues();
    digitalWrite(pumpPin, LOW);
    printSensorValues();
    delay(displayDelay);
  }

  for(int count = 1; count <= 3; count++) {
    digitalWrite(lightPin, HIGH);
    printSensorValues();
    delay(displayDelay);
    printSensorValues();
    digitalWrite(lightPin, LOW);
    printSensorValues();
    delay(displayDelay);
  }

  for(int count = 1; count <= 3; count++) {
    printSensorValues();
    digitalWrite(fanPin, HIGH);
    printSensorValues();
    delay(displayDelay);
    printSensorValues();
    digitalWrite(fanPin, LOW);
    printSensorValues();
    delay(displayDelay);
    printSensorValues();
  }
  
  for(int count = 1; count <= 3; count++) {
    digitalWrite(tungstenBulbPin, HIGH);
    printSensorValues();
    delay(displayDelay);
    printSensorValues();
    digitalWrite(tungstenBulbPin, LOW);
    printSensorValues();
    delay(displayDelay);
    printSensorValues();
  }
}


#endif