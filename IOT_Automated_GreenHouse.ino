#if 1

#include <secretCredentials.h> // contains my password, bot token, etc

#include "DHT.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   //By Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

#include <SPIFFS.h>


//LCD LIBRARIES
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); 

#define projectTitle "IOT ENABLED AUTOMATED GREEN HOUSE"

const byte maxbotUsers = 30;
byte botUsersCount = 1; 
String botUsersId[maxbotUsers];


WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

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


#define scrollSpeed 300
void printTitle() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  int charPerRow = 20;
  int rowCount = 4;
  int maxDisplayLength = charPerRow * rowCount;
  
  String title = String(projectTitle);
  int titleLength = title.length();

  // Trim or pad if necessary
  if (titleLength > maxDisplayLength) {
    title = title.substring(0, maxDisplayLength); // Truncate to fit 20x4
  } else {
    while (title.length() < maxDisplayLength) {
      title += ' '; // pad with space
    }
  }

  // Typing effect: print character by character, row by row
  for (int i = 0; i < title.length(); i++) {
    int row = i / charPerRow;
    int col = i % charPerRow;
    lcd.setCursor(col, row);
    lcd.print(title[i]);
    delay(80); // Typing speed
  }
}

void initializeSpiffs() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    bot.sendMessage(adminChatId, "SPIFFS Memory failed", "");
    return;
  }

  loadIdsFromFile();
}

// 🔄 Load existing ids from SPIFFS
void loadIdsFromFile() {
  String eachId = "";

  File file = SPIFFS.open("/usersId.txt", FILE_READ);
  if (!file) return;

  botUsersCount = 0;
  while (file.available() && botUsersCount < maxbotUsers) {
    eachId = file.readStringUntil('\n');
    eachId.trim(); // remove any extra whitespace, including \r
    if (eachId.length() > 0) { // skip eachId if.....
      botUsersId[botUsersCount++] = eachId ; // remove extra spaces including \r by skipping
    }
  }

  file.close();
}

void saveToFile() {
  File file = SPIFFS.open("/usersId.txt", FILE_WRITE);
  if (!file) {
    Serial.println("❌ Failed to open file for writing");
    return;
  }

  for (int i = 0; i < botUsersCount; i++) {
    file.println(botUsersId[i]);
  }
  file.close();
}


int findStringIndex(String targetString) {
  for (int i = 0; i < botUsersCount; i++) {
    if (botUsersId[i].equalsIgnoreCase(targetString)) {
      return i; // Found the match
    }
  }
  return -1; // Not found
}

void addUserId(String chatId, String idToAdd) {
  idToAdd.trim(); // remmove white spaces just in case

  if (idToAdd.length() == 0) {
    Serial.println("❌ Cannot add an empty string.");
    return;
  }

  if (findStringIndex(idToAdd) != -1) {
    Serial.println("⚠️ Duplicate entry! That string already exists.");
    bot.sendMessage(chatId, "⚠️ Duplicate entry! That string already exists.", "");
    return;
  }

  if (botUsersCount < maxbotUsers) {
    botUsersId[botUsersCount++] = idToAdd; //incrementing string count and saveing to the new position
    saveToFile();
    Serial.println("✅ Added: " + idToAdd);
  } else {
    Serial.println("❌ List is full!");
  }
}


String listUserIds() {
  if (botUsersCount == 0) {
    Serial.println("📭 List is empty!");
    return "No user id saved";
  }
  
  Serial.println("📜 Current Strings:");
  String allUserIds = "";
  for (int i = 0; i < botUsersCount; i++) {
    allUserIds += "\nUser " + String(i) + ": " + botUsersId[i].c_str();
  }

  return allUserIds;
}

