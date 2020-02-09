#include <PubSubClient.h>
#include <WiFi.h>

#include "HttpOTA.h"
#include "InjectedSerial.h"
#include "TelnetMonitor.h"
#include "WiFiUtil.h"
#include "openssl\ssl.h"

#define PIN_NO 15
uint8_t pinLevel = HIGH;

char hostname[] = "ESP32-LIGHT";
char ssid[] = "Penitentiary 202";
const char *password = "dontuseourwifi";
const char *mqttServer = "192.168.86.30";
const char *mqttUser = "pi";
const char *mqttPassword = "qazwsxedc";
u16_t mqttPort = 1883;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Debug tools
InjectedSerial iSerial(Serial);
#define MAX_TELNET_CLIENTS 1
WiFiServer telnetServer(23);
TelnetMonitor telnetMonitor(telnetServer, MAX_TELNET_CLIENTS, iSerial);
AsyncWebServer httpServer(80);
HttpOTA ota(httpServer, "/ota/", iSerial);

void onMqttConnected() {
  mqttClient.subscribe("/light/set");
  mqttClient.publish("/light/status", pinLevel ? "ON" : "OFF");
}

void connectMqtt() {
  iSerial.printf("MQTT> Start Connecting to %s@%s:%d\r\n", mqttUser, mqttServer,
                 mqttPort);
  while (WiFi.status() == WL_CONNECTED && !mqttClient.connected()) {
    iSerial.print("MQTT> Attempting connection...");
    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str(), mqttUser, mqttPassword)) {
      iSerial.println("connected");
      onMqttConnected();
    } else {
      iSerial.print("failed, rc=");
      iSerial.print(mqttClient.state());
      iSerial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void onMqttMessage(char *topic, byte *payload, unsigned int length) {
  iSerial.printf("MQTT> New message on %s\r\n", topic);
  iSerial.print("MQTT> Data: ");
  iSerial.write(payload, length);
  iSerial.println();
  if (!strcmp(topic, "/light/set")) {
    pinLevel = *payload == '0' ? LOW : HIGH;
    digitalWrite(PIN_NO, pinLevel);
    mqttClient.publish("/light/status", pinLevel ? "ON" : "OFF");
  }
}

void setup() {
  pinMode(PIN_NO, OUTPUT);
  digitalWrite(PIN_NO, pinLevel);
  iSerial.begin(115200);
  iSerial.setTimeout(500);
  delay(10);
  iSerial.println();
  iSerial.setWriteFn([&](const uint8_t *buffer, size_t size) {
    std::vector<WiFiClient> &telnetClients = telnetMonitor.getClients();
    for (uint8_t i = 0; i < MAX_TELNET_CLIENTS; i++) {
      if (telnetClients[i] && telnetClients[i].connected()) {
        telnetClients[i].write(buffer, size);
        delay(1);
      }
    }
  });
  WiFi.begin(ssid, password);
  WiFi.setHostname(hostname);
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(onMqttMessage);
}

void loop() {
  telnetMonitor.loop();
  if (WiFi.status() == WL_CONNECTED) {
    if (mqttClient.connected()) {
      mqttClient.loop();
    } else {
      iSerial.println("MQTT> Connection lost, reconnecting");
      connectMqtt();
    }
  } else {
    iSerial.println("WiFi> Connection lost, reconnecting");
    WiFiUtil.syncConnect(iSerial);
    telnetServer.setNoDelay(true);
    telnetServer.begin();
    httpServer.begin();
  }
}
