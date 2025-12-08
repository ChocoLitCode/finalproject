// Libraries
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <ArduinoJson.h>
#include "doorSystem.h"
#include "roomSystem_1.h"
#include "roomSystem_2.h"
#include "roomSystem_3.h"
#include "lcd.h"



// Network and Firebase credentials
#define WIFI_SSID "HUAWEI Y6 Pro 2019"
#define WIFI_PASSWORD "66666666"

#define Web_API_KEY "AIzaSyAgd3i1tk7zsSDVeMYIiTQPOk9iEuVAMyI"
#define DATABASE_URL "https://iot-diorama-70640-default-rtdb.asia-southeast1.firebasedatabase.app"
#define USER_EMAIL "eixirtellehcirt@gmail.com"
#define USER_PASSWORD "12345678"

// User function
void processData(AsyncResult &aResult);

// Authentication
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASSWORD);

// Firebase components
FirebaseApp app;
WiFiClientSecure ssl_client;
WiFiClientSecure stream_ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
AsyncClient streamClient(stream_ssl_client);
RealtimeDatabase Database;

// Timer variables for sending data every 10 seconds
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000;

// Variable to save USER UID
String uid;

// Variables to save database paths
String databasePath;
String tempPath;
String humPath;
// Database  path (where the data is)
String listenerPath = "board1/outputs/digital/";

float temperature;
float humidity;

void setup() {
  Serial.begin(115200);

    // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)    {
    Serial.print(".");
    delay(300);
  }
  Serial.println();

  ssl_client.setInsecure();
//   ssl_client.setConnectionTimeout(1000);
  ssl_client.setHandshakeTimeout(5);

  // Initialize Firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "🔐 authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);

//   if(!setDoorPins()){
//     Serial.println("Door System not yet ready!");
//   }

//   if(!setRoomOne()){
//     Serial.println("Room 1 Lights not yet ready!");
//   }
  
//   if(!setRoomTwo()){
//     Serial.println("Room 2 Lights not yet ready!");
//   } 

 if(!setRoomThree()){
    Serial.println("Room 3 not yet ready!");
  }

  if(!setLCD()){
    Serial.println("LCD not yet ready!");
  }

}

void loop() {
  // showLCD();
//   startDoor();
//   startRoomOne();
//   startRoomTwo();

app.loop();
  // Check if authentication is ready
  if (app.ready()){

    // Periodic data sending every 10 seconds
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= sendInterval){
      // Update the last send time
      lastSendTime = currentTime;
        
      // Get User UID
      Firebase.printf("User UID: %s\n", app.getUid().c_str());
      uid = app.getUid().c_str();
      databasePath = "UsersData/" + uid;

      // Update database path for sensor readings
      tempPath = databasePath + "/temperature"; // --> UsersData/<user_uid>/temperature
      humPath = databasePath + "/humidity"; // --> UsersData/<user_uid>/humidity
          
      startRoomThree();
        
      Serial.println("Writing to: " + tempPath);

      Database.set<float>(aClient, tempPath, temperature, processData, "RTDB_Send_Temperature");
      Database.set<float>(aClient, humPath, humidity, processData, "RTDB_Send_Humidity");

    }
  }
}

void processData(AsyncResult &aResult){
  if (!aResult.isResult())
    return;

  if (aResult.isEvent())
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.eventLog().message().c_str(), aResult.eventLog().code());

  if (aResult.isDebug())
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());

  if (aResult.isError())
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());

  if (aResult.available())
    Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());
}