void deleteUserId(String chatId, int inputIdIdex) {
  
    if (inputIdIdex >= 0 && inputIdIdex < botUsersCount) {
      for (int i = inputIdIdex; i < botUsersCount - 1; i++) {
        botUsersId[i] = botUsersId[i + 1]; //shifting all strings from right of the item deleted to the left 
      }
      botUsersCount--;
      saveToFile();
      Serial.printf("🗑️ Deleted item at index %d\n", inputIdIdex);
      bot.sendMessage(chatId, "🗑️ Deleted user id:" + String(inputIdIdex), "");
    } else {
      Serial.println("❌ Invalid index!\n String Not found");
    }
}

bool isUserRegistered(String userId) {
  for (int currentUser = 0; currentUser < botUsersCount; currentUser++) {
    if (userId == botUsersId[currentUser]){
      return true;
    }
  }
  return false;
}

String commandList[] = {"/start", "/checkStatus","/listUsers", "/deleteUserId", "/addUser"};
String commandsDescription[] = {
      "/start to view the list of commands\n",
      "/checkStatus to check if cooker is on or off\n",
      "/listUsers to list this telegram bot users\n",
      "/deleteUserId to delete a user\n",
      "/addUser to add a user."
    };

void handleStart(String command, String chatId, String userName);
void checkGreenHouseStatus(String command, String chatId, String userName);
void listUsers(String command, String chatId, String userName);
void deleteUser(String command, String chatId, String userName);
void addUser(String command, String chatId, String userName);


void (*commandActions[])(String, String, String) = { // the string, string is to show that the fucntions takes parameter ========    SO NOW, EVERY FUNCTION IN THAT ARRAY MUST ACCEPT 3 PARAMETERS, WHERTHER YOU NEED IT OR NOT
  handleStart,
  checkGreenHouseStatus,
  listUsers,
  deleteUser,
  addUser
};

void handleWrongCommand(String command, String chatId, String userName);
void handleCommand(String command, String chatId, String userName);
String extractWord(String command, char startChar, char endChar);
void handleAdminActivities(String command, String chatId, String userName);
void handleNewMessages(int numNewMessages);
void connectWifi();


String extractWord(String command, char startChar, char endChar) {
  Serial.println("input command = \n" + command);
  int wordStartIndex = command.indexOf(startChar);
  int wordEndIndex = command.indexOf(endChar);

  if (wordStartIndex != -1 && wordEndIndex != -1 && wordStartIndex < wordEndIndex) {
    String word = command.substring(wordStartIndex + 1, wordEndIndex);
    Serial.println("Extracted word is: " + word);
    return word;
  } else {
    Serial.println("Command format is invalid.");
    return "invalid";
  }
}

void handleAdminActivities(String command, String chatId, String userName) {
  String userId = "";

  // Extract password
  String password = extractWord(command, '[', ']');
  
  if (password == "invalid") {
    bot.sendMessage(chatId, "Sorry Mr/Mrs " + userName + ", Command format is invalid. \nTry again", "");
    return;
  }

  if (password != adminPassword) {
    Serial.println("wrong password, try again");
    bot.sendMessage(chatId, "Sorry Mr/Mrs " + userName + ", ❌ wrong password. \nTry again", "");
    return;
  }

  bot.sendMessage(chatId, "✅ Password is correct 👌", "");

  String inputId = extractWord(command, '<', '>');

  if (command.startsWith("add ")) {
    if (chatId != adminChatId) {
      bot.sendMessage(chatId, "❌ Sorry, you're not the admin.\nOnly Admin is allowed to run this command", "");
      return;
    }

    addUserId(chatId, inputId);
  }else if (command.startsWith("del ")) {
    if (chatId != adminChatId) {
      bot.sendMessage(chatId, "❌ Sorry, you're not the admin.\nOnly Admin is allowed to run this command", "");
      return;
    }
    int inputIdIndex = inputId.toInt();
    deleteUserId(chatId, inputIdIndex);
  }else {
    handleWrongCommand(chatId, userName, command);
  }
}

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) { //do this for all newmessages, all meassages are stored in array numNewMessages
    
    // Print the received message
    String receivedCommand = bot.messages[i].text;
    Serial.println(receivedCommand);

    String userName = bot.messages[i].from_name ;

    // Chat id of the requester
    String chatId = String(bot.messages[i].chat_id);
    if (!isUserRegistered(chatId) && chatId != adminChatId){
      bot.sendMessage(chatId, "Sorry Mr/Mrs " + userName + ", You're unauthorized to use this bot", "");
      // reportUserToMaster();
      continue;
    }


    String msg = "";
    // I am converting the first letter of the command to lowercase to prevent errors.
    if (receivedCommand.length() > 0) {
      receivedCommand[0] = tolower(receivedCommand[0]);  // Convert first character to lowercase
    }

    if(receivedCommand.startsWith("add ") || receivedCommand.startsWith("del ") || receivedCommand.startsWith("remove ")  || receivedCommand.startsWith("/list ")) {
      handleAdminActivities(receivedCommand, chatId, userName);
      continue;
    }

    // HANDLE other COMMANDS RECEIVED ======================
    handleCommand(receivedCommand, chatId, userName);
    

  }
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.print("WIFI CONNECTED");
  delay(displayDelay);
}

