#include <Arduino.h>
#include <string>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"
//#include "index.h"
//#include "FS.h"
//#include "LITTLEFS.h"

#define WIFI_SSID "FERNANDA 2G"
#define WIFI_PASSWORD "joca1524"


const char* PARAM_MESSAGE = "start";

AsyncWebServer server(80);

char buff[256];

void connectToWifi() {
  int k =0;
  while(WiFi.status() != WL_CONNECTED) {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  delay(8000);
  if (WiFi.status() == WL_CONNECTED){
    Serial.print("Wifi Connected");
    Serial.println(WiFi.localIP());
    return;
  }
  Serial.print("Connection Failed : ");
  Serial.println(WiFi.localIP());
  k++;
  if(k > 20){
    return;
  }
  }
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


void off(){
  delay(1000);
  digitalWrite(D6,HIGH);
  Serial.println("d6 off");
  }
void setup(){
  Serial.begin(9600);
  digitalWrite(D6,HIGH);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  
  connectToWifi();
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/reiniciar?start=start
  server.on("/reiniciar", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
            digitalWrite(D6,LOW);
            Serial.println("Ligando");
            Serial.println(digitalRead(D6));
        } 
        else {
            message = "Nao iniciado";
        }
        //String mess = "\n<a href='/reiniciar?message=start'>Start</a>";
        //String message2 = message+mess;
        request->send(200, "text/plain", "PC: " + message);
  });

  server.onNotFound(notFound);
  server.begin();
}

void loop(){
  delay(20);
  if(digitalRead(D6)==LOW){
    off();
  }
  yield();
}