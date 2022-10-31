#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <ArduinoJson.h>
#include <TaskScheduler.h>

#define MSG_BUFFER_SIZE	(50)
#define REPORTING_PERIOD_MS 10000
#define SENSORREADING_PERIOD_MS 60

void task_beat_callback();
void task_beat_onDisable();
void task_send_callback();
void task_send_onDisable();

uint32_t tsLastReport = 0;
PulseOximeter pox;

const char* ssid = "MERCUSYS";
const char* password = "ic123456";
const char* mqtt_server = "192.168.1.113";
const int mqtt_port = 1883;

DynamicJsonDocument doc(1024);
char buff[1000];

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

char msg[MSG_BUFFER_SIZE];
int value = 0;
int count = 0;

Task beat

void onBeatDetected()
{
    count++;
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Config BioHub
  if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }
  
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
  // End Config Bio

}

void loop() {
  
  pox.update();

  if (count>6||(millis() - tsLastReport)>REPORTING_PERIOD_MS) {
    if (!client.connected()) {
      reconnect();
    }
    count=0;
    client.loop();
    doc["heartrate"] = pox.getHeartRate();
    doc["oxygen"] = pox.getSpO2();
    serializeJson(doc,buff);
    Serial.println(buff);
    client.publish("Device/BO",buff);
    tsLastReport = millis();
  }
}