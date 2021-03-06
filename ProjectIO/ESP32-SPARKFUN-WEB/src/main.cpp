#include <Arduino.h>

#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

#include <WiFi.h>

#include <AsyncTCP.h>
#include <WebServer.h>

//#include <ESPAsyncWebServer.h>

//#include <ArduinoJson.h>
//#include "AsyncJson.h"
//#include "AsyncWebSynchronization.h"

#include "index.h"
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
WebServer server(80);
//AsyncWebServer server(80);

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

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleSensor() {
 string buff[256];
 serializeJsonPretty(jsonsens,buff);
 server.send(200, "text/plane", buff); //Send ADC value only to client ajax request
}

void setup(){
  
  Serial.begin(115200);
  
  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  
  connectToWifi();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  

  server.on("/", handleRoot);
  server.on("sensor",handleSensor);
/*
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, world");
  });
  
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    DynamicJsonDocument jsonbuffer;
    JsonObject &jsonsens = jsonbuffer.createObject();
    body = bioHub.readBpm();
    jsonsens["Heart"] = body.heartRate;
    jsonsens["Oxygen"] = body.oxygen;
    jsonsens["Confidence"] = body.confidence;
    jsonsens["Status"] = body.status;
    jsonsens["ExtStatus"] = body.extStatus;
    jsonsens.printTo(*response)
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
*/

  server.onNotFound(notFound);
  
  server.begin();

  Wire.begin();

  startBioHub();

}

void loop(){

  server.handleClient();
  delay(1);
}