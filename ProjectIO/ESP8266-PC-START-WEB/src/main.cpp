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


#include "index.h"
#include "FS.h"
#include "LITTLEFS.h"

#define WIFI_SSID "2G_Netvirtua367"
#define WIFI_PASSWORD "3336530000"


const char* PARAM_MESSAGE = "start";

AsyncWebServer server(80);

char buff[256];

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
  Serial.begin(9600);
  pinMode(D5,OUTPUT);
  digitalWrite(D5,LOW);
  connectToWifi();
  Serial.println(WiFi.localIP());

      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/reiniciar?message=reiniciar
    server.on("/reiniciar", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
            digitalWrite(D5,HIGH);
            delay(800);
            digitalWrite(D5,LOW);
        } else {
            message = "Não iniciado";
        }
        String mess = "\n<a href='/reiniciar?message=start'>Start</a>";
        String message2 = message+mess;
        request->send(200, "text/plain", "PC: " + message);
    });



    server.onNotFound(notFound);

    server.begin();
}

void loop(){
}