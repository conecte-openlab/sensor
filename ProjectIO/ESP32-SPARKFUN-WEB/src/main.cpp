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

#define WIFI_SSID "MERCUSYS"
#define WIFI_PASSWORD "12345678"

const char* PARAM_MESSAGE = "message";



//Pinos I2C SDA 21,SCL 21
#define resPin 4
#define mfioPin 5

AsyncWebServer server(80);
TimerHandle_t wifiReconnectTimer;
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 
bioData body;  
char buff[256];
DynamicJsonDocument jsonsens(1024);

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
  Serial.println("Loading up the buffer with data....");
  delay(4000); 
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
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, world");
  });
  
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument jsonsens(1024);
    body = bioHub.readBpm();
    jsonsens["Heart"] = body.heartRate;
    jsonsens["Oxygen"] = body.oxygen;
    jsonsens["Confidence"] = body.confidence;
    jsonsens["Status"] = body.status;
    jsonsens["ExtStatus"] = body.extStatus;
    serializeJson(jsonsens,*response);

    request->send(response);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });
  
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });
  
  server.onNotFound(notFound);
  server.begin();

  Wire.begin();

  startBioHub();

}


//Loop 
void loop(){

    // Information from the readBpm function will be saved to our "body"
    // variable.  
    body = bioHub.readBpm();
    Serial.println("Reading Sensor");
    jsonsens["Heart"] = body.heartRate;
    jsonsens["Oxygen"] = body.oxygen;
    jsonsens["Confidence"] = body.confidence;
    jsonsens["Status"] = body.status;
    jsonsens["ExtStatus"] = body.extStatus;
    serializeJson(jsonsens,buff);

    delay(2500); 
   
}