void periodicallyCheckMsg() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
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
  
  printTitle();
  
  // Connect to Wi-Fi
  connectWifi();

  initializeSpiffs(); // doing this last so that it can send error message 
}


float humidity = 0.0, temperature = 0.0;
int moistureValue = 0, ldrValue = 0;

void loop() {
  moistureValue = analogRead(soilMoistureSensorPin);  // Read analog value (0 - 4095)
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

  periodicallyCheckMsg();
  vTaskDelay(2000/portTICK_PERIOD_MS);//read dht every 1 to 2 secs
}

String getSoilMoistureLevel(int moistureValue) {
    // Map moisture to percentage: 100% = wet (low value), 0% = dry (high value)
  int moisturePercent = map(moistureValue, 4095, 1500, 0, 100); 
  moisturePercent = constrain(moisturePercent, 0, 100);

  String msg = "Soil Moisture: " + String(moisturePercent) + "%";
  return msg;
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
  if (moistureValue < 1000) {
    digitalWrite(pumpPin, HIGH); // dry
    Serial.println("Pump ON - Soil is dry");
  } else {
    digitalWrite(pumpPin, LOW);
    Serial.println("Pump OFF - Soil is moist");
  }

  // Light regulation using artificial light
  int lightLevel = analogRead(ldrPin);
  if (lightLevel < 600) {
    digitalWrite(lightPin, HIGH); // dark
    Serial.println("Light ON - It's dark");
  } else {
    digitalWrite(lightPin, LOW);
    Serial.println("Light OFF - Environment is bright");
  }

  Serial.println("Greenhouse regulation complete.\n");
}


void handleStart(String command, String chatId, String userName) {
  String msg = "Welcome, to ";
  msg += projectTitle;
  msg += " Mr/Mrs ";
  msg += userName;
  msg += ".\n";
  msg += "Use the following commands to interact with the ";
  msg += projectTitle;
  msg += ".\n\n";
  // bot.sendMessage(chatId, "START message received + CHATID: ", "");
  // bot.sendMessage(chatId, msg, "");
  for( int count = 0; count < sizeof(commandsDescription)/sizeof(commandsDescription[0]); count++) {
    msg += commandsDescription[count];
  }
  // Serial.println("handle start msg:    \n\n + CHATID: " + String(chatId));
  // Serial.println(msg);

  bot.sendMessage(chatId, msg, "");
  Serial.println("Start sent to : " + chatId);
}

