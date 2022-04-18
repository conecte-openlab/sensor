#include <Arduino.h>

#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

#include <WiFi.h>

#include <AsyncTCP.h>
#include <WebServer.h>

#include <ESPAsyncWebServer.h>

#include <ArduinoJson.h>
#include "FS.h"
#include "LITTLEFS.h"
extern "C" {
	#include "freertos/FreeRTOS.h"
	#include "freertos/timers.h"
}

#define WIFI_SSID "...."
#define WIFI_PASSWORD "...."

const char* PARAM_MESSAGE = "message";

#define resPin 4
#define mfioPin 5


AsyncWebServer server(80);
TimerHandle_t wifiReconnectTimer;
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 
bioData body;  
char buff[256];
DynamicJsonDocument  doc(200);

void startBioHub(){

  int result = bioHub.begin();
  if (result == 0) //Zero errors!
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");
 
  Serial.println("Configuring Sensor...."); 
  
  int error = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 
  if(error == 0){ // Zero errors
    Serial.println("Sensor configured.");
  }
  else {
    Serial.println("Error configuring sensor.");
    Serial.print("Error: "); 
    Serial.println(error); 
  }
}

void connectToWifi() {

  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(4000);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connection Failed");
  }
}



void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setup(){
  
  Serial.begin(115200);
  
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  
  connectToWifi();

  Wire.begin();

  startBioHub();

  Serial.println("Loading up the buffer with data....");
  delay(4000); 
  mqttClient.subscribe("devices/esp",0);
  
}


//Loop 
void loop(){

    // Information from the readBpm function will be saved to our "body"
    // variable.  
    body = bioHub.readBpm();
    Serial.print("Heartrate: ");
    Serial.println(body.heartRate); 
    doc["Heart"] = body.heartRate;

    Serial.print("Oxygen: ");
    Serial.println(body.oxygen); 
    doc["Oxygen"] = body.oxygen;
    doc["Confidence"] = body.confidence;

    Serial.print("Status: ");
    Serial.println(body.status); 
    doc["Status"] = body.status;

    Serial.print("Extended Status: ");
    Serial.println(body.extStatus);
    doc["ExtStatus"] = body.extStatus;
 
    serializeJson(doc,buff);

    
    delay(2500); 
}