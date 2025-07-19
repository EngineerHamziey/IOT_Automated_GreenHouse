#if 0

#define displayDelay 1500;

#define buzzerPin 13 // DONE
#define pumpPin 23 // DONE
#define lightPin 25 // DONE
#define fanPin 26 // DONE
#define tungstenBulbPin 27 // DONE
#define humiditySensorPin 32 // DONE
#define soilMoistureSensorPin 33 // DONE
#define ldrPin 34 // DONE

//  FOR TEMPERATURE SENSOR
#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#endif