void checkGreenHouseStatus(String command, String chatId, String userName) {
  float temperature = dht.readTemperature();     // Read temperature in Celsius
  float humidity = dht.readHumidity();           // Read humidity
  int moistureValue = analogRead(soilMoistureSensorPin); // Read soil moisture
  int lightValue = analogRead(ldrPin);           // Read light level from LDR

  // Fallback in case DHT reading fails
  if (isnan(temperature)) temperature = 0.0;
  if (isnan(humidity)) humidity = 0.0;
   // Determine device states
  String buzzerState = digitalRead(buzzerPin) ? "ON" : "OFF";
  String pumpState = digitalRead(pumpPin) ? "ON" : "OFF";
  String lightState = digitalRead(lightPin) ? "ON" : "OFF";
  String fanState = digitalRead(fanPin) ? "ON" : "OFF";
  String bulbState = digitalRead(tungstenBulbPin) ? "ON" : "OFF";

  // Construct the message
  String status = "👨‍🌾 *Greenhouse Status*\n\n";
  status += "🌡 Temperature: " + String(temperature, 1) + "°C\n";
  status += "💧 Humidity: " + String(humidity, 1) + "%\n";
  status += "🌱 Soil Moisture: " + String(moistureValue) + "\n";
  status += "☀️ Light Intensity (LDR): " + String(ldrValue) + "\n\n";
  status += "🔌 *Device States:*\n";
  status += "🔊 Buzzer: " + buzzerState + "\n";
  status += "💦 Pump: " + pumpState + "\n";
  status += "💡 Light: " + lightState + "\n";
  status += "🌬 Fan: " + fanState + "\n";
  status += "🔥 Tungsten Bulb: " + bulbState + "\n";

  bot.sendMessage(chatId, status, "Markdown");
}


void listUsers(String command, String chatId, String userName) {
  String msg = "";
  if (chatId != adminChatId) {
    bot.sendMessage(chatId, "❌ Sorry, you're not the admin.\nOnly Admin is allowed to run this command", "");
    return;
  }
  msg += listUserIds();
  bot.sendMessage(chatId, msg, "");
}

void deleteUser(String command, String chatId, String userName) {
  String msg = "";
  if (chatId != adminChatId) {
    bot.sendMessage(chatId, "❌ Sorry, you're not the admin.\nOnly Admin is allowed to run this command", "");
    return;
  }
  msg += "To delete a user, you must be the admin and know the passsword\n";
  msg += "get the new user's chat ID using the guide here:  https://randomnerdtutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/ \n";
  msg += "Then follow this formula: addUser <new user chat ID> \"password\"\n";
  msg += "eg: /addUser <31123987> [I love my wife] \n, then send it";
  bot.sendMessage(chatId, msg, "");
}

void addUser(String command, String chatId, String userName) {
  String msg = "";
  if (chatId != adminChatId) {
    bot.sendMessage(chatId, "❌ Sorry, you're not the admin.\nOnly Admin is allowed to run this command", "");
    return;
  }
  msg += "To add a new user, you must be the admin and know the passsword\n";
  msg += "get the new user's chat ID using the guide here:  https://randomnerdtutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/ \n";
  msg += "Then follow this formula: add <new user chat ID> \"password\"\n\ncopy and use the sample below as a TEMPLATE ⬇️ ⬇️ ⬇️ ";
  bot.sendMessage(chatId, msg, "");
  msg = "eg: add <31123987> [I love my wife] \n, then send it ";
  bot.sendMessage(chatId, msg, "");
}

void handleCommand(String command, String chatId, String userName) {
  int commandCount = sizeof(commandList) / sizeof(commandList[0]);
  for (int i = 0; i < commandCount; i++) {
    if (command == commandList[i]) {
      commandActions[i](command, chatId, userName);  // Call the corresponding function with parameters
      return;
    }
  }
  handleWrongCommand(command, chatId, userName);  // If no match found
}

void handleWrongCommand(String command, String chatId, String userName) {
  String msg = "";
  msg = "Sorry Mr/Mrs " + userName + ", You have entered the wrong command.\n";
  msg += "Use the following commands to interact with the ";
  msg += projectTitle;
  msg += ".\n\n";
  
  for( int count = 0; count < sizeof(commandsDescription)/sizeof(commandsDescription[0]); count++) {
    msg += commandsDescription[count];
  }

  bot.sendMessage(chatId, msg, "");
}


#endif




